#include "function.h"
#include <iostream>  
#include <string>  
#include <vector>  
#include <string.h>
#include <cmath>
using namespace std;
const char * previous_flavor= " ";  //��һ��VM�����ʲô
int count_flavor;  //���VM������

/******************�ָ��ַ�������*********************************/
void split(const char *s, vector<string> &strs, char delim)
{
	if (s == nullptr) {
		return;
	}

	const char *head, *tail;
	head = tail = s;

	while (*head != '\0') {
		while (*head != '\0' && *head == delim) {
			head++;
		}

		tail = head;
		while (*tail != '\0' && *tail != delim) {
			tail++;
		}

		if (head != tail) {
			strs.push_back(string(head, tail));
			head = tail;
		}
		else {
			break;
		}
	}
}
char * strrstr(const char* _pBegin, int _MaxLen, const char* _szKey)
{
	if (NULL == _pBegin || NULL == _szKey || _MaxLen <= 0)
	{
		return NULL;
	}
	int i = _MaxLen - 1;
	int j = strlen(_szKey) - 1;
	int k = 0;
	int s32CmpLen = strlen(_szKey);

	for (i = _MaxLen - 1; i >= 0; i--)
	{
		if (_pBegin[i] == _szKey[j])
		{
			for (j = strlen(_szKey) - 1, k = i; j >= 0; j--, k--)
			{
				if (_pBegin[k] != _szKey[j])
				{
					j = strlen(_szKey) - 1;
					break;
				}
				if (j == 0)
				{
					return (char*)_pBegin + i - strlen(_szKey) + 1;
				}
			}
		}
	}

	return NULL;
}

void simple_dp(Input input_data, int &now_phy_cpu, int &now_mome_size, string &str1,char &next_line)
{
	string s;
	now_phy_cpu = now_phy_cpu - input_data.flavorcpu;
	now_mome_size = now_mome_size - input_data.flavormemo;
	if (next_line == 1)
	{
		count_flavor = 1;
		str1 += ' '+string(input_data.flavorname) + ' ' + to_string(count_flavor);
		next_line = 0;
		previous_flavor = input_data.flavorname; //��¼��һ�����
	}
	else
	{
		if (strcmp(input_data.flavorname,previous_flavor)!=0)
		{
			count_flavor = 1;
			str1 += ' '+string(input_data.flavorname) + ' ' + to_string(count_flavor);
		}
		else
		{	
			char * p = strrstr((char *)str1.c_str(), str1.size(), "flavor");
			p += 6;
			while ((' ' != *p) && p++);
			int nNum = atoi(p) + 1;
			str1.erase(str1.size() - strlen(p) + 1);

			str1 += to_string(nNum);
		}

		previous_flavor = input_data.flavorname; //��¼��һ�����
	}
	
}
void split_time(Train *train_data,int data_num)  //�ָ�ʱ��
{
	char szBuf[5] = { 0 };
	string tt;
	vector<string> temp_6(0);
	vector<string> temp_7(0);
	Train *p;
	p = train_data;
	for (int i = 0; i < data_num; ++i)
	{
		split(p->createatime, temp_6);
		split(temp_6[0].data(), temp_7, '-');
		memset(szBuf, 0, 5);
		strcpy(szBuf, temp_7[1].data());
		p->month = atoi(szBuf);  //��ȡ��
		memset(szBuf, 0, 5);
		strcpy(szBuf, temp_7[2].data());  //��ȡ��
		p->day=atoi(szBuf);
		temp_6.clear(); //�������
		temp_7.clear();
		vector<string>(temp_6).swap(temp_6); //�ͷ�����
		vector<string>(temp_7).swap(temp_7);
		p++;	
	}

}
/*
 * ָ��ƽ��Ԥ�⺯��  
 */
void time_series(Train *train_data, Input *input_data, int train_day, int input_day,
	int phy_num, int data_num, vector<vector<int>> &swp1, vector<vector<int>> &swp2)
{
	int rest_day = 0;
	int day = 0;
	Train *p1;
	Input *p2;
	p1 = train_data;
	p2 = input_data;
	int a = 0;  //��ʱ����
	day = int(train_day / input_day);
    rest_day = train_day - (day*input_day);
	int variable_relative_day = 1;
	int variable_relative_week = 1;
	for (int m = 0; m <data_num; ++m)
	{
		p1[m].relative_day = (p1[m].month - p1[0].month) * 30 + p1[m].day - p1[0].day + 1; //����������  ���������
		if (p1[m].relative_day == variable_relative_day)
			p1[m].relative_week = variable_relative_week;
		else
		{
			variable_relative_week += 1;
			if (variable_relative_week > 7)
			{
				variable_relative_week = 1;
				p1[m].relative_week = variable_relative_week;
			}
			else
				p1[m].relative_week = variable_relative_week;
		}
			
		variable_relative_day = p1[m].relative_day;
	}
	for (int i = 0; i < phy_num; i++)
	{	
		int j = day;
		a = p1[data_num - 1].relative_day;
		while ((j!=0)&&(a>=7))
		{
			for (int k = a; k >(a - 7); k--)
			{
				for (int n = data_num - 1; n > 0; n--)
				{
					if ((p1[n].relative_day == k) && (0 == strcmp(p1[n].flavorName, p2[i].flavorname)))
					{
						swp1[i][j]++;
					}
				}
			}
			a = a - 7;
			j--;
		}	
	}

	for (int i = 0; i < phy_num; i++)
	{
		for (int j = 1; j < train_day+1; j++)
		{
			for (int k = 0; k < data_num; k++)
			{
				if ((p1[k].relative_day == j) && (strcmp(p1[k].flavorName, p2[i].flavorname) == 0))
				{
					swp2[i][j]++;
			
				}
			}

		}
	}
	float tt = 0.0; //��ʱ����
	for (int i = 0; i < phy_num; i++)
	{
		for (int j = 1; j < 4; j++)
			tt += swp2[i][j];
		swp2[i][0] = tt /3+0.5;
		tt = 0.0;
	}	
}


void Exp_smoothing_1(vector<vector<int>> &swp1, Input *input_data, int phy_num, int day_num, vector<float> &results_1, float a, vector<vector<float>> &update)
{
	vector<float>s_0;
	//vector<vector<float>> update(phy_num,vector<float>(day_num + 1));
	for (int i = 0; i < phy_num; i++)
	{
		swp1[i][0] = swp1[i][1];
		s_0.push_back(swp1[i][1] + swp1[i][2] + swp1[i][3]);
	}

	for (int i = 0; i < phy_num; i++)
	{
		for (int j = 1; j < day_num + 1; j++)
		{
			update[i][j] = a*swp1[i][j - 1] + (1 - a)*s_0[i];  //ÿ�ε�Ԥ��ֵ
			s_0[i] = update[i][j];
		}
	}
	for (int i = 0; i < phy_num; i++)
	{
		results_1.push_back(a*swp1[i][day_num] + (1 - a)*update[i][day_num]); //һ��ƽ��������

	}

}

void Exp_smoothing_2(vector<int> &results_2, int phy_num, int day_num, float b, vector<vector<float>> &update)
{
	float a1, b1;
	vector<float>s_1;
	vector<vector<float>> update1(phy_num, vector<float>(day_num + 1));
	for (int i = 0; i < phy_num; i++)
	{
		s_1.push_back(update[i][1]);
		update1[i][1] = update[i][1];
	}

	for (int i = 0; i < phy_num; i++)
	{
		for (int j = 2; j < day_num + 1; j++)
		{
			update1[i][j] = b*update[i][j] + (1 - b)*s_1[i];  //ÿ�ε�Ԥ��ֵ
			s_1[i] = update1[i][j];
		}
	}
	for (int k = 0; k < phy_num; k++)
	{
		a1 = 2 * update[k][day_num] - update1[k][day_num];
		b1 = b / (1 - b)*(update[k][day_num] - update1[k][day_num]);
		if ((a1 + b1) < 0)
			results_2.push_back(0);
		else
			results_2.push_back(a1 + b1);  //������
	}

}

vector<float> Linear_regression(const vector<vector<float>> &data_train_flavor)
{
	int N = data_train_flavor[0].size();  //����ά��
	vector<float> w(N,0);  //ÿ����������  ���һ����b����
	vector<float> wNEW(N, 0);
	vector<float> g(N, 0);  //ÿ���������ݶ�
	int times = 0;          //��������
	float alpha = 0.1;   //ѧϰ��
	while (times < 1000)  //��������������10000��
	{	
		for(int m=0;m < N;m++)
			g[m] = gradient(data_train_flavor, w, m, N);  //������������ݶȺ�
		normalize(g,N);  //�����ݶ�
		//alpha = calcAlpha(w, g, alpha, data_train_flavor,N);  //����alpha
		for (int i = 0; i < N; i++)
			wNEW[i] += g[i] * alpha;
		if (isSame(w, wNEW, N) == true)  //�жϵ�ǰw��wNEW�仯�Ƿ��
			break;
		w.assign(wNEW.begin(), wNEW.end());   //��wNEWֵ��ֵ��w
		//cout << lossfunction(w, data_train_flavor, N)<<endl;
		times += 1;
	}
	
	return w;
}									

float gradient(const vector<vector<float>> &data_train_flavor, const vector<float> &w, int m,int N)  //data_train  ȫ������   w Ȩֵ  i ѭ������ ��0 1...
{
	int M = data_train_flavor.size();
	float g = 0; //��ǰά�ȵ��ݶ� 
	float y = 0;
	int k = rand()%M;
	for (int i = k; i < M; i++)
	{
		y = function1(w, data_train_flavor[i], N);   //��ǰ����ֵ
		if (m != N - 1)
		{
			g += (data_train_flavor[i][N - 1] - y)*data_train_flavor[i][m];  //���ÿ���������ݶ�
		}
		else
			g += data_train_flavor[i][N - 1] - y;
	}
	return g/(M-k);
}

float function1(const vector<float> &w, const vector<float> &data,int N)  //����ֵ
{
	float y = 0;
	for (int i = 0; i < N - 1; i++)
	{
		y += w[i] * data[i];   
	}
	y += w[N - 1];  //����b

	return y;
}

void normalize(vector<float> &g,int N)
{
	float s = 0;
	for (int i=0; i < N; i++)
		s += g[i] * g[i];
	s = sqrt(s);
	for (int j = 0; j < N; j++)
		g[j] /= s;
}

float calcAlpha(const vector<float> &w, const vector<float> &g, float alpha, const vector<vector<float>> &data_train_flavor,int N)
{
	float cl = 0.3,now,next;
	int count = 30;
	vector<float> wNEXT;
	now = lossfunction(w, data_train_flavor,N);    //��ǰ��ʧ����
	wNEXT.assign(w.begin(), w.end());   //��wֵ��ֵ��wNEXT
	for (int i = 0; i < N; i++)
		wNEXT[i] += g[i] * alpha;     //����wNEXT
	next = lossfunction(wNEXT, data_train_flavor, N);   //��һ����ʧ����ֵ
    
	while (next < now)
	{
		alpha *= 2;
		wNEXT.assign(w.begin(), w.end());
		for (int i = 0; i < N; i++)
			wNEXT[i] += g[i] * alpha;     //����wNEXT
		next = lossfunction(wNEXT, data_train_flavor, N);   //��һ����ʧ����ֵ
		count -=1;
		if (count == 0)
			break;
	}
	count = 50;
	//Ѱ�Һ��ʵ�ѧϰ��alpha
	while (next > now - cl*alpha*dotProduct(g, g, N))
	{
		alpha /= 2;
		wNEXT.assign(w.begin(), w.end());
		for(int i = 0; i < N; i++)
			wNEXT[i] += g[i] * alpha;     //����wNEXT
		next = lossfunction(wNEXT, data_train_flavor, N);   //��һ����ʧ����ֵ
		count -= 1;
		if (count == 0)
			break;
	}
	return alpha;
}

float lossfunction(const vector<float> &w,const vector<vector<float>> &data_train_flavor,int N)
{
	int M = data_train_flavor.size();
	float s = 0;
	for (int i = 0; i < M; i++)
		s+=(data_train_flavor[i][N - 1] - function1(w, data_train_flavor[i],N))*(data_train_flavor[i][N - 1] - function1(w, data_train_flavor[i], N));
	return s/2;
}

float dotProduct(vector<float> a, vector<float> b,int N)
{
	float dp = 0;
	for (int i = 0; i < N; i++)
		dp += a[i] * b[i];
	return dp;
}

bool isSame(const vector<float> &w,const vector<float> &wNEW,int N)
{
	for (int i = 0; i < N; i++)
		if ((w[i] - wNEW[i]) > 0.00001|| (w[i] - wNEW[i]) < -0.00001)
			return false;
	return true;
}

void characteristics_process(vector<vector<vector<float>>> &data_train, const vector<vector<int>> swp2, vector<vector<vector<float>>> &predict_data)  //��������
{
	int week_variable = 1;
	for (int i = 0; i < data_train.size(); i++)
	{
		week_variable = 1;
		for (int j = 0; j < data_train[0].size(); j++)
		{
			if (week_variable > 7)
				week_variable = 1;
			data_train[i][j][0] = week_variable;  //ѵ������1 �������
			week_variable += 1;
		}
	}
		
	for (int i = 0; i < data_train.size(); ++i)
		for (int j = 0; j < data_train[0].size(); j++)
            data_train[i][j][1] = swp2[i][j];    //ѵ������2  ǰ��һ��Ĺ������	


	for (int i = 0; i < data_train.size(); ++i)
		for (int j = 0; j < data_train[0].size(); j++)
		{
			if ((j - 1) < 0)
				data_train[i][j][2] = swp2[i][0];
			else
				data_train[i][j][2] = swp2[i][j - 1];    //ѵ������3  ǰ��2��Ĺ������
		}
	for (int i = 0; i < data_train.size(); ++i)
		for (int j = 0; j < data_train[0].size(); j++)
		{
			if ((j - 2) < 0)
				data_train[i][j][3] = swp2[i][0];
			else
				data_train[i][j][3] = swp2[i][j - 2];    //ѵ������4  ǰ��3��Ĺ������
		}
	for (int i = 0; i < data_train.size(); ++i)
		for (int j = 0; j < data_train[0].size(); j++)
		{
			if ((j - 3) < 0)
				data_train[i][j][4] = swp2[i][0];
			else
				data_train[i][j][4] = swp2[i][j - 3];    //ѵ������5  ǰ��4��Ĺ������
		}
	for (int i = 0; i < data_train.size(); i++)
		for (int j = 0; j < data_train[0].size(); j++)
			data_train[i][j][5] = swp2[i][j + 1];      //y 

	for (int i = 0; i < predict_data.size(); i++)
	{
		week_variable = data_train[i][data_train[0].size()-1][0]+1;
		for (int j = 0; j < predict_data[0].size(); j++)
		{
			if (week_variable > 7)
				week_variable = 1;
			predict_data[i][j][0] = week_variable;     //Ԥ������1  �������
			week_variable += 1;
		}
	}
	for (int i = 0; i < data_train.size(); i++)
		for (int j = 1; j < predict_data[0][0].size() - 1; j++)
			predict_data[i][0][j] = data_train[i][data_train[0].size() - j ][data_train[0][0].size()-1];  //ÿ�����ĵ�һ������
}

void predict_lr(const vector<vector<float>> &wRet,vector<vector<vector<float>>> &predict_data, vector<unsigned int> &predict_day)
{
	for (int i = 0; i < wRet.size(); i++)
	{
		for (int j = 0; j < predict_data[0].size(); j++)
		{
			predict_day[0] = (int)(function1(wRet[i], predict_data[i][j], wRet[0].size()) + 0.5)>=0 ? (function1(wRet[i], predict_data[i][j], wRet[0].size()) + 0.5):0;
			predict_data[i][j][predict_data[0][0].size() - 1] = predict_day[0];
			if (j + 1 >predict_data[0].size()-1)
				break;
			else
				update_data(predict_data, i, j);
		}
	}    
}

void update_data(vector<vector<vector<float>>> &predict_data, int i, int j)
{
	predict_data[i][j + 1][1] = predict_data[i][j][predict_data[0][0].size()-1];

	for (int k = 2; k < predict_data[0][0].size()-1; k++)
	{
		predict_data[i][j + 1][k] = predict_data[i][j][k -1];
	}
	 
}