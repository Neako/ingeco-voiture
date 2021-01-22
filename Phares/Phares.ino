const int maRes = A0;
int valeurRes = 0;
int phare1 = 12; 
int phare2 = 8;


void setup() {
  // put your setup code here, to run once:
Serial.begin(9600); 
pinMode (phare1, OUTPUT);
pinMode (phare2, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
valeurRes = analogRead(maRes);
  if (valeurRes < 100) {
    digitalWrite (phare1, HIGH);
    digitalWrite (phare2, HIGH);
    }
   else if (valeurRes > 100) {
    digitalWrite (phare1, LOW);
    digitalWrite (phare2, LOW); 
   }
Serial.print ("\t");
Serial.println (valeurRes);
}
 
