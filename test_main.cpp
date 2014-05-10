#include <iostream>
using namespace std;
#include "jam_json.h"

int main()
{
	jam_json jj;
	jam_json jjj;
	jj.add("key1","value1" );
	jj.add("key2",(1.0) );
	jj.add("key3",(true) );
	jj.add("key4",(false) );
	jjj.add(jj);
	jjj.add(jj);
	jjj.add(jj);
	jj.add("array",jjj);
	cout<<jj.serialization();
	return 0;
}
