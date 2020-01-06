/*
  CatCompanion 2020 © All Rights Reserved
  Author: Wit Zieliński (ziewit@gmail.com)
  Project: https://github.com/witko0/catcompanion
*/

#include <Servo.h>

// SG90 data
Servo tail;
const int MIN_ANGLE   = 450;
const int MID_ANGLE   = 1450;
const int MAX_ANGLE   = 2450;
const int RIGHT_ANGLE = 1850;
const int LEFT_ANGLE  = 1050;

// Pin configuration
const int PWR_PIN_1  = 0;  // additional 5V power supply
const int PWR_PIN_2  = 1;  // additional 5V power supply
const int PWR_PIN_3  = 4;  // additional 5V power supply
const int TAIL_PIN   = 2;  // tail PWM
const int A_1A       = 6;  // left wheel PWM
const int A_1B       = 7;  // left wheel direction
const int B_1A       = 10; // right wheel PWM
const int B_1B       = 11; // right wheel direction
const int TRIG_PIN_M = 12; // middle distance sensor trigger pin
const int ECHO_PIN_M = 3;  // middle distance sensor echo pin
const int TRIG_PIN_R = 13; // right distance sensor trigger pin
const int ECHO_PIN_R = 5;  // right distance sensor echo pin
const int TRIG_PIN_L = 8;  // left distance sensor trigger pin
const int ECHO_PIN_L = 9;  // left distance sensor echo pin

// loop counter
int counter = 1;
int distance_mid, distance_l, distance_r;

void setup()
{
   // Engines L9110S
   pinMode(A_1A, OUTPUT);
   pinMode(A_1B, OUTPUT);
   pinMode(B_1A, OUTPUT);
   pinMode(B_1B, OUTPUT);

   // Tail servo SG90
   tail.attach(TAIL_PIN);

   // Distance sensors HC-SR04
   pinMode(TRIG_PIN_M, OUTPUT);
   pinMode(ECHO_PIN_M, INPUT);
   pinMode(TRIG_PIN_L, OUTPUT);
   pinMode(ECHO_PIN_L, INPUT);
   pinMode(TRIG_PIN_R, OUTPUT);
   pinMode(ECHO_PIN_R, INPUT);

   // Arduino diode
   pinMode(LED_BUILTIN, OUTPUT);

   // Digital pins as additional power supply
   pinMode(PWR_PIN_1, OUTPUT);
   digitalWrite(PWR_PIN_1, HIGH);
   pinMode(PWR_PIN_2, OUTPUT);
   digitalWrite(PWR_PIN_2, HIGH);
   pinMode(PWR_PIN_3, OUTPUT);
   digitalWrite(PWR_PIN_3, HIGH);

   delay(1000);
   wheels("forward");
   delay(1000);
   wheels("stop");
   delay(1000);
}

void loop()
{
   distance_mid = get_distance_mid();
   distance_l = get_distance_l();
   distance_r = get_distance_r();

   if ((distance_mid < 50) || (distance_l < 30) || (distance_r < 30))
   {
      digitalWrite(LED_BUILTIN, HIGH);
   
      wheels("stop");
      delay(800);

      tail_movement_long();
      delay(200);

      while ((distance_mid < 50) || (distance_l < 30) || (distance_r < 30))
      {
         wheels("turnright");
         delay(200);
         wheels("stop");

         distance_mid = get_distance_mid();
         distance_l = get_distance_l();
         distance_r = get_distance_r();
      }
 
      digitalWrite(LED_BUILTIN, LOW);
      wheels("stop");
      delay(300);
   }
   else
   {
      digitalWrite(LED_BUILTIN, LOW);
      wheels("forward");
      delay(100);
   }

   counter++;
}

int get_distance_mid()
{
   long impulse_time, distance;

   digitalWrite(TRIG_PIN_M, LOW);
   delayMicroseconds(2);
   digitalWrite(TRIG_PIN_M, HIGH);
   delayMicroseconds(10);
   digitalWrite(TRIG_PIN_M, LOW);

   impulse_time = pulseIn(ECHO_PIN_M, HIGH);
   distance = impulse_time / 58;

   return distance;
}

int get_distance_l()
{
   long impulse_time, distance;

   digitalWrite(TRIG_PIN_L, LOW);
   delayMicroseconds(2);
   digitalWrite(TRIG_PIN_L, HIGH);
   delayMicroseconds(10);
   digitalWrite(TRIG_PIN_L, LOW);

   impulse_time = pulseIn(ECHO_PIN_L, HIGH);
   distance = impulse_time / 58;

   return distance;
}

int get_distance_r()
{
   long impulse_time, distance;

   digitalWrite(TRIG_PIN_R, LOW);
   delayMicroseconds(2);
   digitalWrite(TRIG_PIN_R, HIGH);
   delayMicroseconds(10);
   digitalWrite(TRIG_PIN_R, LOW);

   impulse_time = pulseIn(ECHO_PIN_R, HIGH);
   distance = impulse_time / 58;

   return distance;
}

void tail_movement_long()
{
   tail.writeMicroseconds(RIGHT_ANGLE);
   delay(400);
   tail.writeMicroseconds(LEFT_ANGLE);
   delay(400);
   tail.writeMicroseconds(MID_ANGLE);
}

void wheels(const char* command)
{
  if ("forward" == command)
  {
    wheel_L("forward");
    wheel_R("forward");
  }
  else if ("backward" == command)
  {
    wheel_L("backward");
    wheel_R("backward");
  }
  else if ("turnleft" == command)
  {
    wheel_L("stop");
    wheel_R("backward");
  }
  else if ("turnright" == command)
  {
    wheel_L("backward");
    wheel_R("stop");
  }
  else
  {
    wheel_L("stop");
    wheel_R("stop");
  }
}

void wheel_L(const char* command)
{
  if ("forward" == command)
  {
    digitalWrite(A_1A, LOW);
    digitalWrite(A_1B, HIGH);
  }
  else if ("backward" == command)
  {
    digitalWrite(A_1A, HIGH);
    digitalWrite(A_1B, LOW);
  }
  else
  {
    digitalWrite(A_1A, LOW);
    digitalWrite(A_1B, LOW);
  }
}

void wheel_R(const char* command)
{
  if ("forward" == command)

  {
    digitalWrite(B_1A, HIGH);
    digitalWrite(B_1B, LOW);
  }
  else if ("backward" == command)
  {
    digitalWrite(B_1A, LOW);
    digitalWrite(B_1B, HIGH);
  }
  else
  {
    digitalWrite(B_1A, LOW);
    digitalWrite(B_1B, LOW);
  }
}
