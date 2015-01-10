#include <iostream>
#include <fstream>
#include <cstring>

using namespace std;

typedef unsigned char      u8;
typedef unsigned short     u16;
typedef unsigned int       u32;
typedef unsigned long long u64;

#pragma pack(1)
struct INFO{
	u32 option;
	u8  key;
} info;

u16 opcode;
u8 tag;
u8 hdr[]={0xEF, 0xBB, 0xBF};

void cipher(u8 key, u8* content, u16 size)
{
	for(u16 i=0; i<size; i++)
	{
		content[i]^=key;
	}
}

u8 getkey(INFO info)
{
	u8 key;
	switch(info.option/0x1000000)
	{
	case 0x00:
		{
			key = info.key;
			break;
		}
	case 0x01:
		{
			key = ((u8)((info.key^0x40)+0x24))%0x80;
			break;
		}
	case 0x02:
		{
			key = ((u8)((info.key^0x40)+0x08))%0x80;
			break;
		}
	case 0x03:
		{
			key = ((u8)((info.key^0x40)-0x14))%0x80;
			break;
		}
	case 0x04:
		{
			key = ((u8)((info.key^0x40)-0x30))%0x80;
			break;
		}
	default: break;
	}
	return key;
}

u8 generatekey(INFO *info, u16 newlen)
{
	u8 newkey;	
	if(newlen<0x100)
	newlen+=2;
	else
	newlen+=3;
	
	newkey = newlen%0x80;
	
	switch(info->option/0x1000000)
	{
	case 0x00:
		{
			info->key = newkey;
			break;
		}
	case 0x01:
		{
			info->key = ((u8)(newkey-0x24)^0x40)%0x80;
			break;
		}
	case 0x02:
		{
			info->key = ((u8)(newkey-0x08)^0x40)%0x80;
			break;
		}
	case 0x03:
		{
			info->key = ((u8)(newkey+0x14)^0x40)%0x80;
			break;
		}
	case 0x04:
		{
			info->key = ((u8)(newkey+0x30)^0x40)%0x80;
			break;
		}
	default: break;
	}
	
	return newkey;
}

int main(int argc, char* argv[])
{
	if(argc!=4){
		cerr<<"usage:  "<<argv[0]<<" original.bin input.txt output.bin\n";
		return 1;
	}
	ifstream infile(argv[1], ios::binary);
	ifstream intxt(argv[2], ios::binary);
	ofstream outfile(argv[3], ios::binary);
	ofstream outlist("ins_length_list.txt");
	
	//read header
	intxt.read((char*)hdr, sizeof(hdr));
	
	while(1)
	{
		infile.read((char *)&opcode, sizeof(opcode));
		if(infile.eof()) break;
		outfile.write((const char *)&opcode, sizeof(opcode));
		
		switch(opcode){
		case 0x1000://setting:END FIlE
			outlist<<sizeof(opcode)<<endl;
			break;
			
		case 0x1028://DIALOG TIP
		case 0x1888://SAVE TITLE
			/*以下setting专用*/
		case 0xC000://*Game.Title
		case 0xC001://*Game.Author
		case 0xC002://*Game.ID
		case 0xC003:
		case 0xC004://ABOUT IMAGE
		case 0xC005://HOME PAGE
			{
				infile.read((char *)&info, sizeof(info));
				u8 key=getkey(info);
				infile.read((char *)&tag, 1);	
				cipher(key, (u8*)&tag, 1);
				//跳过
				u8 len;//没有判断88 89一般不会出现89
				infile.read((char *)&len, 1);
				cipher(key, (u8*)&len, 1);
				u8* buf=new u8[len];
				infile.read((char *)buf, len);
				
				//new content
				string content;
				getline(intxt,content);
				u8 newlen=content.length();
				u8 newkey=generatekey(&info,newlen);
				
				outlist<<sizeof(opcode)+sizeof(info)+2+newlen<<endl;
				
				cipher(newkey, (u8*)&tag, 1);
				cipher(newkey, (u8*)content.c_str(), newlen);
				cipher(newkey, (u8*)&newlen, 1);
				
				outfile.write((const char *)&info, sizeof(info));
				outfile.write((const char *)&tag, 1);
				outfile.write((const char*)&newlen, 1);
				outfile<<content;
				break;
			}

		case 0x3980:
		case 0xA180:
		case 0xA280:
			{
				infile.read((char *)&info, sizeof(info));
				u8 key=getkey(info);
				
				infile.read((char *)&tag, 1);
				cipher(key, (u8*)&tag, 1);
				
				u16 len;
				if(tag==0x88)
				{
					u8 tmp;
					infile.read((char *)&tmp, 1);
					cipher(key, (u8*)&tmp, 1);
					len=tmp;
				}
				if(tag==0x89)
				{
					u16 tmp;
					infile.read((char *)&tmp, 2);
					cipher(key, (u8*)&tmp, 2);
					len=tmp;
				}
				
				u8* buf=new u8[len];
				infile.read((char *)buf, len);
				cipher(key, buf, len);
				
				u16 newlen=0;
				u8* outbuf=new u8[65536];
				
				for(u16 i=0; i<len; )
				{
					u16 sublen, newsublen;
					tag=buf[i];
					if(tag==0x88)
					{
						sublen=buf[i+1];
						i+=2;
						i+=sublen;
						
						string content;
						getline(intxt,content);
						newsublen=content.length();
						if(newsublen<0x100)
						{ 
							outbuf[newlen]=0x88;
							outbuf[newlen+1]=newsublen;
							newlen += 2;
						}
						else
						{
							outbuf[newlen]=0x89;
							outbuf[newlen+1]=newsublen%0x100;
							outbuf[newlen+2]=newsublen/0x100;
							newlen += 3;
						}
						memcpy(&outbuf[newlen], content.c_str(), newsublen);
						newlen += newsublen;
					}
					if(tag==0x89)
					{
						sublen=buf[i+1]+buf[i+2]*0x100;
						i+=3;
						i+=sublen;
						
						string content;
						getline(intxt,content);
						newsublen=content.length();
						if(newsublen<0x100)
						{ 
							outbuf[newlen]=0x88;
							outbuf[newlen+1]=newsublen;
							newlen += 2;
						}
						else
						{
							outbuf[newlen]=0x89;
							outbuf[newlen+1]=newsublen%0x100;
							outbuf[newlen+2]=newsublen/0x100;
							newlen += 3;
						}
						memcpy(&outbuf[newlen], content.c_str(), newsublen);
						newlen += newsublen;
					}
					if(tag==0x98)
					{
						outbuf[newlen]=buf[i];
						outbuf[newlen+1]=buf[i+1];
						i+=2;
						newlen += 2;
					}
				}
				
				//output
				if(newlen+2>=0x80 && info.option==0x00)
				 info.option=0x01000000;
				 
				u8 newkey=generatekey(&info,newlen);
				
				outfile.write((const char *)&info, sizeof(info));
				u16 outlen=newlen;
				if(newlen<0x100)
				{
					tag=0x88;
					cipher(newkey, (u8*)&tag, 1);
					cipher(newkey, (u8*)&outlen, 1);
					outfile.write((const char*)&tag, 1);
					outfile.write((const char*)&outlen, 1);
					
					outlist<<sizeof(opcode)+sizeof(info)+2+newlen<<endl;
				}
				else
				{
					tag=0x89;
					cipher(newkey, (u8*)&tag, 1);
					cipher(newkey, (u8*)&outlen, 2);
					outfile.write((const char*)&tag, 1);
					outfile.write((const char*)&outlen, 2);
					outlist<<sizeof(opcode)+sizeof(info)+3+newlen<<endl;
				}

				cipher(newkey, outbuf, newlen);
				outfile.write((const char*)outbuf, newlen);
				break;
			}

		case 0x1003:case 0x1018:case 0x1019:
		case 0x1100:case 0x1102:case 0x1110:case 0x1112:case 0x1140:case 0x1142:case 0x1160:case 0x11F0:
		case 0x1880:case 0x1881:case 0x188E:case 0x188F:case 0x1890:case 0x18A0:
		case 0x2001://等待鼠标点击
		case 0x2100:case 0x2112:case 0x2113:case 0x2800:case 0x2F00:
		case 0x3000:case 0x300F:case 0x390F:
		case 0x3918:case 0x3919://贴图横纵坐标
		case 0x3998:case 0x3999:
		case 0x39C0:case 0x39C1:case 0x39C2:case 0x39C3:case 0x39C5:case 0x39C6:case 0x39C7:case 0x39C8:
		case 0x39D0:case 0x39D1:case 0x39D2:case 0x39D3:
		case 0x4000:case 0x4010:case 0x4080:case 0x4100:case 0x4110:case 0x4800:
		case 0x7020:case 0x7021:case 0x7022:case 0x7120:case 0x7121:case 0x7122:
		case 0x7700:case 0x7701:case 0x7702:case 0x7710:case 0x7711:case 0x7720:case 0x7721:
		case 0x8000:case 0x8010:case 0x8011:case 0x8012:case 0x8018:case 0x8019:
		case 0x811B:case 0x8180:case 0x818F:
		case 0x81C0:case 0x81C1:case 0x81C2:case 0x81C5:case 0x81C6:case 0x81C7:case 0x81C8:
		case 0x81D0:case 0x81D1:case 0x81D2:case 0x81D3:case 0x8210:
		case 0x8310:case 0x8312:case 0x8313:case 0x8314:case 0x831F:
		case 0x8320:case 0x8321:case 0x8322:case 0x8328:case 0x8329:case 0x832F:
		case 0x9000:case 0x9101:case 0x9102:case 0x9800:case 0x9810:
		case 0xA102:case 0xA103:case 0xA108:case 0xA10A:
		case 0xA118:case 0xA119:case 0xA18F:
		case 0xA1C0:case 0xA1C1:case 0xA1C2:case 0xA1C3:case 0xA1C4:case 0xA1C5:case 0xA1C6:case 0xA1C7:case 0xA1C8:
		case 0xA1D0:case 0xA1D1:case 0xA1D2:case 0xA1D3:
		case 0xA218:case 0xA219:
		case 0xA2C0:case 0xA2C6:
		case 0xA2D0:case 0xA2D1:case 0xA2D2:case 0xA2D3:
		case 0xA600:case 0xA680:case 0xA681:case 0xA682:case 0xA683:
		case 0xAF11:case 0xAF12:case 0xAF20:case 0xA7C1:case 0xA7C8:
			{
				infile.read((char *)&info, sizeof(info));
				outfile.write((const char *)&info, sizeof(info));
				u8 len=info.key;
				if(len == 0) len=1;
				u8* buf=new u8[len];
				infile.read((char *)buf, len);
				outfile.write((const char *)buf, len);
				
				outlist<<sizeof(opcode)+sizeof(info)+len<<endl;
				break;
			}
			
		case 0x1010:case 0x1011:
		case 0x3802:	
		case 0x38FF://跳转
		case 0x3900:
		case 0x39A0:case 0x39A1:
		case 0x7100:case 0x7101:case 0x7102:
		case 0x7800://播放视频 后面的数字是list中的顺序
		case 0x8200://显示图片 同上
		case 0x8201:
		case 0xA101:case 0xA201:
			{
				u8 buf[2];
				infile.read((char *)&tag, 1);
				outfile.write((const char *)&tag, 1);
				if(tag == 0x81){
					infile.read((char *)buf, 2);
					outfile.write((const char *)buf, 2);
					outlist<<sizeof(opcode)+3<<endl;
				}
				if(tag == 0x80){
					infile.read((char *)buf, 1);
					outfile.write((const char *)buf, 1);
					outlist<<sizeof(opcode)+2<<endl;
				}
				break;
			}
			
			/*以下setting专用*/
		case 0xC010:case 0xC011:case 0xC012:case 0xC013://版本号
			{
				u8 buf[2];
				infile.read((char *)buf, 2);
				outfile.write((const char *)buf, 2);
				outlist<<sizeof(opcode)+2<<endl;
				break;
			}
		default:
			{
				outlist<<sizeof(opcode)<<endl;
				break;
			}
		}
	}
	return 0;
}