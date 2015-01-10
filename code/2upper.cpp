#include <iostream>
#include <cstring>
using namespace std;

typedef unsigned char u8;
typedef unsigned int u32;
int main()
{
	char str1[] = "setting.txt@";
	char str2[] = "scenario.txt@";
	
	u32 result,t;
	result = t = 0;
	for(int i=0;i<strlen(str1);i++)
	{
		if(str1[i]>='a' && str1[i]<='z')
			str1[i] -= 0x20;
		result = (t<<5) - t + str1[i];
		t = result;
	}
	cout<<hex<<result<<endl;
	result = t = 0;
	for(int i=0;i<strlen(str2);i++)
	{
		if(str2[i]>='a' && str2[i]<='z')
			str2[i] -= 0x20;
		result = (t<<5) - t + str2[i];
		t = result;
	}
	cout<<hex<<result<<endl;
	return 0;
}