#!/usr/bin/env python

"""
voice_cmd_vel.py is a simple demo of speech recognition.
  You can control a mobile base using commands found
  in the corpus file.
"""

import roslib; roslib.load_manifest('pocketsphinx')
import rospy
import math

from geometry_msgs.msg import Twist
from std_msgs.msg import String

message="0"

class voice_cmd_vel:

    def __init__(self):
        rospy.on_shutdown(self.cleanup)
        self.speed = 0.2
        self.msg = Twist()

        # publish to cmd_vel, subscribe to speech output
        self.pub_ = rospy.Publisher('cmd_vel', Twist)
	self.str_ = rospy.Publisher('message_pub', String, queue_size=10)
        rospy.Subscriber('recognizer/output', String, self.speechCb)

        r = rospy.Rate(10.0)
        while not rospy.is_shutdown():
            self.pub_.publish(self.msg)
            r.sleep() 
	    self.str_.publish(message)
	    r.sleep()
        
    def speechCb(self, msg):
        rospy.loginfo(msg.data)

	if ("fetch" in msg.data) or ("one" in msg.data):
	    message="1"
	    self.str_.publish(message)
	elif ("score" in msg.data) or ("two" in msg.data):
	    message="2"
	    self.str_.publish(message)
	elif ("save" in msg.data) or ("three" in msg.data):
	    message="3"
            self.str_.publish(message)
	elif ("pass" in msg.data) or ("four" in msg.data):
	    message="4"
            self.str_.publish(message)

        if msg.data.find("full speed") > -1:
            if self.speed == 0.2:
                self.msg.linear.x = self.msg.linear.x*2
                self.msg.angular.z = self.msg.angular.z*2
                self.speed = 0.4
        if msg.data.find("half speed") > -1:
            if self.speed == 0.4:
                self.msg.linear.x = self.msg.linear.x/2
                self.msg.angular.z = self.msg.angular.z/2
                self.speed = 0.2

        if msg.data.find("forward") > -1:    
            self.msg.linear.x = self.speed
            self.msg.angular.z = 0
        elif msg.data.find("left") > -1:
            if self.msg.linear.x != 0:
                if self.msg.angular.z < self.speed:
                    self.msg.angular.z += 0.05
            else:        
                self.msg.angular.z = self.speed*2
        elif msg.data.find("right") > -1:    
            if self.msg.linear.x != 0:
                if self.msg.angular.z > -self.speed:
                    self.msg.angular.z -= 0.05
            else:        
                self.msg.angular.z = -self.speed*2
        elif msg.data.find("back") > -1:
            self.msg.linear.x = -self.speed
            self.msg.angular.z = 0
        elif msg.data.find("stop") > -1 or msg.data.find("halt") > -1:          
            self.msg = Twist()
     
        self.pub_.publish(self.msg)
#	self.str_.publish(message)
	message="0"

    def cleanup(self):
        # stop the robot!
        twist = Twist()
        self.pub_.publish(twist)
	self.str_.publish(message)

if __name__=="__main__":
    rospy.init_node('voice_cmd_vel')
    try:
        voice_cmd_vel()
    except:
        pass


