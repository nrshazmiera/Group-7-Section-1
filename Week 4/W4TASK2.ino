#include <Wire.h>
#include <Servo.h>
#include <MPU6050.h> // Ensure you have the MPU6050 library installed

Servo servo;
MPU6050 mpu;

const int servoPin = 9;
const int greenLED = 4;
const int redLED = 3;

void setup() {
  Serial.begin(9600);
  Wire.begin();
  
  // Initialize MPU6050
  mpu.initialize();
  
  servo.attach(servoPin);
  servo.write(90); // Default locked position
  
  pinMode(greenLED, OUTPUT);
  pinMode(redLED, OUTPUT);
  
  // Initial state: Locked (Red ON, Green OFF)
  digitalWrite(greenLED, LOW);
  digitalWrite(redLED, HIGH);
}

void loop() {
  // 1. Read MPU6050 accelerometer and gyroscope data
  int16_t ax, ay, az, gx, gy, gz;
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

  // Stream data over serial as comma-separated values
  Serial.print(ax); Serial.print(",");
  Serial.print(ay); Serial.print(",");
  Serial.print(az); Serial.print(",");
  Serial.print(gx); Serial.print(",");
  Serial.print(gy); Serial.print(",");
  Serial.println(gz);

  // 2. Check for commands from Python ('A' for Access/Unlock, 'D' for Deny/Lock)
  if (Serial.available() > 0) {
    char command = Serial.read();
    if (command == 'A') {
      servo.write(180); // Unlock
      digitalWrite(greenLED, HIGH);
      digitalWrite(redLED, LOW);
    } else if (command == 'D') {
      servo.write(90); // Lock
      digitalWrite(greenLED, LOW);
      digitalWrite(redLED, HIGH);
    }
  }
  
  delay(50); // Small delay to stabilize data stream
}