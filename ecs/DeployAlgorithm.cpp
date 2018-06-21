#include "DeployAlgorithm.h"
#include "PredictAlgorithm.h"
#include "DataCenter.h"
#include "Tools.h"
using namespace std;
#include <iostream>
#include <cstring>

DeployAlgorithm::DeployAlgorithm()
{
	// ģ���˻����
	m_T = 100.0;   //ģ���˻��ʼ�¶�
	m_Tmin = 1.0;	//ģ���˻���ֹ�¶�
	m_maxRate = 0; // �������
	m_optimizationResult.clear();
	m_r = TEMPERATURE; //�¶��½�ϵ��
	m_result.clear();
	m_deployResult.clear();
}

DeployAlgorithm::DeployAlgorithm(PredictAlgorithm &pa, DataCenter &dc, int deployModel)
{
	// ģ���˻����
	m_T = 100.0;   //ģ���˻��ʼ�¶�
	m_Tmin = 1.0;	//ģ���˻���ֹ�¶�
	m_maxRate = 0; // �������
	m_optimizationResult.clear();
	m_r = TEMPERATURE; //�¶��½�ϵ��
	m_deployResult.clear();

	m_pa = &pa;
	m_dc = &dc;
	m_deployModel = deployModel;
}

DeployAlgorithm::~DeployAlgorithm()
{
}

void DeployAlgorithm::setPredictAlgorithmObject(PredictAlgorithm &pa, DataCenter &dc)
{
	m_pa = &pa;
	m_dc = &dc;
}

std::string &DeployAlgorithm::getDeployResultStr()
{
	return m_result;
}

void DeployAlgorithm::setDeployModel(int deployModel)
{
	m_deployModel = deployModel;
}

std::string &DeployAlgorithm::getError()
{
	return m_error;
}

void DeployAlgorithm::startDeploy()
{
	//���������
	DeployFlavor();
}

void DeployAlgorithm::DeployFlavor()
{
	//��ȡ�Ż���ʽ CPU or MEM
	//m_optimizationModel = m_dc->getOptimizationModel();
	switch (m_deployModel)
	{
	case GREEDY_ALGORITHM:
		//{
		//	std::vector<Flavor> PredictResult;
		//	Tools::flavorListInit(PredictResult, m_pa->getPredictResultVector());
		//	std::vector<std::vector<Flavor>> deployServerList; //��ʱ�洢���
		//	DeployFlavor_GREEDY_ALGORITHM(deployServerList, m_result, PredictResult);
		//	break;
		//}
		DeployFlavor_GREEDY_ALGORITHM(m_result);
		break;
	case GREEDY_ALGORITHM_KNAPSACK:
		DeployFlavor_GREEDY_ALGORITHM_KNAPSACK(m_result);
		break;
	case DEPLOY_AUTO:
		DeployFlavor_DEPLOY_AUTO();
		break;
	case SIMULATED_ANNEALING:
		DeployFlavor_SIMULATED_ANNEALING(m_result);
		break;
	default:
		break;
	}
}

double DeployAlgorithm::DeployFlavor_GREEDY_ALGORITHM(std::vector<std::vector<Flavor>> & deployServerList, std::string &result, std::vector<Flavor> PredictResult, int type)
{
	//ִ��̰���㷨
	int nServerCpuCount = m_dc->getServerCpuCount(type).serverCpuCount;
	int nServerMemCount = m_dc->getServerMemCount(type).serverMemCount;
	double serverResourceProportion = (double)nServerCpuCount / (double)nServerMemCount;

	while (checkFlavorFromFlavorList(PredictResult))
	{ //����δ����������
		std::vector<Flavor> currentServerFlavorList;
		Flavor *fl = getMaxResourceFlavor(PredictResult);
		if (nullptr != fl)
		{
			Flavor *tempFl = new Flavor;
			memcpy(tempFl, fl, sizeof(Flavor));
			currentServerFlavorList.push_back(*tempFl);
		}
		while (true)
		{ //���ϳ�ȡ����������������ֱ����ȡ�����ʧ��
			fl = extractFlavor(PredictResult, currentServerFlavorList, serverResourceProportion, nServerCpuCount, nServerMemCount);
			if (nullptr == fl)
			{
				break;
			}

			if (!checkResource(currentServerFlavorList, fl, nServerCpuCount, nServerMemCount))
			{ //��ȡ��Դ�ϸ�
				Flavor *tempFl = new Flavor;
				memcpy(tempFl, fl, sizeof(Flavor));
				currentServerFlavorList.push_back(*tempFl);
			}
			else
			{
				//���ڳ���,������������ػ�����
				for (size_t i = 0; i < PredictResult.size(); ++i)
				{
					if (PredictResult[i].flavorName == fl->flavorName)
					{
						PredictResult[i].nCount++;
						break;
					}
				}
			}
		}
		deployServerList.push_back(currentServerFlavorList);
	}
	//deployList_ServerTypes ds;
	//ds.serverType = DataCenter::GENERAL == 
	//m_deployResult.push_back(ds);
	return calcOccupancyRate(deployServerList, type);

	// double rate = calcOccupancyRate(m_optimizationModel, deployServerList);

	// /*****************************************************����������*******************************************************/
	// std::string predictResult = m_pa->getResultStr();
	// std::vector<std::string> flavorList = Tools::split(predictResult, "\n");
	// std::vector<std::vector<Flavor>> tempDeployServerList;
	// int nTotal = 0;
	// for (std::vector<std::vector<Flavor>>::iterator it = deployServerList.begin(); it != deployServerList.end(); ++it)
	// {
	// 	rate = calcOccupancyRate(m_dc->getOptimizationModel(), *it);
	// 	if (rate < 0.83)
	// 	{ //����ռ���ʽϵ͵��������񣬲�ͳ��ÿ�ֹ����ֵĴ���,��������Ԥ����
	// 		for (int i = 0; i < (*it).size(); ++i)
	// 		{
	// 			for (int j = 1; j < flavorList.size(); ++j)
	// 			{
	// 				std::vector<std::string> ss = Tools::split(flavorList[j], " ");
	// 				if (!ss.empty())
	// 				{
	// 					if ((*it)[i].flavorName == ss[0])
	// 					{
	// 						nTotal++;
	// 						flavorList[j] = ss[0] + " " + std::to_string(std::stoi(ss[1]) - 1);
	// 					}
	// 				}
	// 			}
	// 		}
	// 	}
	// 	else
	// 	{
	// 		tempDeployServerList.push_back(*it);
	// 	}
	// }
	// std::string deployResult;
	// flavorList[0] = std::to_string(std::stoi(flavorList[0]) - nTotal);
	// predictResult.clear();
	// for (int i = 0; i < flavorList.size(); ++i)
	// {
	// 	predictResult += flavorList[i] + "\n";
	// }
	// //ͳ�Ʒ�������������ַ����� result
	// statisticsFlavor(tempDeployServerList, deployResult);

	// //*****************************************************����������*******************************************************/

	// result = predictResult + deployResult;
	// return rate;
}

void DeployAlgorithm::DeployFlavor_GREEDY_ALGORITHM(std::string &result)
{
	//1.��ȡ������
	//2.��ȡ��������������
	std::vector<Flavor> PredictResult;
	Tools::flavorListInit(PredictResult, m_pa->getPredictResultVector());
	std::vector<std::vector<Flavor>> GeneralServerList;         //ͨ���ͷ������б�
	std::vector<std::vector<Flavor>> LargeMemoryServerList;     //���ڴ�������б�
	std::vector<std::vector<Flavor>> HighPerformanceServerList; //���ڴ�������б�

	int nServerIndex = calcNeedServerType(PredictResult);
	//1.��������������������͵���������
	//2.������һ������������һ̨�µķ�����
	//3.�Ѹտ����ķ�����װ��
	//4.�ص���һ��
	int nServerCpuCount = 0;
	int nServerMemCount = 0;
	double serverResourceProportion = 0;
	while (checkFlavorFromFlavorList(PredictResult) && (-1 != nServerIndex))
	{ //����δ����������
		nServerCpuCount = m_dc->getServerCpuCount(nServerIndex).serverCpuCount;
		nServerMemCount = m_dc->getServerMemCount(nServerIndex).serverMemCount;
		serverResourceProportion = (double)nServerCpuCount / (double)nServerMemCount;

		std::vector<Flavor> currentServerFlavorList;
		Flavor *fl = getMatchMaxResourceFlavor(PredictResult, nServerIndex);
		if (nullptr != fl)
		{
			Flavor *tempFl = new Flavor;
			memcpy(tempFl, fl, sizeof(Flavor));
			currentServerFlavorList.push_back(*tempFl);
		}
		while (true)
		{ //���ϳ�ȡ����������������ֱ����ȡ�����ʧ��
			fl = extractFlavor(PredictResult, currentServerFlavorList, serverResourceProportion, nServerCpuCount, nServerMemCount);
			if (nullptr == fl)
			{
				break;
			}

			if (!checkResource(currentServerFlavorList, fl, nServerCpuCount, nServerMemCount))
			{ //��ȡ��Դ�ϸ�
				Flavor *tempFl = new Flavor;
				memcpy(tempFl, fl, sizeof(Flavor));
				currentServerFlavorList.push_back(*tempFl);
			}
			else
			{
				//���ڳ���,������������ػ�����
				for (size_t i = 0; i < PredictResult.size(); ++i)
				{
					if (PredictResult[i].flavorName == fl->flavorName)
					{
						PredictResult[i].nCount++;
						break;
					}
				}
			}
		}
		switch (nServerIndex)
		{//��ͬ���͵ķ������ֿ��洢
		case DataCenter::GENERAL:
			GeneralServerList.push_back(currentServerFlavorList);
			break;
		case DataCenter::LARGE_MEMORY:
			LargeMemoryServerList.push_back(currentServerFlavorList);
			break;
		case DataCenter::HIGH_PERFORMANCE:
			HighPerformanceServerList.push_back(currentServerFlavorList);
			break;
		default:
			break;
		}
		nServerIndex = calcNeedServerType(PredictResult);
	}

	float occ = 0.83;

	std::vector<Flavor> cutFlavorList;
	Tools::flavorListInit(cutFlavorList, m_dc->m_FlavorList);

	for (int nServerIndex = 0; nServerIndex < GeneralServerList.size(); ++nServerIndex)
	{
		if (occ > calcOccupancyRate(GeneralServerList[nServerIndex], DataCenter::GENERAL))
		{
			for (int nFlavorIndex = 0; nFlavorIndex < GeneralServerList[nServerIndex].size(); ++nFlavorIndex)
			{
				for (int nIndex = 0; nIndex < cutFlavorList.size(); ++nIndex)
				{
					if (cutFlavorList[nIndex].flavorName == GeneralServerList[nServerIndex][nFlavorIndex].flavorName)
					{
						cutFlavorList[nIndex].nCount++;
					}
				}
			}
			GeneralServerList.erase(GeneralServerList.begin() + nServerIndex);
		}
	}

	for (int nServerIndex = 0; nServerIndex < LargeMemoryServerList.size(); ++nServerIndex)
	{
		if (occ > calcOccupancyRate(LargeMemoryServerList[nServerIndex], DataCenter::LARGE_MEMORY))
		{
			for (int nFlavorIndex = 0; nFlavorIndex < LargeMemoryServerList[nServerIndex].size(); ++nFlavorIndex)
			{
				for (int nIndex = 0; nIndex < cutFlavorList.size(); ++nIndex)
				{
					if (cutFlavorList[nIndex].flavorName == LargeMemoryServerList[nServerIndex][nFlavorIndex].flavorName)
					{
						cutFlavorList[nIndex].nCount++;
					}
				}
			}
			LargeMemoryServerList.erase(LargeMemoryServerList.begin() + nServerIndex);
		}
	}

	for (int nServerIndex = 0; nServerIndex < HighPerformanceServerList.size(); ++nServerIndex)
	{
		if (occ > calcOccupancyRate(HighPerformanceServerList[nServerIndex], DataCenter::HIGH_PERFORMANCE))
		{
			for (int nFlavorIndex = 0; nFlavorIndex < HighPerformanceServerList[nServerIndex].size(); ++nFlavorIndex)
			{
				for (int nIndex = 0; nIndex < cutFlavorList.size(); ++nIndex)
				{
					if (cutFlavorList[nIndex].flavorName == HighPerformanceServerList[nServerIndex][nFlavorIndex].flavorName)
					{
						cutFlavorList[nIndex].nCount++;
					}
				}
			}
			HighPerformanceServerList.erase(HighPerformanceServerList.begin() + nServerIndex);
		}
	}

	std::string predictResult = m_pa->getResultStr();

	earseFlavor(predictResult, cutFlavorList);
	//ͳ�Ʒ�����
	std::string deployResult;
	statisticsFlavor(GeneralServerList, LargeMemoryServerList, HighPerformanceServerList, deployResult);
	
	result = predictResult + "\n" + deployResult;
}
int DeployAlgorithm::calcNeedServerType(std::vector<Flavor> & PredictResult)
{//1.��������������������͵���������
	//��ȡ�����������б�
	std::vector<Server> serverList = m_dc->getServerList();
	int nGeneralFlavorCount = 0;
	int nLargeMemoryFlavorCount = 0;
	int nHighPerformanceFlavorCount = 0;

	double dbGeneralServerProportion = (double)serverList[0].serverCpuCount / (double)serverList[0].serverMemCount;
	double dbLargeMemoryServerProportion = (double)serverList[1].serverCpuCount / (double)serverList[1].serverMemCount;
	double dbHighPerformanceServerProportion = (double)serverList[2].serverCpuCount / (double)serverList[2].serverMemCount;

	int ServerIndex = -1;

	for (int flavorIndex = 0; flavorIndex < PredictResult.size(); ++flavorIndex)
	{
		double minDifference = MAXIMUM;
		double dbCurrentFlavorProportion = (double)PredictResult[flavorIndex].flavorCpu / (double)PredictResult[flavorIndex].flavorMem;
		//���㵱ǰ����������ַ����������ͱȽϽӽ������ݼ���������������з���
		if (minDifference > fabs(dbGeneralServerProportion - dbCurrentFlavorProportion))
		{//ͨ���������
			minDifference = fabs(dbGeneralServerProportion - dbCurrentFlavorProportion);
			ServerIndex = DataCenter::GENERAL;
		}
		if (minDifference > fabs(dbLargeMemoryServerProportion - dbCurrentFlavorProportion))
		{//���ڴ��������
			minDifference = fabs(dbLargeMemoryServerProportion - dbCurrentFlavorProportion);
			ServerIndex = DataCenter::LARGE_MEMORY;
		}
		if (minDifference > fabs(dbHighPerformanceServerProportion - dbCurrentFlavorProportion))
		{//��������������
			minDifference = fabs(dbHighPerformanceServerProportion - dbCurrentFlavorProportion);
			ServerIndex = DataCenter::HIGH_PERFORMANCE;
		}

		if (0 == ServerIndex)
		{//ͨ�����͵����������
			nGeneralFlavorCount += PredictResult[flavorIndex].nCount;
			PredictResult[flavorIndex].flavorType = DataCenter::GENERAL;
		}
		else if (1 == ServerIndex)
		{//���ڴ����͵����������
			nLargeMemoryFlavorCount += PredictResult[flavorIndex].nCount;
			PredictResult[flavorIndex].flavorType = DataCenter::LARGE_MEMORY;
		}
		else if (2 == ServerIndex)
		{//��������͵����������
			nHighPerformanceFlavorCount += PredictResult[flavorIndex].nCount;
			PredictResult[flavorIndex].flavorType = DataCenter::HIGH_PERFORMANCE;
		}
	}
	//ͳ���������͵���������ͣ����������϶����͵����������ҵ�����һ��Ӧ�÷���ʲô���͵ķ�����
	if (nGeneralFlavorCount > nLargeMemoryFlavorCount)
	{//ͨ���ͽϴ�
		if (nGeneralFlavorCount > nHighPerformanceFlavorCount)
		{//ͨ�������
			return DataCenter::GENERAL;
		}
		else
		{//���������
			return DataCenter::HIGH_PERFORMANCE;
		}
	}
	else
	{//���ڴ�ϴ�
		if (nLargeMemoryFlavorCount > nHighPerformanceFlavorCount)
		{//���ڴ����
			return DataCenter::LARGE_MEMORY;
		}
		else
		{//���������
			return DataCenter::HIGH_PERFORMANCE;
		}
	}
	return -1;
}

double DeployAlgorithm::DeployFlavor_GREEDY_ALGORITHM_KNAPSACK(std::string &result)
{
	std::vector<Flavor> PredictResult;
	Tools::flavorListInit(PredictResult, m_pa->getPredictResultVector());
	std::vector<std::vector<Flavor>> deployServerList; //��ʱ�洢���

	//���ر�����̰���㷨,��ʱû��ʵ��

	return calcOccupancyRate(m_optimizationModel, deployServerList);
}

double DeployAlgorithm::DeployFlavor_SIMULATED_ANNEALING(std::string &result)
{
	// �����������
	srand((unsigned)time(NULL));

	//ȡԤ��ṹ
	std::vector<Flavor> PredictResult;
	Tools::flavorListInit(PredictResult, m_pa->getPredictResultVector());

	std::vector<std::vector<Flavor>> deployServerList; //��ʱ�洢���

	std::vector<std::vector<Flavor>> flavorList; // �洢����flavor ���ͷ���
	int flavorCount = m_dc->getFlavorCount();
	for (int count = 0; count < m_dc->m_ServerList.size(); count++)
	{
		std::vector<Flavor> tempFlavorList; // ��ʱ���ͷ���

		// ��flavor ��Ϊ3��
		for (int i = 0; i < flavorCount; i++)
		{
			Flavor currentFlavor = PredictResult[i];
			currentFlavor.nCount = floor(currentFlavor.nCount / (3 - count));
			PredictResult[i].nCount -= currentFlavor.nCount;
			tempFlavorList.push_back(currentFlavor);
		}
		flavorList.push_back(tempFlavorList);
	}

	while (m_T > m_Tmin)
	{
		std::vector<std::vector<Flavor>> tempFlavorList = flavorList; // �洢��ʱ�ķ��䷽���������ָ��ڵ�ǰ����ʱ�滻��ǰ���䷽��
		// ���ֵ�б�
		std::vector<float> randomList;
		for (int i = 0; i < m_dc->m_ServerList.size(); i++)
		{
			randomList.push_back((rand() / double(RAND_MAX)) * 0.5);
		}

		for (int i = 0; i < randomList.size(); i++)
		{
			auto afterBatchFlavorList = tempFlavorList[(i + 1) % randomList.size()]; // ��һ�����ͷ���
			auto currentBatchFlavorList = tempFlavorList[i];		 // ��ǰ���ͷ���

			// ������������ƶ��� ����ǰ����������� random% �ƶ�����һ��������
			for (int j = 0; j < flavorCount; j++)
			{
				int needToMove = floor(currentBatchFlavorList[j].nCount * randomList[i]); // ��Ҫ�ƶ������������
				afterBatchFlavorList[j].nCount += needToMove;							  // ��һ�����ͷ��� + ��Ҫ�ƶ������������
				currentBatchFlavorList[j].nCount -= needToMove;							  // ��ǰ���ͷ��� - ��Ҫ�ƶ������������
			}
		}
		std::vector<deployList_ServerTypes> tempDeployResult;
		double rate = 0;
		for (int i = 0; i < randomList.size(); i++)
		{
			deployList_ServerTypes ds;
			ds.serverType = DataCenter::GENERAL == i ? "General" : DataCenter::LARGE_MEMORY == i ? "Large-Memory" : "High-Performance";
			std::string result;
			//std::vector<std::vector<Flavor>> deployServerList; //��ʱ�洢���
			rate += DeployFlavor_GREEDY_ALGORITHM(ds.deployServerList, result, tempFlavorList[i], i);
			tempDeployResult.push_back(ds);
		}

		// ������ڵ�ǰ���֣��滻���䷽����������һ�η���
		if (rate > m_maxRate)
		{
			m_maxRate = rate;
			m_optimizationResult = tempFlavorList;
			flavorList = tempFlavorList;
			m_deployResult = tempDeployResult;
		}
		else //����������ͣ�����һ�����ʱ���������ֹ�Ż�����ֲ����Ž�
		{
			if (exp((m_maxRate - rate) / m_T) > rand() / RAND_MAX)
			{
				m_maxRate = rate;
				m_optimizationResult = tempFlavorList;
				flavorList = tempFlavorList;
			}
		}

		m_T = m_r * m_T; // һ��ѭ���������¶Ƚ���
	}
	//string deployResult;
	//statisticsFlavor(m_optimizationResult, deployResult);
	return m_maxRate;
}

void DeployAlgorithm::DeployFlavor_DEPLOY_AUTO()
{
	std::string result_GREEDY_ALGORITHM;
	std::vector<Flavor> PredictResult;
	Tools::flavorListInit(PredictResult, m_pa->getPredictResultVector());
	std::vector<std::vector<Flavor>> deployServerList; //��ʱ�洢���
	double OccupancyRate_GREEDY_ALGORITHM =
		DeployFlavor_GREEDY_ALGORITHM(deployServerList, result_GREEDY_ALGORITHM, PredictResult);

	std::string result_GREEDY_ALGORITHM_KNAPSACK;
	double OccupancyRate_GREEDY_ALGORITHM_KNAPSACK =
		DeployFlavor_GREEDY_ALGORITHM_KNAPSACK(result_GREEDY_ALGORITHM_KNAPSACK);

	//Ŀǰ���ر����㷨��ûʵ�֣�ֱ�ӷ���̰���㷨�Ľ��
	//m_result = OccupancyRate_GREEDY_ALGORITHM > OccupancyRate_GREEDY_ALGORITHM_KNAPSACK ?
	//	result_GREEDY_ALGORITHM : result_GREEDY_ALGORITHM_KNAPSACK;
	m_result = result_GREEDY_ALGORITHM;
}

double DeployAlgorithm::calcOccupancyRate(int optimizationModel, const std::vector<std::vector<Flavor>> &deployResult, int type)
{
	//�����з���������Դռ������ӣ������ƽ��������
	double rate = 0.0;
	int nServerCpuCount = m_dc->getServerCpuCount(type).serverCpuCount;
	int nServerMemCount = m_dc->getServerMemCount(type).serverMemCount;

	for (size_t nServerIndex = 0; nServerIndex < deployResult.size(); ++nServerIndex)
	{
		int nCurrentServerCpuCount = 0, nCurrentServerMemCount = 0; //������������Դ�ܺ�
		for (size_t nFlavorIndex = 0; nFlavorIndex < deployResult[nServerIndex].size(); ++nFlavorIndex)
		{
			nCurrentServerCpuCount += deployResult[nServerIndex][nFlavorIndex].nCount * deployResult[nServerIndex][nFlavorIndex].flavorCpu;
			nCurrentServerMemCount += deployResult[nServerIndex][nFlavorIndex].nCount * deployResult[nServerIndex][nFlavorIndex].flavorMem;
		}

		if (OPTIMIZATION_CPU == m_optimizationModel)
		{
			rate += (nCurrentServerCpuCount / nServerCpuCount);
		}

		if (OPTIMIZATION_MEM == m_optimizationModel)
		{
			rate += (nCurrentServerMemCount / nServerMemCount);
		}
	}
	rate /= (int)deployResult.size();

	return rate;
}

//����Ƿ����δ����������
bool DeployAlgorithm::checkFlavorFromFlavorList(std::vector<Flavor> &PredictResult)
{
	for (size_t i = 0; i < PredictResult.size(); ++i)
	{
		if (PredictResult[i].nCount > 0)
		{
			return true; //��ʣ��
		}
	}
	return false; //û��ʣ��
}

Flavor *DeployAlgorithm::getMaxResourceFlavor(std::vector<Flavor> &PredictResult)
{
	for (size_t maxIndex = PredictResult.size() - 1; 0 != maxIndex; --maxIndex)
	{
		if (0 != PredictResult[maxIndex].nCount)
		{
			PredictResult[maxIndex].nCount--;
			return &PredictResult[maxIndex];
		}
	}
	return nullptr;
}

//��ȡһ��������������ƥ����������Դ���������
Flavor *DeployAlgorithm::getMatchMaxResourceFlavor(std::vector<Flavor> &PredictResult, int ServerType)
{
	for (size_t maxIndex = PredictResult.size() - 1; 0 != maxIndex; --maxIndex)
	{
		if ((0 != PredictResult[maxIndex].nCount) && (ServerType == PredictResult[maxIndex].flavorType))
		{
			PredictResult[maxIndex].nCount--;
			return &PredictResult[maxIndex];
		}
	}
	return nullptr;
}

bool DeployAlgorithm::checkResource(const std::vector<Flavor> &currentFlavorList, Flavor *fl, int serverCpu, int serverMem)
{
	int totalCPU = 0, totalMem = 0;
	for (int i = 0; i < (int)currentFlavorList.size(); ++i)
	{
		totalCPU += currentFlavorList[i].flavorCpu;
		totalMem += currentFlavorList[i].flavorMem;
	}
	if (nullptr != fl)
	{
		totalCPU += fl->flavorCpu;
		totalMem += fl->flavorMem;
	}

	if ((totalCPU > serverCpu) || (totalMem > serverMem))
	{
		return true;
	}
	return false;
}

Flavor *DeployAlgorithm::extractFlavor(std::vector<Flavor> &PredictResult, std::vector<Flavor> currentFlavorList, double serverProportion, int serverCpu, int serverMem)
{
	double tempCpu = 0.0; //CPUռ�ñ�
	double tempMem = 0.0; //MEMռ�ñ�

	int index = 0;
	double min_cpu_mem = MAXIMUM; //�ڴ�ʹ������CPUʹ�������
	double max_cpu = -1;		  //���CPUռ����
	double max_mem = -1;		  //���MEMռ����

	if (checkFlavorFromFlavorList(PredictResult))
	{ //��ʣ��������ų�ȡ
		for (size_t i = 0; i < PredictResult.size(); ++i)
		{
			if (0 < PredictResult[i].nCount)
			{
				//��ǰ�������ѷ������CPUʹ����
				tempCpu = (double)(getTotalResource(currentFlavorList, CPU) + PredictResult[i].flavorCpu) / (double)serverCpu;
				//��ǰ�������ѷ������MEMʹ����
				tempMem = (double)(getTotalResource(currentFlavorList, MEM) + PredictResult[i].flavorMem) / (double)serverMem;

				if (!checkResource(currentFlavorList, &PredictResult[i], serverCpu, serverMem))
				{
					//�����Ż�
					double sub_cpu_mem = fabs(tempCpu - tempMem); //CPUʹ������MEMʹ����֮��
					if (min_cpu_mem > sub_cpu_mem)
					{ //ѡȡCPU��������MEM���������ϵ͵������
						min_cpu_mem = sub_cpu_mem;
						index = i;
					}
				}
			}
		}

		if (MAXIMUM == min_cpu_mem)
		{
			return nullptr;
		}
	}
	else
	{
		return nullptr;
	}
	PredictResult[index].nCount--;
	return &PredictResult[index];
}

int DeployAlgorithm::getTotalResource(std::vector<Flavor> currentServerFlavorList, int CPU_or_MEM)
{
	int nTotal = 0;
	for (size_t i = 0; i < currentServerFlavorList.size(); ++i)
	{
		(CPU == CPU_or_MEM) ? (nTotal += currentServerFlavorList[i].flavorCpu) : (nTotal += currentServerFlavorList[i].flavorMem);
	}
	return nTotal;
}

//ͳ��ÿ��������������������������
void DeployAlgorithm::statisticsFlavor(std::vector<std::vector<Flavor>> &deployServerList, std::string &reslut)
{
	reslut += std::to_string((int)deployServerList.size());
	//reslut += "\n";
	std::string flavorNameList;
	for (size_t i = 0; i < deployServerList.size(); ++i)
	{
		reslut += ("\n" + std::to_string(i + 1) + " ");
		flavorNameList.clear();

		double cpu = 0.0, mem = 0.0;
		for (size_t j = 0; j < deployServerList[i].size(); ++j)
		{
			cpu += deployServerList[i][j].flavorCpu; //��ǰ�������ܹ�ʹ�õ�CPU��
			mem += deployServerList[i][j].flavorMem; //��ǰ�������ܹ�ʹ�õ��ڴ���

			char *p = strstr((char *)flavorNameList.c_str(), std::string(deployServerList[i][j].flavorName + std::string(" ")).c_str());
			if (nullptr != p)
			{
				//�ҵ���
				char *pPre = p, *pEnd;
				while ((' ' != *pPre) && pPre++)
					;
				pEnd = pPre + 1;
				while ((' ' != *pEnd) && pEnd++)
					;
				int nCount = stoi(std::string(pPre, pEnd));
				nCount++;
				char *pszBuf = new char[(int)flavorNameList.size() + 10];
				memset(pszBuf, 0, (int)flavorNameList.size() + 10);
				strncpy(pszBuf, flavorNameList.c_str(), flavorNameList.length() - strlen(pPre));
				strcat(pszBuf, " ");
				strcat(pszBuf, std::to_string(nCount).c_str());
				strcat(pszBuf, pEnd);
				flavorNameList = pszBuf;
				delete[] pszBuf;
			}
			else
			{ //û�ҵ�
				flavorNameList += deployServerList[i][j].flavorName;
				flavorNameList += " 1 ";
			}
		}
		//ɾ��ĩβ�Ŀո�
		flavorNameList.erase((int)flavorNameList.size() - 1);
		reslut += flavorNameList;
		//reslut += flavorNameList + std::string(" cpu rate:" + std::to_string(cpu / m_dc->getServerCpuCount() * 100) + "% ") + std::string(" mem rate:" + std::to_string(mem / m_dc->getServerMemCount() * 100) + "%") + "\n";
	}
	//�����Է��֣����һ̨����������û��װ�κ�����������ڴ˴���飬����һ̨�յķ��������Ƴ�
	char *pLastServer = strrchr((char *)reslut.c_str(), '\n');
	if (nullptr == strstr(pLastServer, "flavor"))
	{
		memset(pLastServer, 0, strlen(pLastServer));
		int nServerCount = std::stoi(reslut);
		nServerCount--;
		char *pTail = strstr((char *)reslut.c_str(), "\n");
		std::string tempResult = std::to_string(nServerCount);
		tempResult += pTail;
		reslut.clear();
		reslut = tempResult;
	}
}

void DeployAlgorithm::statisticsFlavor(std::vector<std::vector<Flavor>> & GeneralServerList, std::vector<std::vector<Flavor>> & LargeMemoryServerList, 
	std::vector<std::vector<Flavor>> & HighPerformanceServerList, std::string &reslut)
{
	reslut += std::string("General ") + std::to_string(GeneralServerList.size()) + "\n";
	for (int nIndex = 0; nIndex < GeneralServerList.size(); ++nIndex)
	{
		reslut += "General-" + std::to_string(nIndex + 1) + " ";
		std::string currentServerFlavor;
		int nTotalCpu = 0;
		int nTotalMem = 0;
		for (int nFlavorIndex = 0; nFlavorIndex < GeneralServerList[nIndex].size(); ++nFlavorIndex)
		{
			nTotalCpu += GeneralServerList[nIndex][nFlavorIndex].flavorCpu;
			nTotalMem += GeneralServerList[nIndex][nFlavorIndex].flavorMem;
			insertFlavor(currentServerFlavor, GeneralServerList[nIndex][nFlavorIndex].flavorName);
		}
		//reslut += currentServerFlavor + "            cpu:" + std::to_string((nTotalCpu / 56.0) * 100.0) + "%   mem:" + std::to_string((nTotalMem / 128.0) * 100.0) + "%";
		reslut += currentServerFlavor;
		reslut += "\n";
	}
	reslut += std::string("\nLarge-Memory ") + std::to_string(LargeMemoryServerList.size()) + "\n";
	for (int nIndex = 0; nIndex < LargeMemoryServerList.size(); ++nIndex)
	{
		reslut += "Large-Memory-" + std::to_string(nIndex + 1) + " ";
		std::string currentServerFlavor;
		int nTotalCpu = 0;
		int nTotalMem = 0;
		for (int nFlavorIndex = 0; nFlavorIndex < LargeMemoryServerList[nIndex].size(); ++nFlavorIndex)
		{
			nTotalCpu += LargeMemoryServerList[nIndex][nFlavorIndex].flavorCpu;
			nTotalMem += LargeMemoryServerList[nIndex][nFlavorIndex].flavorMem;
			insertFlavor(currentServerFlavor, LargeMemoryServerList[nIndex][nFlavorIndex].flavorName);
		}
		//reslut += currentServerFlavor + "            cpu:" + std::to_string((nTotalCpu / 84.0) * 100.0) + "%   mem:" + std::to_string((nTotalMem / 256.0) * 100.0) + "%";
		reslut += currentServerFlavor;
		reslut += "\n";
	}
	reslut += std::string("\nHigh-Performance ") + std::to_string(HighPerformanceServerList.size()) + "\n";
	for (int nIndex = 0; nIndex < HighPerformanceServerList.size(); ++nIndex)
	{
		reslut += "High-Performance-" + std::to_string(nIndex + 1) + " ";
		std::string currentServerFlavor;
		int nTotalCpu = 0;
		int nTotalMem = 0;
		for (int nFlavorIndex = 0; nFlavorIndex < HighPerformanceServerList[nIndex].size(); ++nFlavorIndex)
		{
			nTotalCpu += HighPerformanceServerList[nIndex][nFlavorIndex].flavorCpu;
			nTotalMem += HighPerformanceServerList[nIndex][nFlavorIndex].flavorMem;
			insertFlavor(currentServerFlavor, HighPerformanceServerList[nIndex][nFlavorIndex].flavorName);
		}
		//reslut += currentServerFlavor + "    cpu:" + std::to_string((nTotalCpu / 112.0) * 100.0) + "%   mem:" + std::to_string((nTotalMem / 192.0) * 100.0) + "%";
		reslut += currentServerFlavor;
		reslut += "\n";
	}
}
void DeployAlgorithm::insertFlavor(std::string & currentServerFlavor, std::string & flavorName)
{
	std::vector<std::string> ss = Tools::split(currentServerFlavor, flavorName + " ");
	if (1 != ss.size())
	{//�ַ������Ѿ����ڸ����������
		int nCount = std::stoi(ss[1]);
		nCount++;
		const char * pSpace = ss[1].c_str();
		while ((' ' != *pSpace) && (pSpace++));
		pSpace++;
		currentServerFlavor = ss[0] + flavorName + " " + std::to_string(nCount) + std::string(" ") + pSpace;
	}
	else
	{//�ַ����в����ڸ����������
		currentServerFlavor += flavorName + " 1 ";
	}
}
float DeployAlgorithm::calcOccupancyRate(int optimizationModel, const std::vector<Flavor> &cur_deployResult, int type)
{
	int nTotalCpu = 0;
	int nTotalMem = 0;
	for (int i = 0; i < cur_deployResult.size(); ++i)
	{
		nTotalCpu += cur_deployResult[i].flavorCpu;
		nTotalMem += cur_deployResult[i].flavorMem;
	}
	if (OPTIMIZATION_CPU == m_optimizationModel)
	{
		return ((float)nTotalCpu / (float)m_dc->getServerCpuCount(type).serverCpuCount);
	}

	if (OPTIMIZATION_MEM == m_optimizationModel)
	{
		return ((float)nTotalMem / (float)m_dc->getServerMemCount(type).serverMemCount);
	}
	return 0;
}

double DeployAlgorithm::calcOccupancyRate(const std::vector<std::vector<Flavor>> &deployResult, int type)
{
	//�����з���������Դռ������ӣ������ƽ��������
	double rate = 0.0;
	int nServerCpuCount = m_dc->getServerCpuCount(type).serverCpuCount;
	int nServerMemCount = m_dc->getServerMemCount(type).serverMemCount;

	for (size_t nServerIndex = 0; nServerIndex < deployResult.size(); ++nServerIndex)
	{//�����������Դ�ۺ�
		int nCurrentServerCpuCount = 0, nCurrentServerMemCount = 0; //������������Դ�ܺ�
		for (size_t nFlavorIndex = 0; nFlavorIndex < deployResult[nServerIndex].size(); ++nFlavorIndex)
		{//��������������Դ�ܺ�
			//nCurrentServerCpuCount += deployResult[nServerIndex][nFlavorIndex].nCount * deployResult[nServerIndex][nFlavorIndex].flavorCpu;
			//nCurrentServerMemCount += deployResult[nServerIndex][nFlavorIndex].nCount * deployResult[nServerIndex][nFlavorIndex].flavorMem;
			nCurrentServerCpuCount += deployResult[nServerIndex][nFlavorIndex].flavorCpu;
			nCurrentServerMemCount += deployResult[nServerIndex][nFlavorIndex].flavorMem;
		}

		rate += (nCurrentServerCpuCount / nServerCpuCount) * 0.5;

		rate += (nCurrentServerMemCount / nServerMemCount) * 0.5;
	}
	rate /= (int)deployResult.size();

	return rate;
}

float DeployAlgorithm::calcOccupancyRate(const std::vector<Flavor> &cur_deployResult, int nServerType)
{
	int nTotalCpu = 0;
	int nTotalMem = 0;

	for (int nIndex = 0; nIndex < cur_deployResult.size(); ++nIndex)
	{
		nTotalCpu += cur_deployResult[nIndex].flavorCpu;
		nTotalMem += cur_deployResult[nIndex].flavorMem;
	}
	float cpu_occ = (float)nTotalCpu / (float)m_dc->getServerCpuCount(nServerType).serverCpuCount;
	float mem_occ = (float)nTotalMem / (float)m_dc->getServerCpuCount(nServerType).serverMemCount;

	return cpu_occ < mem_occ ? cpu_occ : mem_occ;
}

void DeployAlgorithm::earseFlavor(std::string & predictResult, std::vector<Flavor> & cutList)
{
	for (int nIndex = 0; nIndex < cutList.size(); ++nIndex)
	{
		size_t szIndex = predictResult.find(cutList[nIndex].flavorName);
		szIndex += cutList[nIndex].flavorName.length() + 1;
		std::string sCount(predictResult.begin() + szIndex, predictResult.end());
		int nCount = std::stoi(sCount);
		nCount -= cutList[nIndex].nCount;
		cutList[nIndex].nCount = nCount;
	}

	int nTotalPredictCount = 0;
	//Ԥ������ʱ�洢��Flavor�ṹ���nCount��

	std::string result;
	std::string tempStr;

	for (size_t nFlavorIndex = 0; nFlavorIndex < cutList.size(); ++nFlavorIndex)
	{
		nTotalPredictCount += cutList[nFlavorIndex].nCount;
		tempStr += cutList[nFlavorIndex].flavorName;
		tempStr += " ";
		tempStr += std::to_string(cutList[nFlavorIndex].nCount);
		tempStr += "\n";
	}
	result = std::to_string(nTotalPredictCount);
	result += "\n";
	result += tempStr;
	// return result;

	predictResult = result;
}