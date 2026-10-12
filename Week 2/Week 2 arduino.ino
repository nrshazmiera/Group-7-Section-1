const int segmentA = 2; 
const int segmentB = 3; 
const int segmentC = 4;
const int segmentD = 5;
const int segmentE = 6; 
const int segmentF = 7; 
const int segmentG = 8;
const int buttonIncrement = 9;
const int buttonReset = 10;

int count = 0;
bool lastButtonIncrement = HIGH;
bool lastButtonReset = HIGH;

void displayDigit(int num){
  switch (num){
    case 0:
      digitalWrite(segmentA, HIGH);
      digitalWrite(segmentB, HIGH);
      digitalWrite(segmentC, HIGH);
      digitalWrite(segmentD, LOW);
      digitalWrite(segmentE, HIGH);
      digitalWrite(segmentF, HIGH);
      digitalWrite(segmentG, HIGH);
      break;

    case 1:
      digitalWrite(segmentA, HIGH);
      digitalWrite(segmentB, LOW);
      digitalWrite(segmentC, LOW);
      digitalWrite(segmentD, LOW);
      digitalWrite(segmentE, LOW);
      digitalWrite(segmentF, LOW);
      digitalWrite(segmentG, HIGH);
      break;

    case 2:
      digitalWrite(segmentA, HIGH);
      digitalWrite(segmentB, HIGH);
      digitalWrite(segmentC, LOW);
      digitalWrite(segmentD, HIGH);
      digitalWrite(segmentE, HIGH);
      digitalWrite(segmentF, HIGH);
      digitalWrite(segmentG, LOW);
      break;

      case 3:
        digitalWrite(segmentA, HIGH);
        digitalWrite(segmentB, HIGH);
        digitalWrite(segmentC, LOW);
        digitalWrite(segmentD, HIGH);
        digitalWrite(segmentE, LOW);
        digitalWrite(segmentF, HIGH);
        digitalWrite(segmentG, HIGH);
        break;

      case 4:
        digitalWrite(segmentA, HIGH);
        digitalWrite(segmentB, LOW);
        digitalWrite(segmentC, HIGH);
        digitalWrite(segmentD, HIGH);
        digitalWrite(segmentE, LOW);
        digitalWrite(segmentF, LOW);
        digitalWrite(segmentG, HIGH);
        break;

      case 5:
        digitalWrite(segmentA, LOW);
        digitalWrite(segmentB, HIGH);
        digitalWrite(segmentC, HIGH);
        digitalWrite(segmentD, HIGH);
        digitalWrite(segmentE, LOW);
        digitalWrite(segmentF, HIGH);
        digitalWrite(segmentG, HIGH);
        break;

      case 6:
        digitalWrite(segmentA, LOW);
        digitalWrite(segmentB, HIGH);
        digitalWrite(segmentC, HIGH);
        digitalWrite(segmentD, HIGH);
        digitalWrite(segmentE, HIGH);
        digitalWrite(segmentF, HIGH);
        digitalWrite(segmentG, HIGH);
        break;

      case 7:
        digitalWrite(segmentA, HIGH);
        digitalWrite(segmentB, HIGH);
        digitalWrite(segmentC, LOW);
        digitalWrite(segmentD, LOW);
        digitalWrite(segmentE, LOW);
        digitalWrite(segmentF, LOW);
        digitalWrite(segmentG, HIGH);
        break;

      case 8:
        digitalWrite(segmentA, HIGH);
        digitalWrite(segmentB, HIGH);
        digitalWrite(segmentC, HIGH);
        digitalWrite(segmentD, HIGH);
        digitalWrite(segmentE, HIGH);
        digitalWrite(segmentF, HIGH);
        digitalWrite(segmentG, HIGH);
        break;

      case 9:
        digitalWrite(segmentA, HIGH);
        digitalWrite(segmentB, HIGH);
        digitalWrite(segmentC, HIGH);
        digitalWrite(segmentD, HIGH);
        digitalWrite(segmentE, LOW);
        digitalWrite(segmentF, HIGH);
        digitalWrite(segmentG, HIGH);
        break;
  }
}

void setup() {
  pinMode(segmentA, OUTPUT);
  pinMode(segmentB, OUTPUT);
  pinMode(segmentC, OUTPUT);
  pinMode(segmentD, OUTPUT);
  pinMode(segmentE, OUTPUT);
  pinMode(segmentF, OUTPUT);
  pinMode(segmentG, OUTPUT);
  pinMode(buttonIncrement, INPUT_PULLUP);
  pinMode(buttonReset, INPUT_PULLUP);
  displayDigit(count);
}

void loop() {
  bool currentButtonState = digitalRead(buttonIncrement);
  bool currentResetState = digitalRead(buttonReset);

  if (lastButtonIncrement == HIGH && currentButtonState == LOW) {
    count = (count + 1) % 10;
    displayDigit(count);
    delay(500);
  }

  if (lastButtonReset == HIGH && currentResetState == LOW){
    count = 0;
    displayDigit(count);
    delay(500);
  }

  lastButtonIncrement = currentButtonState;
  lastButtonReset = currentResetState;
}