#include <Servo.h>

Servo myservo;
const int potPin = A0;
const int servoPin = 9;
const int ledPin = 5;

void setup() {
  Serial.begin(9600);
  while (!Serial); // Wait for serial port to connect (safeguard for native USB boards)
  
  myservo.attach(servoPin);
  pinMode(ledPin, OUTPUT);
}

void loop() {
  // Check for commands from Python
  if (Serial.available() > 0) {
    char command = Serial.read();
    
    // If 'q' is received, perform a software "reset" of all variables and outputs
    if (command == 'q') {
      digitalWrite(ledPin, LOW);
      myservo.write(90); // Center servo to safe default
      
      // Clear out any leftover bytes in the serial buffer
      while (Serial.available() > 0) {
        Serial.read();
      }
      
      Serial.println("SYSTEM RESET ACKNOWLEDGED");
      delay(500); // Brief pause to let Python catch up
      return;     // Skip the rest of this loop iteration and start fresh
    }
  }

  // Read potentiometer value (0-1023)
  int potValue = analogRead(potPin);
  
  // Convert potentiometer value to servo angle (0-180)
  int servoPos = map(potValue, 0, 1023, 0, 180);
  
  // Move servo according to potentiometer
  myservo.write(servoPos);
  
  // Turn LED ON if potentiometer value > 511
  if (potValue > 511) {
    digitalWrite(ledPin, HIGH);
  } else {
    digitalWrite(ledPin, LOW);
  }
  
  // Send potentiometer value and servo angle to Python
  Serial.print("Potentiometer: ");
  Serial.print(potValue);
  Serial.print(" | Servo Angle: ");
  Serial.println(servoPos);
  
  delay(50);
}