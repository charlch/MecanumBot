#include <HBridge.h>
#include "FlySkyIBus.h"

const float DEAD_BAND_FRC = 0.01;

//Create the motor driver, setting it's pins and dead band fraction
HBridge motor1(14, 15, 10, DEAD_BAND_FRC);
HBridge motor2(8, 16, 9, DEAD_BAND_FRC);
HBridge motor3(5, 7, 6, DEAD_BAND_FRC);
HBridge motor4(2, 4, 3, DEAD_BAND_FRC);

void setup(){
  Serial.begin(115200);
  IBus.begin(Serial);
}

void loop() 
{
  IBus.loop();
  
  int forward_raw = IBus.readChannel(1);
  int strafe_raw = IBus.readChannel(0);
  int turn_raw = IBus.readChannel(3);
  Serial.println(forward_raw);f
  if ((forward_raw<900) || (IBus.millisSinceUpdate() > 500)) {
    forward_raw = 1500;
    strafe_raw = 1500;
    turn_raw = 1500;
  }

  float forward = norm(forward_raw);
  float strafe = norm(strafe_raw);
  float turn = norm(turn_raw);

  float m1 = (forward+strafe+turn)/3.0;
  float m2 = (forward-strafe-turn)/3.0;
  float m3 = (forward-strafe+turn)/3.0;
  float m4 = (forward+strafe-turn)/3.0;

  motor1.set_signal(m1);
  motor2.set_signal(m2);
  motor3.set_signal(m3);
  motor4.set_signal(m4);

}

float norm(int in) {
  return (float(in-1500.0)) / 500.0; 
}
