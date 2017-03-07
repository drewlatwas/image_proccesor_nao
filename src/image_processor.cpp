#include "ros/ros.h"
#include "std_msgs/String.h"
#include "sensor_msgs/Image.h"
#include <opencv/cv.h>
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/image_encodings.h>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <iostream>
using namespace std;

ros::Publisher chatter_pub;

void chatterCallback(const sensor_msgs::Image::ConstPtr& msg)
{
  //ROS_INFO("I heard: [%s]", msg->data.c_str());
	ROS_INFO("Received message");

	cv_bridge::CvImagePtr cv_ptr;
	cv::Mat cvImage;

	cv_ptr = cv_bridge::toCvCopy(msg/*, sensor_msgs::image_encodings::RGB16*/);
	cvImage = cv_ptr->image;
/*
	for(size_t a = 0; a < cvImage.rows; a++)
    {
        for(size_t b = 0; b < cvImage.cols; b++)
        {
			int rr = cvImage.at<cv::Vec3b>(a, b)[0];
			int bb = cvImage.at<cv::Vec3b>(a, b)[1];
			int gg = cvImage.at<cv::Vec3b>(a, b)[2];
			
			cout << "(";
            cout << rr << ", ";
            cout << bb << ", ";
            cout << gg << ")" << endl;
        }
    }
*/
	int tmpColor;
	
    for(size_t a = 0; a < cvImage.rows; a++)
    {
        for(size_t b = 0; b < cvImage.cols; b++)
        {
            tmpColor = cvImage.at<cv::Vec3b>(a, b)[0];
            cvImage.at<cv::Vec3b>(a, b)[0] = cvImage.at<cv::Vec3b>(a, b)[2];
            cvImage.at<cv::Vec3b>(a, b)[2] = tmpColor;
        }
    }
	//do OpenCV image processing stuff...
	
	cv::Mat hsv_image;
	cv::cvtColor(cvImage, hsv_image, cv::COLOR_BGR2HSV);
	cv::imshow("source", cvImage);



	cv::waitKey(2);
	cv::imshow("my hsv", hsv_image);
	cv::waitKey(3); 
	cv::Mat upper_red_image;
	cv::Mat lower_red_image;
	cv::Mat deep_sky_blue;

	//white = valid
  	cv::inRange(hsv_image, cv::Scalar(0, 100, 100), cv::Scalar(10, 255, 255), upper_red_image);
	cv::inRange(hsv_image, cv::Scalar(160, 100, 100), cv::Scalar(179, 255, 255), lower_red_image);
	cv::inRange(hsv_image, cv::Scalar(80, 100, 100), cv::Scalar(110, 255, 255), deep_sky_blue);
	
	cv::imshow("upper result - ball", upper_red_image);
	cv::waitKey(4);
	
	cv::imshow("lower result - ball", lower_red_image);
	cv::waitKey(5);
	
	cv::imshow("blue result - paddle", deep_sky_blue);
	cv::waitKey(6);
 	

	cv_ptr->image = cvImage;
    chatter_pub.publish(cv_ptr->toImageMsg() );
}

int main(int argc, char **argv)
{
  ros::init(argc, argv, "listener");

  ros::NodeHandle n;

  ros::Subscriber sub = n.subscribe("/usb_cam/image_raw", 1, chatterCallback);

  chatter_pub=n.advertise<sensor_msgs::Image>("/camera/rgb/image_processed", 1);

  ros::spin();

  return 0;
}
