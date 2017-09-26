/*

������Ϊ�����ֺ��ַ����໥ת���Ĺ��ߡ�

stringstream ss(str);
�����洢str�ĸ����� stringstream ��������str�� string ���͵Ķ���

�ַ�������
1.istringstream����istream�����������ṩ ��string�Ĺ��ܡ�
2.ostringstream����ostream�����������ṩ дstring�Ĺ��ܡ�
3.stringstream����iostream�����������ṩ ��дstring�Ĺ��ܡ�

���������ͷ�ļ� <iostream> 
string����ͷ�ļ� <sstream> 
�ļ�����ͷ�ļ�   <fstream>
*/
#include <iostream>
#include <string>
#include <strstream>    //��stringstream   strstream�������ˣ���׼�������
#include <sstream> 
using namespace std;

int main()
{
	int n = 65535;
	strstream ss;
	string s;
	ss << n;// ��int��������
	ss >> s;// ��ss_stream�е���ֵ�����str��
	cout << s << endl;

	stringstream ss_stream;
	string str;
	int i = 10;
	ss_stream << i;   // ��int��������
	ss_stream >> str; // ��ss_stream�е���ֵ�����str��

	//ע�⣺������������ת�����������clear()������ת��ģʽ
	ss_stream << "456"; 
	ss_stream >> i;   // ���Ƚ��ַ���ת��Ϊint
	cout<<i<<endl;
	ss_stream.clear();
	ss_stream << true;
	ss_stream >> i;   // Ȼ��bool��ת��Ϊint������֮ǰû����clear,��ôi�����
	cout<<i<<endl;


	return 0;
}

/*

��C++�������ַ�������һ����sstream�ж��壬��һ����strstream�ж��塣����ʵ�ֵĶ�������һ����

strstream�����
class strstreambuf;
class istrstream;
class ostrstream;
class strstream;
�����ǻ���C�����ַ���char*��д��

sstream�а���
class istringstream;
class ostringstream;
class stringbuf;
class stringstream;
class ����.
�����ǻ���std::string��д��

���ostrstream::str()���ص���char*���͵��ַ���
��ostringstream::str()���ص���std::string���͵��ַ���

��ʹ�õ�ʱ��Ҫע�⵽���ߵ�����һ��������Ƽ�ʹ��std::string���͵��ַ���
��Ȼ���Ϊ�˱��ֺ�C�ļ��ݣ�ʹ��strstreamҲ�ǲ����ѡ��

*/


/*
*	1.�����������������ת��


	2.֧��char*����������

		char sz_buf[20];
		ss_stream << 8888;
		ss_stream >> sz_buf; // ֱ�ӽ��������sz_buf�ַ�������

	3.���洢�ɱ����ݵ��б�

		stringstream ss_stream;
		ss_stream << "�ַ���һ" << endl;
		ss_stream << "�ַ�����" << endl;
		ss_stream << "�ַ�����" << endl;
		ss_stream << "�ַ�����" << endl;
		ss_stream << "�ַ�����" << endl;

		char buffer[100];
		while ( ss_stream.getline(buffer, sizeof(buffer))
		{
		printf("msg=%s\n", buffer);
		}
		ss_stream("");// �ͷ��ַ������е���Դ

		// ������string������
		stringstream ss_stream;
		string stemp;
		while ( getline(ss_stream, stemp) )
		{
		task_download(stemp.c_str(), relate.c_str());
		}


*/