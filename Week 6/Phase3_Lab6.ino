#include <ESP32Servo.h>


Servo myServo;


int buttonPin = 13;
int servoPin = 2;


int pos = 0;
bool direction = true;


void setup() {
  pinMode(buttonPin, INPUT_PULLUP);
  myServo.attach(servoPin);
}


void loop() {


  int buttonState = digitalRead(buttonPin);


  if (buttonState == LOW) {


    // Servo panning
    if (direction) {
      pos++;
      if (pos >= 180) direction = false;
    } else {
      pos--;
      if (pos <= 0) direction = true;
    }


    myServo.write(pos);
    delay(10);
  }




}
