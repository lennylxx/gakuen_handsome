#include <iostream>
#include <fstream>

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

int main(int argc, char* argv[])
{
	if(argc!=3){
	  cerr<<"usage:  "<<argv[0]<<" input.bin output.txt\n";
		return 1;
	}
	ifstream infile(argv[1], ios::binary);
	ofstream outtxt(argv[2], ios::binary);
	
	//header
	outtxt.write((const char*)hdr, sizeof(hdr));
	
	while(1)
	{
		infile.read((char *)&opcode, sizeof(opcode));
		if(infile.eof()) break;
		
		char str[20];//临时
		sprintf(str, "%04X " , opcode);
		outtxt<<str;
		
		switch(opcode){
		case 0x1000://setting:END FIlE
			outtxt<<"<ENDFILE>"<<endl;
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
				
				sprintf(str, "%08X %02X " , info.option, info.key);
				outtxt<<str;
				
				infile.read((char *)&tag, 1);
				cipher(key, (u8*)&tag, 1);
				
				u8 len;//此处没有判断88 89一般不会出现89

				infile.read((char *)&len, 1);
				cipher(key, (u8*)&len, 1);
				
				sprintf(str, "%02X %02X " , tag, len);
				outtxt<<str;
				
				u8* buf=new u8[len];
				infile.read((char *)buf, len);
				cipher(key, buf, len);
				
				//输出内容 可以改变
				for(u8 i=0;i<len;i++)
				{
					outtxt<<buf[i];
				}
				outtxt<<endl;
				break;
			}
			
		case 0x4010://@str
			{
				infile.read((char *)&info, sizeof(info));
				u8 key=getkey(info);
				sprintf(str, "%08X %02X " , info.option, info.key);
				outtxt<<str;
				
				infile.read((char *)&tag, 1);
				cipher(key, (u8*)&tag, 1);
				
				u8 len;
				if(tag==0x88)
				{//此处没有判断88 89一般不会出现89
					infile.read((char *)&len, 1);
					cipher(key, (u8*)&len, 1);

					sprintf(str, "%02X %02X " , tag, len);
					outtxt<<str;
					
					u8* buf=new u8[len];
					infile.read((char *)buf, len);
					cipher(key, buf, len);

					//输出内容 内容不可改变
					for(u8 i=0;i<len;i++)
					{
						outtxt<<buf[i];
					}
				}
				else
				{
					sprintf(str, "%02X " , tag);
					outtxt<<str;
					
					len = info.key - 1;
					
					u8* buf=new u8[len];
					infile.read((char *)buf, len);
					cipher(key, buf, len);
					
					for(u8 i=0;i<len;i++)
					{
						sprintf(str, "%02X " , buf[i]);
						outtxt<<str;
					}
				}
				outtxt<<endl;
				break;
			}
		case 0x2F00://CHAPTER
		case 0x3000://@str
		case 0x300F://file@str
		case 0x8180:
			/*以下setting专用*/
		case 0x1003:
		case 0x1102:
		case 0x1112:
		case 0x1142:
		case 0x11F0:
		case 0x4110:
			{
				infile.read((char *)&info, sizeof(info));
				u8 key=getkey(info);
				
				sprintf(str, "%08X %02X " , info.option, info.key);
				outtxt<<str;
				
				infile.read((char *)&tag, 1);
				cipher(key, (u8*)&tag, 1);
				
				
				u8 len;//此处没有判断88 89一般不会出现89
				infile.read((char *)&len, 1);
				cipher(key, (u8*)&len, 1);
				
				
				sprintf(str, "%02X %02X " , tag, len);
				outtxt<<str;
				
				u8* buf=new u8[len];
				infile.read((char *)buf, len);
				cipher(key, buf, len);
				
				//输出内容 不可以改变
				for(u8 i=0;i<len;i++)
				{
					outtxt<<buf[i];
				}
				outtxt<<endl;
				break;
			}
			
		case 0x3980:
		case 0xA180:
		case 0xA280:
			{
				infile.read((char *)&info, sizeof(info));
				u8 key=getkey(info);
				
				sprintf(str, "%08X %02X " , info.option, info.key);
				outtxt<<str;
				
				infile.read((char *)&tag, 1);
				cipher(key, (u8*)&tag, 1);
				
				u16 len;
				if(tag==0x88)
				{
					u8 tmp;
					infile.read((char *)&tmp, 1);
					cipher(key, (u8*)&tmp, 1);
					len=tmp;
					
					sprintf(str, "%02X %02X " , tag, len);
					outtxt<<str;
				}
				if(tag==0x89)
				{
					u16 tmp;
					infile.read((char *)&tmp, 2);
					cipher(key, (u8*)&tmp, 2);
					len=tmp;
					
					sprintf(str, "%02X %04X " , tag, len);
					outtxt<<str;
				}
				
				u8* buf=new u8[len];
				infile.read((char *)buf, len);
				cipher(key, buf, len);
				
				for(u16 i=0; i<len; )
				{
					u16 sublen;
					tag=buf[i];
					if(tag==0x88)
					{
						sublen=buf[i+1];
						sprintf(str, "%02X %02X " , tag, sublen);
						outtxt<<str;
						i+=2;
						
						for(u8 j=0;j<sublen;j++)
						{ outtxt<<buf[i+j]; }
						i+=sublen;
					}
					if(tag==0x89)
					{
						sublen=buf[i+1]+buf[i+2]*0x100;
						
						sprintf(str, "%02X %04X " , tag, sublen);
						outtxt<<str;
						
						i+=3;
						
						for(u16 j=0;j<sublen;j++)
						{ outtxt<<buf[i+j]; }
						
						i+=sublen;
					}
					if(tag==0x98)
					{
						if(buf[i+1]==0x01)
						outtxt<<"<LAST_NAME>";
						if(buf[i+1]==0x02)
						outtxt<<"<UNKN_9802>";
						if(buf[i+1]==0x04)
						outtxt<<"<FIRSTNAME>";
						
						i+=2;
					}
				}
				outtxt<<endl;
				break;
			}
			
			
		case 0x1018:case 0x1019:
		case 0x1100:case 0x1110:case 0x1140:case 0x1160:
		case 0x1880:case 0x1881:case 0x188E:case 0x188F:case 0x1890:case 0x18A0:
		case 0x2001://等待鼠标点击
		case 0x2100:case 0x2112:case 0x2113:case 0x2800:
		case 0x390F:
		case 0x3918:case 0x3919://指定贴图横纵坐标
		case 0x3998:case 0x3999:
		case 0x39C0:case 0x39C1:case 0x39C2:case 0x39C3:case 0x39C5:case 0x39C6:case 0x39C7:case 0x39C8:
		case 0x39D0:case 0x39D1:case 0x39D2:case 0x39D3:
		case 0x4000:case 0x4080:case 0x4100:case 0x4800:
		case 0x7020:case 0x7021:case 0x7022:case 0x7120:case 0x7121:case 0x7122:
		case 0x7700:case 0x7701:case 0x7702:case 0x7710:case 0x7711:case 0x7720:case 0x7721:
		case 0x8000:case 0x8010:case 0x8011:case 0x8012:case 0x8018:case 0x8019:
		case 0x811B:case 0x818F:
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
			{ //unknown
				infile.read((char *)&info, sizeof(info));
				sprintf(str, "%08X %02X " , info.option, info.key);
				outtxt<<str;
				u8 len = info.key;
				if(len == 0) len = 1;
				u8* buf = new u8[len];
				infile.read((char *)buf, len);
				for (u8 i=0;i<len;i++)
				{
					buf[i] ^= info.key;
					sprintf(str, "%02X " , buf[i]);
					outtxt<<str;
				}
				outtxt<<endl;
				break;
			}
			
		case 0x1010:
		case 0x1011:
			/*1010和1011在setting中为分辨率x,y 跟的是81 在scenario中作用未知 跟的是80*/
		case 0x3802://贴图 后面的数字是list中的顺序
		case 0x38FF://跳转
		case 0x3900:
		case 0x39A0://hover音效
		case 0x39A1://push音效
		case 0x7100:case 0x7101:case 0x7102:
		case 0x7800://播放视频 后面的数字是list中的顺序
		case 0x8200://显示图片 同上
		case 0x8201:
		case 0xA101:case 0xA201:
			{
				u8 unknown1;
				infile.read((char *)&unknown1, sizeof(unknown1));
				sprintf(str, "%02X " , unknown1);
				outtxt<<str;
				if(unknown1 == 0x81){
					infile.read((char *)&unknown1, sizeof(unknown1));
					sprintf(str, "%02X " , unknown1);
					outtxt<<str;
					infile.read((char *)&unknown1, sizeof(unknown1));
					sprintf(str, "%02X\n" , unknown1);
					outtxt<<str;
				}
				else if(unknown1 == 0x80){
					infile.read((char *)&unknown1, sizeof(unknown1));
					sprintf(str, "%02X\n" , unknown1);
					outtxt<<str;
				}
				break;
			}
			
			/*以下setting专用*/
		case 0xC010:case 0xC011:case 0xC012:case 0xC013://版本号
			{
				u8 buf[2];
				infile.read((char *)buf, 2);
				sprintf(str, "%02X %02X\n" , buf[0], buf[1]);//buf[0]=0x80;
				outtxt<<str;
				break;
			}
		default:{
		outtxt<<endl;
		break;
		}
		}
	}
	return 0;
}