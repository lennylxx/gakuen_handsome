
/*请用Visual C++编译*/
#include <iostream>
#include <fstream.h>
#include "fw_util.h"

int main()
{
	ifstream intxt("ins_length_list.txt");
	ofstream outfile("ins_offset_table.new", ios::binary);
	
	DWORD obscure,len;
	DWORD offset=0;
	DWORD i=0;
	while(1)
	{
		intxt>>len;
		if(intxt.eof()) break;
		obscure = offset ^ GetEncryptIndex(GetStr(i));
		outfile.write((const char*)&obscure, 4);
		i += 4;
		offset += len;
	}
	
	return 0;
}
