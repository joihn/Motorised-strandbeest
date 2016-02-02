#include <SPI.h>
#include "RF24.h" //using rf24 library by TMRH20, Go here for the wiring setup https://tmrh20.github.io/RF24/

byte addresses[][6] = {"1Node", "2Node"}; //radio stuff, if you want to learn more about he radio go here https://tmrh20.github.io/RF24/
RF24 radio(7, 8);//radio stuff
int yPin = A6; //pin of the joystick y axis
int xPin = A7; // pin of the joystick x axis
//int buttonPin = 5; //pin for the button of the joystick (not currently supported)
struct dataStruct { //we put the varaible in a structure for sending it easily
  int xPosition;
  int yPosition;
  //int buttonState = 0; // button of the joystick (not currently supported)
} myData;

void setup() {

  Serial.begin(115200);
  radio.begin();//radio stuff
  radio.setPALevel(RF24_PA_HIGH);//radio stuff
  radio.openWritingPipe(addresses[0]);//radio stuff
  radio.openReadingPipe(1, addresses[0]); //radio stuff
  pinMode(xPin, INPUT); //joystick
  pinMode(yPin, INPUT);
  //pinMode(buttonPin, INPUT_PULLUP); // button of the joystick (not currently supported)
}

void loop() {
  myData.xPosition = analogRead(xPin); //reads the joystick
  myData.yPosition = analogRead(yPin);
  //myData.buttonState =! digitalRead(buttonPin); // button of the joystick (not currently supported)


  Serial.print("X: "); //for debugging
  Serial.print(myData.xPosition);

  Serial.print(" | Y: ");
  Serial.print(myData.yPosition);
  //Serial.print(" | button: "); // button of the joystick (not currently supported)
  //Serial.println(myData.buttonState); // button of the joystick (not currently supported)



  if (!radio.write( &myData, sizeof(myData) )) {
    Serial.println(F("sorry failed to send the data")); //don't know why, he always says sorry... However it still sends the data. Strange but works :)

  } else {
    Serial.println(F("yay I sent the data"));






  }
}
