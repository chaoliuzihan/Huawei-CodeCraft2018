#include "predict.h"
#include "iostream"
#include "define.h"
#include "DataCenter.h"
#include "PredictAlgorithm.h"
#include "DeployAlgorithm.h"

extern void write_result(const char *const buff, const char *const filename);

//��Ҫ��ɵĹ��������
void predict_server(char *info[MAX_INFO_NUM], char *data[MAX_DATA_NUM], int data_num, char *filename)
{
	/*- ------------------------------------------------------------------------------ -/
	 *- ���ݱ���Ҫ��������¼�����(����ʵ��ϸ�ڣ�����������������л��㷨��          -*
	 *-                                                                                -*
	 *- ��Ĺ������£�                                                                 -*
	 *-                                                                                -*
	 *- DataCenter��(����������)������¹���:                                          -*
	 *-             1.��ȡ����                                                         -*
	 *-             2.�ָ�����                                                         -*
	 *-             3.Ϊ�������ṩ�ӿ�                                                 -*
	 *-                                                                                -*
	 *- PredictAlgorithm��(Ԥ���㷨��)������¹���:                                    -*
	 *-             1.�����������ĵķָ���ִ��Ԥ��                                   -*
	 *-             2.���У׼                                                         -*
	 *-             ��ע:PredictAlgorithm��һ���㷨�ֿ��࣬�������ù���                -*
	 *-                  Ԥ���㷨���ܵ������У�ʵ������ҷ����л�Ԥ���㷨��            -*
	 *-                                                                                -*
	 *- DeployAlgorithm��(�����㷨��)������¹��ܣ�                                    -*
	 *-             1.����Ԥ���㷨���Ԥ������������Ԥ�������Ҫ����������𵽷����� -*
	 *-             ��ע��DeployAlgorithm��һ�������㷨�ֿ��࣬�������ù���            -*
	 *-                   �����㷨���ܵ������У�ʵ������ҷ����л�Ԥ���㷨��           -*
	 *-                   ���ݱ���Ҫ����Ż������Զ�ѡ�����㷨����ò����㷨��       -*
	 *-                   �����ŵĽ�����ء�                                           -*
	 *- ------------------------------------------------------------------------------ -*/

	std::string result;
	//�����������Ķ���ͬʱ���ñ�Ҫ�Ĳ���
	DataCenter dc(info, data, data_num, DataCenter::SLACK_TIME_QUANTUM); //�ɳ� 7��
	//�������Ŀ�ʼ������������
	if (dc.start())
	{
		//����Ԥ���㷨����
		PredictAlgorithm pa;
		//����Ԥ���㷨�����Ҫ�Ĳ���
		pa.setDataCenterObject(dc);
		pa.setPredictAlgorithm(PredictAlgorithm::AVERAGE);  //����ָ��ƽ��
		pa.setWeightModel(PredictAlgorithm::AUTO_WEIGHT); //ָ��ƽ���Զ�����
		//pa.setWeight(0.57, 0.4, 0.5);
		//pa.setPredictMethod(PredictAlgorithm::DAY); FIXED_ERRORS
		//pa.setErrorsModel(PredictAlgorithm::DYNAMIC_ERRORS);
		pa.setErrorsModel(PredictAlgorithm::FIXED_ERRORS);  //�趨��ʽ
		pa.setErrorsFixed(60);
		//��ʼԤ��
		pa.startPredict();
		//��ȡԤ����

		//���������㷨����ͬʱ���ñ�Ҫ�Ĳ���
		DeployAlgorithm da(pa, dc, DeployAlgorithm::GREEDY_ALGORITHM);
		//��ʼ����
		da.startDeploy(); 
		//��ȡ������
		result += da.getDeployResultStr();

		std::cout << result << std::endl;
	}
	const char *result_file = result.c_str();
	// ֱ�ӵ�������ļ��ķ��������ָ���ļ���(ps��ע���ʽ����ȷ�ԣ�����н⣬��һ��ֻ��һ�����ݣ��ڶ���Ϊ�գ������п�ʼ���Ǿ�������ݣ�����֮����һ���ո�ָ���)
	write_result(result_file, filename);
}
