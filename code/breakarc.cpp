#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <cstring>
#include <io.h>
#include <sys/stat.h>

using namespace std;

typedef unsigned char u8;
typedef unsigned int u32;


u8 key[] = {0xC9,0x5D,0x55,0x36,0x9C,0x6D,0x5E,0x55,0x23,0x9C,0x6E,0xA0,0xA1,0x24,0x12,0xA6};

void make_path(string filename){
	int i;
	for(i = filename.length()-1; i>=0; i--){
		if(filename[i] == '/')	break;
	}
	string pathname = filename.substr(0,i+1);
	
	if(pathname[pathname.length()-1] != '/')	pathname += '/' ;
	int end = pathname.rfind('/');
	int pt = pathname.find('/');
	const char * path;
	while(pt != -1 && pt<=end)
	{
		path = pathname.substr(0,pt+1).c_str();
		if(access(path, 0) == -1)
		mkdir(path);
		pt = pathname.find('/', pt+1);
	}
}

int main(int argc, char* argv[])
{
	if(argc!=2)
	{
		cerr<<"usage: "<<argv[0]<<" filelist.txt"<<endl;
		return 1;
	}

	ifstream infile("arc.fwd", ios::binary);
	if(!infile)
	{
		cerr<<"Can't find arc.fwd!"<<endl;
		return 1;
	}
	
	ifstream fnlist(argv[1]);
	if(!fnlist)
	{
		cerr<<"Can't open file list!"<<endl;
		return 1;
	}
	
	infile.seekg(0, ios::end);
	u32 streampos = infile.tellg();
	infile.seekg(-1,ios::end);//EOF
	
	//File Size
	infile.seekg(-4,ios::cur);
	u32 totalsize;
	infile.read((char *)&totalsize, 4);
	infile.seekg(-4,ios::cur);
	
	//count
	infile.seekg(-4,ios::cur);
	u32 count;
	infile.read((char *)&count, 4);
	infile.seekg(-4,ios::cur);
	
	if(totalsize!=(streampos-count*4-9))
	{
		cerr<<"File size error! The file is damaged!"<<endl;
		return 1;
	}
	
	u32* offset = new u32[count];
	u32* filesize = new u32[count];
	u32 i;
	//make offset table
	for(i=0;i<count;i++)
	{
		u32 current;
		infile.seekg(-4,ios::cur);
		infile.read((char *)&current, 4);
		infile.seekg(-4,ios::cur);
		offset[i] = current;
	}
	
	for(i=0;i<count-1;i++)
	{
		filesize[i] = offset[i+1] - offset[i];
	}
	filesize[i] = totalsize - offset[i];
	
	u32 j;
	for(i=0;i<count;i++)
	{
		j=i;
		while(filesize[j] == 0)
		{
			j++;
		}
		if(j!=i)
		{
			u32 temp = filesize[i];
			filesize[i] = filesize[j];
			filesize[j] = temp;
			i = j;
		}
	}
	
	u32 cnt = 0;
	for(i=0;i<count;i++)
	{
		string filename;
		getline(fnlist, filename);
		
		if(filename.substr(0,2) == "f:") continue;
		
		filename = "./" + filename;
		make_path(filename);
		ofstream outfile(filename.c_str(), ios::binary);
		
		u8 *buf = new u8[filesize[i]];
		infile.seekg(offset[i], ios::beg);
		infile.read((char *)buf, filesize[i]);
		
		for(u8 j=0;j<0x10;j++)
		{ buf[j] ^= key[j]; }
		
		outfile.write((const char*)buf, filesize[i]);
		delete []buf;
		outfile.close();
		
		printf("[+] %-32s offset[%08x] size[%08x]\n",
		filename.substr(2,filename.length()).c_str(),offset[i],filesize[i]);
		cnt++;
	}
	
  cout<<"\nTotal(actual): "<<cnt<<endl;
	
	infile.close();
	fnlist.close();
	return 0;
}