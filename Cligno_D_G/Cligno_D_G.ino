int ledD = 9;
int ledG = 2 ;
int droite = 0;
int gauche = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode (ledD, OUTPUT);
  pinMode (ledG, OUTPUT);
  clignoD(droite);
  clignoG(gauche);
}

void loop() {
  // put your main code here, to run repeatedly:
}
void clignoD(int droite) {
    if (droite == 1) {
      digitalWrite(ledD, HIGH);
      delay(100);
      digitalWrite(ledD, LOW);
      delay(100);
    }
}
void clignoG(int gauche) {
      if (gauche = 1) {
        digitalWrite(ledG, HIGH);
        delay(100);
        digitalWrite(ledG, LOW);
        delay(100);
      }
}
