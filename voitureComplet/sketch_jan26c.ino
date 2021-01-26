// PINS
// phares
const int phareSensor = A0;
int phares[] = {12, 8, 1, 0}; 
// stop
int ledStop = 13;
int ledStopOutput = 0;
// moteurs - idx: Left = 0 | Right = 1 | Avance = 2
int pinMoteurs[2] = {11, 10};
int valueMoteurs[3] = {0,0,0}; // HIGH = 1 | LOW = 0
float moteursWeights[2] = {1, 0.9};
// cligno - idx: Left = 0 | Right = 1
int ledCligno[2] = {2, 9};
int valueCligno[2] = {0, 0}; // HIGH = 1 | LOW = 0
int delayCligno = 100;
int clignoCompare;
int klaxonPin = 3;
#define trigPin 7
#define echoPin 4
long duration, distance;

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
  if (Serial.available() > 0) {
    parse_data();
  }
  // stop - only write when new information (avoid overflow of information)
  if (valueMoteurs[0] == 0 && valueMoteurs[1] == 0 && ledStopOutput == 0){
    ledStopOutput = !ledStopOutput;
    digitalWrite(ledStop, ledStopOutput); 
  } else if (ledStopOutput == 1 && !(valueMoteurs[0] == 0 && valueMoteurs[1] == 0)) { // turn off lights otherwise
    ledStopOutput = !ledStopOutput;
    digitalWrite(ledStop, ledStopOutput); 
  }
  // cligno
  //Serial.println(millis() - clignoCompare);
  if (valueMoteurs[0] > 0 && valueMoteurs[1] == 0){
    if (millis() - clignoCompare >= delayCligno) {
      cligno(0);
      clignoCompare = millis();
    }
    if (valueCligno[1] == 1) {
      cligno(1); // turn off in case still on
    }
  } else if (valueMoteurs[1] > 0 && valueMoteurs[0] == 0){
    if (millis() - clignoCompare >= delayCligno) {
      cligno(1);
      clignoCompare = millis();
    }
    if (valueCligno[0] == 1) {
      cligno(0); // turn off in case still on
    }
  } else { // turn off cligno otherwise
    if (valueCligno[0] == 1){
      cligno(0);
    } else if (valueCligno[1] == 1){
      cligno(1);
    }
  }
  // moteur
  controlMotor(valueMoteurs);
  // antichoc
  antiChoc();
}


// ------------------------- ANTICHOC ------------------------
void klaxon() {
  tone(klaxonPin, 440, 500);
}

void antiChoc() {
 // Envoie de l'onde
 digitalWrite(trigPin, LOW);
 delayMicroseconds(2);
 digitalWrite(trigPin, HIGH);
 delayMicroseconds(10);
 digitalWrite(trigPin, LOW);
 // Réception de l'écho
 duration = pulseIn(echoPin, HIGH);
 // Calcul de la distance
 distance = (duration/2) / 29.1;
 // Reaction
 if (distance <= 11){
 //valueMoteur = [0, 0, 0];
 //modifier pour avoir le bruit voulu (insérer un interval si on veut) 
 klaxon();
 }
}

// --------------------------- LED ---------------------------
void cligno(int lrCligno) {
  valueCligno[lrCligno] = !valueCligno[lrCligno];
  digitalWrite(ledCligno[lrCligno], valueCligno[lrCligno]);
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
void controlMotor(int val[3]) { 
  // moteurs - idx: Left = 0 | Right = 1 | Avance = 2
  for (int i = 0; i < 2; i++){
    analogWrite(pinMoteurs[i], moteursWeights[i]*125*val[i]); // motor imbalance
  }
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
  Serial.print(valueMoteurs[0]);
  Serial.println(valueMoteurs[1]);
}
