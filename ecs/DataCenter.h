#ifndef DATA_CENTER_H_
#define DATA_CENTER_H_

#include "lib_io.h"
#include "define.h"

class DataCenter
{
  public:
	friend class PredictAlgorithm;
	friend class DeployAlgorithm;
  public:
	enum
	{//Model
		DAY,				 //����ָ�
		STRICT_TIME_QUANTUM, //�ϸ��ʱ��ηָ�, 7��һ����λ(����UNIXʱ���)
		SLACK_TIME_QUANTUM,  //�ɳڵ�ʱ��ηָ7��һ����λ(�������ڷָ�)
	};
	enum
	{// ���������
		GENERAL,		  // ͨ��
		LARGE_MEMORY,	  // ���ڴ�
		HIGH_PERFORMANCE, // ������
	};

	DataCenter();
	DataCenter(char *info[MAX_INFO_NUM], char *data[MAX_DATA_NUM], int data_num, int model = DataCenter::SLACK_TIME_QUANTUM);
	~DataCenter();
	void setTrainData(char *data[MAX_DATA_NUM], int data_num);
	void setInputData(char *info[MAX_INFO_NUM]);
	void setSplitModel(int model = DataCenter::SLACK_TIME_QUANTUM);

	bool start();
	const std::string &getError();

  private:
	void readTrainData();
	void readInputData();

	//�ָ�����
	void splitTrainData();
	void splitTrainData_DAY();
	void splitTrainData_SLACK_TIME_QUANTUM();
	void splitTrainData_STRICT_TIME_QUANTUM(int time_lenth);

	//��ȡʱ��������
	size_t getTimeQuantumCount();
	//��ȡ��һ��ʱ�����ڹ���б�
	bool getFirstTimeQuantumFlavorList(std::vector<Flavor> &fl);
	//��ȡ��һ��ʱ�����ڹ���б�
	bool getNextTimeQuantumFlavorList(std::vector<Flavor> &fl);

	//��ȡ�������
	size_t getFlavorCount();
	//��ȡÿ��ʱ��������ָ�������б�
	bool getFirstFlavorListFromTimeQuantum(std::vector<Flavor> &fl);
	//��ȡ��һ��ָ�������б�
	bool getNextFlavorListFromTimeQuantum(std::vector<Flavor> &fl);

	//��ȡָ������ָ�������ֵĴ���
	int getFlavorCount(int nTimeQuantum, int nFlavor);

	//��ȡ�ָ�ģʽ
	int getSplitModel();
	time_t getPredictStartTime_unix();
	time_t getPredictEndTime_unix();

	std::string getFlavorName(int nTimeQuantum, int nFlavor);

	//���á���ȡƽ��ֵ
	void setFlavor_s1(int nTimeQuantum, int nFlavor, double value);
	double getFlavor_s1(int nTimeQuantum, int nFlavor);
	void setFlavor_s2(int nTimeQuantum, int nFlavor, double value);
	double getFlavor_s2(int nTimeQuantum, int nFlavor);
	void setFlavor_s3(int nTimeQuantum, int nFlavor, double value);
	double getFlavor_s3(int nTimeQuantum, int nFlavor);

	//��ȡInput�ļ��е��Ż���ʽ MEM or CPU
	//int getOptimizationModel();

	Server getServerCpuCount(int type = DataCenter::GENERAL);
	Server getServerMemCount(int type = DataCenter::GENERAL);

	//��ǰ�������һ��֮�������
	int getDayFromCurrentToFirstDay(std::string strTime);

	//��ȡʱ��γ���
	int getTimeQuantumLenth()
	{
		return ((m_endPredictTime_unix - m_startPredictTime_unix) / (24 * 3600));
	}

	std::vector<Server> & getServerList()
	{
		return m_ServerList;
	}
#ifdef _DEBUG
	void showTotalFlavor();
#endif // ! _DEBUG
	time_t getTrainStartTime_unix()
	{
		return m_startTrainTime_unix;
	}
	time_t getTrainEndTime_unix()
	{
		return m_endTrainTime_unix;
	}
  private:
	//input
	char **inputTrain;
	char **inputFlavor;
	int m_data_num;

	//�����ѵ�����������б�
	std::vector<Train> m_TrainList;   
	std::vector<Flavor> m_FlavorList;

	//�ָ���ɵ��б����Ϊʱ��Σ��ڲ�Ϊ���
	std::vector<std::vector<Flavor>> m_SplitedFlavorList;

	//ʱ��������͹������������ѭ��ʹ��
	size_t m_currentTimeQuantumIndex;
	size_t m_FlavorIndex;

	//�����ļ���ȡ������Ϣ [input.txt]
	std::vector<Server> m_ServerList;

	int m_flavorCount;		 // ���������
	//int m_optimizationModel; //�����ļ��ж�ȡ�����Ż�ģʽ CPU or MEM

	//ѵ�����ݵĿ�ʼ�����ʱ��
	std::string m_startTrainTime;
	time_t m_startTrainTime_unix;
	std::string m_endTrainTime;
	time_t m_endTrainTime_unix;

	std::string m_startPredictTime;
	time_t m_startPredictTime_unix;
	std::string m_endPredictTime;
	time_t m_endPredictTime_unix;

	//�ָ�ģʽ
	int m_splitModel;

	std::string m_error;
};

#endif
