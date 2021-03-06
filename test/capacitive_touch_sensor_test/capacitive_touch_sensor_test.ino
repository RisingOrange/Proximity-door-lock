#define ctsPin 2
#define ledPin 7

void setup() {
  Serial.begin(9600);
  pinMode(ctsPin, INPUT);
  pinMode(ledPin, OUTPUT);
}

void loop() {
  int ledValue = LOW;
  
  // put your main code here, to run repeatedly:
  int ctsValue = digitalRead(ctsPin);
  
  if (ctsValue == HIGH){
    Serial.println("sensor touched");
    ledValue = HIGH;
  }

  digitalWrite(ledPin, ledValue);
}
