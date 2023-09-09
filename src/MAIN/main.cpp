#include <MAIN/config.h>
#include <MAIN/ultrasonic.h>
#include "led_strip.h"
#include <MAIN/ros_com.h>
#include <movingAvg.h>

#include <MAIN/imu.h>

#include "filter.h"

bool imu_status, dmp_status; 
bool _connect = false;

float quaternions[4];           //x, y, z, w
float linaer_acceleration[3];   //x, y, z
float angular_velocity[3];      //x, y, z 

float yaw = 0; 
movingAvg avgYaw(10);                  // define the moving average object
MedianFilter MedianYaw(10, 0);
int avg_yaw, median_yaw;

int ultrasonic_range[NUMBER_ULTRASONIC_SENSORS];  
int previousTime; 

// ----------- ultrasonic sensor - distance

void setup(){

  led_strip_init();
  ros_init();

  avgYaw.begin();

  pinMode(LED_BUILD_IN,OUTPUT);
  digitalWrite(LED_BUILD_IN,LOW);

  //Serial.begin(9600);

  previousTime = millis();

  // check that the IMU initializes correctly
  imu_status, dmp_status = imu_setup();


  if(dmp_status == 0) {
    nh.logwarn("MPU6050 connection successful");
    digitalWrite(LED_BUILD_IN, HIGH); 
  }
}


void loop(){

  if(!rosConnected(nh,_connect)) {
    pixels.fill(0x000000);
    pixels.show();
  }

  int* ultrasonic_range = ultrasonic_measurments(previousTime); 


  yaw = imu_get_yaw(); 
  MedianYaw.in(yaw);
  avg_yaw = avgYaw.reading(yaw);
  median_yaw = MedianYaw.out();
  
  
  ros_loop(ultrasonic_range[0], ultrasonic_range[1], 
           ultrasonic_range[2], median_yaw);

  nh.spinOnce();

  
}   