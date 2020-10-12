
const int trigpin = 8;
const int echopin = 9; 

const int led1 = A0;
const int led2 = A1; 
const int led3 = A2; 
const int led4 = A3;
const int led5 = A4 ; 

void setup() {

  Serial.begin(9600); 
  pinMode(trigpin, OUTPUT); 
  pinMode(echopin, INPUT); 

  pinMode(led1, OUTPUT); 
  pinMode(led2, OUTPUT); 
  pinMode(led3, OUTPUT); 
  pinMode(led4, OUTPUT); 
  pinMode(led5, OUTPUT); 

  digitalWrite(led1, LOW); 
  digitalWrite(led2, LOW); 
  digitalWrite(led3, LOW); 
  digitalWrite(led4, LOW); 
  digitalWrite(led5, LOW); 

  delay(1000); 

}

void loop() {

 int duration, distance; 
 
 digitalWrite(trigpin, HIGH); 
 
 delayMicroseconds(1000); 
 digitalWrite(trigpin, LOW); 

 duration = pulseIn(echopin,HIGH); 
 distance = (duration/2)/29.1; 

 Serial.println("cm: "); 
 Serial.println(distance); 

 if(distance > 0 && distance <= 5){
      digitalWrite(led1, HIGH); 
      digitalWrite(led2, HIGH); 
      digitalWrite(led3, HIGH); 
      digitalWrite(led4, HIGH); 
      digitalWrite(led5, HIGH); 
 }else
 if(distance > 5 && distance <= 10){
      digitalWrite(led1, LOW); 
      digitalWrite(led2, HIGH); 
      digitalWrite(led3, HIGH); 
      digitalWrite(led4, HIGH); 
      digitalWrite(led5, HIGH); 
 }else
 if(distance > 10 && distance <=15){
      digitalWrite(led1, LOW); 
      digitalWrite(led2, LOW); 
      digitalWrite(led3, HIGH); 
      digitalWrite(led4, HIGH); 
      digitalWrite(led5, HIGH); 
 }else
 if(distance > 15 && distance <= 20){
      digitalWrite(led1, LOW); 
      digitalWrite(led2, LOW); 
      digitalWrite(led3, LOW); 
      digitalWrite(led4, HIGH); 
      digitalWrite(led5, HIGH); 
 }else
 if(distance > 20 && distance <= 25){
      digitalWrite(led1, LOW); 
      digitalWrite(led2, LOW); 
      digitalWrite(led3, LOW); 
      digitalWrite(led4, LOW); 
      digitalWrite(led5, HIGH); 
 }else
 if(distance > 30){
      digitalWrite(led1, LOW); 
      digitalWrite(led2, LOW); 
      digitalWrite(led3, LOW); 
      digitalWrite(led4, LOW); 
      digitalWrite(led5, LOW); 
 }
 

}
