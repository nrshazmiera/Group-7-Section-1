const int portLED = 5;

void setup() {
  pinMode(portLED, OUTPUT);
  Serial.begin(9600);
}
void loop() {
int potValue = analogRead(A0);
if(potValue>511){
  digitalWrite(portLED, HIGH);
}
else{
  digitalWrite(portLED, LOW);
}
Serial.println(potValue);
delay(1000); // add a delay to avoid sending data too fast
}