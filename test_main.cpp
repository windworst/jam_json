#include <iostream>
using namespace std;
#include <stdlib.h>

#define  json_number int
#include "jam_json.h"

int main()
{
	jam_json aa;

	//key-value
	aa["string"] = "I'm a string";
	aa["number"] = json_number(12);
	aa["true"]	 = true;
	aa["false"]	 = false;
	aa["null"]   = jam_json();
	aa["data"]   = vector<char>(20,'x');

	string jstr_aa = aa.to_json();

	cout <<"aa json string: "<< jstr_aa <<endl<<endl;

	cout <<"aa[\"number\"] :"<<(json_number)aa["number"]<<endl<<endl;

	//array
	jam_json array;

	array<<aa<<aa;

	//json-convert
	string jstr_array = array.to_json();

	cout <<"array json string: "<<jstr_array <<endl<<endl;

	array<<array<<array;

	jstr_array = array.to_json();

	cout <<"array--array json string: "<<jstr_array<<endl<<endl;

	jam_json unseria = jam_json::from_json(jstr_array);

	cout<<"unserialization json-str: "<<unseria.to_json()<<endl<<endl;

	//unicode
	jam_json jjs = jam_json::from_json("\"月上柳梢头，人约黄昏后\"");

	cout<<jjs.to_json()<<endl;
	cout<<jjs.to_str()<<endl;

	return 0;
}
