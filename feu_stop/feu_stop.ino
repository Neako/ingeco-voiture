int ledStop = 13;
int accelere= 0;

void setup() {
  // put your setup code here, to run once:
  pinMode (ledStop, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
}

void feuStop() {
  if (accelere == 1) {
    digitalWrite(ledStop, LOW);
  } else {
      digitalWrite (ledStop, HIGH); 
    }
  }
