#include <ros/ros.h>
#include <sensor_msgs/Imu.h> //定义IMU头文件的标准格式
#include <tf/tf.h> //引入tf坐标转换工具将IMU发来的四元数转变为欧拉角（翻滚、俯仰、朝向）
#include <geometry_msgs/Twist.h> //引入速度头文件的标准格式

ros::Publisher vel_pub; //定义速度发布器 

void IMUCallback(sensor_msgs::Imu msg)//引入回调函数其中msg为收到的IMU数据包
{
    if(msg.orientation_covariance[0] < 0)
        return;  //如果IMU没给有效的姿态数据，就直接退出这个函数，防止程序因为读取无效数据而报错。
    tf::Quaternion quaternion(  //将四元数装入装进一个叫quaternion的tf::Quaternion容器中
        msg.orientation.x,
        msg.orientation.y,
        msg.orientation.z,
        msg.orientation.w
    );

    double roll ,pitch, yaw;
    tf::Matrix3x3(quaternion).getRPY(roll, pitch, yaw);//把四元数转换成3x3的旋转矩阵并从旋转矩阵中提取出RPY三个角度，赋值给前面的变量
    roll = roll*180/M_PI;                  
    pitch = pitch*180/M_PI;   //将弧度u切换为角度M_PI为pi
    yaw = yaw*180/M_PI;

    ROS_INFO("翻滚= %.0f  俯仰= %.0f  朝向= %.0f",roll, pitch, yaw);

    double target_yaw = 90.0; //目标朝向角度
    double yaw_error = target_yaw - yaw; //计算当前朝向与目标朝向的误差
    geometry_msgs::Twist vel_cmd; //定义一个速度消息包
    vel_cmd.angular.z = yaw_error * 0.01; //根据误差计算角速度，0.01为比例系数（PID控制）
    vel_cmd.linear.x = 0.1; //设置线速度为0.1m/s
    vel_pub.publish(vel_cmd); //发布速度消息包
    


}
int main(int argc, char *argv[])
{
    setlocale(LC_ALL,"");
    ros::init(argc, argv, "imu_node");//初始化名为imu_node的节点

    ros::NodeHandle n;
    ros::Subscriber imu_sub = n.subscribe("/imu/data",10,IMUCallback);//订阅IMU数据。话题名叫“/imu/data“
    vel_pub = n.advertise<geometry_msgs::Twist>("/cmd_vel",10); //发布速度指令

    ros::spin();

    return 0;
}
