#ifndef __FUNCTION_H__
#define __FUNCTION_H__
#include <vector> 
#include <string>
using namespace std;

/*****************************Train*******************************************/
struct Train  //�����ѵ������
{
	const char *uuid;  //�����id
	const char *flavorName; //��������
	const char *createatime;  //����ʱ��
	int month;
	int day;
	int relative_day;  //�������
	int relative_week; //�������
};

typedef Train *train;
/*****************************input*************************************/
struct Input
{
	const char *flavorname;  //��ҪԤ��������
	int flavorcpu;  //�����cpu��
	int flavormemo;  //������ڴ��С
	int flavornum=0;   //ͳ�Ƹ��������ѵ����������
	int flavor_pred = 0;  //Ԥ���������
};
typedef Input *input;

struct Time
{
    int T_start_day; //ѵ����ʼʱ��
	int T_start_month;   
    int T_end_day;  //ѵ������ʱ��
    int T_end_month;    

	int In_start_day; //���뿪ʼʱ��
	int In_start_month;   
	int In_end_day;  //�������ʱ��
	int In_end_month;    
};
typedef Time time1;

void split_time(Train *train_data,int data_num);
void split(const char *s, vector<string> &strs, char delim = ' ');
void simple_dp(Input input_data, int &now_phy_cpu, int &now_mome_size, string &str1, char &next_line);
void time_series(Train *train_data, Input *input_data, int train_day, int input_day,
	int phy_num, int data_num, vector<vector<int>> &swp1, vector<vector<int>> &swp2);
void Exp_smoothing_1(vector<vector<int>> &swp1, Input *input_data, int phy_num, int day_num, vector<float> &results_1, float a, vector<vector<float>> &update);
void Exp_smoothing_2(vector<int> &results_2, int phy_num, int day_num, float b, vector<vector<float>> &update);
vector<float> Linear_regression(const vector<vector<float>> &data_train_flavor);
float gradient(const vector<vector<float>> &data_train_flavor, const vector<float> &w, int m,int N);
float function1(const vector<float> &w, const vector<float> &data, int N);
void normalize(vector<float> &g,int N);
float calcAlpha(const vector<float> &w, const vector<float> &g, float alpha, const vector<vector<float>> &data_train_flavor,int N);
float lossfunction(const vector<float> &w, const vector<vector<float>> &data_train_flavor, int N);
float dotProduct(vector<float> a, vector<float> b, int N);
bool isSame(const vector<float> &w, const vector<float> &wNEW, int N);
void characteristics_process(vector<vector<vector<float>>> &data_train, const vector<vector<int>> swp2, vector<vector<vector<float>>> &predict_data);  //��������
void predict_lr(const vector<vector<float>> &wRet, vector<vector<vector<float>>> &predict_data, vector<unsigned int> &predict_day);  //Ԥ�⺯��
void update_data(vector<vector<vector<float>>> &predict_data, int i, int j);//����Ԥ������
#endif
