#pragma once
#include <string.h>
#include <string>
#include <map>
#include <vector>
#include <iomanip>
#include <sstream>
using namespace std;

typedef double json_number;

class jam_json
{
public:
	enum TYPE
	{
		JSON_NULL = 0,
		JSON_TRUE,
		JSON_FALSE,
		JSON_ARRAY,
		JSON_OBJECT,
		JSON_NUMBER,
		JSON_STRING,
		JSON_BYTES,
	};
public:
	//construct and destruct
	jam_json(const char* str = NULL,int len = 0)
	{
		this->set_value(str,len);
	}
	jam_json(const vector<char>& j_data)
	{
		this->set_value(j_data);
	}
	jam_json(const jam_json& o)
	{
		this->set_value(o);
	}
	jam_json(json_number number)
	{
		this->set_value(number);
	}
	jam_json(bool bool_value)
	{
		this->set_value(bool_value);
	}
	
	~jam_json()
	{
	}

public:
	//operator
	jam_json& operator=(const jam_json& o)
	{
		this->set_value(o);
		return *this;
	}

	jam_json& operator[](const char* key)
	{
		this->j_type = JSON_OBJECT;
		return this->key_value[key];
	}

	jam_json& operator[](int index)
	{
		return this->j_array.at(index);
	}

	jam_json& operator<<(const jam_json& o)
	{
		return this->add(o);
	}

public:
	const char* to_str()
	{
		if(this->j_type != JSON_STRING)return NULL;
		return this->j_data.data();
	}
	json_number to_number()
	{
		if(this->j_type != JSON_NUMBER)return 0;
		return *(json_number*)this->j_data.data();
	}
	bool to_bool()
	{
		return this->j_type==JSON_TRUE;
	}
	const vector<char>& data()
	{
		return this->j_data;
	}
	//set value: string/null
	void set_value(const char* str,int len = 0)
	{
		if(str==NULL)
		{
			this->j_type = JSON_NULL;
			return;
		}

		this->j_type = JSON_STRING;
		len ==0 ? len = strlen(str):0;
		this->j_data.resize(len+1);
		memcpy(this->j_data.data(),str,len);
		this->j_data[len] = '\0';
	}

	//set value: number
	void set_value(json_number number)
	{
		this->j_type = JSON_NUMBER;
		this->j_data.resize(sizeof(json_number));
		(*(json_number*)this->j_data.data()) = number;
	}

	//set value: j_data
	void set_value(const vector<char>& j_data)
	{
		this->j_type = JSON_BYTES;
		this->j_data = j_data;
	}

	//set value: object
	void set_value(const jam_json& o)
	{
		if(this==&o)return;

		this->j_type = o.j_type;
		this->j_data = o.j_data;
		this->j_array = o.j_array;
		this->key_value = o.key_value;
	}

	//set value: 
	void set_value(bool bool_value)
	{
		this->j_type = bool_value ? JSON_TRUE : JSON_FALSE;
	}

	//add object to itself
	jam_json& add(const jam_json& o)
	{
		this->j_type = JSON_ARRAY;
		this->j_array.push_back(o);
		return *this;
	}

	//add key-value item
	jam_json& add(const char* key,const jam_json& o)
	{
		this->j_type = JSON_OBJECT;
		this->key_value[key] = o;
		return *this;
	}
	//map
	const map<string,jam_json>& keyvalue()
	{
		return this->key_value;
	}
	//j_array size
	int size()
	{
		if(this->j_type == JSON_ARRAY)
		{
			return this->j_array.size();
		}
		if(this->j_type == JSON_OBJECT)
		{
			return this->key_value.size();
		}
		return 0;
	}

	//clean
	void clear()
	{
		this->key_value.clear();
		this->j_data.clear();
		this->j_array.clear();
		this->j_type = JSON_NULL;
	}

	//type
	int type()
	{
		return this->j_type;
	}

	//serialization
	string serialization()
	{
		stringstream ss;
		this->serialization(ss);
		return ss.str();
	}

	//unserialization
	static jam_json unserialization(const char* str,int len = 0 )
	{
		stringstream ss;
		jam_json jj;
		if(str!=NULL)
		{
			ss.write(str,len==0?strlen(str):len);
			jj.unserialization(ss);
		}
		return jj;
	}

	//string to json-string
	ostream& json_escape(ostream &os,const char* str,int len)
	{
		int i;
		for(i=0;i<len;++i)
		{
			switch(str[i])
			{
				case '\"': os<<"\\\"";continue;
				case '\\': os<<"\\\\";continue;
				case '/' : os<<"\\/";continue;
			}
			if(' '<= str[i] && str[i]<='~')
			{
				os<<str[i];
			}
			else if( ((unsigned char)str[i])>=0X80)
			{
				os<<"\\u"<<setw(4)<<setfill('0')
					<<setiosflags(ios::right)<<hex 
					<<*(short*)(str+i);
				++i;
			}
			else
			{
				switch(str[i])
				{
					case '\t': os<<"\\t";break;
					case '\b': os<<"\\b";break;
					case '\f': os<<"\\f";break;
					case '\n': os<<"\\n";break;
					case '\r': os<<"\\r";break;
					default:
							   os<<"\\u"<<setw(2)<<setfill('0')
								   <<setiosflags(ios::right)<<hex 
								   <<(int)str[i];
							   break;
				}
			}
		}
		return os;
	}

	ostream& serialization(ostream &os)
	{
		switch(this->j_type)
		{
			case JSON_NULL: 
				os<< "NULL";
				break;

			case JSON_TRUE: 
				os<< "true";
				break;

			case JSON_FALSE: 
				os<< "false";
				break;

			case JSON_NUMBER:
				os<<(*(json_number*)this->j_data.data());
				break;

			case JSON_STRING:
				os<<"\"";
				this->json_escape(os,this->j_data.data(),this->j_data.size()-1);
				os<<"\"";
				break;

			case JSON_OBJECT:
				{
					map<string,jam_json>::iterator it =this->key_value.begin();
					os<<"{";
					while(it!=this->key_value.end())
					{
						if(it!=this->key_value.begin())
						{
							os<<",";
						}

						os<<"\""<<it->first<<"\":";
						it->second.serialization(os);
						++it;
					}
					os<<"}";
				}
				break;
			case JSON_ARRAY:
				{
					vector <jam_json>::iterator it = this->j_array.begin();
					os<<"[";
					while(it!=this->j_array.end())
					{
						if(it!=this->j_array.begin())
						{
							os<<",";
						}

						it->serialization(os);
						++it;
					}
					os<<"]";
				}
				break;
			case JSON_BYTES:
				{
					os<<"b"<<this->j_data.size()<<"\"";
					os.write(this->j_data.data(),this->j_data.size());
					os<<"\"";
				}
				break;
			default:break;
		}
		return os;
	}

	istream& unserialization(istream& is)
	{
		return is;
	}

private:
	map<string,jam_json> key_value;
	vector <char> j_data;
	vector <jam_json> j_array; 
	int j_type;
};
