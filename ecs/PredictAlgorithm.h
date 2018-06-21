#ifndef PREDICT_ALGORITHM_H_
#define PREDICT_ALGORITHM_H_
#include "define.h"

class DataCenter;

class PredictAlgorithm
{
public:
	friend class DeployAlgorithm;
public:
	enum {//Algorithm
		SINGLE_EXPONENTIAL_SMOOTHING, //һ��ָ��ƽ��
		SECOND_EXPONENTIAL_SMOOTHING, //����ָ��ƽ��
		THREE_EXPONENTIAL_SMOOTHING,  //����ָ��ƽ��
		LINEAR_REGRESSION,            //���Իع�
		AVERAGE                       //ƽ��
	};
	enum {//�����ڷ�ʽ
		FIXED_ERRORS,                 //�̶����
		DYNAMIC_ERRORS,               //��̬���
	};
	enum {//Ԥ�ⷽʽ
		DAY,                          //����Ԥ��, ��Ҫ����Ԥ�⣬���ݷָ����Ҳ����ָ�
		TIME_QUANTUM                  //��ʱ������Ԥ��
	};
	enum {//�Ƿ����Զ�����
		AUTO_WEIGHT,
		MANUAL_WEIGHT
	};

	PredictAlgorithm();
	~PredictAlgorithm();
	void setDataCenterObject(DataCenter & dc);
	//����Ԥ���㷨 SINGLE_EXPONENTIAL_SMOOTHING/SECOND_EXPONENTIAL_SMOOTHING/THREE_EXPONENTIAL_SMOOTHING
	void setPredictAlgorithm(int predict_model = SECOND_EXPONENTIAL_SMOOTHING);
	void startPredict();
	const std::string & getError();//���ش�����Ϣ
	
	/*********************************ָ��ƽ���㷨***********************************/
	//����ƽ��Ȩ��
	void setWeight(double weight_s1 = 0.57, double weight_s2 = 0.4, double weight_s3 = 0.0);
	
	void setWeightModel(int weightModel = AUTO_WEIGHT);
	//���ù̶����
	void setErrorsFixed(int errorsFixed = 20);

	//����������Ȩ��
	void setErrorsWeight(double errorsWeight = 0.8);
	/*********************************ָ��ƽ���㷨***********************************/

	//������ģʽ FIXED_ERRORS or DYNAMIC_ERRORS
	void setErrorsModel(int errorsModel = FIXED_ERRORS);

	//����Ԥ�ⷽ�� DAY or TIME_QUANTUM
	void setPredictMethod(int predictMethod);

	int getPredictAlgorithm();

	//��ȡ����Ԥ�����ַ���
	std::string getResultStr();

private:

	//����ָ������ҪԤ�ⲿ�ֵ�ָ��ƽ��ֵ
	void calcCommonExponentialSmoothing();

	//����ָ������ҪԤ�ⲿ�ֵ�ָ��ƽ��ֵ(�Զ�����)
	void calcCommonExponentialSmoothing_AUTO_WEIGHT(std::vector<Flavor> & flBuf, int predictMethod, float weight);


	void calcCommonExponentialSmoothing_AUTO_WEIGHT();

	//���㹫������S1
	void calcCommonS1();

	void calcWeight(int predictAlgorithm);


#ifdef _DEBUG
	void showPredictResult_s1();
#endif

private:
	//Ԥ��
	void predict_SINGLE_EXPONENTIAL_SMOOTHING();
	void predict_SECOND_EXPONENTIAL_SMOOTHING();
	void predict_THREE_EXPONENTIAL_SMOOTHING();

	//�Զ�����
	void predict_SINGLE_EXPONENTIAL_SMOOTHING_AUTO_WEIGHT();
	void predict_SECOND_EXPONENTIAL_SMOOTHING_AUTO_WEIGHT();
	void predict_THREE_EXPONENTIAL_SMOOTHING_AUTO_WEIGHT();
	void predict_AVERAGE();

	void predict_LINEAR_REGRESSION();
	//���Իع��Ӻ���

	//�����ݶ�
	float calcGradient(const std::vector<std::vector<float>> & trainData, const std::vector<float> & feature, int nAlpha, int nDimensions);

	//����Ԥ�⺯��
	float calcPredictFunction(const std::vector<float> & cur_trainData, const std::vector<float> & feature, int nDimensions);

	//���� �ݶ�
	void normalize(std::vector<float> & feature, int nDimensions);

	//���� alpha
	float updateAlpha(const std::vector<std::vector<float>> & trainData, const std::vector<float> & feature, std::vector<float>  & featureGradient, float nAlpha, int nDimensions);

	bool isSame(std::vector<float> &feature, std::vector<float> &nextFeature, int nDimensions);
	
	float dotProduct(std::vector<float> & featureGradient, std::vector<float> & featureGradient_1, int nDimensions);
	
	float lossFunction(const std::vector<float> &feature,const  std::vector<std::vector<float>> &trainData, int nDimensions);
	//���Իع��Ӻ���

	//���У׼
	void errorCalibration();

	//ͳ��Ԥ����
	void statisticalPredictResults();

	//����ͳ�ƽ���ַ����������ʱ����õ�
	const std::vector<Flavor> & getPredictResultVector();

	void cutPredictHeader();
	void cutPredictTail();
private:
	DataCenter * m_dc;
	int m_predictAlgorithm;//Ԥ���㷨
	std::string m_error;//������Ϣ
	double m_weight_s1;//һ��ƽ��Ȩ��
	double m_weight_s2;//����ƽ��Ȩ��
	double m_weight_s3;//����ƽ��Ȩ��
	int m_errorsFixed; //�̶����ֵ
	double m_errorsWeight;//��̬���Ȩ��
	int m_errorsModel;//�����ڷ�ʽ
	int m_predictMethod;//Ԥ�ⷽ��

	int m_weightModel;//�Ƿ����Զ�����

	int m_TimeSpan;//Ԥ�⿪ʼʱ����ѵ�����ݽ���ʱ��֮��Ŀ��

	std::vector<std::vector<Flavor>> m_PredictedFlavor;//Ԥ�����õĹ���б��Լ�ÿ����������
	
	//Ԥ��������
	std::vector<Flavor> m_PredictResult;
	int nTotalPredictCount;//Ԥ����������������

	//�Զ��������
	std::vector<float> m_alphaList_s1;//һ��ָ��ƽ�������б�
	std::vector<float> m_alphaList_s2;//����ָ��ƽ�������б�
	std::vector<float> m_alphaList_s3;//����ָ��ƽ�������б�

	std::vector<float> m_errorsList_s1;//����б�
	std::vector<float> m_errorsList_s2;//����б�
	std::vector<float> m_errorsList_s3;//����б�

	std::vector<double> flavorProportionList;

};

#endif
