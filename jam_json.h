#pragma once
#include <string.h>
#include <string>
#include <map>
#include <vector>
#include <iomanip>
#include <sstream>
using namespace std;

#ifndef json_number
#define json_number double
#endif

#define STRING_TRUE "true"
#define STRING_FALSE "false"
#define STRING_NULL "null"

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
		template <typename T>
			jam_json(const T& o)
			{
				this->set_value(o);
			}

		~jam_json()
		{
		}

	public:
		//operator
		template <typename T>
			jam_json& operator=(const T& o)
			{
				this->set_value(o);
				return *this;
			}

		template<typename T>
			jam_json& operator[](const T& key)
			{
				if(this->j_type != JSON_OBJECT)
				{
					this->clear();
					this->j_type = JSON_OBJECT;
				}
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

		operator bool()
		{
			return this->to_bool();
		}

		operator const char*()
		{
			return this->to_str();
		}

		operator json_number()
		{
			return this->to_number();
		}

		operator vector<char>()
		{
			return this->j_data;
		}

	public:
		const char* to_str() const
		{
			if(this->j_type != JSON_STRING)return NULL;
			return this->j_data.data();
		}
		json_number to_number() const
		{
			if(this->j_type != JSON_NUMBER)return 0;
			return *(json_number*)this->j_data.data();
		}
		bool to_bool() const
		{
			return this->j_type==JSON_TRUE;
		}
		const vector<char>& data() const
		{
			return this->j_data;
		}
		string	to_string()
		{
			if(this->j_type != JSON_STRING)return string();
			return string(this->j_data.data(),this->j_data.size()-1);
		}

		//set value: string/null
		void set_value(const void* str,int len = 0)
		{
			this->clear();
			if(str==NULL)
			{
				return;
			}

			this->j_type = JSON_STRING;
			len ==0 ? len = strlen((const char*)str):0;
			this->j_data.resize(len+1);
			memcpy(this->j_data.data(),str,len);
			this->j_data[len] = '\0';
		}
		//set value: string
		void set_value(const string& str)
		{
			this->set_value(str.data(),str.size());
		}
		//set value: number
		void set_value(json_number number)
		{
			this->clear();
			this->j_type = JSON_NUMBER;
			this->j_data.resize(sizeof(json_number));
			(*(json_number*)this->j_data.data()) = number;
		}

		//set value: j_data
		void set_value(const vector<char>& j_data)
		{
			this->clear();
			this->j_type = JSON_BYTES;
			this->j_data = j_data;
		}

		//set value: object
		void set_value(const jam_json& o)
		{
			if(this==&o)
			{
				return;
			}

			this->j_type = o.j_type;
			this->j_data = o.j_data;
			this->j_array = o.j_array;
			this->key_value = o.key_value;
		}

		//set value: 
		void set_value(bool bool_value)
		{
			this->clear();
			this->j_type = bool_value ? JSON_TRUE : JSON_FALSE;
		}

		//add object to itself
		jam_json& add(const jam_json& o)
		{
			if(this->j_type != JSON_ARRAY && this->j_type!= JSON_NULL)
			{
				jam_json tmp(*this);
				this->clear();
				this->j_array.push_back(tmp);
			}
			this->j_type = JSON_ARRAY;
			this->j_array.push_back(o);
			return *this;
		}

		//add key-value item
		template <typename T>
			jam_json& add(const string& key,const T& o)
			{
				if(this->j_type != JSON_OBJECT)
				{
					this->clear();
					this->j_type = JSON_OBJECT;
				}
				map<string,jam_json>::iterator it = this->key_value.find(key);
				if(it!=this->key_value.end())
				{
					it->second.add(o);
				}
				else
				{
					this->key_value[key].set_value(o);
				}
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
		string to_json()
		{
			stringstream ss;
			this->to_json(ss);
			return ss.str();
		}

		//unserialization
		static jam_json from_json(const char* str,int len = 0 )
		{
			stringstream ss;
			jam_json jj;
			if(str!=NULL)
			{
				ss.write(str,len==0?strlen(str):len);
				jj.from_json(ss);
			}
			return jj;
		}

		static jam_json from_json(const string& str)
		{
			return from_json(str.data(),str.size());
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
					unsigned short num = *(unsigned short*)(str+i);
					os<<"\\u"<<setw(4)<<setfill('0')
						<<setiosflags(ios::right)<<hex 
						<<num;
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
						case  127:
						default:
								   os<<"\\x"<<setw(2)<<setfill('0')
									   <<setiosflags(ios::right)<<hex 
									   <<(unsigned short)str[i];
								   break;
					}
				}
			}
			return os;
		}
		//json-string to string
		istream& json_unescape(istream &is)
		{
			this->clear();
			int c = is.get();
			if(c=='"')
			{
				c = is.get();
			}
			bool escape_flag = false;
			while(c!=-1)
			{
				if(escape_flag)
				{
					switch(c)
					{
						case '"':	
							this->j_data.push_back('"');break;
						case '/': 	
							this->j_data.push_back('/');break;
						case '\\': 	
							this->j_data.push_back('\\');break;
						case 'b': 	
							this->j_data.push_back('\b');break;
						case 't':	
							this->j_data.push_back('\t');break;
						case 'r':	
							this->j_data.push_back('\r');break;
						case 'n':	
							this->j_data.push_back('\n');break;
						case 'f':	
							this->j_data.push_back('\f');break;
						case 'x':
							{
								unsigned short num;
								char num_str[3]={0};
								stringstream ss;
								if(is.read(num_str,2) && ss<<num_str && ss>>num)
								{
									this->j_data.push_back((unsigned char)num);
								}
							}
							break;
						case 'u':
							{
								unsigned short num;
								char num_str[5]={0};
								stringstream ss;
								if(is.read(num_str,4) && ss<<num_str && ss>>num)
								{
									this->j_data.push_back( ((unsigned char*)&num)[0] );
									this->j_data.push_back( ((unsigned char*)&num)[1] );
								}
							}
							break;
						default:	this->j_data.push_back(c);break;
					}
					escape_flag = false;
				}
				else
				{
					if(c=='"')
					{
						break;
					}
					else if(c=='\\')
					{
						escape_flag = true;
					}
					else
					{
						this->j_data.push_back(c);
					}
				}
				c = is.get();
			}
			if(is)
			{
				this->j_data.push_back('\0');
				this->j_type = JSON_STRING;
			}
			return is;
		}


		ostream& to_json(ostream &os)
		{
			switch(this->j_type)
			{
				case JSON_NULL: 
					os<< "null";
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
							it->second.to_json(os);
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

							it->to_json(os);
							++it;
						}
						os<<"]";
					}
					break;
				case JSON_BYTES:
					{
						os<<"&"<<this->j_data.size()<<":";
						os.write(this->j_data.data(),this->j_data.size());
					}
					break;
				default:break;
			}
			return os;
		}

		istream& from_json(istream& is)
		{
			this->clear();
			//get no-space
			int c = jump_space(is);
			if(c==-1) 
			{
				return is;
			}

			if(c=='"') //string
			{
				this->json_unescape(is);
			}
			else if(c=='t') //true
			{
				this->set_value(true);
				char out[sizeof(STRING_TRUE)]={0};
				is.read(out,sizeof(STRING_TRUE)-1);
			}
			else if(c=='f') //false
			{
				this->set_value(false);
				char out[sizeof(STRING_FALSE)]={0};
				is.read(out,sizeof(STRING_FALSE)-1);
			}
			else if(c=='n') //null
			{
				//this->clear();
				char out[sizeof(STRING_NULL)]={0};
				is.read(out,sizeof(STRING_NULL)-1);
			}
			else if(c=='{') //object
			{
				c = is.get();
				while(true)
				{
					string key;
					c = jump_space(is);
					if(c == -1 || c == '}')
					{
						c = is.get(); // '}'
						break;
					}
					c = is.get(); // '"'
					getline(is,key,(char)c);
					c = jump_space(is);
					c = is.get(); // ':'

					jam_json value;
					if(value.from_json(is))
					{
						this->add(key,value);
					}
					else
					{
						break;
					}
				}
			}
			else if(c=='[') //array
			{
				c = is.get();
				while(true)
				{
					c = jump_space(is);
					if(c == -1 || c == ']')
					{
						c = is.get(); // ']'
						break;
					}

					jam_json value;
					if(value.from_json(is))
					{
						this->add(value);
					}
					else
					{
						break;
					}
				}

			}
			else if(c=='&') //byte
			{
				c = is.get(); //'&'
				int len = 0;
				if(is>>len)
				{
					this->j_data.resize(len);
					c = is.get(); //':'
					is.read(this->j_data.data(),len);
					this->j_type = is ? JSON_BYTES : JSON_NULL;
				}
			}
			else
			{
				json_number jn;
				if(is>>jn)
				{
					this->set_value(jn);
				}
			}

			return is;
		}
	protected:
		int jump_space(istream& is)
		{
			int c = -1;
			while(c=is.peek(), c!=-1 && (c<=' '||c>=127||c==',') )
			{
				c = is.get();
			}
			return c;
		}
	private:
		map<string,jam_json> key_value;
		vector <char> j_data;
		vector <jam_json> j_array; 
		int j_type;
};


