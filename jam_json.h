#include <string.h>
#include <string>
#include <map>
#include <pair>
#include <vector>
#include <stringstream>
using std::vector;
using std::map;
using std::pair;
using std::string;

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
	jam_json(const char* str = NULL,int len = 0)
	{
		this->set_value(str,len);
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
	
	jam_json& operator=(const jam_json& o)
	{
		this->set_value(o);
		return *this;
	}

	~jam_json()
	{
	}

public:
	//set value: string/null
	void set_value(const char* str,int len = 0)
	{
		if(str==NULL)
		{
			this->type = JSON_NULL;
			return;
		}

		this->type = JSON_STRING;
		len ==0 ? len = strlen(str)+1:0;
		this->data.resize(len);
		memcpy(this->data.data(),str,len);
	}

	//set value: number
	void set_value(json_number number)
	{
		this->type = JSON_NUMBER;
		this->data.resize(sizeof(json_number));
		(*(json_number*)this->data.data()) = number;
	}

	//set value: object
	void set_value(const jam_json& o)
	{
		*this = o;
	}

	//set value: 
	void set_value(bool bool_value)
	{
		this->type = bool_value ? JSON_TRUE : JSON_FALSE;
	}

	//add object to itself
	jam_json& add(const jam_json& o)
	{
		this->type = JSON_ARRAY;
		this->array.push_back(o);
		return *this;
	}

	//add item
	jam_json& add(cosnt char* key,const jam_json& o)
	{
		this->type = JSON_OBJECT;
		this->key_value[key] = o;
		return *this;
	}

	//clean
	void clear()
	{
		this->key_value.clear();
		this->data.clear();
		this->array.clear();
		this->type = JSON_NULL;
	}

	stringstream& json_escape(stringstream &ss,const char* str,int len)
	{
		int i;
		for(i=0;i<len;++i)
		{
			switch(str[i])
			{
				case '\"': ss<<"\\\"";continue;
				case '\\': ss<<"\\\\";continue;
				case '/' : ss<<"\\/";continue;
			}
			if(' '<= str[i] && str[i]<='~')
			{
				ss<<str[i];
			}
			else if( ((unsigned char)str[i])>=0X80)
			{
				ss<<"\\u"<<setw(4)<<setfill('0')
					<<setiosflags(ios::right)<<hex 
					<<*(short*)(str+i);
				++i;
			}
			else
			{
				switch(str[i])
				{
					case '\t': ss<<"\\t";break;
					case '\b': ss<<"\\b";break;
					case '\f': ss<<"\\f";break;
					case '\n': ss<<"\\n";break;
					case '\r': ss<<"\\r";break;
					default:
							   ss<<"\\u"<<setw(2)<<setfill('0')
								   <<setiosflags(ios::right)<<hex 
								   <<(int)str[i];
							   break;
				}
			}
		}
		return ss;
	}

	stringstream& serialization(stringstream &ss)
	{
		switch(this->type)
		{
			case JSON_NULL: 
				ss<< "NULL";
				break;

			case JSON_TRUE: 
				ss<< "true";
				break;

			case JSON_FALSE: 
				ss<< "false";
				break;

			case JSON_NUMBER:
				ss<<(*(json_number*)this->data.data());
				break;

			case JSON_STRING:
				this->json_escape(ss,this->data.data(),this->data.size());
				break;

			case JSON_OBJECT:
				{
					map<string,jam_json>::iterator it =this->key_value.begin();
					ss<<"{";
					while(it!=this->key_value.end())
					{
						if(it!=this->key_value.begin())
						{
							ss<<",";
						}

						ss<<"\""<<it->first<<"\":";
						it->second->serialization(ss);
						++it;
					}
					ss<<"}"
				}
				break;
			case JSON_ARRAY:
				{
					vector <jam_json>::iterator it = this->array.begin();
					ss<<"[";
					while(it!=this->array.end())
					{
						if(it!=this->array.begin())
						{
							ss<<",";
						}

						it->second->serialization(ss);
						++it;
					}
					ss<<"]";
				}
				break;
		}
		return ss;
	}

private:
	map<string,jam_json> key_value;
	vector <char> data;
	vector <jam_json> array; 
	int type;
}
