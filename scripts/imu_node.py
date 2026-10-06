#!/usr/bin/env python3
#coding=utf-8
import rospy
from sensor_msgs.msg import Imu
from tf.transformations import euler_from_quaternion #直接用tf库中现成的函数euler_from_quaternion将四元数转变为三个欧拉角
import math

def ImuCallback(msg):
    if msg.orientation_covariance[0] < 0:
        return

    quaternion = [          #将四元数按顺序放进一个列表euler_from_quaternion函数正好接收这种格式的参数
        msg.orientation.x,
        msg.orientation.y,
        msg.orientation.z,
        msg.orientation.w
    ]
    (roll, pitch, yaw) = euler_from_quaternion(quaternion)#返回三个欧拉角
    roll = roll*180/math.pi
    pitch = pitch*180/math.pi
    yaw = yaw*180/math.pi
    rospy.loginfo("翻滚= %.0f  俯仰= %.0f  朝向= %.0f",roll, pitch, yaw)

if __name__ == "__main__":
    rospy.init_node("imu_node")
    imu_sub = rospy.Subscriber("/imu/data", Imu,ImuCallback,queue_size=10)#订阅IMU数据，话题名/imu/data，消息类型Imu，回调函数ImuCallback，队列长度10
    rospy.spin()