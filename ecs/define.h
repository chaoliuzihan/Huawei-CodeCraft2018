#ifndef DEFINE_H_
#define DEFINE_H_

#include <string>
#include <cstring>
#include <algorithm>
#include <ctime>
#include <cmath>
#include <vector>

/************************Train**********************/
struct Train //�����ѵ������
{
	Train()
	{
		id.clear();
		flavorName.clear();
		createaTime.clear();
	}
	std::string id;			 //�����id
	std::string flavorName;  //��������
	std::string createaTime; //����ʱ��
};
/************************Train**********************/

/************************Flavor*********************/
struct Flavor
{
	Flavor()
	{
		flavorName.clear();
		flavorCpu = 0;
		flavorMem = 0;
		nCount = 0;
		s1 = 0.000001;
		s2 = 0.000001;
		s3 = 0.000001;
		createTimeUnix = 0;
		flavorType = -1;
	}
	std::string flavorName; //��ҪԤ��������
	int flavorCpu;			//�����cpu��
	int flavorMem;			//������ڴ��С
	int nCount;				//���ִ���
	double s1;				//һ��ָ��ƽ��ֵ
	double s2;				//����ָ��ƽ��ֵ
	double s3;				//����ָ��ƽ��ֵ
	time_t createTimeUnix;  //����ʱ���UNIXʱ���
	int flavorType;//�������Ӧ������������
};

/************************Flavor*********************/

/************************Server*********************/
struct Server
{
	Server()
	{
		serverCpuCount = 0;
		serverMemCount = 0;
		serverDiskCount = 0;
	}

	int serverCpuCount;  // cpu����
	int serverMemCount;  // �ڴ�����
	int serverDiskCount; // Ӳ��
};
/************************Server*********************/

struct deployList_ServerTypes
{
	deployList_ServerTypes()
	{
		serverType.clear();
		deployServerList.clear();
	}
	std::string serverType;
	std::vector<std::vector<Flavor>> deployServerList;
};

#define OPTIMIZATION_CPU 0
#define OPTIMIZATION_MEM 1

#define MAXIMUM 999999

#define CPU 0
#define MEM 1

#define TEMPERATURE 0.99

#endif