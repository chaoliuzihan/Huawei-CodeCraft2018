#include "PredictAlgorithm.h"
#include "DataCenter.h"
#include "Tools.h"

#ifdef _DEBUG
#include <iostream>
#endif


PredictAlgorithm::PredictAlgorithm()
{
	m_predictAlgorithm = -1;
	m_weight_s1 = 0.57;
	m_weight_s2 = 0.4;
	m_weight_s3 = 0.5;
	m_errorsFixed = 0;
	m_errorsWeight = 0.0;

	m_errorsModel = FIXED_ERRORS;
	m_predictAlgorithm = SECOND_EXPONENTIAL_SMOOTHING;
	m_predictMethod = TIME_QUANTUM;
	m_TimeSpan = 0;

	m_weightModel = MANUAL_WEIGHT;
	m_alphaList_s1.clear();
	m_alphaList_s2.clear();
}


PredictAlgorithm::~PredictAlgorithm()
{
}


void PredictAlgorithm::setDataCenterObject(DataCenter & dc)
{
	m_dc = &dc;
}

void PredictAlgorithm::setPredictAlgorithm(int predict_model)
{
	m_predictAlgorithm = predict_model;
}

void PredictAlgorithm::startPredict()
{
	if (m_predictMethod == DAY)
	{//������Ԥ�⣬ֻ��ѡ����Ρ�����ָ��ƽ������һ��ָ��ƽ����������
		if (m_dc->DAY == m_dc->getSplitModel())
		{
			//m_predictAlgorithm = SECOND_EXPONENTIAL_SMOOTHING;
		}
		else
		{//����Ԥ�⣬���Ƿָʽ���ǰ���ָֱ�ӷ���
			return;
		}
	}

	//Ԥ�⿪ʼʱ����ѵ�����ݽ���ʱ��ļ���ʱ����,����ָ��������ȵĵ�λ���죬����7��һ����λ
	if (m_dc->DAY == m_dc->getSplitModel())
	{
		m_TimeSpan = (m_dc->m_startPredictTime_unix - m_dc->m_endTrainTime_unix) / (3600 * 24);
	}
	else
	{//��ʱû�����ƣ�֮������
		m_TimeSpan = 0;
	}

	//���ѡ�����Զ����Σ���ô���ڿ�ʼִ��Ԥ���㷨ǰ���ò�����������Ԥ������ѵĲ���
	if (m_weightModel == AUTO_WEIGHT)
	{//�Զ�����

		for (int nPredictAlgorithm = SINGLE_EXPONENTIAL_SMOOTHING; nPredictAlgorithm < THREE_EXPONENTIAL_SMOOTHING + 1; ++nPredictAlgorithm)
		{//��1��2��3��ָ��ƽ���ֱ�Ԥ��
			calcWeight(nPredictAlgorithm);
		}
		//������ɣ���ʼԤ��
		////ִ�в�ͬ�㷨ǰ������ִ�й������ִ��룬�����������ָ��ƽ��ֵ
		calcCommonExponentialSmoothing_AUTO_WEIGHT();
		//1.predict
		//Ԥ�ⲿ��ֻ�����㷨��ص�Ԥ�ⲿ�֣�����������һ�������Ѿ�ִ�����
		switch (m_predictAlgorithm)
		{
		case SINGLE_EXPONENTIAL_SMOOTHING: //һ��ָ��ƽ��
			predict_SINGLE_EXPONENTIAL_SMOOTHING_AUTO_WEIGHT();
			break;
		case SECOND_EXPONENTIAL_SMOOTHING: //����ָ��ƽ��
			predict_SECOND_EXPONENTIAL_SMOOTHING_AUTO_WEIGHT();
			break;
		case THREE_EXPONENTIAL_SMOOTHING:  //����ָ��ƽ��
			predict_THREE_EXPONENTIAL_SMOOTHING_AUTO_WEIGHT();
			break;
		case AVERAGE:
			predict_AVERAGE();
			break;
		default:
			break;
		}
	}
	else if (m_weightModel == MANUAL_WEIGHT)
	{//�ֶ�����
		//ִ�в�ͬ�㷨ǰ������ִ�й������ִ��룬�����������ָ��ƽ��ֵ
		calcCommonExponentialSmoothing();
		//1.predict
		//Ԥ�ⲿ��ֻ�����㷨��ص�Ԥ�ⲿ�֣�����������һ�������Ѿ�ִ�����
		switch (m_predictAlgorithm)
		{
		case SINGLE_EXPONENTIAL_SMOOTHING: //һ��ָ��ƽ��
			predict_SINGLE_EXPONENTIAL_SMOOTHING();
			break;
		case SECOND_EXPONENTIAL_SMOOTHING: //����ָ��ƽ��
			predict_SECOND_EXPONENTIAL_SMOOTHING();
			break;
		case THREE_EXPONENTIAL_SMOOTHING:  //����ָ��ƽ��
			predict_THREE_EXPONENTIAL_SMOOTHING();
			break;
		case LINEAR_REGRESSION://���Իع�
			//predict_LINEAR_REGRESSION();
			break;
		default:
			break;
		}
	}
	if (m_predictAlgorithm != AVERAGE)
	{
#ifdef _DEBUG
		showPredictResult_s1();
#endif
		//ͳ��Ԥ����,������
		statisticalPredictResults();
	}
	else
	{
		//��ҪԤ���������
		double dbCycle = (double)(m_dc->getPredictEndTime_unix() - m_dc->getPredictStartTime_unix()) / (double)(24 * 3600 * 7);
		double err = 0.3;
		for (size_t j = 0; j < m_PredictResult.size(); ++j)
		{
			double dbCount = m_PredictResult[j].nCount * dbCycle;
			m_PredictResult[j].nCount = dbCount + (fmod(dbCount, 1.0) > err ? 1 : 0);
			m_PredictResult[j].nCount += m_errorsFixed;
		}
	}
}

const std::string & PredictAlgorithm::getError()
{
	return m_error;
}

void PredictAlgorithm::setErrorsModel(int errorsModel)
{
	m_errorsModel = errorsModel;
}

int PredictAlgorithm::getPredictAlgorithm()
{
	return m_predictAlgorithm;
}

void PredictAlgorithm::setPredictMethod(int predictMethod)
{
	m_predictMethod = predictMethod;
}

void PredictAlgorithm::setWeight(double weight_s1, double weight_s2, double weight_s3)
{
	m_weight_s1 = weight_s1;
	m_weight_s2 = weight_s2;
	m_weight_s3 = weight_s3;
}

void PredictAlgorithm::setWeightModel(int weightModel)
{
	m_weightModel = weightModel;
}

void PredictAlgorithm::setErrorsFixed(int errorsFixed)
{
	m_errorsFixed = errorsFixed;
}

void PredictAlgorithm::setErrorsWeight(double errorsWeight)
{
	m_errorsWeight = errorsWeight;
}

void PredictAlgorithm::predict_SINGLE_EXPONENTIAL_SMOOTHING()
{
	size_t nFlavorCount = m_dc->getFlavorCount();
	size_t nTimeQuantumCount = m_dc->getTimeQuantumCount();

	//��ҪԤ���ʱ��γ���
	size_t nPredictQuantumCount = 1;//����ָ��ƽ��ֻ��Ԥ����һ����
	for (int i = 0; i < nPredictQuantumCount; ++i)
	{
		//������Ԥ�⣬һ��ָ��ƽ�����ȴ洢��һ���Ԥ��ֵ
		/*******************************************************/
		std::vector<Flavor> fl;
		Tools::flavorListInit(fl, m_dc->m_FlavorList);
		m_PredictedFlavor.push_back(fl);
		/*******************************************************/
	}

	for (int nFlavorIndex = 0; nFlavorIndex < nFlavorCount; ++nFlavorIndex)
	{
		double predict_s1 = m_weight_s1 * m_dc->getFlavorCount(nTimeQuantumCount - 1, nFlavorIndex) + (1 - m_weight_s1) * m_dc->getFlavor_s1(nTimeQuantumCount - 1, nFlavorIndex);
		m_PredictedFlavor[0][nFlavorIndex].s1 = predict_s1;
	}
}
void PredictAlgorithm::predict_SECOND_EXPONENTIAL_SMOOTHING()
{
	//����ָ��ƽ��������ִ��һ��ָ��ƽ���������ݵ�һ��ָ��ƽ���Ľ�����еڶ���ָ��ƽ������
	predict_SINGLE_EXPONENTIAL_SMOOTHING();

	size_t nFlavorCount = m_dc->getFlavorCount();
	size_t nTimeQuantumCount = m_dc->getTimeQuantumCount();

	//��ҪԤ���ʱ��γ���
	//����ָ��ƽ��������Ԥ�������ڣ�Ԥ�⼸�����ڸ���ѵ�����ݽ���ʱ����Ԥ�⿪ʼʱ��������
	size_t nPredictQuantumCount = 1 + m_TimeSpan;
	for (int i = 1; i < nPredictQuantumCount; ++i)
	{
		//������Ԥ�⣬һ��ָ��ƽ�����ȴ洢��һ���Ԥ��ֵ
		/*******************************************************/
		std::vector<Flavor> fl;
		Tools::flavorListInit(fl, m_dc->m_FlavorList);
		m_PredictedFlavor.push_back(fl);
		/*******************************************************/
	}
	for (int nTimeQuantumIndex = 0; nTimeQuantumIndex < nPredictQuantumCount; ++nTimeQuantumIndex)
	{
		for (int nFlavorIndex = 0; nFlavorIndex < nFlavorCount; ++nFlavorIndex)
		{
			double A_T = 2 * m_dc->getFlavor_s1(nTimeQuantumCount - 1, nFlavorIndex) - m_dc->getFlavor_s2(nTimeQuantumCount - 1, nFlavorIndex);
			double B_T = ((m_weight_s2) / (1 - m_weight_s2)) * (m_dc->getFlavor_s1(nTimeQuantumCount - 1, nFlavorIndex) - m_dc->getFlavor_s2(nTimeQuantumCount - 1, nFlavorIndex));
			double predict_s2 = A_T + B_T * (nTimeQuantumIndex + 1);
			m_PredictedFlavor[nTimeQuantumIndex][nFlavorIndex].s2 = predict_s2;
		}
	}
}
void PredictAlgorithm::predict_THREE_EXPONENTIAL_SMOOTHING()
{
	//����ָ��ƽ��������ִ��һ������ָ��ƽ����
	predict_SECOND_EXPONENTIAL_SMOOTHING();

	size_t nFlavorCount = m_dc->getFlavorCount();
	size_t nTimeQuantumCount = m_dc->getTimeQuantumCount();

	for (int nTimeQuantumIndex = 0; nTimeQuantumIndex < m_PredictedFlavor.size(); ++nTimeQuantumIndex)
	{
		for (int nFlavorIndex = 0; nFlavorIndex < nFlavorCount; ++nFlavorIndex)
		{
			double A_T = 3 * m_dc->getFlavor_s1(nTimeQuantumCount - 1, nFlavorIndex) - 3 * m_dc->getFlavor_s2(nTimeQuantumCount - 1, nFlavorIndex) + m_dc->getFlavor_s3(nTimeQuantumCount - 1, nFlavorIndex);
			double B_T = (m_weight_s3 / (2 * (1 - m_weight_s3) * (1 - m_weight_s3))) * ((6 - 5 * m_weight_s3) *  m_dc->getFlavor_s1(nTimeQuantumCount - 1, nFlavorIndex) - 2 * (5 - 4 * m_weight_s3) * m_dc->getFlavor_s2(nTimeQuantumCount - 1, nFlavorIndex) + (4 - 3 * m_weight_s3) *  m_dc->getFlavor_s3(nTimeQuantumCount - 1, nFlavorIndex));
			double C_T = (m_weight_s3 * m_weight_s3) / 2 * (1 - m_weight_s3) * (1 - m_weight_s3) * (m_dc->getFlavor_s1(nTimeQuantumCount - 1, nFlavorIndex) - 2 * m_dc->getFlavor_s2(nTimeQuantumCount - 1, nFlavorIndex) + m_dc->getFlavor_s3(nTimeQuantumCount - 1, nFlavorIndex));
			double predict_s3 = A_T + B_T * (nTimeQuantumIndex + 1) + C_T * (nTimeQuantumIndex + 1) * (nTimeQuantumIndex + 1);
			m_PredictedFlavor[nTimeQuantumIndex][nFlavorIndex].s3 = predict_s3;
		}
	}
}


void PredictAlgorithm::predict_LINEAR_REGRESSION()
{//���Իع�

	int Dimensions = 5;
	//3��������alhpa + b

	const int nDataCount = 100;

	//50�죬3������ + y
	//std::vector<std::vector<int>> trainData(m_dc->getTimeQuantumCount(), std::vector<int>(Dimensions));//����
	std::vector<std::vector<float>> trainData(nDataCount, std::vector<float>(Dimensions));//����

	std::vector<float>  feature(Dimensions, 0);  //װalhpa��b
	std::vector<float>  nextFeature(Dimensions, 0);  //װalhpa��b ����ʱ�ռ�
	std::vector<float>  featureGradient(Dimensions, 0);  //ÿ���������ݶ�

	for (int i = 0; i < nDataCount; ++i)
	{
		trainData[i][0] = rand() % 10;                 //x_1
		trainData[i][1] = rand() % 10;                 //x_2
		trainData[i][2] = rand() % 10;
		trainData[i][3] = rand() % 10;
		trainData[i][4] = 7 * trainData[i][0] + 2 * trainData[i][1] + 11 * trainData[i][2] + 17 * trainData[i][3] - 5;//y
	}


	float alpha = 0.1;

	int nTimes = 0;//ѭ������
	while ((nTimes < 10000))
	{
		for (int nAlpha = 0; nAlpha < Dimensions; ++nAlpha)
		{
			featureGradient[nAlpha] = calcGradient(trainData, feature, nAlpha, Dimensions);//������������ݶȺ�
		}

		normalize(featureGradient, Dimensions);//����
		//alpha = updateAlpha(trainData, feature, featureGradient, alpha, Dimensions);//����alpha

		for (int i = 0; i < Dimensions; ++i)
		{
			nextFeature[i] += featureGradient[i] * alpha;
		}

		if (isSame(feature, nextFeature, Dimensions))  //�жϵ�ǰw��wNEW�仯�Ƿ��
		{
			break;
		}

		feature.assign(nextFeature.begin(), nextFeature.end());
		//std::cout << lossFunction(feature, trainData, Dimensions) << std::endl;
		//std::cout << alpha << std::endl;
		nTimes++;
	}
}


float PredictAlgorithm::calcGradient(const std::vector<std::vector<float>> & trainData, const std::vector<float> & feature, int nAlpha, int nDimensions)
{
	float current_gradient = 0;
	float current_predict_y = 0;
	int nTotalDay = trainData.size();

	int k = rand() % nTotalDay;
	for (int nDay = k; nDay < nTotalDay; ++nDay)
	{
		current_predict_y = calcPredictFunction(trainData[nDay], feature, nDimensions);
		if (nAlpha != nDimensions - 1)
		{//��alpha
			current_gradient += (trainData[nDay][nDimensions - 1] - current_predict_y) * trainData[nDay][nAlpha];
		}
		else
		{//��b
			current_gradient += trainData[nDay][nDimensions - 1] - current_predict_y;
		}
	}

	return current_gradient / (nTotalDay - k);
}


float PredictAlgorithm::calcPredictFunction(const std::vector<float> & cur_trainData, const std::vector<float> & feature, int nDimensions)
{
	//y = alpha_1 * x_1 + alpha_2 * x2 ... + b
	float current_predict_y = 0;
	for (int i = 0; i < nDimensions - 1; ++i)
	{
		current_predict_y += feature[i] * cur_trainData[i];
	}
	current_predict_y += feature[nDimensions - 1]; //b

	return current_predict_y;
}

//���� �ݶ�
void PredictAlgorithm::normalize(std::vector<float> &feature, int nDimensions)
{
	float sum_of_square = 0;
	for (int i = 0; i < nDimensions; i++)
	{
		sum_of_square += feature[i] * feature[i];
	}

	sum_of_square = sqrt(sum_of_square);

	for (int j = 0; j < nDimensions; j++)
	{
		feature[j] /= sum_of_square;
	}
}

float PredictAlgorithm::updateAlpha(const std::vector<std::vector<float>> & trainData, const std::vector<float> & feature, std::vector<float>  & featureGradient, float nAlpha, int nDimensions)
{
	float cl = 0.3, now, next;
	int count = 30;
	std::vector<float> wNEXT;
	now = lossFunction(feature, trainData, nDimensions);    //��ǰ��ʧ����
	wNEXT.assign(feature.begin(), feature.end());   //��wֵ��ֵ��wNEXT

	for (int i = 0; i < nDimensions; i++)
	{
		wNEXT[i] += featureGradient[i] * nAlpha;     //����wNEXT
	}

	next = lossFunction(wNEXT, trainData, nDimensions);   //��һ����ʧ����ֵ

	while (next < now)
	{
		nAlpha *= 2;
		wNEXT.assign(feature.begin(), feature.end());
		for (int i = 0; i < nDimensions; i++)
		{
			wNEXT[i] += featureGradient[i] * nAlpha;     //����wNEXT
		}
		next = lossFunction(wNEXT, trainData, nDimensions);   //��һ����ʧ����ֵ
		count -= 1;

		if (0 == count)
		{
			break;
		}
	}
	count = 50;
	//Ѱ�Һ��ʵ�ѧϰ��alpha
	while (next > now - cl * nAlpha * dotProduct(featureGradient, featureGradient, nDimensions))
	{
		nAlpha /= 2;
		wNEXT.assign(feature.begin(), feature.end());
		for (int i = 0; i < nDimensions; i++)
		{
			wNEXT[i] += featureGradient[i] * nAlpha;     //����wNEXT
		}
		next = lossFunction(wNEXT, trainData, nDimensions);   //��һ����ʧ����ֵ

		count -= 1;
		if (0 == count)
		{
			break;
		}
	}
	return nAlpha;
}


bool PredictAlgorithm::isSame(std::vector<float> &feature, std::vector<float> &nextFeature, int nDimensions)
{
	for (int i = 0; i < nDimensions; i++)
	{
		if ((feature[i] - nextFeature[i]) > 0.00001 || (feature[i] - nextFeature[i]) < -0.00001)
			return false;
	}

	return true;
}

void PredictAlgorithm::errorCalibration()
{
	double err = 0.4;
	if (m_errorsModel == FIXED_ERRORS)
	{
		for (size_t j = 0; j < m_PredictResult.size(); ++j)
		{
			m_PredictResult[j].nCount += m_errorsFixed;

			if (m_predictAlgorithm == SINGLE_EXPONENTIAL_SMOOTHING)
			{
				m_PredictResult[j].nCount += fmod(m_PredictResult[j].s1, 1.0) > err ? 1 : 0;
			}
			if (m_predictAlgorithm == SECOND_EXPONENTIAL_SMOOTHING)
			{
				m_PredictResult[j].nCount += fmod(m_PredictResult[j].s2, 1.0) > err ? 1 : 0;
			}
			if (m_predictAlgorithm == THREE_EXPONENTIAL_SMOOTHING)
			{
				m_PredictResult[j].nCount += fmod(m_PredictResult[j].s3, 1.0) > err ? 1 : 0;
			}
		}
	}
	if (m_errorsModel == DYNAMIC_ERRORS)
	{
		double proportion = 0;

		for (size_t j = 0; j < m_PredictResult.size(); ++j)
		{
			if (m_predictAlgorithm == SINGLE_EXPONENTIAL_SMOOTHING)
			{
				m_PredictResult[j].nCount += m_errorsList_s1[j];
			}
			if (m_predictAlgorithm == SECOND_EXPONENTIAL_SMOOTHING)
			{
				m_PredictResult[j].nCount += m_errorsList_s1[j];
			}
			if (m_predictAlgorithm == THREE_EXPONENTIAL_SMOOTHING)
			{
				m_PredictResult[j].nCount += m_errorsList_s1[j];
			}
			m_PredictResult[j].nCount += fmod(m_PredictResult[j].s1, 1.0) > err ? 1 : 0;
		}
	}
}

#ifdef _DEBUG

void PredictAlgorithm::showPredictResult_s1()
{
	for (size_t i = 0; i < m_PredictedFlavor.size(); ++i)
	{
		for (size_t j = 0; j < m_PredictedFlavor[i].size(); ++j)
		{
			std::cout << m_PredictedFlavor[i][j].flavorName << " s1: " << m_PredictedFlavor[i][j].s1 << std::endl;
		}
		std::cout << std::endl;
	}
}

#endif

std::string PredictAlgorithm::getResultStr()
{
	nTotalPredictCount = 0;
	//Ԥ������ʱ�洢��Flavor�ṹ���nCount��

	std::string result;
	std::string tempStr;

	for (size_t nFlavorIndex = 0; nFlavorIndex < m_PredictResult.size(); ++nFlavorIndex)
	{
		nTotalPredictCount += m_PredictResult[nFlavorIndex].nCount;
		tempStr += m_PredictResult[nFlavorIndex].flavorName;
		tempStr += " ";
		tempStr += std::to_string(m_PredictResult[nFlavorIndex].nCount);
		tempStr += "\n";
	}
	result = std::to_string(nTotalPredictCount);
	result += "\n";
	result += tempStr;
	return result;
}


void PredictAlgorithm::statisticalPredictResults()
{
	Tools::flavorListInit(m_PredictResult, m_dc->m_FlavorList);

	if (PredictAlgorithm::DAY == m_predictMethod)
	{
		size_t nStartDay = m_TimeSpan;
		for (size_t nDay = nStartDay; nDay < m_PredictedFlavor.size(); ++nDay)
		{
			for (size_t nFlavor = 0; nFlavor < m_PredictedFlavor[nDay].size(); ++nFlavor)
			{
				if (m_predictAlgorithm == PredictAlgorithm::SINGLE_EXPONENTIAL_SMOOTHING)
				{
					m_PredictResult[nFlavor].nCount += (int)m_PredictedFlavor[nDay][nFlavor].s1;
				}

				if (m_predictAlgorithm == PredictAlgorithm::SECOND_EXPONENTIAL_SMOOTHING)
				{
					m_PredictResult[nFlavor].nCount += (int)m_PredictedFlavor[nDay][nFlavor].s2;
				}

				if (m_predictAlgorithm == PredictAlgorithm::THREE_EXPONENTIAL_SMOOTHING)
				{
					m_PredictResult[nFlavor].nCount += (int)m_PredictedFlavor[nDay][nFlavor].s3;
				}
			}
		}
	}

	if (PredictAlgorithm::TIME_QUANTUM == m_predictMethod)
	{
		//������һ�����ڵĸ��Ԥ��̶���Ԥ���7��ʱ��Σ�����ҪԤ���ʱ�䳤�ȵ�Unixʱ�����Ϊһ�����ڣ����������Ԥ����������滬��ȡֵ��
		//�Ȱ�����Ԥ��ֵ���ܣ�Ȼ�󿳵�ͷ����β��

		for (size_t nCycleIndex = 0; nCycleIndex < m_PredictedFlavor.size(); ++nCycleIndex)
		{
			for (size_t nFlavor = 0; nFlavor < m_PredictedFlavor[nCycleIndex].size(); ++nFlavor)
			{
				if (m_predictAlgorithm == PredictAlgorithm::SINGLE_EXPONENTIAL_SMOOTHING)
				{
					m_PredictResult[nFlavor].nCount += (int)(m_PredictedFlavor[nCycleIndex][nFlavor].s1);
				}

				if (m_predictAlgorithm == PredictAlgorithm::SECOND_EXPONENTIAL_SMOOTHING)
				{
					m_PredictResult[nFlavor].nCount += (int)(m_PredictedFlavor[nCycleIndex][nFlavor].s2);
				}

				if (m_predictAlgorithm == PredictAlgorithm::THREE_EXPONENTIAL_SMOOTHING)
				{
					m_PredictResult[nFlavor].nCount += (int)(m_PredictedFlavor[nCycleIndex][nFlavor].s3);
				}
			}
		}
		if (m_predictAlgorithm != PredictAlgorithm::SINGLE_EXPONENTIAL_SMOOTHING)
		{
			cutPredictHeader();//ȥͷ
			cutPredictTail();//ȥβ
		}
	}
	//2.error calibration
	errorCalibration();
}

void PredictAlgorithm::cutPredictHeader()
{
	long predictStartTime = m_dc->getPredictStartTime_unix();//Ԥ�⿪ʼʱ��
	long trainEndTime = m_dc->getTrainEndTime_unix();//ѵ�����ݽ���ʱ��
	double timeSpace = abs(predictStartTime - trainEndTime);
	int nCycle = timeSpace / (3600 * 24 * 7);//Ԥ�⿪ʼ��ѵ�����ݽ���֮������ڿ��

	double proportion_start = fmod(timeSpace, 3600 * 24 * 7) / (3600 * 24 * 7);

	for (size_t nCycleIndex = 0; nCycleIndex < nCycle; ++nCycleIndex)
	{//�����������ڶ̵�Ԥ��ֵ
		for (size_t nFlavor = 0; nFlavor < m_PredictedFlavor[nCycleIndex].size(); ++nFlavor)
		{
			if (m_predictAlgorithm == PredictAlgorithm::SINGLE_EXPONENTIAL_SMOOTHING)
			{
				m_PredictResult[nFlavor].nCount -= (int)(m_PredictedFlavor[nCycleIndex][nFlavor].s1);
			}

			if (m_predictAlgorithm == PredictAlgorithm::SECOND_EXPONENTIAL_SMOOTHING)
			{
				m_PredictResult[nFlavor].nCount -= (int)(m_PredictedFlavor[nCycleIndex][nFlavor].s2);
			}

			if (m_predictAlgorithm == PredictAlgorithm::THREE_EXPONENTIAL_SMOOTHING)
			{
				m_PredictResult[nFlavor].nCount -= (int)(m_PredictedFlavor[nCycleIndex][nFlavor].s3);
			}
		}
	}

	for (size_t nFlavor = 0; nFlavor < m_PredictedFlavor[nCycle].size(); ++nFlavor)
	{//��������һ�����ڵ�Ԥ��ֵ
		if (m_predictAlgorithm == PredictAlgorithm::SINGLE_EXPONENTIAL_SMOOTHING)
		{
			m_PredictResult[nFlavor].nCount -= (int)((m_PredictedFlavor[nCycle][nFlavor].s1) * proportion_start);
		}

		if (m_predictAlgorithm == PredictAlgorithm::SECOND_EXPONENTIAL_SMOOTHING)
		{
			m_PredictResult[nFlavor].nCount -= (int)((m_PredictedFlavor[nCycle][nFlavor].s2) * proportion_start);
		}

		if (m_predictAlgorithm == PredictAlgorithm::THREE_EXPONENTIAL_SMOOTHING)
		{
			m_PredictResult[nFlavor].nCount -= (int)((m_PredictedFlavor[nCycle][nFlavor].s3) * proportion_start);
		}
	}

}
void PredictAlgorithm::cutPredictTail()
{
	//long predictStartTime = m_dc->getPredictStartTime_unix();//Ԥ�⿪ʼʱ��
	long predictEndTime = m_dc->getPredictEndTime_unix();//Ԥ�����ʱ��
	long trainEndTime = m_dc->getTrainEndTime_unix();//ѵ�����ݽ���ʱ��

	double timeSpace = abs(predictEndTime - trainEndTime);
	int nCycle = timeSpace / (3600 * 24 * 7);//Ԥ�⿪ʼ��ѵ�����ݽ���֮������ڿ��

	double proportion_end = fmod(timeSpace, 3600 * 24 * 7) / (3600 * 24 * 7);
	proportion_end = 1 - proportion_end;//����ͷ��ռ�����ڵ����ڵı���

	for (size_t nFlavor = 0; nFlavor < m_PredictedFlavor[nCycle].size(); ++nFlavor)
	{//��������һ�����ڵ�Ԥ��ֵ
		if (m_predictAlgorithm == PredictAlgorithm::SINGLE_EXPONENTIAL_SMOOTHING)
		{
			m_PredictResult[nFlavor].nCount -= (int)((m_PredictedFlavor[nCycle][nFlavor].s1) * proportion_end);
		}

		if (m_predictAlgorithm == PredictAlgorithm::SECOND_EXPONENTIAL_SMOOTHING)
		{
			m_PredictResult[nFlavor].nCount -= (int)((m_PredictedFlavor[nCycle][nFlavor].s2) * proportion_end);
		}

		if (m_predictAlgorithm == PredictAlgorithm::THREE_EXPONENTIAL_SMOOTHING)
		{
			m_PredictResult[nFlavor].nCount -= (int)((m_PredictedFlavor[nCycle][nFlavor].s3) * proportion_end);
		}
	}

	for (size_t nCycleIndex = nCycle + 1; nCycleIndex < m_PredictedFlavor.size(); ++nCycleIndex)
	{//�����������ڶ̵�Ԥ��ֵ
		for (size_t nFlavor = 0; nFlavor < m_PredictedFlavor[nCycleIndex].size(); ++nFlavor)
		{
			if (m_predictAlgorithm == PredictAlgorithm::SINGLE_EXPONENTIAL_SMOOTHING)
			{
				m_PredictResult[nFlavor].nCount -= (int)(m_PredictedFlavor[nCycleIndex][nFlavor].s1);
			}

			if (m_predictAlgorithm == PredictAlgorithm::SECOND_EXPONENTIAL_SMOOTHING)
			{
				m_PredictResult[nFlavor].nCount -= (int)(m_PredictedFlavor[nCycleIndex][nFlavor].s2);
			}

			if (m_predictAlgorithm == PredictAlgorithm::THREE_EXPONENTIAL_SMOOTHING)
			{
				m_PredictResult[nFlavor].nCount -= (int)(m_PredictedFlavor[nCycleIndex][nFlavor].s3);
			}
		}
	}

	for (size_t nCycleIndex = 0; nCycleIndex < m_PredictedFlavor.size(); ++nCycleIndex)
	{//�����������ڶ̵�Ԥ��ֵ
		for (size_t nFlavor = 0; nFlavor < m_PredictedFlavor[nCycleIndex].size(); ++nFlavor)
		{
			m_PredictResult[nFlavor].nCount = m_PredictResult[nFlavor].nCount < 0 ? 1 : m_PredictResult[nFlavor].nCount;
		}
	}
}

const std::vector<Flavor> & PredictAlgorithm::getPredictResultVector()
{
	return m_PredictResult;
}

void PredictAlgorithm::calcCommonExponentialSmoothing()
{
	//��ȡ��������
	size_t nFlavorCount = m_dc->getFlavorCount();

	//��ȡ������
	size_t nTimeQuantumCount = m_dc->getTimeQuantumCount();

	//���㹫����S1,������
	calcCommonS1();

	////���� S2 �� Sn
	for (size_t nFlavorIndex = 0; nFlavorIndex < nFlavorCount; ++nFlavorIndex)
	{
		for (size_t nTimeQuantumIndex = 1; nTimeQuantumIndex < nTimeQuantumCount; ++nTimeQuantumIndex)
		{
			//һ��ָ��ƽ����Sn
			double Sn_1 = m_weight_s1 * m_dc->getFlavorCount(nTimeQuantumIndex - 1, nFlavorIndex) + (1 - m_weight_s1) * m_dc->getFlavor_s1(nTimeQuantumIndex - 1, nFlavorIndex);
			m_dc->setFlavor_s1(nTimeQuantumIndex, nFlavorIndex, Sn_1);

			//����ָ��ƽ����Sn
			double Sn_2 = m_weight_s2 * m_dc->getFlavor_s1(nTimeQuantumIndex, nFlavorIndex) + (1 - m_weight_s2) * m_dc->getFlavor_s2(nTimeQuantumIndex - 1, nFlavorIndex);
			m_dc->setFlavor_s2(nTimeQuantumIndex, nFlavorIndex, Sn_2);

			//����ָ��ƽ����Sn
			double Sn_3 = m_weight_s3 * m_dc->getFlavor_s2(nTimeQuantumIndex, nFlavorIndex) + (1 - m_weight_s3) * m_dc->getFlavor_s3(nTimeQuantumIndex - 1, nFlavorIndex);
			m_dc->setFlavor_s3(nTimeQuantumIndex, nFlavorIndex, Sn_3);
		}
	}
}

void PredictAlgorithm::calcCommonExponentialSmoothing_AUTO_WEIGHT(std::vector<Flavor> & flBuf, int predictMethod, float weight)
{
	//���� S2 �� Sn-1 ���һ��ѵ���������ڲ���Ԥ��Ч��
	for (size_t nTimeQuantumIndex = 1; nTimeQuantumIndex < flBuf.size(); ++nTimeQuantumIndex)
	{
		//һ��ָ��ƽ����Sn
		double Sn_1 = weight * flBuf[nTimeQuantumIndex - 1].nCount + (1 - weight) * flBuf[nTimeQuantumIndex - 1].s1;
		flBuf[nTimeQuantumIndex].s1 = Sn_1;

		//����ָ��ƽ����Sn
		double Sn_2 = weight * flBuf[nTimeQuantumIndex].s1 + (1 - weight) * flBuf[nTimeQuantumIndex - 1].s2;
		flBuf[nTimeQuantumIndex].s2 = Sn_2;

		//����ָ��ƽ����Sn
		double Sn_3 = weight * flBuf[nTimeQuantumIndex].s2 + (1 - weight) * flBuf[nTimeQuantumIndex - 1].s3;
		flBuf[nTimeQuantumIndex].s3 = Sn_3;
	}
}

void PredictAlgorithm::calcCommonExponentialSmoothing_AUTO_WEIGHT()
{
	//��ȡ��������
	size_t nFlavorCount = m_dc->getFlavorCount();

	//��ȡ������
	size_t nTimeQuantumCount = m_dc->getTimeQuantumCount();

	//���㹫����S1,������
	//calcCommonS1();

	////���� S2 �� Sn
	for (size_t nFlavorIndex = 0; nFlavorIndex < nFlavorCount; ++nFlavorIndex)
	{
		for (size_t nTimeQuantumIndex = 1; nTimeQuantumIndex < nTimeQuantumCount; ++nTimeQuantumIndex)
		{
			//һ��ָ��ƽ����Sn
			double Sn_1 = m_alphaList_s1[nFlavorIndex] * m_dc->getFlavorCount(nTimeQuantumIndex - 1, nFlavorIndex) + (1 - m_alphaList_s1[nFlavorIndex]) * m_dc->getFlavor_s1(nTimeQuantumIndex - 1, nFlavorIndex);
			m_dc->setFlavor_s1(nTimeQuantumIndex, nFlavorIndex, Sn_1);

			//����ָ��ƽ����Sn
			double Sn_2 = m_alphaList_s2[nFlavorIndex] * m_dc->getFlavor_s1(nTimeQuantumIndex, nFlavorIndex) + (1 - m_alphaList_s2[nFlavorIndex]) * m_dc->getFlavor_s2(nTimeQuantumIndex - 1, nFlavorIndex);
			m_dc->setFlavor_s2(nTimeQuantumIndex, nFlavorIndex, Sn_2);

			//����ָ��ƽ����Sn
			double Sn_3 = m_alphaList_s3[nFlavorIndex] * m_dc->getFlavor_s2(nTimeQuantumIndex, nFlavorIndex) + (1 - m_alphaList_s3[nFlavorIndex]) * m_dc->getFlavor_s3(nTimeQuantumIndex - 1, nFlavorIndex);
			m_dc->setFlavor_s3(nTimeQuantumIndex, nFlavorIndex, Sn_3);
		}
	}
}

void PredictAlgorithm::calcCommonS1()
{
	//��ȡ������
	//�����㷨���������ڶ�Ӧ��ֵ����������������
	size_t nTimeQuantumCount = m_dc->getTimeQuantumCount();

	//��ȡ��������
	size_t nFlavorCount = m_dc->getFlavorCount();
	for (size_t nFlavorIndex = 0; nFlavorIndex < nFlavorCount; ++nFlavorIndex)
	{
		int nTotalFlavor = 0;
		for (size_t nTimeQuantumIndex = 0; nTimeQuantumIndex < nTimeQuantumCount; ++nTimeQuantumIndex)
		{
			//��ͬһ�ֹ���ڸ��������г��ֵ�����
			//std::string ss =  m_dc->getFlavorName(nTimeQuantumIndex, nFlavorIndex);
			nTotalFlavor += m_dc->getFlavorCount(nTimeQuantumIndex, nFlavorIndex);
		}
		//������������ĳ�������ֵ��ܴ�������������������õ� S1 S2 S3

		//����õ�����ָ��ƽ�����еĳ�ʼֵ S1
		double commonS1 = (double)nTotalFlavor / (double)nTimeQuantumCount;
		m_dc->setFlavor_s1(0, nFlavorIndex, commonS1);
		m_dc->setFlavor_s2(0, nFlavorIndex, commonS1);
		m_dc->setFlavor_s3(0, nFlavorIndex, commonS1);
	}
}


float PredictAlgorithm::dotProduct(std::vector<float> & featureGradient, std::vector<float> & featureGradient_1, int nDimensions)
{
	float dp = 0;
	for (int i = 0; i < nDimensions; i++)
	{
		dp += featureGradient[i] * featureGradient_1[i];
	}
	return dp;
}

float PredictAlgorithm::lossFunction(const std::vector<float> &feature, const std::vector<std::vector<float>> &trainData, int nDimensions)
{
	float s = 0;
	for (int i = 0; i < trainData.size(); i++)
	{
		s += (trainData[i][nDimensions - 1] - calcPredictFunction(trainData[i], feature, nDimensions))*(trainData[i][nDimensions - 1] - calcPredictFunction(trainData[i], feature, nDimensions));
	}
	return s / 2;
}

void PredictAlgorithm::predict_SINGLE_EXPONENTIAL_SMOOTHING_AUTO_WEIGHT()
{
	size_t nFlavorCount = m_dc->getFlavorCount();
	size_t nTimeQuantumCount = m_dc->getTimeQuantumCount();

	//��ҪԤ���ʱ��γ���
	size_t nPredictQuantumCount = 1;//����ָ��ƽ��ֻ��Ԥ����һ����
	for (int i = 0; i < nPredictQuantumCount; ++i)
	{
		//������Ԥ�⣬һ��ָ��ƽ�����ȴ洢��һ���Ԥ��ֵ
		/*******************************************************/
		std::vector<Flavor> fl;
		Tools::flavorListInit(fl, m_dc->m_FlavorList);
		m_PredictedFlavor.push_back(fl);
		/*******************************************************/
	}

	for (int nFlavorIndex = 0; nFlavorIndex < nFlavorCount; ++nFlavorIndex)
	{
		double predict_s1 = m_alphaList_s1[nFlavorIndex] * m_dc->getFlavorCount(nTimeQuantumCount - 1, nFlavorIndex) + (1 - m_alphaList_s1[nFlavorIndex]) * m_dc->getFlavor_s1(nTimeQuantumCount - 1, nFlavorIndex);
		m_PredictedFlavor[0][nFlavorIndex].s1 = predict_s1;
	}
}
void PredictAlgorithm::predict_SECOND_EXPONENTIAL_SMOOTHING_AUTO_WEIGHT()
{
	//����ָ��ƽ��������ִ��һ��ָ��ƽ���������ݵ�һ��ָ��ƽ���Ľ�����еڶ���ָ��ƽ������
	predict_SINGLE_EXPONENTIAL_SMOOTHING_AUTO_WEIGHT();

	size_t nFlavorCount = m_dc->getFlavorCount();
	size_t nTimeQuantumCount = m_dc->getTimeQuantumCount();

	//��ҪԤ���ʱ��γ���
	//����ָ��ƽ��������Ԥ�������ڣ�Ԥ�⼸�����ڸ���ѵ�����ݽ���ʱ����Ԥ�⿪ʼʱ��������
	//�����Ҫ�󣬿�ʼԤ��ʱ����ѵ�����ݽ���ʱ�������1��15��ļ����Ҫ��Ԥ���ʱ��Ϊ1~4���ڣ��ʣ�����Ԥ��7������
	//Ԥ�������������Ԥ��ʱ���ȥȡ
	size_t nPredictQuantumCount = 7;
	for (int i = 1; i < nPredictQuantumCount; ++i)
	{
		//������Ԥ�⣬һ��ָ��ƽ�����ȴ洢��һ���Ԥ��ֵ
		/*******************************************************/
		std::vector<Flavor> fl;
		Tools::flavorListInit(fl, m_dc->m_FlavorList);
		m_PredictedFlavor.push_back(fl);
		/*******************************************************/
	}
	for (int nTimeQuantumIndex = 0; nTimeQuantumIndex < nPredictQuantumCount; ++nTimeQuantumIndex)
	{	
		for (int nFlavorIndex = 0; nFlavorIndex < nFlavorCount; ++nFlavorIndex)
		{
			double A_T = 2 * m_dc->getFlavor_s1(nTimeQuantumCount - 1, nFlavorIndex) - m_dc->getFlavor_s2(nTimeQuantumCount - 1, nFlavorIndex);
			double B_T = ((m_alphaList_s2[nFlavorIndex]) / (1 - m_alphaList_s2[nFlavorIndex])) * (m_dc->getFlavor_s1(nTimeQuantumCount - 1, nFlavorIndex) - m_dc->getFlavor_s2(nTimeQuantumCount - 1, nFlavorIndex));
			double predict_s2 = A_T + B_T * (nTimeQuantumIndex + 1);
			m_PredictedFlavor[nTimeQuantumIndex][nFlavorIndex].s2 = predict_s2;
		}
	}
}
void PredictAlgorithm::predict_THREE_EXPONENTIAL_SMOOTHING_AUTO_WEIGHT()
{
	//����ָ��ƽ��������ִ��һ������ָ��ƽ����
	predict_SECOND_EXPONENTIAL_SMOOTHING_AUTO_WEIGHT();

	size_t nFlavorCount = m_dc->getFlavorCount();
	size_t nTimeQuantumCount = m_dc->getTimeQuantumCount();

	for (int nTimeQuantumIndex = 0; nTimeQuantumIndex < m_PredictedFlavor.size(); ++nTimeQuantumIndex)
	{
		for (int nFlavorIndex = 0; nFlavorIndex < nFlavorCount; ++nFlavorIndex)
		{
			double A_T = 3 * m_dc->getFlavor_s1(nTimeQuantumCount - 1, nFlavorIndex) - 3 * m_dc->getFlavor_s2(nTimeQuantumCount - 1, nFlavorIndex) + m_dc->getFlavor_s3(nTimeQuantumCount - 1, nFlavorIndex);
			double B_T = (m_alphaList_s3[nFlavorIndex] / (2 * (1 - m_alphaList_s3[nFlavorIndex]) * (1 - m_alphaList_s3[nFlavorIndex]))) * ((6 - 5 * m_alphaList_s3[nFlavorIndex]) *  m_dc->getFlavor_s1(nTimeQuantumCount - 1, nFlavorIndex) - 2 * (5 - 4 * m_alphaList_s3[nFlavorIndex]) * m_dc->getFlavor_s2(nTimeQuantumCount - 1, nFlavorIndex) + (4 - 3 * m_alphaList_s3[nFlavorIndex]) *  m_dc->getFlavor_s3(nTimeQuantumCount - 1, nFlavorIndex));
			double C_T = (m_alphaList_s3[nFlavorIndex] * m_alphaList_s3[nFlavorIndex]) / 2 * (1 - m_alphaList_s3[nFlavorIndex]) * (1 - m_alphaList_s3[nFlavorIndex]) * (m_dc->getFlavor_s1(nTimeQuantumCount - 1, nFlavorIndex) - 2 * m_dc->getFlavor_s2(nTimeQuantumCount - 1, nFlavorIndex) + m_dc->getFlavor_s3(nTimeQuantumCount - 1, nFlavorIndex));
			double predict_s3 = A_T + B_T * (nTimeQuantumIndex + 1) + C_T * (nTimeQuantumIndex + 1) * (nTimeQuantumIndex + 1);
			m_PredictedFlavor[nTimeQuantumIndex][nFlavorIndex].s3 = predict_s3;
		}
	}
}

void PredictAlgorithm::predict_AVERAGE()
{
	size_t nFlavorCount = m_dc->getFlavorCount();
	size_t nTimeQuantumCount = m_dc->getTimeQuantumCount();

	/*******************************************************/
	Tools::flavorListInit(m_PredictResult, m_dc->m_FlavorList);
	/*******************************************************/

	int nTotalFlavor = 0;

	int nCycle = 2;
	for (int nTimeQuantumIndex = nTimeQuantumCount - nCycle; nTimeQuantumIndex < nTimeQuantumCount; ++nTimeQuantumIndex)
	{
		for (int nFlavorIndex = 0; nFlavorIndex < nFlavorCount; ++nFlavorIndex)
		{//ͳ��ÿ����������
			m_PredictResult[nFlavorIndex].nCount += m_dc->getFlavorCount(nTimeQuantumIndex, nFlavorIndex);

			//ͳ�����������
			nTotalFlavor += m_dc->getFlavorCount(nTimeQuantumIndex, nFlavorIndex);//ͳ�����й�������
		}
	}

	double err = 0.9;
	for (int nFlavorIndex = 0; nFlavorIndex < nFlavorCount; ++nFlavorIndex)
	{//
		//double proportion = 0;
		//proportion = (double)m_PredictResult[nFlavorIndex].nCount / nTotalFlavor;//����ÿ�������������ռ�ı���

		//flavorProportionList.push_back(proportion);
		//m_PredictResult[nFlavorIndex].nCount = (m_PredictResult[nFlavorIndex].nCount / nTimeQuantumCount) * dbCycle;
		m_PredictResult[nFlavorIndex].nCount = (m_PredictResult[nFlavorIndex].nCount / nCycle);
		//m_PredictResult[nFlavorIndex].nCount += fmod(m_PredictResult[nFlavorIndex].nCount, nCycle) > err ? 1 : 0;
	}
}

void PredictAlgorithm::calcWeight(int predictAlgorithm)
{
	//���㹫����s1
	calcCommonS1();
	std::vector<Flavor> flBuf;
	m_dc->getFirstFlavorListFromTimeQuantum(flBuf);
	do
	{
		float optimum_alpha = MAXIMUM;//�洢���ŵ�alpha
		float min_answer = MAXIMUM;
		float errors = 0;//���
		//��ȡ�������������ʱ����ڵ�����

		for (int alpha = 1; alpha < 100; ++alpha)
		{
			float tempAlpha = (float)(alpha / 100.0);
			//std::cout << "alpha: " << tempAlpha << std::endl;
			//���ȡ�������ݼ�����Ӧ��ƽ��ֵ
			calcCommonExponentialSmoothing_AUTO_WEIGHT(flBuf, m_predictMethod, tempAlpha);

			//������һ��������Ԥ�����һ������
			//�Լ�������������
			if (predictAlgorithm == SINGLE_EXPONENTIAL_SMOOTHING)
			{
				//����һ��ָ��ƽ���Ľ��
				flBuf[flBuf.size() - 1].s1 = tempAlpha * flBuf[flBuf.size() - 2].nCount + (1 - tempAlpha) * flBuf[flBuf.size() - 2].s1;
			}
			if (predictAlgorithm == SECOND_EXPONENTIAL_SMOOTHING)
			{
				//�������ָ��ƽ����Ч��
				float At = 2 * flBuf[flBuf.size() - 2].s1 - flBuf[flBuf.size() - 2].s2;
				float Bt = (tempAlpha / (1 - tempAlpha)) * (flBuf[flBuf.size() - 2].s1 - flBuf[flBuf.size() - 2].s2);
				flBuf[flBuf.size() - 1].s2 = At + Bt * 1;
			}

			if (predictAlgorithm == THREE_EXPONENTIAL_SMOOTHING)
			{
				//��������ָ��ƽ��
				float At = 3 * flBuf[flBuf.size() - 2].s1 - 3 * flBuf[flBuf.size() - 2].s2 + flBuf[flBuf.size() - 2].s3;
				float Bt = (tempAlpha / 2 * powf((1 - tempAlpha), 2)) * ((6 - 5 * tempAlpha) * flBuf[flBuf.size() - 2].s1 - 2 * (5 - 4 * tempAlpha) * flBuf[flBuf.size() - 2].s2 + (4 - 3 * tempAlpha) * flBuf[flBuf.size() - 2].s3);
				float Ct = powf(tempAlpha, 2) / 2 * powf((1 - tempAlpha), 2) * (flBuf[flBuf.size() - 2].s1 - 2 * flBuf[flBuf.size() - 2].s2 + flBuf[flBuf.size() - 2].s3);
				flBuf[flBuf.size() - 1].s3 = At + Bt * 1 + Ct * 1;
			}

			//�Լ�������������
			float temp_answer = 0;
			if (predictAlgorithm == SINGLE_EXPONENTIAL_SMOOTHING)
			{
				temp_answer = (sqrtf(powf(flBuf[flBuf.size() - 1].nCount - flBuf[flBuf.size() - 1].s1, 2)) / (sqrtf(powf(flBuf[flBuf.size() - 1].nCount, 2)) + sqrtf(powf(flBuf[flBuf.size() - 1].s1, 2))));
			}
			if (predictAlgorithm == SECOND_EXPONENTIAL_SMOOTHING)
			{
				temp_answer = (sqrtf(powf(flBuf[flBuf.size() - 1].nCount - flBuf[flBuf.size() - 1].s2, 2)) / (sqrtf(powf(flBuf[flBuf.size() - 1].nCount, 2)) + sqrtf(powf(flBuf[flBuf.size() - 1].s2, 2))));
			}
			if (predictAlgorithm == THREE_EXPONENTIAL_SMOOTHING)
			{
				temp_answer = (sqrtf(powf(flBuf[flBuf.size() - 1].nCount - flBuf[flBuf.size() - 1].s3, 2)) / (sqrtf(powf(flBuf[flBuf.size() - 1].nCount, 2)) + sqrtf(powf(flBuf[flBuf.size() - 1].s3, 2))));
			}
			if (temp_answer < min_answer)
			{//�����μ��������ϴκã��򱣴���õĽ����Ӧ��alpha����ý��
				min_answer = temp_answer;
				optimum_alpha = tempAlpha;

				if (predictAlgorithm == SINGLE_EXPONENTIAL_SMOOTHING)
				{
					errors = flBuf[flBuf.size() - 1].nCount - flBuf[flBuf.size() - 1].s1;
				}
				if (predictAlgorithm == SECOND_EXPONENTIAL_SMOOTHING)
				{
					errors = flBuf[flBuf.size() - 1].nCount - flBuf[flBuf.size() - 1].s2;
				}
				if (predictAlgorithm == THREE_EXPONENTIAL_SMOOTHING)
				{
					errors = flBuf[flBuf.size() - 1].nCount - flBuf[flBuf.size() - 1].s3;
				}
			}
			//�Աȵ�ǰalpha�Ľ��������alpha�Ľ��������ǰalpha������ţ��򽫵�ǰalpha�϶�Ϊ����alpha
		}
		if (predictAlgorithm == SINGLE_EXPONENTIAL_SMOOTHING)
		{//�������ŵ�alpha
			m_alphaList_s1.push_back(optimum_alpha);
		}
		if (predictAlgorithm == SECOND_EXPONENTIAL_SMOOTHING)
		{
			m_alphaList_s2.push_back(optimum_alpha);
		}
		if (predictAlgorithm == THREE_EXPONENTIAL_SMOOTHING)
		{
			m_alphaList_s3.push_back(optimum_alpha);
		}

		if (predictAlgorithm == SINGLE_EXPONENTIAL_SMOOTHING)
		{//�������ŵ�errors
			m_errorsList_s1.push_back(errors);
		}
		if (predictAlgorithm == SECOND_EXPONENTIAL_SMOOTHING)
		{
			m_errorsList_s2.push_back(errors);
		}
		if (predictAlgorithm == THREE_EXPONENTIAL_SMOOTHING)
		{
			m_errorsList_s3.push_back(errors);
		}
	} while (m_dc->getNextFlavorListFromTimeQuantum(flBuf));
}
