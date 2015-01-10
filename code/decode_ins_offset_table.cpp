
/*请用Visual C++编译*/
#include <iostream>
#include <fstream.h>
#include "fw_util.h"

int main()
{
	
	ifstream infile("ins_offset_table.bin", ios::binary);
	ofstream outtxt("ins_length_list.txt");
	
	DWORD obscure,len;
	DWORD offset=0;
	DWORD last=0;
	DWORD i=0;
	while(1)
	{
		infile.read((char *)&obscure, 4);
		if(infile.eof()) break;
		offset = obscure ^ GetEncryptIndex(GetStr(i));
		len = offset - last;
		outtxt<<len<<endl;;
		last = offset;
		i += 4;
	}
	
	return 0;
}
