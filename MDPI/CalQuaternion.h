#pragma once
#include<Eigen/Eigen>

class Calibration
{
private:
	//������¼��ĸ���
	int point_num;
public:
	Calibration();

	void set_point_num(int point_num);
	//��һ��������DH�Ĳ����� �ڶ���������UR��е�۵�ÿ���ؽڽǶ�
	//�����γ��˻�����ĩ������ϵ�ı�ʾ
	Eigen::Matrix4d RobotDHMatrixAndJointAngle(std::vector<std::vector<double>> param_list, std::vector<double> jointAngle);
	//����D-H�����������ڹؽڵ�ת������
	// param ����˳���� theta dis alpha a_dis
	Eigen::Matrix4d DHParam2Matrix(std::vector<double> param);
	//���DH�����γ��˻����˵�ĩ������ϵ
	Eigen::Matrix4d RobotDHMatrix(std::vector<std::vector<double>> param_list);

	//���������е�Q(r)��ʾ������
	//������ʾ��Ԫ��������
	Eigen::Matrix4d calculateQuaternionQr(Eigen::Vector4d quaternionR);

	//���������е�W(r)��ʾ������
	//������ʾ��Ԫ��������
	Eigen::Matrix4d calculateQuaternionWr(Eigen::Vector4d quaternionR);
	//������ά�����ķ��Գƾ���
	Eigen::Matrix3d calculateSkewSymMatrix(Eigen::Vector3d threeDVector);

	//����������ϵ��ͳһ
	//��һ������marker����ڻ�е�ۻ���������ϵ��ʾ n*4*4��ת������ n�ǵ�ĸ���
	//�ڶ�������marker��NDI�������µ�����ϵ��ʾ n*4*4��ת������ n�ǵ�ĸ���
	//������������ ÿ���Է��������Ĳ��� 
	//������������ ÿ����Ĳ���
	//����ֵ��������Ԫ�� ��һ����R �ڶ�����S
	Eigen::VectorXd optimalTransQuaternion(Eigen::MatrixXd robotMarkerMatrix, Eigen::MatrixXd NDIMarkerMatrix, 
		Eigen::VectorXd vectorParam, Eigen::VectorXd pointParam, Eigen::VectorXd& optimalQuaternion);
	
	//���C1�ľ���
	Eigen::Matrix4d calculateC1(Eigen::MatrixXd robotMarkerMatrixRotation, Eigen::MatrixXd robotMarkerMatrixTranslation,
								Eigen::MatrixXd NDIMarkerMatrixRotation, Eigen::MatrixXd NDIMarkerMatrixTranslation,
								Eigen::VectorXd vectorParam, Eigen::VectorXd pointParam);
	//���C2�ľ���
	Eigen::Matrix4d calculateC2(Eigen::VectorXd pointParam);
	//���C3�ľ���
	Eigen::Matrix4d calculateC3(Eigen::MatrixXd robotMarkerMatrixTranslation, Eigen::MatrixXd NDIMarkerMatrixTranslation, 
		Eigen::VectorXd pointParam);
	//��ⳣ��
	double calculateConst(Eigen::MatrixXd robotMarkerMatrixTranslation, Eigen::MatrixXd NDIMarkerMatrixTranslation,
		Eigen::VectorXd vectorParam, Eigen::VectorXd pointParam);
	//�����������ֵ������������
	void calculateMaxEigenValueAndVector(Eigen::Matrix4d matrix, double& max_value, Eigen::Vector4d& corr_vector);
	//�����˵�����
	Eigen::MatrixXd RobotToTransformationMatrix(Eigen::MatrixXd robotMaxtrix);

};

