
const int EN1 = 5;
const int EN2 = 6;
const int DIR1 = 4;
const int DIR2 = 7;

unsigned long startTime;
unsigned long elapsedTime;

void setup()
{
  pinMode(EN1, OUTPUT);
  pinMode(EN2, OUTPUT);
  pinMode(DIR1, OUTPUT);
  pinMode(DIR2, OUTPUT);

  Serial.begin(9600);

  Serial.println("================================");
  Serial.println(" DC MOTOR INERTIA TEST");
  Serial.println("================================");

  delay(2000);
}

void runMotor()
{
  digitalWrite(DIR1, LOW);
  digitalWrite(DIR2, HIGH);

  analogWrite(EN1, 255);
  analogWrite(EN2, 255);

  Serial.println("Motor STARTED - PWM 255");

  startTime = millis();

  // Display rotation timer for 3 seconds
  while (millis() - startTime < 3000)
  {
    Serial.print("Rotation time: ");
    Serial.print((millis() - startTime) / 1000.0, 2);
    Serial.println(" s");

    delay(200);
  }

  Serial.println("3 seconds completed!");
}

void coastingTest()
{
  Serial.println("\n========== COASTING TEST ==========");

  runMotor();

  // Disable motor output to allow free rotation
  analogWrite(EN1, 0);
  analogWrite(EN2, 0);

  digitalWrite(DIR1, LOW);
  digitalWrite(DIR2, LOW);

  Serial.println("COASTING ACTIVATED!");
  Serial.println("Measuring inertia time...");

  unsigned long brakeStart = millis();

  // Display inertia timer
  while (millis() - brakeStart < 10000)
  {
    Serial.print("Inertia time: ");
    Serial.print((millis() - brakeStart) / 1000.0, 2);
    Serial.println(" s");

    delay(200);
  }

  Serial.println("Coasting observation completed.");
}

void dynamicBrakingTest()
{
  Serial.println("\n====== DYNAMIC BRAKING TEST ======");

  runMotor();

  // Apply dynamic braking
  digitalWrite(DIR1, HIGH);
  digitalWrite(DIR2, HIGH);

  Serial.println("DYNAMIC BRAKING ACTIVATED!");
  Serial.println("Measuring inertia time...");

  unsigned long brakeStart = millis();

  // Display inertia timer
  while (millis() - brakeStart < 10000)
  {
    Serial.print("Inertia time: ");
    Serial.print((millis() - brakeStart) / 1000.0, 2);
    Serial.println(" s");

    delay(200);
  }

  // Disable motor output after observation
  analogWrite(EN1, 0);
  analogWrite(EN2, 0);

  Serial.println("Dynamic braking observation completed.");
}

void loop()
{
  coastingTest();

  Serial.println("\nWaiting 5 seconds...");
  delay(5000);

  dynamicBrakingTest();

  Serial.println("\nBoth tests completed!");
  Serial.println("Repeating in 5 seconds...");

  delay(5000);
}