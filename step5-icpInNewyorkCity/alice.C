#include <iostream>
#include <fstream>
#include <vector>
#include <numeric>
#include <sys/time.h>
#include "icp.h"
#include "Eigen/Eigen"

using namespace std;
using namespace Eigen;

//
// global variables
//
Eigen::MatrixXd A_;

int numPoints = 0;
int numFranmes = 0;

// Ground truth
vector<double> angles; // unit : rad
vector<double> axis_x; //
vector<double> axis_y; //
vector<double> axis_z; //
vector<double> xs, ys, zs; // translation

string file_pointsOriginal_ = "../../data_rasberrypi/data_buildings.txt";
string file_Rotation_Translation_VehicleFrame_ = "../../data_rasberrypi/rotation_angle_axis_translation_NewyorkCity.txt";

// Point clouds
vector<Eigen::MatrixXd> PointClouds_;

//
// functions
//
int Reading_PointCloud_OriginalFrame();
int Reading_Rotation_Translation_VehicleFrame();
int uniform(Eigen::Vector3d & axis);
Eigen::MatrixXd SetPointCloud(int FrameID);
Eigen::Matrix4d test_icp4(Eigen::MatrixXd A, Eigen::MatrixXd B);

// functions
int Reading_PointCloud_OriginalFrame()
{
	string filename = file_pointsOriginal_;
	ifstream file(filename); 

	if(file.fail())
	{
		cout<<"Can not find the file \" "<<filename<<" \""<<endl;
		return 0;
	}

	string nameTemp;
	double x, y, z;
	
	vector<double> xs, ys, zs;
	while(!file.eof())
	{
		file>>nameTemp>>x>>y>>z;
		if(file.eof()) break;
		xs.push_back(x/1e3);
		ys.push_back(y/1e3);
		zs.push_back(z/1e3);
		numPoints ++;
	}

	A_ = Eigen::MatrixXd::Random(numPoints,3);
	for(int i=0;i<numPoints;i++)
	{
		A_(i,0) = xs[i];
		A_(i,1) = ys[i];
		A_(i,2) = zs[i];
	}

	return 0;
}
	
int Reading_Rotation_Translation_VehicleFrame()
{
	string filename = file_Rotation_Translation_VehicleFrame_;
	ifstream file(filename); 

	if(file.fail())
	{
		cout<<"Can not find the file \" "<<filename<<" \""<<endl;
		return 0;
	}

	int ID = 0;
	double angle_deg = 0;
	double ax, ay, az;
	double x, y, z;

	
	while(!file.eof())
	{
		file>>ID>>angle_deg>>ax>>ay>>az>>x>>y>>z;
		if(file.eof()) break;
		angles.push_back(angle_deg/180.*M_PI);
		axis_x.push_back(ax);
		axis_y.push_back(ay);
		axis_z.push_back(az);
		xs.push_back(x);
		ys.push_back(y);
		zs.push_back(z);
		numFranmes ++;
	}

	return 0;
}

Eigen::MatrixXd SetPointCloud(int FrameID)
{
	Eigen::MatrixXd A = A_;
	Eigen::MatrixXd B;
	Eigen::MatrixXd C;
	Eigen::Vector3d t;
	Eigen::Matrix3d R;
	Eigen::Matrix4d T;

	// 
	int i = FrameID;

	//
	B = A;

	// translation vecgtor
	t << xs[i],ys[i],zs[i];

	// rotation angle, axis and matrix
	double theta = angles[i];

	Eigen::Vector3d axis(axis_x[i],axis_y[i],axis_z[i]);
	int isUnifromationGood = uniform(axis);

	Eigen::AngleAxisd rotationVector_1(theta,axis);
	R=rotationVector_1.toRotationMatrix();

	// translate the point
	for( int jj =0; jj< numPoints; jj++){
		B.block<1,3>(jj,0) = B.block<1,3>(jj,0) - t.transpose();
	}

	// rotate the point
	B = (R * B.transpose()).transpose();

	return B;
}

int uniform(Eigen::Vector3d & axis)
{
	// uniform the axis
	double norm2_squared = axis(0)*axis(0) + axis(1)*axis(1) + axis(2)*axis(2);
	double norm2 = sqrt(norm2_squared);

	axis(0) /= norm2;
	axis(1) /= norm2;
	axis(2) /= norm2;

	return 1;
}

Eigen::Matrix4d test_icp4(Eigen::MatrixXd A, Eigen::MatrixXd B)
{
	Eigen::Matrix4d T;
	ICP_OUT icp_result;

	// solve the ICP problem
	icp_result = icp(B, A, 20,  0.000001);

	// get Transformation matrix
	T = icp_result.trans;

	// get translation vector
	Eigen::Vector3d t_estimated = T.block<3,1>(0,3); // get Rotation Matrix 
	Eigen::Matrix3d R_estimated_reverse = T.block<3,3>(0,0);
	Eigen::Matrix3d R_estimated = R_estimated_reverse.transpose();

	//cout<<"\nICP result: "<<endl;
	//cout<<"Transformation Matrix: \n"<<T<<endl;
	//cout<<"Translation vector: \n"<<t_estimated.transpose()<<endl;
	//cout<<"Rotation Matrix: \n"<<R_estimated<<endl;

	// eigen rotation matrix to angleAxis
	Eigen::AngleAxisd rotationVector;
	rotationVector.fromRotationMatrix(R_estimated);

	//cout<<"Angle: "<<rotationVector.angle()/M_PI*180.<<" deg"<<endl;
	//cout<<"Axis:  "<<rotationVector.axis().transpose()<<endl;

	// 
	//Rs.push_back(R_estimated);
	//ts.push_back(t_estimated);

	Eigen::Vector3d t_inverse = -1.*R_estimated*t_estimated;

	Eigen::Matrix4d T_estimated;
	T_estimated<<	R_estimated(0,0), R_estimated(0,1), R_estimated(0,2), t_inverse(0),
					R_estimated(1,0), R_estimated(1,1), R_estimated(1,2), t_inverse(1),
					R_estimated(2,0), R_estimated(2,1), R_estimated(2,2), t_inverse(2),
					0,0,0,1;
	//Ts.push_back(T_estimated);

	//cout<<"Transformation Matrix:\n"<<T_estimated<<endl;
	//cout<<"Transformation Matrix (inverse of the result):\n"<<T.inverse()<<endl;

	//// write
	//fileTransformation<<rotationVector.angle()<<" ";
	//fileTransformation<<rotationVector.axis()(0)<<" ";
	//fileTransformation<<rotationVector.axis()(1)<<" ";
	//fileTransformation<<rotationVector.axis()(2)<<" ";
	//fileTransformation<<t_estimated(0)<<" ";
	//fileTransformation<<t_estimated(1)<<" ";
	//fileTransformation<<t_estimated(2)<<endl;

	return T_estimated;
}
	

int main(int argc, char *argv[])
{
	cout<<"Hello"<<endl;

	// A_: points in the original frame
	Reading_PointCloud_OriginalFrame();
	Reading_Rotation_Translation_VehicleFrame();

	//
	// Generate Point clouds, as the observations
	//
	for(int i=0;i<numFranmes;i++)
	{
		int FrameID = i;
		Eigen::MatrixXd A = SetPointCloud(FrameID);
		PointClouds_.push_back(A);
	}

	//// debug
	//for(int i=0;i<numFranmes;i++)
	//{
	//	cout<<"Frame ID : "<<i<<"\n"<<PointClouds_[i].block<1,3>(0,0)<<endl;
	//}
	//// !debug

	//
	// loop closure
	//
	int k_ref = 0;
	Eigen::Matrix4d T_ref_0 = Eigen::Matrix4d::Identity();
	Eigen::MatrixXd PC_ref = PointClouds_[0];

	int frameInterval = 50;
	// write rotation matrix and translation
	string writeFileName = "Transformation_AllFrames_LC_"+to_string(frameInterval)+".txt";
	ofstream fileTransformation(writeFileName);

	for(int i=0;i<numFranmes;i++)
	{
		//
		// Step 1:
		// ICP,
		// Pose Estimation
		//
		int v = i; // current frame ID
		Eigen::MatrixXd PC_cur = PointClouds_[v];
		Eigen::Matrix4d T_cur_ref = test_icp4(PC_ref, PC_cur);
		Eigen::Matrix4d T_cur_0 = T_cur_ref * T_ref_0;

		cout<<"\n----"<<endl;
		cout<<"------------"<<endl;
		cout<<"Current-Frame ID: "<<v<<endl;

		// Get rotation matrix and translation vector
		Eigen::Matrix3d R_cur_0 = T_cur_0.block<3,3>(0,0); // rotaiton matrix
		Eigen::Vector3d t_cur_0 = T_cur_0.inverse().block<3,1>(0,3); //translation vector

		// eigen rotation matrix to angleAxis
		Eigen::AngleAxisd rotationVector;
		rotationVector.fromRotationMatrix(R_cur_0);
		cout<<"Translation vector: \n"<<t_cur_0.transpose()<<endl;
		//cout<<"Rotation Matrix: \n"<<R_cur_0<<endl;
		cout<<"Angle: "<<rotationVector.angle()/M_PI*180.<<" deg, "<<rotationVector.angle()<<" rad"<<endl;
		cout<<"Axis:  "<<rotationVector.axis().transpose()<<endl;

		//
		// Step 2:
		// Get the referenced frame of the current frame
		// if the referenced frame is not the previous one, i.e. k_ref
		// set the k_ref, T_ref, PC_rec  
		//
		int FrameID_Ref = i/frameInterval * frameInterval;
		if(k_ref!=FrameID_Ref)
		{
			cout<<"----"<<endl;
			cout<<"Step 2 is being trggered: Current-Frame ID: "<<v<<endl;

			k_ref = FrameID_Ref;
			T_ref_0 = T_cur_0;
			PC_ref = PointClouds_[k_ref];

		}

		//
		// Step 3:
		// write
		//
		fileTransformation<<rotationVector.angle()<<" ";
		fileTransformation<<rotationVector.axis()(0)<<" ";
		fileTransformation<<rotationVector.axis()(1)<<" ";
		fileTransformation<<rotationVector.axis()(2)<<" ";
		fileTransformation<<t_cur_0(0)<<" ";
		fileTransformation<<t_cur_0(1)<<" ";
		fileTransformation<<t_cur_0(2)<<endl;
	}
	fileTransformation.close();

	return 1;
}
