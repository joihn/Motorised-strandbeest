#include <SPI.h>
#include "RF24.h" //using rf24 library by TMRH20, Go here for the wiring setup https://tmrh20.github.io/RF24/


//H bridge connection, more info https://www.bananarobotics.com/shop/How-to-use-the-HG7881-(L9110)-Dual-Channel-Motor-Driver-Module
#define A_PWM  3// Motor A, motor left, PWM Speed, digital pin 3, this pin controls the speed, wired to hbridge pin A-1A
#define A_DIR  4// Motor A, motor left, Direction, digital pin 4, this pin controls direction, wired to hbridge pin A-1B
#define B_PWM  5// Motor B, motor right, PWM Speed, digital pin 5, this pin controls speed, wired to hbridge pin B-1A
#define B_DIR  6// Motor B, motor right, Direction, digital pin 6, this pin controls direction, wired to hbridge pin B-1B

byte addresses[][6] = {"1Node", "2Node"}; //radio setup
RF24 radio(7, 8);  //radio setup




struct dataStruct { //radio structure to receive the data of the joystick
  int xPosition ;
  int yPosition ;
} myData;


int xPosition = 0; //raw data of the joystick, from 0 to 1023
int yPosition = 0;
float fxPosition ; // modified data of the joystick, between -1 and 1
float fyPosition;
float left; //the speed of each motor, between -1 and 1
float right;






void setup() {
    //hbridge setup, more info: https://www.bananarobotics.com/shop/How-to-use-the-HG7881-(L9110)-Dual-Channel-Motor-Driver-Module
  pinMode(A_PWM, OUTPUT); 
  pinMode(A_DIR, OUTPUT);
  pinMode(B_PWM, OUTPUT);
  pinMode(B_DIR, OUTPUT);

  Serial.begin(115200);
  radio.begin();
  radio.setPALevel(RF24_PA_HIGH);
  radio.openWritingPipe(addresses[0]);
  radio.openReadingPipe(1, addresses[0]);

  radio.startListening();
}

void loop() {

  while (radio.available()) {                          // While there is data ready  //radio stuff
    radio.read( &myData, sizeof(myData) );             // Get the payload   //radio stuff
    Serial.println("got the data");
  }
  xPosition = myData.xPosition;
  yPosition = myData.yPosition;
  fxPosition = mapfloat(xPosition, 0, 1023, -1, 1); // x position is now between -1 and 1, no user input on the joystick give 0. maximum jostick left =-1, maximum joystick right=1
  fyPosition = mapfloat(yPosition, 0, 1023, 1, -1); //y position is now between -1 and 1, no user input on the joystick give 0. maximum jostick top =1, maximum joystick back=-1
  Serial.print("fX: "); //for debugging
  Serial.print(fxPosition);

  Serial.print(" | fY: ");
  Serial.print(fyPosition);

  if (fyPosition >= -0.2) { //avoiding the robot to move alone
    left = fyPosition + fxPosition; //some maths to define how the robot move and turns when going forward
    right = fyPosition - fxPosition;
  } else { //some math to define how the robot move and turns when going backward
    left = fyPosition - fxPosition;
    right = fyPosition + fxPosition;
  }


  if (left > 1) // avoid values out of range
  {
    left = 1;
  }
  if (left < -1)
  {
    left = -1;
  }


  if (right > 1)
  {
    right = 1;
  }
  if (right < -1)
  {
    right = -1;
  }

  // moving the beast
  if (right >= 0.2 or right <= -0.2 ) { ////avoiding the robot to move alone
    move('r', right);  /// turn to the right (r) with the speed "right"
  }
  else {
    move('r', 0); //shut down the motor
  }


  if (left >= 0.2 or left <= -0.2) {
    move('l', left); /// turn to the left (l) with the speed "left"
  }
  else {
    move('l', 0); /////shut down the motor



  }

  Serial.print(" | left: ");
  Serial.print(left);

  Serial.print(" | right: ");
  Serial.println(right);
  delay(10);
}

float mapfloat(float x, float in_min, float in_max, float out_min, float out_max) //native map function doesn't allow float. I wanted float.
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
void move(char motor, float speed) { // this is the move function, designed to move the robot ;) motor variable is l or r  (left and right)  // speed is between 1 and -1, 1=maximum forward, -1= maximum backward
  if (motor == 'l') {
    if (speed == 0) { //shutdown the motor
      digitalWrite(A_DIR, LOW);
      digitalWrite(A_PWM, LOW);
    }
    else if (speed > 0) {
      digitalWrite(A_DIR, LOW); //we want to go forward
      speed = mapfloat(speed, 0, 1, 0.6, 1); // not very powerfull motor, so I start them with 60% of the PWM (0.6)
      analogWrite(A_PWM, speed * 255); // writing the speed
    } else if (speed < 0) { //we want to go backward
      digitalWrite(A_DIR, HIGH); //we want to go backward
      speed = mapfloat(speed, 0, -1, -0.6, -1); //// // not very powerfull motor, so I start them with 60% of the PWM (0.6)
      analogWrite( A_PWM, mapfloat(-speed * 255, 0, 255, 255, 0)); // when you go backward, you need to remap the pwm. 255 is minimum and 0 is maximum. This is due to the h-bridge. https://www.bananarobotics.com/shop/How-to-use-the-HG7881-(L9110)-Dual-Channel-Motor-Driver-Module
    }
  }
  else { //motor r (right)

    if (speed == 0) { //shutdown
      digitalWrite(B_DIR, LOW);
      digitalWrite(B_PWM, LOW);
    } else if (speed > 0) {
      digitalWrite(B_DIR, LOW);
      speed = mapfloat(speed, 0, 1, 0.6, 1); //// not very powerfull motor
      analogWrite(B_PWM, speed * 255);
    } else if (speed < 0) {
      digitalWrite(B_DIR, HIGH);
      speed = mapfloat(speed, 0, -1, -0.6, -1); //// not very powerfull motor
      analogWrite( B_PWM, mapfloat(-speed * 255, 0, 255, 255, 0));
    }
  }



}

