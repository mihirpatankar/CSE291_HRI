#include "ros/ros.h"
#include "std_msgs/String.h"
#include <sensor_msgs/Image.h>
#include <cmvision/Blob.h>
#include <cmvision/Blobs.h>
#include <geometry_msgs/Twist.h>
#include <kobuki_msgs/BumperEvent.h>
#include <depth_image_proc/depth_traits.h>

const float min_x_ = -0.2;
const float max_x_ = 0.2;
const float min_y_ = 0.1;
const float max_y_ = 0.5;
const float max_z_ = 1.5;
const float goal_z_ = 0.1;


int state;
int msgFlag;
int bumpflag;
int prev;
int goflag;

ros::Publisher cmdpub_;
ros::Subscriber msgsub_;
ros::Subscriber blobsub;
ros::Subscriber bumper_;
ros::Subscriber depth_;




void  bumpercb(const kobuki_msgs::BumperEvent::ConstPtr& bump){
      if(bump->state==1){
        std::cout<<"Bump sensor pressed\n";
        bumpflag=1;
      }
  }


void msgsubcb (const std_msgs::String::ConstPtr& msgIn){
      int temp=std::atoi(msgIn->data.c_str());
      if(temp==3){
        std::cout<<"Move the robot\n";
        msgFlag=1;
      }
  }




/* Blob callback*/
  void blobsCallBack (const cmvision::Blobs& blobsIn){
    if(blobsIn.blob_count >0){
	if(blobsIn.blobs[0].area > 8000){
		goflag=1;
	}
	else if(blobsIn.blobs[0].area<8000){
		goflag=0;
	}
        if(msgFlag==1){
		if(goflag==1){
	                //geometry_msgs::TwistPtr cmd(new geometry_msgs::Twist());
			if(blobsIn.blobs[0].y>200 && blobsIn.blobs[0].x<280){
				geometry_msgs::TwistPtr cmd(new geometry_msgs::Twist());
				cmd->angular.z=3.5;
				cmd->linear.x=0.5;
				cmdpub_.publish(cmd);
				std::cout<<"R\n";

			}
			else if(blobsIn.blobs[0].y>200 && blobsIn.blobs[0].x>360){
				geometry_msgs::TwistPtr cmd(new geometry_msgs::Twist());
				cmd->angular.z=-3.5;
				cmd->linear.x=0.5;
				cmdpub_.publish(cmd);
				std::cout<<"L\n";
			}
			else if(blobsIn.blobs[0].y>200 && blobsIn.blobs[0].x>280 && blobsIn.blobs[0].x<360){
				geometry_msgs::TwistPtr cmd(new geometry_msgs::Twist());
				cmd->angular.z=0.0;
                                cmd->linear.x=0.5;
                                cmdpub_.publish(cmd);
                                std::cout<<"S\n";
			}
				msgFlag=0;
				goflag=0;
				//geometry_msgs::TwistPtr cmd(new geometry_msgs::Twist());
				//cmd->linear.x=0.0;
				//cmd->angular.z=0.0;
				//cmdpub_.publish(cmd);
				
		}
		/*if(bumpflag==1){
                        cmdpub_.publish(geometry_msgs::TwistPtr(new geometry_msgs::Twist()));
                        msgFlag=0;
                        bumpflag=0;
			goflag=0;
                }*/
        }
	if(bumpflag==1){
                        cmdpub_.publish(geometry_msgs::TwistPtr(new geometry_msgs::Twist()));
                        msgFlag=0;
                        bumpflag=0;
                        goflag=0;
                }
    }
  else{
//        std::cout<<"No blob detected\n";
        geometry_msgs::TwistPtr cmd(new geometry_msgs::Twist());
        cmdpub_.publish(cmd);
	goflag=0;

    }
  }




int main(int argc, char **argv)
{
  /**
   * The ros::init() function needs to see argc and argv so that it can perform
   * any ROS arguments and name remapping that were provided at the command line.
   * For programmatic remappings you can use a different version of init() which takes
   * remappings directly, but for most command-line programs, passing argc and argv is
   * the easiest way to do it.  The third argument to init() is the name of the node.
   *
   * You must call one of the versions of ros::init() before using any other
   * part of the ROS system.
   */
  ros::init(argc, argv, "goal_keep_node");
	
  /**
   * NodeHandle is the main access point to communications with the ROS system.
   * The first NodeHandle constructed will fully initialize this node, and the last
   * NodeHandle destructed will close down the node.
   */
  ros::NodeHandle n;
  /**
   * The subscribe() call is how you tell ROS that you want to receive messages
   * on a given topic.  This invokes a call to the ROS
   * master node, which keeps a registry of who is publishing and who
   * is subscribing.  Messages are passed to a callback function, here
   * called chatterCallback.  subscribe() returns a Subscriber object that you
   * must hold on to until you want to unsubscribe.  When all copies of the Subscriber
   * object go out of scope, this callback will automatically be unsubscribed from
   * this topic.
   *
   * The second parameter to the subscribe() function is the size of the message
   * queue.  If messages are arriving faster than they are being processed, this
   * is the number of messages that will be buffered up before beginning to throw
   * away the oldest ones.
   */
    cmdpub_ = n.advertise<geometry_msgs::Twist> ("cmd_vel_mux/input/teleop", 1);
    msgsub_=n.subscribe<std_msgs::String>("/message_pub",1,msgsubcb);
    blobsub = n.subscribe("/blobs", 1, blobsCallBack);
    bumper_ = n.subscribe("mobile_base/events/bumper",100,bumpercb);
  //  depth_= n.subscribe<sensor_msgs::Image>("/camera/depth/image_rect", 1,imagecb);

  /**
   * ros::spin() will enter a loop, pumping callbacks.  With this version, all
   * callbacks will be called from within this thread (the main one).  ros::spin()
   * will exit when Ctrl-C is pressed, or the node is shutdown by the master.
   */
  ros::spin();

  return 0;
}
