/************************************************************
MPU9250_DMP_Quaternion
 Quaternion example for MPU-9250 DMP Arduino Library 
Jim Lindblom @ SparkFun Electronics
original creation date: November 23, 2016
https://github.com/sparkfun/SparkFun_MPU9250_DMP_Arduino_Library

The MPU-9250's digital motion processor (DMP) can calculate
four unit quaternions, which can be used to represent the
rotation of an object.

This exmaple demonstrates how to configure the DMP to 
calculate quaternions, and prints them out to the serial
monitor. It also calculates pitch, roll, and yaw from those
values.

Development environment specifics:
Arduino IDE 1.6.12
SparkFun 9DoF Razor IMU M0

Supported Platforms:
- ATSAMD21 (Arduino Zero, SparkFun SAMD21 Breakouts)

CHANGES by Gabor Nemeth 2020
I'm using this for VR head tracking with the Relativ-OSVR plugin.
*************************************************************/
#include <SparkFunMPU9250-DMP.h>
#include "Wire.h"
#include "I2Cdev.h"

#include "HID.h"
#if !defined(_USING_HID)
#error "Using legacy HID core (non pluggable)"
#endif

static const uint8_t _hidReportDescriptor[] PROGMEM = {
 0x06, 0x00, 0xff,         // USAGE_PAGE (vendor defined)
    0x09, 0x00,         // USAGE (Undefined)
    0xa1, 0x01,         // COLLECTION (Application)
    0x15, 0x00,         //   LOGICAL_MINIMUM (0)
    0x26, 0xff, 0x00,   //   LOGICAL_MAXIMUM (255)
    0x85, 0x01,         //   REPORT_ID (1)
    0x75, 0x08,         //   REPORT_SIZE (8)
    
    0x95, 0x3f,         //   REPORT_COUNT (63)
    
    0x09, 0x00,         //   USAGE (Undefined)
    0x81, 0x02,         //   INPUT (Data,Var,Abs) - to the host
    0xc0           
};

//#define SerialPort SerialUSB

MPU9250_DMP imu;
int arrout[16];
int i = 0;
//float quat[4];
union
  {
    float input; // assumes sizeof(float) == sizeof(int)
    int   output;
    uint8_t bytes[sizeof(float)];
  } quat0;
union
  {
    float input; // assumes sizeof(float) == sizeof(int)
    int   output;
    uint8_t bytes[sizeof(float)];
  } quat1;
union
  {
    float input; // assumes sizeof(float) == sizeof(int)
    int   output;
    uint8_t bytes[sizeof(float)];
  } quat2;
union
  {
    float input; // assumes sizeof(float) == sizeof(int)
    int   output;
    uint8_t bytes[sizeof(float)];
  } quat3;

void setup() 
{
  static HIDSubDescriptor node (_hidReportDescriptor, sizeof(_hidReportDescriptor));
  HID().AppendDescriptor(&node);

  Serial.begin(115200);
  Wire.begin();
  Wire.setClock(2000000);

  delay(2000);

  while ( i < 16) {
    arrout[i] = 0;
    i++;
  }

  // Call imu.begin() to verify communication and initialize
  if (imu.begin() != INV_SUCCESS)
  {
    while (1)
    {
      Serial.println("Unable to communicate with MPU-9250");
      Serial.println("Check connections, and try again.");
      Serial.println();
      delay(5000);
    }
  }
  
  imu.dmpBegin(DMP_FEATURE_6X_LP_QUAT | // Enable 6-axis quat
               DMP_FEATURE_GYRO_CAL, // Use gyro calibration
              60); // Set DMP FIFO rate to 10 Hz
  // DMP_FEATURE_LP_QUAT can also be used. It uses the 
  // accelerometer in low-power mode to estimate quat's.
  // DMP_FEATURE_LP_QUAT and 6X_LP_QUAT are mutually exclusive
}

void loop() 
{
  // Check for new data in the FIFO
  if ( imu.fifoAvailable() )
  {
    // Use dmpUpdateFifo to update the ax, gx, mx, etc. values
    if ( imu.dmpUpdateFifo() == INV_SUCCESS)
    {
      // computeEulerAngles can be used -- after updating the
      // quaternion values -- to estimate roll, pitch, and yaw
      imu.computeEulerAngles();
      printIMUData();
    }
  }
}

void printIMUData(void)
{  
  // After calling dmpUpdateFifo() the ax, gx, mx, etc. values
  // are all updated.
  // Quaternion values are, by default, stored in Q30 long
  // format. calcQuat turns them into a float between -1 and 1
  /*
  quat[0] = imu.calcQuat(imu.qw);
  quat[1] = imu.calcQuat(imu.qx);
  quat[2] = imu.calcQuat(imu.qy);
  quat[3] = imu.calcQuat(imu.qz);
  */

  // just to get consistent output for debug
  quat0.input = -0.1234;
  quat1.input = 0.1234;
  quat2.input = 0.5678;
  quat3.input = -0.5678;

  arrout[0]  = quat0.bytes[0];
  arrout[1]  = quat0.bytes[1];
  arrout[2]  = quat0.bytes[2];
  arrout[3]  = quat0.bytes[3];
  arrout[4]  = quat1.bytes[0];
  arrout[5]  = quat1.bytes[1];
  arrout[6]  = quat1.bytes[2];
  arrout[7]  = quat1.bytes[3];
  arrout[8]  = quat2.bytes[0];
  arrout[9]  = quat2.bytes[1];
  arrout[10] = quat2.bytes[2];
  arrout[11] = quat2.bytes[3];
  arrout[12] = quat3.bytes[0];
  arrout[13] = quat3.bytes[1];
  arrout[14] = quat3.bytes[2];
  arrout[15] = quat3.bytes[3];
  
  //String qu0 = String(quat[0], 6);
  //String qu1 = String(quat[1], 6);
  //String qu2 = String(quat[2], 6);
  //String qu3 = String(quat[3], 6);

  

  //Serial.println(qu3 + ',' + qu0 + ',' + qu1 + ',' + qu2);
  HID().SendReport(1,arrout,63);
}
