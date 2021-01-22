const int maRes = A0;
int valeurRes = 0;
int phare1 = 12; 
int phare2 = 8;
int phare3 = 1; 
int phare4 = 0;


void setup() {
  // put your setup code here, to run once:
pinMode (phare1, OUTPUT);
pinMode (phare2, OUTPUT);
pinMode (phare3, OUTPUT);
pinMode (phare4, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
valeurRes = analogRead(maRes);
  if (valeurRes < 100) {
    digitalWrite (phare1, HIGH);
    digitalWrite (phare2, HIGH);
    digitalWrite (phare3, HIGH);
    digitalWrite (phare4, HIGH);
    }
   else if (valeurRes > 100) {
    digitalWrite (phare1, LOW);
    digitalWrite (phare2, LOW); 
    digitalWrite (phare3, LOW);
    digitalWrite (phare4, LOW);
   }
}
 
