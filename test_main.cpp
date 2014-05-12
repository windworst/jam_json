#include <iostream>
using namespace std;
#include "jam_json.h"

int main()
{
	jam_json jj;
	jam_json jjj;
	vector<char> c(20,'2');
	jj["key1"]="value1";
	jj["key2"]=1.0;
	jj["key3"]=true;
	jj["key4"]=false;
	jj["key5"]=c;
	jjj<<jj<<jj<<jj;
	jj["array"]=jjj;
	string str = jj.serialization();
	cout<<str<<endl;

	jam_json rj = jam_json::unserialization(str);
	cout<<rj.type()<<endl;
	rj.serialization(cout)<<endl;

	return 0;
}
