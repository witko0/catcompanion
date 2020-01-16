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
const int PWR_PIN_1   = 4;  // additional 5V power supply
const int TAIL_PIN    = 2;  // tail PWM
const int A_1A_LW_PWM = 6;  // left wheel PWM (Timer 0)
const int A_1B_LW_DIR = 7;  // left wheel direction
const int B_1A_RW_PWM = 5;  // right wheel PWM (Timer 0)
const int B_1B_RW_DIR = 11; // right wheel direction
const int TRIG_PIN_M  = 12; // middle distance sensor trigger pin
const int ECHO_PIN_M  = 3;  // middle distance sensor echo pin
const int TRIG_PIN_R  = 13; // right distance sensor trigger pin
const int ECHO_PIN_R  = 10; // right distance sensor echo pin
const int TRIG_PIN_L  = 8;  // left distance sensor trigger pin
const int ECHO_PIN_L  = 9;  // left distance sensor echo pin

// PWM duty cycle
const int PWM_SPEED_MID  = 128;

int distance_mid, distance_l, distance_r;

/* PWM Timer 0 prescaler options (pins 5 & 6)
Setting                          Prescale_factor
TCCR0B = _BV(CS00);              1
TCCR0B = _BV(CS01);              8
TCCR0B = _BV(CS00) | _BV(CS01);  64
TCCR0B = _BV(CS02);              256
TCCR0B = _BV(CS00) | _BV(CS02);  1024
*/

void setup()
{
  // Set PWM Timer 0 prescaler to 1 (Fast PWM 62.500 kHz)
  //TCCR0A = _BV(COM0A1) | _BV(COM0B1) | _BV(WGM01) | _BV(WGM00); 
  //TCCR0B = _BV(CS01);
  // Also modified: Arduino\hardware\arduino\avr\cores\arduino\wiring.c :
  //                #define MICROSECONDS_PER_TIMER0_OVERFLOW (clockCyclesToMicroseconds(1 * 256))

  // Engines steering by L9110S
  pinMode(A_1A_LW_PWM, OUTPUT);
  pinMode(A_1B_LW_DIR, OUTPUT);
  pinMode(B_1A_RW_PWM, OUTPUT);
  pinMode(B_1B_RW_DIR, OUTPUT);
  digitalWrite(A_1A_LW_PWM, LOW);
  digitalWrite(A_1B_LW_DIR, LOW);
  digitalWrite(B_1A_RW_PWM, LOW);
  digitalWrite(B_1B_RW_DIR, LOW);

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

    wheels("stop");
    delay(300);
  }
  else
  {
    digitalWrite(LED_BUILTIN, LOW);
    wheels("forward");
    delay(100);
  }
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
    digitalWrite(A_1B_LW_DIR, HIGH);
    analogWrite(A_1A_LW_PWM, 255 - PWM_SPEED_MID);
  }
  else if ("backward" == command)
  {
    digitalWrite(A_1B_LW_DIR, LOW);
    analogWrite(A_1A_LW_PWM, PWM_SPEED_MID);
  }
  else
  {
    digitalWrite(A_1A_LW_PWM, LOW);
    digitalWrite(A_1B_LW_DIR, LOW);
  }
}

void wheel_R(const char* command)
{
  if ("forward" == command)
  {
    digitalWrite(B_1B_RW_DIR, LOW);
    analogWrite(B_1A_RW_PWM, PWM_SPEED_MID);
  }
  else if ("backward" == command)
  {
    digitalWrite(B_1B_RW_DIR, HIGH);
    analogWrite(B_1A_RW_PWM, 255 - PWM_SPEED_MID);
  }
  else
  {
    digitalWrite(B_1A_RW_PWM, LOW);
    digitalWrite(B_1B_RW_DIR, LOW);
  }
}
