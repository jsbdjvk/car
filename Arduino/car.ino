#include <Servo.h>

/**************************start of car*********************************/
const int MOTOR_NUM = 4;
const int MOTOR_PIN_NUM = 2;

const int MOTOR_PIN[MOTOR_NUM][MOTOR_PIN_NUM] = {
  {3,2},
  {5,4},
  {6,7},
  {9,8},
};

const int MOTOR_ACTION_STOP = 0;  // 停止
const int MOTOR_ACTION_START = 1; // 启动
const int MOTOR_ACTION_BACK = 2;  // 后退
const int MOTOR_ACTION_TURN = 3;  // 转弯

const int CAR_ACTION_SPEED[4] = {0, 120, 250, 130}; // 小车动作转速，[0, 250]

const int CAR_TURN_ANGLE = 200; // 小车转弯角度，这里用转弯时长来表示，单位ms
const int CAR_STOP_TIMES = 500; // 小车遇到障碍物停车时长，单位ms

/*
 * desc：获取小车移动速度
 * @action：动作
 */
int getSpeed(int action = 0)
{
  return CAR_ACTION_SPEED[action];
}

/*
 * desc: 电机
 * @motorIndex：电机编号
 * @reverse：反转
 * @speeds：速度（0 - 250），注意：只有正转可以控制速度，因为反向的所有引脚不在pwm上
 */
void motor(int motorIndex = 0, boolean reverse = false, int speeds = 0)
{
  int pin1 = MOTOR_PIN[motorIndex][0];
  int pin2 = MOTOR_PIN[motorIndex][1];

  if (0 == speeds)
  {
    digitalWrite(pin1,LOW);
    digitalWrite(pin2,LOW);
  } else
  {
    if (!reverse) //正转
    {
      digitalWrite(pin1,HIGH);
      digitalWrite(pin2,LOW);
    } else //反转
    {
      digitalWrite(pin1,LOW);
      digitalWrite(pin2,HIGH);
    }
  }

  analogWrite(pin1, speeds);
}

void startCar()
{
  int i,j;
  for (i = 0; i < MOTOR_NUM; ++i)
  {
    motor(i, false, getSpeed(MOTOR_ACTION_START));
  }
}

/*
 * desc：停车
 * @times: 停车时长，默认0是一直停车
 */
void stopCar(int times = 0)
{
  int i,j;
  for (i = 0; i < MOTOR_NUM; ++i)
  {
    motor(i, false, getSpeed(MOTOR_ACTION_STOP));
  }

  if (0 != times)
  {
    delay(times);
  }
}

void goCar()
{
  startCar();
}

/*
 * desc：左转
 * @turnAngle: 转弯角度，用转弯时间来定义
 */
void leftCar(int turnAngle)
{
  motor(0, true, getSpeed(MOTOR_ACTION_TURN));
  motor(1, true, getSpeed(MOTOR_ACTION_TURN));
  motor(2, false, getSpeed(MOTOR_ACTION_TURN));
  motor(3, false, getSpeed(MOTOR_ACTION_TURN));
  delay(turnAngle);
}

/*
 * desc：右转
 * @turnAngle: 转弯角度，用转弯时间来定义
 */
void rightCar(int turnAngle)
{
  motor(0, false, getSpeed(MOTOR_ACTION_TURN));
  motor(1, false, getSpeed(MOTOR_ACTION_TURN));
  motor(2, true, getSpeed(MOTOR_ACTION_TURN));
  motor(3, true, getSpeed(MOTOR_ACTION_TURN));
  delay(turnAngle);
}

/**************************end of car*********************************/

/**************************start of 舵机*********************************/
const int ServoPin = 11;
const int SERVO_REVOLVE_ANGLE = 20; // 每次旋转角度
const int SERVO_REVOLVE_MIN_ANGLE = 10; // 转动的最小角度
const int SERVO_REVOLVE_MAX_ANGLE = 100; // 转动的最大角度
int angle_now = SERVO_REVOLVE_MIN_ANGLE;

Servo myservo;

void servoRevolve(int angle = 0)
{
  myservo.write(angle);
}
/**************************end of 舵机*********************************/

/**************************start of HC-SR04（超声波）*********************************/
const int HCSRPingPin = 13;
const int HCSREchoPin = 12;

const int OBSTACLE_CM = 10; // 小车转弯条件，离障碍物10，单位cm
const int GET_DISTANCE_NUM = 3; // 每次超声波探测3次

long microsecondsToCentimeters(long microseconds) {
   return microseconds / 29 / 2;
}

/*
 * desc：获取3次距离求平均值
 */
long getDistance()
{
  long sum = 0;
  
  for (int i = 0; i < GET_DISTANCE_NUM; ++i)
  {
    long duration, cm;
    digitalWrite(HCSRPingPin, LOW);
    delayMicroseconds(2);
    digitalWrite(HCSRPingPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(HCSRPingPin, LOW);
    duration = pulseIn(HCSREchoPin, HIGH);
    cm = microsecondsToCentimeters(duration);
    
    sum += cm;

    delayMicroseconds(10);
  }
  
  return (sum / GET_DISTANCE_NUM);
}

boolean hasObstacle()
{
  // 如果转到90度了，就反转
  if (angle_now >= SERVO_REVOLVE_MAX_ANGLE)
  {
    for (; SERVO_REVOLVE_MIN_ANGLE <= angle_now; angle_now -= SERVO_REVOLVE_ANGLE)
    {
      servoRevolve(angle_now);
  
      int cm = getDistance();
      if (0 < cm && cm <= OBSTACLE_CM)
      {
        return true;
      }
    }
  } else
  {
    for (;angle_now < SERVO_REVOLVE_MAX_ANGLE; angle_now += SERVO_REVOLVE_ANGLE)
    {
      servoRevolve(angle_now);
  
      int cm = getDistance();
      if (0 < cm && cm <= OBSTACLE_CM)
      {
        return true;
      }
    }
  }
  
  
  
  return false;
}
/**************************end of HC-SR04（超声波）*********************************/

void setup()
{
  // 波特率
  Serial.begin(9600);

  // 电机
  int i,j;
  for (i = 0; i < MOTOR_NUM; ++i)
  {
    for (j = 0; j < MOTOR_PIN_NUM; ++j)
    {
      pinMode(MOTOR_PIN[i][j], OUTPUT);
      digitalWrite(MOTOR_PIN[i][j], LOW);
    }
  }

  // 超声波
  pinMode(HCSRPingPin, OUTPUT);
  pinMode(HCSREchoPin, INPUT);

  // 舵机
  myservo.attach(ServoPin);
}

void loop()
{
  //return;

  // 判断是否有障碍物
  // 有：停车3s，右转一定角度，再前进
  while (hasObstacle())
  {
    Serial.println("有障碍物!!");
    stopCar(CAR_STOP_TIMES);
    rightCar(CAR_TURN_ANGLE);
  }
  
  startCar();
}
