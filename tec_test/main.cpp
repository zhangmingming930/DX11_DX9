/*

可以作为将数字和字符串相互转化的工具。

stringstream ss(str);
创建存储str的副本的 stringstream 对象，其中str是 string 类型的对象

字符串流：
1.istringstream，由istream派生而来，提供 读string的功能。
2.ostringstream，由ostream派生而来，提供 写string的功能。
3.stringstream，由iostream派生而来，提供 读写string的功能。

输入输出的头文件 <iostream> 
string流的头文件 <sstream> 
文件流的头文件   <fstream>
*/
#include <iostream>
#include <string>
#include <strstream>    //用stringstream   strstream是老类了，标准建议别用
#include <sstream> 
using namespace std;

int main()
{
	int n = 65535;
	strstream ss;
	string s;
	ss << n;// 将int输入流中
	ss >> s;// 将ss_stream中的数值输出到str中
	cout << s << endl;

	stringstream ss_stream;
	string str;
	int i = 10;
	ss_stream << i;   // 将int输入流中
	ss_stream >> str; // 将ss_stream中的数值输出到str中

	//注意：如果做多次数据转换；必须调用clear()来设置转换模式
	ss_stream << "456"; 
	ss_stream >> i;   // 首先将字符串转换为int
	cout<<i<<endl;
	ss_stream.clear();
	ss_stream << true;
	ss_stream >> i;   // 然后将bool型转换为int；假如之前没有做clear,那么i会出错
	cout<<i<<endl;


	return 0;
}

/*

在C++有两种字符串流，一种在sstream中定义，另一种在strstream中定义。它们实现的东西基本一样。

strstream里包含
class strstreambuf;
class istrstream;
class ostrstream;
class strstream;
它们是基于C类型字符串char*编写的

sstream中包含
class istringstream;
class ostringstream;
class stringbuf;
class stringstream;
class …….
它们是基于std::string编写的

因此ostrstream::str()返回的是char*类型的字符串
而ostringstream::str()返回的是std::string类型的字符串

在使用的时候要注意到二者的区别，一般情况下推荐使用std::string类型的字符串
当然如果为了保持和C的兼容，使用strstream也是不错的选择

*/


/*
*	1.利用输入输出做数据转换


	2.支持char*的输入和输出

		char sz_buf[20];
		ss_stream << 8888;
		ss_stream >> sz_buf; // 直接将数输出到sz_buf字符数组中

	3.来存储可变数据的列表

		stringstream ss_stream;
		ss_stream << "字符串一" << endl;
		ss_stream << "字符串二" << endl;
		ss_stream << "字符串三" << endl;
		ss_stream << "字符串四" << endl;
		ss_stream << "字符串五" << endl;

		char buffer[100];
		while ( ss_stream.getline(buffer, sizeof(buffer))
		{
		printf("msg=%s\n", buffer);
		}
		ss_stream("");// 释放字符串流中的资源

		// 或者用string来接收
		stringstream ss_stream;
		string stemp;
		while ( getline(ss_stream, stemp) )
		{
		task_download(stemp.c_str(), relate.c_str());
		}


*/