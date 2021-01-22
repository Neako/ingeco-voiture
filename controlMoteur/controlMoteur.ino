int motD = 0;
int motG = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  Serial.begin(9600);

}

void loop() {
  // put your main code here, to run repeatedly:
  controlMotor(1, 0.9);

}

void controlMotor(float spdD, float spdG) {
  motD = spdD*255;
  motG = spdG*255;
  analogWrite(10, motD);
  analogWrite(11, motG);
  Serial.println(motD);
}
