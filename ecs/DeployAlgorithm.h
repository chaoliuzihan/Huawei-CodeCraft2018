#ifndef DEPLOY_SERVER_H_
#define DEPLOY_SERVER_H_
#include "define.h"
#include "DataCenter.h"

class PredictAlgorithm;
class DataCenter;

class DeployAlgorithm
{
  public:
	enum
	{							   //Algorithm
		GREEDY_ALGORITHM,		   //̰���㷨
		GREEDY_ALGORITHM_KNAPSACK, //���ر�����̰���㷨
		DEPLOY_AUTO,			   //�Զ�ѡ�����㷨
		SIMULATED_ANNEALING		   //ģ���˻�
	};

	DeployAlgorithm();
	DeployAlgorithm(PredictAlgorithm &pa, DataCenter &dc, int deploy_model = DEPLOY_AUTO);
	~DeployAlgorithm();
	void setPredictAlgorithmObject(PredictAlgorithm &pa, DataCenter &dc);
	std::string &getDeployResultStr();
	void setDeployModel(int deployModel = DEPLOY_AUTO);
	std::string &getError();
	void startDeploy();

  private:
	//���������
	void DeployFlavor();

	//����Ԥ���㷨�ֱ�ִ�����º���,����ֵ����ƽ��ռ���ʣ�
	//�Ż�CPU�򷵻�CPUƽ��ռ���ʣ��Ż�MEM�򷵻�ƽ��MEMռ����
	double DeployFlavor_GREEDY_ALGORITHM(std::vector<std::vector<Flavor>> & deployServerList, std::string &result, std::vector<Flavor> PredictResult, int type = DataCenter::GENERAL);
	void DeployFlavor_GREEDY_ALGORITHM(std::string &result);
	double DeployFlavor_GREEDY_ALGORITHM_KNAPSACK(std::string &result);
	double DeployFlavor_SIMULATED_ANNEALING(std::string &result);

	//DeployFlavor_DEPLOY_AUTOִ���������ַ����㷨��ѡ��ƽ��ռ���ʸߵ���Ϊ���ս��
	void DeployFlavor_DEPLOY_AUTO();

	//�����Ż���ʽ������Դ��ƽ��ռ����
	double calcOccupancyRate(int optimizationModel, const std::vector<std::vector<Flavor>> &deployResult, int type = DataCenter::GENERAL);

	float calcOccupancyRate(int optimizationModel, const std::vector<Flavor> &cur_deployResult, int type = DataCenter::GENERAL);

	float calcOccupancyRate(const std::vector<Flavor> &cur_deployResult, int nServerType);

	double calcOccupancyRate(const std::vector<std::vector<Flavor>> &deployResult, int type = DataCenter::GENERAL);
	/*************************************̰���㷨�Ӻ���**************************************/

	//����Ƿ����δ����������
	bool checkFlavorFromFlavorList(std::vector<Flavor> &PredictResult);

	//��ȡһ�����������
	Flavor *getMaxResourceFlavor(std::vector<Flavor> &PredictResult);

	//��ȡһ��������������ƥ����������Դ���������
	Flavor *getMatchMaxResourceFlavor(std::vector<Flavor> &PredictResult, int ServerType);

	//����Ƿ���ڳ���, ���ڳ��ַ���true�������ڳ��ַ���false
	bool checkResource(const std::vector<Flavor> &PredictResult, Flavor *fl, int serverCpu, int serverMem);

	//���ݵ�ǰ������CPU��MEM�ı�ֵ����ȡһ�����ʺϵ��������ʹ����������𵽷������󣬶�����CPU��MEMӰ����С
	Flavor *extractFlavor(std::vector<Flavor> &PredictResult, std::vector<Flavor> currentFlavorList, double serverProportion, int serverCpu, int serverMem);

	//��ȡָ����Դ������ CPU or MEM
	int getTotalResource(std::vector<Flavor> currentServerFlavorList, int CPU_or_MEM);

	void statisticsFlavor(std::vector<std::vector<Flavor>> &deployServerList, std::string &reslut);

	void statisticsFlavor(std::vector<std::vector<Flavor>> & GeneralServerList, std::vector<std::vector<Flavor>> & LargeMemoryServerList, 
		std::vector<std::vector<Flavor>> & HighPerformanceServerList, std::string &reslut);
	/*************************************̰���㷨�Ӻ���**************************************/

	/********************************̰��+���ر����㷨�Ӻ���**********************************/

	/********************************̰��+���ر����㷨�Ӻ���**********************************/

	//������Ҫ�ķ���������
	int calcNeedServerType(std::vector<Flavor> & PredictResult);

	void insertFlavor(std::string & currentServerFlavor, std::string & flavorName);

	void earseFlavor(std::string & predictResult, std::vector<Flavor> & cutList);
  private:
	PredictAlgorithm *m_pa;
	DataCenter *m_dc;
	std::string m_result;
	int m_deployModel;
	std::string m_error;

	//�Ż���ʽ CPU or MEM
	int m_optimizationModel;

	// ģ���˻����
	double m_T;   //ģ���˻��ʼ�¶�
	double m_Tmin;	//ģ���˻���ֹ�¶�
	double m_maxRate; // �������
	std::vector<std::vector<Flavor>> m_optimizationResult;
	double m_r; //�¶��½�ϵ��
			  // std::vector<int> dice; //���ӣ�ÿ�����Ͷ����ȡvectorǰ����������Ϊÿ���˻���Ҫ����˳��������

	std::vector<deployList_ServerTypes> m_deployResult;
};

#endif