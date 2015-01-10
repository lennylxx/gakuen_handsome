#include <fstream>

using namespace std;

typedef unsigned char      u8;
typedef unsigned short     u16;
typedef unsigned int       u32;
typedef unsigned long long u64;

int main(int argc, char *argv[]) 
{
	if(argc!=2){
		return 1;
	}
	string outfilename = string(argv[1]) + ".out";
	ifstream infile(argv[1], ios::binary);
	ofstream outfile(outfilename.c_str(), ios::binary);
	
	u32 amount;
	u16 len;
	u8 *buf;
	
	infile.read((char *)&amount, sizeof(amount));
	for (u32 i=0;i<amount;i++)
	{
		infile.read((char *)&len, sizeof(len));
		buf = new u8[len];
		infile.read((char *)buf, len);
		for (u16 j=0;j<len;j++){
			buf[j] ^= len;
			outfile<<buf[j];
		}
		outfile<<endl;
	}
	
	infile.close();
	return 0;
}