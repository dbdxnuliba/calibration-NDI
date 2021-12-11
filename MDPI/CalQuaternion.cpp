#include "CalQuaternion.h"

Calibration::Calibration() {
	point_num = 0;
}

void Calibration::set_point_num(int point_num)
{
	this->point_num = point_num;
}

Eigen::Matrix4d Calibration::RobotDHMatrixAndJointAngle(std::vector<std::vector<double>> param_list, std::vector<double> jointAngle) {
	size_t param_list_num = param_list.size();
	std::vector<std::vector<double>> param_list_new;
	size_t joint_angle_num = jointAngle.size();
	if (param_list_num == joint_angle_num) {
		for (size_t i = 0; i < param_list_num; i++)
		{
			std::vector<double> param = param_list[i];
			size_t param_num = param.size();
			if (param_num == 4) {
				//theta �Ǽ��Ϲؽ���ת�ĽǶ�
				param[0] = param[0] + jointAngle[i];
			}
			param_list_new.push_back(param);
		}
	}
	Eigen::Matrix4d trans_matrix;
	trans_matrix = RobotDHMatrix(param_list_new);
	return trans_matrix;
}

Eigen::Matrix4d Calibration::RobotDHMatrix(std::vector<std::vector<double>> param_list) {
	Eigen::Matrix4d Robot_trans;
	Robot_trans.setIdentity();
	size_t param_list_num = param_list.size();
	for (int i = 0; i < param_list_num; i++) {
		//ȡ�����е�һ��DH������ת��ת������
		std::vector<double> param = param_list[i];
		Eigen::Matrix4d DHMatrix;
		DHMatrix.setIdentity();
		size_t param_num = param.size();
		if (param_num == 4) {
			DHMatrix = DHParam2Matrix(param);
			Robot_trans = Robot_trans * DHMatrix;
		}
	}
	return Robot_trans;
}

Eigen::Matrix4d Calibration::DHParam2Matrix(std::vector<double> param) {
	double theta = param[0];
	double dis = param[1];
	double alpha = param[2];
	double a_dis = param[3];
	Eigen::Matrix4d trans_matrix;
	//����Z����ת�ľ���
	Eigen::Matrix4d Rot_z_theta;
	Rot_z_theta.setIdentity();
	Rot_z_theta(0, 0) = cos(theta);
	Rot_z_theta(1, 0) = sin(theta);
	Rot_z_theta(0, 1) = -sin(theta);
	Rot_z_theta(1, 1) = cos(theta);
	//����Z��ƽ�Ƶľ���
	Eigen::Matrix4d trans_z_d;
	trans_z_d.setIdentity();
	trans_z_d(2, 3) = dis;
	//����X����ת�ľ���
	Eigen::Matrix4d Rot_x_alpha;
	Rot_x_alpha.setIdentity();
	Rot_x_alpha(1, 1) = cos(alpha);
	Rot_x_alpha(2, 1) = sin(alpha);
	Rot_x_alpha(1, 2) = -sin(alpha);
	Rot_x_alpha(2, 2) = cos(alpha);
	//����X��ƽ�Ƶľ���
	Eigen::Matrix4d trans_x_dis;
	trans_x_dis.setIdentity();
	trans_x_dis(0, 3) = a_dis;
	trans_matrix = Rot_z_theta * trans_z_d * Rot_x_alpha * trans_x_dis;
	return trans_matrix;
}

Eigen::Matrix4d Calibration::calculateQuaternionQr(Eigen::Vector4d quaternionR) {
	Eigen::Matrix4d matrix;
	Eigen::Vector3d threeVector = quaternionR.block(0, 0, 1, 3);
	matrix.block(0, 0, 3, 3) = quaternionR[3] * Eigen::Matrix3d::Identity() + calculateSkewSymMatrix(threeVector);
	matrix.block(0, 3, 3, 1) = threeVector;
	matrix.block(3, 0, 1, 3) = -1 * threeVector.transpose();
	matrix(3, 3) = quaternionR[3];
	return matrix;
}

Eigen::Matrix4d Calibration::calculateQuaternionWr(Eigen::Vector4d quaternionR) {
	Eigen::Matrix4d matrix;
	Eigen::Vector3d threeVector = quaternionR.block(0, 0, 1, 3);
	matrix.block(0, 0, 3, 3) = quaternionR[3] * Eigen::Matrix3d::Identity() - calculateSkewSymMatrix(threeVector);
	matrix.block(0, 3, 3, 1) = threeVector;
	matrix.block(3, 0, 1, 3) = -1 * threeVector.transpose();
	matrix(3, 3) = quaternionR[3];
	return matrix;
}

Eigen::Matrix3d Calibration::calculateSkewSymMatrix(Eigen::Vector3d threeDVector) {
	Eigen::Matrix3d skew_symmetric_matrix;
	skew_symmetric_matrix.setZero();
	skew_symmetric_matrix(0, 1) = -1 * threeDVector[2];
	skew_symmetric_matrix(0, 2) = threeDVector[1];
	skew_symmetric_matrix(1, 0) = threeDVector[2];
	skew_symmetric_matrix(1, 2) = -1 * threeDVector[0];
	skew_symmetric_matrix(2, 0) = -1 * threeDVector[1];
	skew_symmetric_matrix(2, 1) = threeDVector[0];
	return skew_symmetric_matrix;
}

Eigen::VectorXd Calibration::optimalTransQuaternion(Eigen::MatrixXd robotMarkerMatrix, Eigen::MatrixXd NDIMarkerMatrix, Eigen::VectorXd vectorParam, Eigen::VectorXd pointParam, Eigen::VectorXd & optimalQuaternion)
{
	// robotMarkerMatrix NDIMarkerMatrix n*4*4
	//���Ƚ���ת�����ƽ����������
	//�������Ԫ����ƽ��ƽ��������1/2 
	Eigen::MatrixXd robotMarkerMatrixRotation = robotMarkerMatrix(Eigen::all, Eigen::seq(0, 2));
	Eigen::MatrixXd robotMarkerMatrixTranslation = 0.5 * robotMarkerMatrix(Eigen::all, Eigen::seq(3, 3));
	Eigen::MatrixXd NDIMarkerMatrixRotation = NDIMarkerMatrix(Eigen::all, Eigen::seq(0, 2));
	Eigen::MatrixXd NDIMarkerMatrixTraTranslation = 0.5 * NDIMarkerMatrix(Eigen::all, Eigen::seq(3, 3));
	//���C1
	Eigen::Matrix4d C1 = calculateC1(robotMarkerMatrixRotation, robotMarkerMatrixTranslation,
		NDIMarkerMatrixRotation, NDIMarkerMatrixTraTranslation,
		vectorParam, pointParam);
	//���C2
	Eigen::Matrix4d C2 = calculateC2(pointParam);
	//���C3
	Eigen::Matrix4d C3 = calculateC3(robotMarkerMatrixTranslation, NDIMarkerMatrixTraTranslation,
		pointParam);
	//��ⳣ��
	double constant = calculateConst(robotMarkerMatrixTranslation, NDIMarkerMatrixTraTranslation,
		vectorParam, pointParam);
	//�������A �Լ���������ֵ���������� ������������������Ҫ���r
	Eigen::Matrix4d A = 0.5*(C3.transpose()*(C2 + C2.transpose()).inverse()*C3 - C1 - C1.transpose());
	double max_eigen_value;
	Eigen::Vector4d max_eigen_vector;
	calculateMaxEigenValueAndVector(A, max_eigen_value, max_eigen_vector);
	Eigen::Vector4d calculate_s = -(C2 + C2.transpose()).inverse()*C3*max_eigen_vector;
	//������õĶ�ż��Ԫ��
	optimalQuaternion(Eigen::seq(0, 3), Eigen::all) = max_eigen_vector;
	optimalQuaternion(Eigen::seq(4, 8), Eigen::all) = calculate_s;
	return Eigen::VectorXd();
}

Eigen::Matrix4d Calibration::calculateC1(Eigen::MatrixXd robotMarkerMatrixRotation, Eigen::MatrixXd robotMarkerMatrixTranslation,
	Eigen::MatrixXd NDIMarkerMatrixRotation, Eigen::MatrixXd NDIMarkerMatrixTranslation,
	Eigen::VectorXd vectorParam, Eigen::VectorXd pointParam)
{
	Eigen::Matrix4d C1;
	Eigen::Matrix4d C1_previous;
	C1_previous.setZero();
	Eigen::Matrix4d C1_next;
	C1_next.setZero();
	for (int i = 0; i < point_num; i++) {
		//ȡ�������˻������Ӧ������
		Eigen::Vector4d robotX_vector = robotMarkerMatrixRotation.block(4 * i, 0, 4, 1);
		Eigen::Vector4d robotY_vector = robotMarkerMatrixRotation.block(4 * i, 1, 4, 1);
		Eigen::Vector4d robotZ_vector = robotMarkerMatrixRotation.block(4 * i, 2, 4, 1);
		//ȡ�������˶�Ӧ��ƫ����
		Eigen::Vector4d robot_translation_vector = robotMarkerMatrixTranslation.block(4 * i, 0, 4, 1);
		//ȡ��NDI����ϵ�¶�Ӧ������
		Eigen::Vector4d NDIX_vector = NDIMarkerMatrixRotation.block(4 * i, 0, 4, 1);
		Eigen::Vector4d NDIY_vector = NDIMarkerMatrixRotation.block(4 * i, 1, 4, 1);
		Eigen::Vector4d NDIZ_vector = NDIMarkerMatrixRotation.block(4 * i, 2, 4, 1);
		//ȡ��NDI����ϵ�¶�Ӧ��ƫ����
		Eigen::Vector4d NDI_translation_vector = NDIMarkerMatrixTranslation.block(4 * i, 0, 4, 1);
		C1_previous = C1_previous - vectorParam[3 * i]*calculateQuaternionQr(NDIX_vector).transpose() * calculateQuaternionWr(robotX_vector);
		C1_previous = C1_previous - vectorParam[3 * i + 1]*calculateQuaternionQr(NDIY_vector).transpose() * calculateQuaternionWr(robotY_vector);
		C1_previous = C1_previous - vectorParam[3 * i + 2] * calculateQuaternionQr(NDIZ_vector).transpose() * calculateQuaternionWr(robotZ_vector);
		C1_next = C1_next - pointParam[i] * calculateQuaternionQr(NDI_translation_vector).transpose() * calculateQuaternionWr(robot_translation_vector);
	}
	C1 = 2 * (C1_previous + C1_next);
	return C1;
}

Eigen::Matrix4d Calibration::calculateC2(Eigen::VectorXd pointParam)
{
	Eigen::Matrix4d C2;
	C2.setZero();
	double sum = 0;
	for (int i = 0; i < point_num; i++) {
		sum += pointParam[i];
	}
	C2 = sum * Eigen::Matrix4d::Identity();
	return C2;
}

Eigen::Matrix4d Calibration::calculateC3(Eigen::MatrixXd robotMarkerMatrixTranslation, Eigen::MatrixXd NDIMarkerMatrixTranslation, Eigen::VectorXd pointParam)
{
	Eigen::Matrix4d C3;
	C3.setZero();
	for (int i = 0; i < point_num; i++) {
		//ȡ�������˶�Ӧ��ƫ����
		Eigen::Vector4d robot_translation_vector = robotMarkerMatrixTranslation.block(4 * i, 0, 4, 1);
		//ȡ��NDI�����Ӧ��ƫ����
		Eigen::Vector4d NDI_translation_vector = NDIMarkerMatrixTranslation.block(4 * i, 0, 4, 1);
		C3 = C3 + pointParam[i] * (calculateQuaternionWr(robot_translation_vector) - calculateQuaternionQr(NDI_translation_vector));
	}
	C3 = 2 * C3;
	return C3;
}

double Calibration::calculateConst(Eigen::MatrixXd robotMarkerMatrixTranslation, Eigen::MatrixXd NDIMarkerMatrixTranslation, Eigen::VectorXd vectorParam, Eigen::VectorXd pointParam)
{
	double constant = 0;
	for (int i = 0; i < point_num; i++) {
		constant += 2 * (vectorParam[3 * i] + vectorParam[3 * i + 1] + vectorParam[3 * i + 2]);
		//ȡ�������˶�Ӧ��ƫ����
		Eigen::Vector4d robot_translation_vector = robotMarkerMatrixTranslation.block(4 * i, 0, 4, 1);
		//ȡ��NDI�����Ӧ��ƫ����
		Eigen::Vector4d NDI_translation_vector = NDIMarkerMatrixTranslation.block(4 * i, 0, 4, 1);
		constant += pointParam[i] * (robot_translation_vector.dot(robot_translation_vector) + NDI_translation_vector.dot(NDI_translation_vector));
	}
	return constant;
}

void Calibration::calculateMaxEigenValueAndVector(Eigen::Matrix4d matrix, double& max_value, Eigen::Vector4d& corr_vector) {
	//���A������ֵ
	Eigen::EigenSolver<Eigen::Matrix4d> eigen_solver(matrix);
	//����ֵ ����ʵ�����鲿 4*1
	Eigen::MatrixXcd eigen_values = eigen_solver.eigenvalues();
	//�������� ����ʵ�����鲿 4*4
	Eigen::MatrixXcd eigen_vector = eigen_solver.eigenvectors();
	//����������ʵ��
	Eigen::VectorXd eigen_values_real = eigen_values.real();
	//����ֵ����λ��
	Eigen::MatrixXf::Index eigen_value_max_index;
	eigen_values_real.rowwise().sum().maxCoeff(&eigen_value_max_index);
	max_value = eigen_values_real[eigen_value_max_index];
	//������������
	Eigen::Vector4d q_vector;
	//�õ���������ֵ
	q_vector << eigen_vector.real()(0, eigen_value_max_index), eigen_vector.real()(1, eigen_value_max_index),
		eigen_vector.real()(2, eigen_value_max_index), eigen_vector.real()(3, eigen_value_max_index);
	corr_vector = q_vector;
}

Eigen::MatrixXd Calibration::RobotToTransformationMatrix(Eigen::MatrixXd robotMaxtrix) {
	int rows = robotMaxtrix.rows();
	int cols = robotMaxtrix.cols();
	Eigen::MatrixXd translationMaxtrix(rows, 3);
	translationMaxtrix = robotMaxtrix.block(0, 0, rows, 3);

	Eigen::MatrixXd tranaformationMatrix(4 * rows, 4);
	tranaformationMatrix.setZero();
	for (int i = 0; i < rows; i++) {
		Eigen::Vector3d vector_tmp = robotMaxtrix.block(i, 3, 1, 3).transpose();
		double angle = sqrt(vector_tmp.transpose() * vector_tmp);
		Eigen::AngleAxisd angleaxis;
		if (angle == 0) {
			angleaxis = Eigen::AngleAxisd(0, Eigen::Vector3d(0, 0, 1));
		}
		else
		{
			angleaxis = Eigen::AngleAxisd(angle, vector_tmp / angle);
		}
		Eigen::Matrix3d rotation = angleaxis.matrix();
		tranaformationMatrix.block(4 * i, 0, 3, 3) = rotation;
		tranaformationMatrix.block(4 * i, 3, 3, 1) = translationMaxtrix.block(i, 0, 1, 3).transpose();
		tranaformationMatrix(4 * i + 3, 3) = 1.0;
	}
	return tranaformationMatrix;
}

Eigen::MatrixXd Calibration::NDIToTransformationMatrix(Eigen::MatrixXd NDIMaxtrix) {
	int rows = NDIMaxtrix.rows();
	int cols = NDIMaxtrix.cols();
	Eigen::MatrixXd translationMaxtrix(rows, 3);
	translationMaxtrix = NDIMaxtrix.block(0, 4, rows, 3);
	//���յõ��� 4*n * 4�ľ���
	Eigen::MatrixXd tranaformationMatrix(4 * rows, 4);
	tranaformationMatrix.setZero();
	for (int i = 0; i < rows; i++) {
		//�ӵ� 0 �п�ʼ�� �� 3 �� ����Ԫ���ı�ʾ
		Eigen::Vector4d quaternion_vector = NDIMaxtrix.block(i, 0, 1, 4).transpose();
		Eigen::Quaternion<double> quaternion_tmp = Eigen::Quaternion<double>(quaternion_vector);
		Eigen::Matrix3d rotation = quaternion_tmp.matrix();
		tranaformationMatrix.block(4 * i, 0, 3, 3) = rotation;
		tranaformationMatrix.block(4 * i, 3, 3, 1) = translationMaxtrix.block(i, 0, 1, 3).transpose();
		tranaformationMatrix(4 * i + 3, 3) = 1.0;
	}
	return tranaformationMatrix;
}