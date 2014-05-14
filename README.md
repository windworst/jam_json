jam json
========

一个轻量级json解释器 (单个头文件,包含即可使用)

除遵循json标准以外,还支持存放内存块


接口

operator<<:			作为数组添加元素

	a<<"123"<<456<<true<<false;

operator[int]:		作为数组访问下标

	a[0].to_string()=="123"

operator[string]:	键值对操作

	a["key"] = "value";



to_json(): 将json对象转换为json字符串
	
	string str = a.to_json();

to_json(ostream&): 将json字符串输出到流

	a.to_json(cout);

from_json(string): 从json字符串来实例化json对象
	
	jam_json a = jam_json::from_json(json_str);

from_json(istream&): 从输入流实例化json对象
	
	a.from_json(cin);
