#include <iostream>
#include <vector>
#include <fstream>
#include <cstdlib>
#include <string>
#include <cstring>
#include <io.h>
#include <sys/stat.h>

using namespace std;

typedef unsigned char u8;
typedef unsigned int u32;

u8 newkey[] = {0xD1,0x0B,0x18,0x36,0x15,0x31,0x08,0x18,0x3F,0x15,0x32,0xA0,0xA1,0x24,0x12,0xA6};

typedef struct FWDHDR
{
	char magic[8];
	char ver[4];
	u32 code;
	u32 unknown1;
	u32 unknown2;
	u32 unknown3;
	u32 unknown4;
}FWDHDR;

int main(int argc, char* argv[])
{
	if(argc!=2)
	{
		cerr<<"usage: "<<argv[0]<<" filelist.txt"<<endl;
		return 1;
	}
	
	ifstream fnlist(argv[1]);
	if(!fnlist)
	{
		cerr<<"Can't open file list!"<<endl;
		return 1;
	}
	ofstream outfile("arc.fw_", ios::binary);
	
	//File Size
	u32 totalsize = 0;
	//count
	u32 count = 0;
	
	FWDHDR hdr;
	char m[] = "FmWrARCD";
	strcpy(hdr.magic, m);
	char v[] = "0163";
	strcpy(hdr.ver, v);
	
	hdr.code = 0;
	hdr.unknown1 = 1;
	hdr.unknown2 = 0;
	hdr.unknown3 = 0;
	hdr.unknown4 = 0;
	
	outfile.write((const char*)&hdr, sizeof(hdr));
	totalsize += sizeof(hdr);
	
	vector<u32> offset;

	string filename;
	while(getline(fnlist,filename))
	{
		if(filename.substr(0,2) == "f:")
		{
			offset.push_back(totalsize);
		}
		else
		{
			filename = "./" + filename;
			ifstream infile(filename.c_str(), ios::binary);
			if(!infile)
			{
				cerr<<"Error! Can't open file: "<<filename.substr(2,filename.length())<<"."<<endl;
				return 1;
			}
			
			struct _stat st;
			_stat(filename.c_str(), &st);
			u8 *buf = new u8[st.st_size];
			infile.read((char*)buf, st.st_size);
			
			for(u8 j=0;j<0x10;j++)
			{ buf[j] ^= newkey[j]; }
			
			outfile.write((const char*)buf, st.st_size);
			delete []buf;
			infile.close();
			
			printf("[-] %-32s offset[%08x] size[%08x]\n",
		  filename.substr(2,filename.length()).c_str(),totalsize,(u32)st.st_size);
			
			offset.push_back(totalsize);
			totalsize += st.st_size;
		}
		
		count++;
	}

	for(int i=offset.size()-1; i>=0; i--)
	{
		outfile.write((const char*)&offset[i], 4);
	}
	outfile.write((const char*)&count, 4);
	outfile.write((const char*)&totalsize, 4);
	u8 null = 0;
	outfile.write((const char*)&null, 1);

	cout<<"\nTotal: "<<count<<endl;
	outfile.close();
	fnlist.close();
	return 0;
}