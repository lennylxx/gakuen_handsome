#include <iostream>
#include <string>

using namespace std;

typedef unsigned char u8;
int main()
{
	string title, author;
	title = "学园handsome";
  author = "チーム欲求腐満";
	u8 first,second,third,forth;
	
	first = second = third = forth = 0;
	for(u8 i=0; i<author.length(); i++)
	{
		first += author[i]*i;
		second += author[i]*(author.length()-1-i);
	}
	
	for(u8 i=0; i<title.length(); i++)
	{
		third += title[i]*i;
		forth += title[i]*(title.length()-1-i);
	}
	
	cout<<hex<<(int)first<<","<<(int)second<<","<<(int)third<<","<<(int)forth<<endl;
}