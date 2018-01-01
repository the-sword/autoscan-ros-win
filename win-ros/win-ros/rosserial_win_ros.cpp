/********************************************************
* @file    : rosserial_win_ros.cpp
* @brief   : communication between windows and ros
* @details : windows传递pose参数到ros，ros返回执行状态，成功即执行scanner扫描
* @author  : xiaohui zhou
* @version : ver 1.0
* @date    : 2017-11-28
*********************************************************/
// rosserial_hello_world.cpp : 定义控制台应用程序的入口点。
#include "stdafx.h"
#include <string>  
#include <stdio.h>  
#include <vector> 
#include <fstream>
#include <iostream>
#include <cstddef>
//#include "rosserial_hello_world.h"

#include "ros.h"  
#include <geometry_msgs/Twist.h>  
#include <geometry_msgs/PoseStamped.h> 
#include <geometry_msgs/PoseWithCovarianceStamped.h>
#include <moveit_msgs/DisplayRobotState.h>
#include <moveit_msgs/DisplayTrajectory.h>
#include <moveit_msgs/AttachedCollisionObject.h>
#include <moveit_msgs/CollisionObject.h>

//goal execute status
//PENDING = 0
//ACTIVE = 1
//SUCCEEDED = 3
//ABORTED = 4
#include <actionlib_msgs/GoalStatus.h>
#include <actionlib_msgs/GoalStatusArray.h>
#include <custom_msg/Execute_Status.h>
#include <windows.h> 
using std::string;
using namespace std;

//subscribe callback
static int goal_exe_status;
//void execute_pose_callback(const custom_msg::Execute_Status &goalStatus)
//{
//	printf("Waiting to receive goalStatus\n");
//	goal_exe_status = goalStatus.data;
//}
void execute_pose_callback(const actionlib_msgs::GoalStatusArray &goalStatus)
{
	actionlib_msgs::GoalStatus *goal = goalStatus.status_list;
	if (goal != NULL)
	{
		printf("非空\n");
		cout<<goal_exe_status<<endl;
		goal_exe_status = goal->status;
	}
	else
	{
		printf("don't get goal_exe_status!\n");
	}

}

/********************************************************
*  @function :  posePublish
*  @brief    :  publish pose to ros to make the robot arrive to the position
*  @input    :  null
*  @return   :  null
*  @author   :  xiaohui zhou  2017/11/27 20:38
*********************************************************/
int posePublish(geometry_msgs::Pose &pose_)
{
	//printf("start publish pose!\n");
	ros::NodeHandle nh;
	char *ros_master = "192.168.186.129";

	//printf("Connecting to server at %s\n", ros_master);
	nh.initNode(ros_master);

	//printf("Advertising cmd_vel message\n");
	geometry_msgs::Pose target_pose1;
	ros::Publisher display_publisher("goal", &target_pose1);
	nh.advertise(display_publisher);

	//printf("Go robot go!\n");
	int spin_pub_i = 30;
	while (spin_pub_i > 0)
	{
		display_publisher.publish(&pose_);

		nh.spinOnce();
		spin_pub_i--;
		Sleep(100);
	}

	printf("publish pose done!\n");
	return 0;
}

/********************************************************
*  @function :  statusSubscriber
*  @brief    :  get robot plan status
*  @input[in]:  null
*  @return   :  int
*  @author   :  xiaohui zhou
*********************************************************/
int statusSubscriber()
{
	//printf("start subscribe execute status!\n");
	ros::NodeHandle nh;
	char *ros_master = "192.168.186.129";

	//printf("Connecting to server at %s\n", ros_master);
	nh.initNode(ros_master);

	int spin_sub_i = 10;
	ros::Subscriber <actionlib_msgs::GoalStatusArray>
		poseSub("execute_pose", &execute_pose_callback);
	nh.subscribe(poseSub);
	//printf("Waiting to receive messages\n");
	while (spin_sub_i > 0)
	{
		nh.spinOnce();
		printf("nh.spinOnce();\n");
		spin_sub_i--;
		Sleep(200);
	}
	printf("Subscriber status done!\n");
	return 0;
}

/********************************************************
*  @function :  split
*  @brief    :  按照空格进行字符串分割
*  @input    :  &s, &seperator
*  @return   :  string,string
*  @author   :  xiaohui zhou  2017/11/27 21:02
*********************************************************/
vector<string> split(const string &s, const string &seperator) {
	vector<string> result;
	typedef string::size_type string_size;
	string_size i = 0;

	while (i != s.size()) {
		//找到字符串中首个不等于分隔符的字母；
		int flag = 0;
		while (i != s.size() && flag == 0) {
			flag = 1;
			for (string_size x = 0; x < seperator.size(); ++x)
				if (s[i] == seperator[x]) {
					++i;
					flag = 0;
					break;
				}
		}

		//找到又一个分隔符，将两个分隔符之间的字符串取出；
		flag = 0;
		string_size j = i;
		while (j != s.size() && flag == 0) {
			for (string_size x = 0; x < seperator.size(); ++x)
				if (s[j] == seperator[x]) {
					flag = 1;
					break;
				}
			if (flag == 0)
				++j;
		}
		if (i != j) {
			result.push_back(s.substr(i, j - i));
			i = j;
		}
	}
	return result;
}

int main(int argc, char * argv[])
{
	//读取pose.txt文件中pose信息
	//pose.txt文件数据已空格隔开
	vector<geometry_msgs::Pose> pose_vector;
	char buffer[512];
	//if (argc != 3)return printf("[usage] %s pose.txt scanner.exe\n", argv[0]);
	//ifstream in(argv[1]);
	ifstream in("pose.txt");
	if (!in.is_open())
	{
		cout << "Error opening file"; exit(1);
	}
	while (!in.eof())
	{
		in.getline(buffer, 100);
		//cout << buffer << endl;
		vector<string> v = split(buffer, " ");
		geometry_msgs::Pose pose_temp;
		pose_temp.position.x = std::stod(v[0]);
		pose_temp.position.y = std::stod(v[1]);
		pose_temp.position.z = std::stod(v[2]);
		pose_temp.orientation.x = std::stod(v[3]);
		pose_temp.orientation.y = std::stod(v[4]);
		pose_temp.orientation.z = std::stod(v[5]);
		pose_temp.orientation.w = std::stod(v[6]);
		printf("#################################start publish!\n");
		cout << v[0] << endl;
		posePublish(pose_temp);
		printf("#################################Go robot go!\n");
		//暂停5秒
		Sleep(2000);
		printf("#################################start subscribe!\n");
		statusSubscriber();
		if (goal_exe_status == 4)
		{
			printf("wait execute successfully!\n");
			Sleep(4000);
			statusSubscriber();
		}

		while (goal_exe_status == 1)
		{
			printf("wait execute successfully!\n");
			Sleep(1000);
			statusSubscriber();
		}
		//Sleep(5000);//time for scanner to capture

		//simple-capture-sampled.exe路径
		std::string sprPath = "C:/Users/mlang/Desktop/autoscanner/artec-sdk-samples-v2.0-20171207/samples/simple-capture/bin-vc14-x64/simple-capture-sample.exe";
		//std::string sprPath = argv[2];
		//判断move plan 执行状态，若succeed，则添加scanner进行扫描
		
		switch (goal_exe_status)
		{
		case 1:
			printf("This goal has been accepted by the simple action server! \n");
			break;
		case 2:
			printf("Arm move plan PREEMPTED=2!\n");
			break;
		case 3:
			printf("Arm move plan SUCCEEDED=3!\n");
			printf("Start Scanner!\n");
			system(sprPath.c_str());
			printf("Scanner done!\n");
			break;
		case 4:
			printf("Arm move plan ABORTED=4!\n");
			cout << "fail path " << buffer << endl;
			break;
		case 5:
			printf("Arm move plan REJECTED=5!\n");
			break;
		case 6:
			printf("Arm move plan PREEMPTING=6!\n");
			break;
		case 7:
			printf("Arm move plan RECALLING=7!\n");
			break;
		case 8:
			printf("Arm move plan RECALLED=8!\n");
			break;
		case 9:
			printf("Arm move plan LOST=9!\n");
			break;
		default:
			break;
		}
		goal_exe_status = 0;
	}

	printf("All done!\n");
	return 0;
}