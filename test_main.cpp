#include <iostream>
using namespace std;
#include <stdlib.h>
#include "jam_json.h"

int main()
{
	jam_json aa;
	aa["string"] = "I'm a string";
	aa["number"] = json_number(-123.456e-3);
	aa["true"]	 = true;
	aa["false"]	 = false;
	aa["null"]   = jam_json();
	aa["data"]   = vector<char>(20,'x');

	string jstr_aa = aa.serialization();

	cout <<"aa json string: "<< jstr_aa <<endl<<endl;

	cout <<"aa[\"number\"] :"<<(json_number)aa["number"]<<endl<<endl;

	jam_json array;

	array<<aa<<aa;

	string jstr_array = array.serialization();

	cout <<"array json string: "<<jstr_array <<endl<<endl;

	array<<array<<array;

	jstr_array = array.serialization();

	cout <<"array-array json string: "<<jstr_array<<endl<<endl;

	jam_json unseria = jam_json::unserialization(jstr_array);

	cout<<"unserialization json-str: "<<unseria.serialization()<<endl<<endl;

	jam_json jjs = jam_json::unserialization("\"月上柳梢头，人约黄昏后\"");

	cout<<jjs.serialization()<<endl;
	cout<<jjs.to_str()<<endl;

	return 0;
}
