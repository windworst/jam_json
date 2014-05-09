#include <string>
#include <map>
#include <pair>
using std::map;
using std::pair;
using std::string;

class jam_json
{
public:
	explicit jam_json(const char* json_str = NULL){}
	explicit jam_json(const char* buffer,int len ){}
	explicit jam_json(double value){}
private:
	double value_num;
	string value_str;
	int type;
}
