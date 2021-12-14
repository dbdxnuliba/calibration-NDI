#include "ReadData.h"

ReadData::ReadData() {

}

//��ȡ��е�۵�����
//ǰ�ĸ������� �ռ����̬ ʹ����Ԫ������ʾ
//������������ �ռ��е�λ�� x y z
void ReadData::readNDIData(std::string filename, Eigen::MatrixXd& matrix)
{
	std::string lineStr;
	std::vector<std::vector<std::string>> table;
	std::ifstream file(filename.c_str());

	while (getline(file, lineStr))
	{
		std::stringstream ss(lineStr);
		std::string elements;
		std::vector<std::string> line;

		while (getline(ss, elements, ','))
		{
			line.push_back(elements);
		}
		table.push_back(line);
	}

	matrix = Eigen::MatrixXd(table.size(), 7);

	for (int i = 0; i < table.size(); i++)
	{
		for (int j = 0; j < 7; j++)
		{
			double string2double = std::stod(table[i][j].c_str());
			matrix(i, j) = string2double;
		}
	}

}

//��ȡ��е�۵�����
//ǰ���������� �ռ��λ��
//������������ �ռ��е���̬ ʹ����ǵķ�������ʾ
void ReadData::readRobotData(std::string filename, Eigen::MatrixXd& matrix)
{
	std::string lineStr;
	std::vector<std::vector<std::string>> table;
	std::ifstream file(filename.c_str());

	while (getline(file, lineStr))
	{
		std::stringstream ss(lineStr);
		std::string elements;
		std::vector<std::string> line;

		while (getline(ss, elements, ','))
		{
			line.push_back(elements);
		}
		table.push_back(line);
	}

	matrix = Eigen::MatrixXd(table.size(), 6);

	for (int i = 0; i < table.size(); i++)
	{
		for (int j = 0; j < 6; j++)
		{
			double string2double = std::stod(table[i][j].c_str());
			matrix(i, j) = string2double;
		}
	}
}

void ReadData::readRobotDHParam(std::string filename, Eigen::MatrixXd& matrix) {
	std::string lineStr;
	std::vector<std::vector<double>> table;
	std::ifstream file(filename.c_str());
	while (getline(file, lineStr))
	{
		std::stringstream ss(lineStr);
		std::string elements;
		std::vector<double> line;
		while (getline(ss, elements, ','))
		{
			double string2double = std::stod(elements.c_str());
			line.push_back(string2double);
		}
		table.push_back(line);
	}
	for (int i = 0; i < 6; i++) {
		std::vector<double> param = table[i];
		for (int j = 0; j < 4; j++) {
			matrix(i, j) = param[j];
		}
	}
}

void ReadData::readInitEffectorMarker(std::string filename, Eigen::Matrix4d & matrix) {
	std::string lineStr;
	std::vector<std::vector<std::string>> table;
	std::ifstream file(filename.c_str());

	while (getline(file, lineStr))
	{
		std::stringstream ss(lineStr);
		std::string elements;
		std::vector<std::string> line;

		while (getline(ss, elements, ','))
		{
			line.push_back(elements);
		}
		table.push_back(line);
	}

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			double string2double = std::stod(table[i][j].c_str());
			matrix(i, j) = string2double;
		}
	}
}