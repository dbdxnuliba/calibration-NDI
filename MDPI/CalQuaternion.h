#pragma once
#include<Eigen/Eigen>
#include<iostream>

class Calibration
{
private:
	//������¼��ĸ���
	int point_num;
	//���DH������
	Eigen::MatrixXd DH_list;
	Eigen::Matrix4d Vet;
	Eigen::Matrix4d Vob;
	//������ƫ����������
	double delta;

	Eigen::VectorXd theta_increment;
	Eigen::VectorXd d_increment;
	Eigen::VectorXd a_increment;

public:
	Calibration();

	void set_point_num(int point_num);
	//��DH�����ŵ���Ա��������
	void setNorminalDHParam(Eigen::MatrixXd DHParam_list);
	//�����ļ��ж�ȡ��Marker����ڻ�����ĩ�˵�����ϵ����
	void setVet(Eigen::Matrix4d Vet);
	//������õ���Vob��������
	void setVob(Eigen::Matrix4d Vob);

	//������UR��е�۵�ÿ���ؽڽǶȣ��൱�ڵ�һ�м����˹ؽڽǶ�
	//�����γ��˻�����ĩ������ϵ�ı�ʾ
	Eigen::Matrix4d RobotDHMatrixAndJointAngle(Eigen::VectorXd jointAngle);
	//���λ�˶�Ӧ�Ĺؽڽǵõ���ĩ������ϵ
	Eigen::MatrixXd RobotDHMatrixAndMultiJointAngle(Eigen::MatrixXd jointAngle_list);
	//��ʼ��̬��DH���� �����е�d������������
	Eigen::Matrix4d RobotDHMatrixAndDParam(Eigen::VectorXd dParam);
	//��ʼ��̬��DH���� �����е�a������������
	Eigen::Matrix4d RobotDHMatrixAndAParam(Eigen::VectorXd aParam);
	//
	Eigen::Matrix4d RobotDHMatrixJointAngleAndDParam(Eigen::VectorXd jointAngle, Eigen::VectorXd dParam);
	//
	Eigen::Matrix4d RobotDHMatrixJointAngleAndAParam(Eigen::VectorXd jointAngle, Eigen::VectorXd aParam);

	//����D-H�����������ڹؽڵ�ת������
	// param ����˳���� theta dis alpha a_dis
	Eigen::Matrix4d DHParam2Matrix(Eigen::VectorXd);
	//���DH�����γ��˻����˵�ĩ������ϵ
	Eigen::Matrix4d RobotDHMatrix(Eigen::MatrixXd param_list);

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
	//�����˵�����������ʾ������ϵ ת���� 4*4 �任����
	Eigen::MatrixXd RobotToTransformationMatrix(Eigen::MatrixXd robotMaxtrix);
	//NDI���߸�������ʾ������ϵ ת���� 4*4 �任���� ǰ�ĸ�����̬ �������ǿռ�λ��
	Eigen::MatrixXd NDIToTransformationMatrix(Eigen::MatrixXd NDIMaxtrix);
	//��ż��Ԫ��ת����4*4��ת������
	Eigen::Matrix4d DualQuaternion2Matrix(Eigen::VectorXd dualQuaternion);
	//ת�������� x y z alpha beta gama
	Eigen::VectorXd matrix2XYZEulerAngle(Eigen::Matrix4d transform_matrixd);

	//���㵥������ؽڽǵ��Ÿ���ϵ������ ��������ֻ����x y z alpha beta gama 6����� �����Ŷ���12������ 
	//��˷���ֵ�� 6*12 �ľ���
	Eigen::MatrixXd calculateOnePointJacobiMatrix(Eigen::VectorXd joint_angle);
	//��������λ���Ÿ���ϵ������
	Eigen::MatrixXd calculateMultiPointJacobiMatrix(Eigen::MatrixXd joint_angle);
	//��������λ����� NDI��õ��ͨ��Vob��������ĵ�Ƚ�
	Eigen::VectorXd calculateMultiPointDifference(Eigen::MatrixXd robot_marker_matrix, Eigen::MatrixXd NDI_matrix);
	//�����������
	void calculateIncrement(Eigen::MatrixXd joint_angle, Eigen::MatrixXd robot_marker_matrix, Eigen::MatrixXd NDI_matrix);
};

