#pragma once

#include<string>
#include<Eigen/Eigen>
#include<fstream>

class ReadData
{
public:
	ReadData();

	//��ȡNDI��õ�����ϵ����
	void readNDIData(std::string filename, Eigen::MatrixXd& matrix);
	//��ȡ������ĩ�˻�õ�����ϵ���� ��������
	//Ҳ����������ȡ�����˵�ÿ����̬�Ĺؽڽǣ� Ҳ����������
	void readRobotData(std::string filename, Eigen::MatrixXd& matrix);
	//��ȡ�����˵�DH����
	void readRobotDHParam(std::string filename, Eigen::MatrixXd & matrix);
	//��ȡ�����˵�ĩ�˺�marker֮���ת����ϵ
	void readInitEffectorMarker(std::string filename, Eigen::Matrix4d & matrix);
};

