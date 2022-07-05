#include <Arduino.h>

#include "ros.h"
#include "std_msgs/Int8.h"
#include "std_msgs/UInt8.h"
#include "std_msgs/String.h"
#include "std_msgs/Float32.h"
#include "geometry_msgs/Vector3.h"
#include "std_msgs/UInt8MultiArray.h"
#include "esp_chatter.h"

ros::NodeHandle nh;

int timeout = 0;
int period = 5000;

int8_t speed_lf = 0;
int8_t speed_rf = 0;
int8_t speed_rb = 0;
int8_t speed_lb = 0;

int8_t speed_lf_pub = 0;
int8_t speed_rf_pub = 0;
int8_t speed_rb_pub = 0;
int8_t speed_lb_pub = 0;


geometry_msgs::Vector3 sensors_1_3_msg;
geometry_msgs::Vector3 sensors_4_6_msg;
geometry_msgs::Vector3 sensors_7_8_msg;

std_msgs::UInt8MultiArray sensors_data_array;

std_msgs::UInt8 battery_charge;

std_msgs::Int8 int_msg_right_front;
std_msgs::Int8 int_msg_left_front;
std_msgs::Int8 int_msg_right_back;
std_msgs::Int8 int_msg_left_back;

void battery_handler(); 

void sensors_1_3_data_handler();
void sensors_4_6_data_handler();
void sensors_7_8_data_handler();

void rpm_rightFront_subCb(const std_msgs::Int8& msg);
void rpm_leftFront_subCb(const std_msgs::Int8& msg);
void rpm_rightBack_subCb(const std_msgs::Int8& msg);
void rpm_leftBack_subCb(const std_msgs::Int8& msg);

void rpm_right_front_handler();
void rpm_left_front_handler();
void rpm_right_back_handler();
void rpm_left_back_handler();

std_msgs::String str_msg;
std_msgs::Float32 float_msg;
ros::Publisher chatter("chatter", &str_msg);
ros::Publisher float_pub("float", &float_msg);

ros::Publisher battery("battery_charge", &battery_charge);

ros::Publisher sensa_data("sensors_1_3_data", &sensors_1_3_msg);
ros::Publisher sensb_data("sensors_4_6_data", &sensors_4_6_msg);
ros::Publisher sensc_data("sensors_7_8_data", &sensors_7_8_msg);

ros::Publisher rpm_right_front("rpm_right_front", &int_msg_right_front);
ros::Publisher rpm_left_front("rpm_left_front", &int_msg_left_front);
ros::Publisher rpm_right_back("rpm_right_back", &int_msg_right_back);
ros::Publisher rpm_left_back("rpm_left_back", &int_msg_left_back);

ros::Subscriber<std_msgs::Int8> rpm_rightFront_sub("rpm_rightFront_sub", rpm_rightFront_subCb);
ros::Subscriber<std_msgs::Int8> rpm_leftFront_sub("rpm_leftFront_sub", rpm_leftFront_subCb);
ros::Subscriber<std_msgs::Int8> rpm_rightBack_sub("rpm_rightBack_sub", rpm_rightBack_subCb);
ros::Subscriber<std_msgs::Int8> rpm_leftBack_sub("rpm_leftBack_sub", rpm_leftBack_subCb);

char hello[13] = "hello world!";
float f = 0;

extern uint8_t charge_in_percent;

void rpm_rightFront_subCb(const std_msgs::Int8& msg)
{
  speed_rf = msg.data;
	// if (msg.data >= 6) {
	// 	speedDataRightFrontWheel = msg.data;
	// 	sideDataRightFrontWheel = 0; //CW
	// }
	// else if (msg.data <= -6) {
	// 	speedDataRightFrontWheel = -(msg.data);
	// 	sideDataRightFrontWheel = 1; //CCW
	// }
	// else if (msg.data == 0) {
	// 	speedDataRightFrontWheel = 0;
	// 	sideDataRightFrontWheel = 2;
	// }
}

void rpm_leftFront_subCb(const std_msgs::Int8& msg)
{
  speed_lf = msg.data;
	// if (msg.data >= 6) {
	// 	speedDataLeftFrontWheel = msg.data;
	// 	sideDataLeftFrontWheel = 1; //CCW
	// }
	// else if (msg.data <= -6) {
	// 	speedDataLeftFrontWheel = -(msg.data);
	// 	sideDataLeftFrontWheel = 0; //CW
	// }
	// else {
	// 	speedDataLeftFrontWheel = 0;
	// 	sideDataLeftFrontWheel = 2;
	// }
}

void rpm_rightBack_subCb(const std_msgs::Int8& msg)
{
  speed_rb = msg.data;
	// if (msg.data >= 6) {
	// 	speedDataRightBackWheel = msg.data;
	// 	sideDataRightBackWheel = 0; //CW
	// }
	// else if (msg.data <= -6) {
	// 	speedDataRightBackWheel = -(msg.data);
	// 	sideDataRightBackWheel = 1; //CCW
	// }
	// else {
	// 	speedDataRightBackWheel = 0;
	// 	sideDataRightBackWheel = 2;
	// }
}

void rpm_leftBack_subCb(const std_msgs::Int8& msg)
{
  speed_lb = msg.data;
	// if (msg.data >= 6) {
	// 	speedDataLeftBackWheel = msg.data;
	// 	sideDataLeftBackWheel = 1; //CCW
	// }
	// else if (msg.data <= -6) {
	// 	speedDataLeftBackWheel = -(msg.data);
	// 	sideDataLeftBackWheel = 0; //CW
	// }
	// else {
	// 	speedDataLeftBackWheel = 0;
	// 	sideDataLeftBackWheel = 2;
	// }
}

void rosserial_setup()
{
  // Initialize ROS
  nh.initNode();

  nh.advertise(chatter);
  nh.advertise(float_pub);

  nh.advertise(battery);

  nh.advertise(sensa_data);
	nh.advertise(sensb_data);
	nh.advertise(sensc_data);

	nh.advertise(rpm_left_front);
	nh.advertise(rpm_right_front);
	nh.advertise(rpm_left_back);
	nh.advertise(rpm_right_back);

  nh.subscribe(rpm_rightFront_sub);
	nh.subscribe(rpm_leftFront_sub);
	nh.subscribe(rpm_rightBack_sub);
	nh.subscribe(rpm_leftBack_sub);
}

void rosserial_publish()
{
  f++;
  str_msg.data = hello;
  float_msg.data = f;
  // Send the message
  chatter.publish(&str_msg);
  float_pub.publish(&float_msg);

  rpm_right_front_handler();
  rpm_left_front_handler();
  rpm_right_back_handler();
  rpm_left_back_handler();

  sensors_1_3_data_handler();
  sensors_4_6_data_handler();
  sensors_7_8_data_handler();
  if (millis() - timeout > period) {
    battery_handler();
    timeout = millis();
  }
  // nh.spinOnce();
}

void rpm_right_front_handler(void)
{
    // if (sideRXDataRightFrontWheel == 2) {
    //     uint_msg_right_front.data = speedRXDataRightFrontWheel;
    // }
    // else if (sideRXDataRightFrontWheel == 1) {
    //     uint_msg_right_front.data = -speedRXDataRightFrontWheel;
    // }
    // else {
    //     uint_msg_right_front.data = 0;
    // }
    
    int_msg_right_front.data = speed_rf_pub;
    if (nh.connected()) {
        rpm_right_front.publish(&int_msg_right_front);
    }
}

void rpm_left_front_handler(void)
{
    // if (sideRXDataLeftFrontWheel == 1) {
    //     uint_msg_left_front.data = speedRXDataLeftFrontWheel;
    // }
    // else if (sideRXDataLeftFrontWheel == 2) {
    //     uint_msg_left_front.data = -speedRXDataLeftFrontWheel;
    // }
    // else {
    //     uint_msg_left_front.data = 0;
    // }
    int_msg_left_front.data = speed_lf_pub;
    if (nh.connected()) {
        rpm_left_front.publish(&int_msg_left_front);
    }
}

void rpm_right_back_handler(void)
{
    // if (sideRXDataRightBackWheel == 2) {
    //     uint_msg_right_back.data = speedRXDataRightBackWheel;
    // }
    // else if (sideRXDataRightBackWheel == 1) {
    //     uint_msg_right_back.data = -speedRXDataRightBackWheel;
    // }
    // else {
    //     uint_msg_right_back.data = 0;
    // }
    int_msg_right_back.data = speed_rb_pub;
    if (nh.connected()) {
        rpm_right_back.publish(&int_msg_right_back);
    }
}

void rpm_left_back_handler(void)
{
    // if (sideRXDataLeftBackWheel == 1) {
    //     uint_msg_left_back.data = speedRXDataLeftBackWheel;
    // }
    // else if (sideRXDataLeftBackWheel == 2) {
    //     uint_msg_left_back.data = -speedRXDataLeftBackWheel;
    // }
    // else {
    //     uint_msg_left_back.data = 0;
    // }
    int_msg_left_back.data = speed_lb_pub;
    if (nh.connected()) {
        rpm_left_back.publish(&int_msg_left_back);
    }
}

void sensors_1_3_data_handler(void)
{
		// sen1_3_msg.x = static_cast<float>(sensorData1);
		// sen1_3_msg.y = static_cast<float>(sensorData2);
		// sen1_3_msg.z = static_cast<float>(sensorData3);
    sensa_data.publish(&sensors_1_3_msg);
}

void sensors_4_6_data_handler(void)
{
		// sen4_6_msg.x = static_cast<float>(sensorData4);
		// sen4_6_msg.y = static_cast<float>(sensorData5);
		// sen4_6_msg.z = static_cast<float>(sensorData6);
    sensb_data.publish(&sensors_4_6_msg);
}

void sensors_7_8_data_handler(void)
{
		// sen7_8_msg.x = static_cast<float>(sensorData7);
		// sen7_8_msg.y = static_cast<float>(sensorData8);
		// sen7_8_msg.z = 0.0;
    sensc_data.publish(&sensors_7_8_msg);
}

void battery_handler(void) 
{
    battery_charge.data = charge_in_percent;
    battery.publish(&battery_charge);
}
