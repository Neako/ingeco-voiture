// PINS
// phares
const int phareSensor = A0;
int phares[] = {12, 8, 1, 0}; 
// stop
int ledStop = 13;
int ledStopOutput = 0;
// moteurs - idx: Left = 0 | Right = 1 | Avance = 2
int pinMoteurs[2] = {10, 11};
int valueMoteurs[3] = {0, 0, 0}; // HIGH = 1 | LOW = 0
// cligno - idx: Left = 0 | Right = 1
int ledCligno[2] = {2, 9};
int valueCligno[2] = {0, 0}; // HIGH = 1 | LOW = 0
int delayCligno = 100;
int clignoCompare;

void setup() {
  // pins as OUTPUT
  for (int i = 0; i < 4; i++){ 
    pinMode(phares[i], OUTPUT);
  }
  pinMode(ledStop, OUTPUT);
  pinMode(ledCligno[0], OUTPUT); // Gauche
  pinMode(ledCligno[1], OUTPUT); // Droite
  pinMode(pinMoteurs[0], OUTPUT); // MotG
  pinMode(pinMoteurs[1], OUTPUT); // MotD
  // Initialize values
  int clignoCompare = millis();
  // Serial (from python / output as control)
  Serial.begin(9600);
}

void loop() {
  // phares
  int valeurSensor = analogRead(phareSensor);
  lightPhares(valeurSensor);
  // from Serial
  parse_data();
  // stop
  if (valueMoteurs[0] == 0 && valueMoteurs[1] == 0 && valueMoteurs[2] == 0){
    ledStopOutput = !ledStopOutput;
    digitalWrite(ledStop, ledStopOutput); 
  } else if (ledStopOutput == 1) { // turn off lights otherwise
    ledStopOutput = !ledStopOutput;
    digitalWrite(ledStop, ledStopOutput); 
  }
  // cligno
  if (valueMoteurs[0] > 0 && valueMoteurs[2] == 0){
    cligno(0);
  } else if (valueMoteurs[1] > 0 && valueMoteurs[2] == 0){
    cligno(1);
  } else { // turn off cligno otherwise
    if (valueCligno[0] == 1){
      cligno(0);
    } else if (valueCligno[1] == 1){
      cligno(1);
    }
  }
  // moteur
  // controlMotor(1, 0.9);
}


// --------------------------- LED ---------------------------
void cligno(int lrCligno) {
      if (millis() - clignoCompare >= delayCligno) {
        digitalWrite(ledCligno[lrCligno], valueCligno[lrCligno]);
        valueCligno[lrCligno] = !valueCligno[lrCligno];
        clignoCompare = millis();
      }
}

void lightPhares(int valeurSensor){
  // Allumage / Extinction des feux de voiture en 
  // fonction de la luminosité
  // Deux pins par phare (leds R+V)
  if (valeurSensor < 100){
    for (int idx = 0; idx < 4; idx++){
      digitalWrite(phares[idx], HIGH);
    }
  } else { // (valeurSensor > 100)
    for (int idx = 0; idx < 4; idx++){ 
      digitalWrite(phares[idx], LOW);
    }
  }
}

// ------------------------- MOTEURS ------------------------
void controlMotor(float spdD, float spdG) {
  int motD = spdD*255;
  int motG = spdG*255;
  analogWrite(10, motD);
  analogWrite(11, motG);
  Serial.println(motD);
}


// -------------------------- DATA --------------------------
void parse_data(){
  // Parse Serial from Python
  valueMoteurs[0] = Serial.readStringUntil(',').toInt();
  Serial.read(); // removing next character ','
  valueMoteurs[1] = Serial.readStringUntil(',').toInt();
  //Serial.read(); // removing next character ','
  //valueMoteurs[2] = Serial.readStringUntil('\n').toInt();
  Serial.read(); // removing next character '\n'
  valueMoteurs[2] = (valueMoteurs[0] == valueMoteurs[1]);
}
