#include <fstream>

using namespace std;

typedef unsigned char      u8;
typedef unsigned short     u16;
typedef unsigned int       u32;
typedef unsigned long long u64;

int main() 
{

	ifstream infile("4.txt_list.bin", ios::binary);
	ofstream outfile("txt_list.out", ios::binary);
	
	u32 amount;
	u16 len;
	u8 *buf;
	
	infile.read((char *)&amount, sizeof(amount));
	for (u32 i=0;i<amount;i++)
	{
		infile.read((char *)&len, sizeof(len));
		
		buf = new u8[len];
		infile.read((char *)buf, len);
		
		outfile<<"OFFSET: ";
		for (int j=3;j>=0;j--){
			buf[j] ^= len;
			char str[8];
			sprintf(str,"%02X ",buf[j]);
			outfile<<str;
		}
		for (u16 j=4;j<len;j++){
			buf[j] ^= len;
			outfile<<buf[j];
		}
		outfile<<endl;
	}
	
	infile.close();
	return 0;
}