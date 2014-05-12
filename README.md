jam_json
========

一个简单的json解释器

除遵循json标准以外,还支持存放内存块  (使用 vector<char>存放)

单个头文件,包含即可使用;

a添加键值对: a["key"] = "value", a["number"]=123.456, a["block"] = vector<char>(100,'x');
a添加数组成员: a<<b<<c<<"string"<<json_number(123456);

取出值: (json_number)a["number"], (const char*)a["string"],(vector<char>)a["memory"];

序列化: string s = a.serialization();
反序列化 jam_json out = jam_json::unserialization(s);

