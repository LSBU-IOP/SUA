//MobileBot V2
//Dr A.Vilches @ LSBU
#include <Arduino.h>
#include <PrintEx.h>
PrintEx serial = Serial;
//(PI * _wheel_diameter) / _encoder_slots                           //linear travel in mm per slot count
#define IR 4                                                        //IR signal in; detector mounted on LEFT MOTOR pin 4
#define PWML 5                                                      //PWM signal out LEFT MOTOR pin 5
#define PWMR 6                                                      //PWM signal out RIGHT MOTOR pin 6
#define INML 7                                                      //Digital signal out LEFT MOTOR pin 7; 0 to move FORWARD
#define INMR 8                                                      //Digital signal out RIGHT MOTOR pin 8; 0 to move FORWARD
#define STATUS 3                                                    //Status indicator (connect to buzzer & LED)
#define echoPin 12 //attach pin to Echo of HC-SR04
#define trigPin 13 //attach pin to Trig of HC-SR04
#define _encoder_slots 20                                           //number of slots on encoder wheel
#define _deg_per_slot 18                                            //theoretical degrees per encoder slot = 360 / _encoder_slots
#define _wheel_diameter 65                                          //wheel diameter in mm
#define _mm_per_slot 10.21017612                                    //mm per slot calculated pre run
//(PI * _wheel_diameter) / _encoder_slots                           //linear travel in mm per slot count
#define tt 400                                                      //timeout wait time for functions in mS
#define forward 1
#define back 0
#define left 0
#define right 1

//Function Prototypes
bool detectChange(int pin);
bool detectSlot();
void alert();
bool drive(bool direction, unsigned long cm);
bool turn(bool direction, unsigned char angle);
void allStop();
float scanAhead();

//Functions
void setup(){
    pinMode(IR, INPUT);
    pinMode(echoPin, INPUT);
    pinMode(trigPin, OUTPUT);
    pinMode(STATUS, OUTPUT);
    pinMode(INML, OUTPUT);
    pinMode(INMR, OUTPUT);
    pinMode(PWMR, OUTPUT);
    pinMode(PWML, OUTPUT);
    pinMode(trigPin, OUTPUT); // Sets the trigPin as an OUTPUT
    pinMode(echoPin, INPUT); // Sets the echoPin as an INPUT
    pinMode(11,OUTPUT);
    pinMode(2,OUTPUT); pinMode(3,OUTPUT); // beep speaker
    digitalWrite(2,LOW);
    digitalWrite(11,HIGH);
    Serial.begin(9600);
}
//Reads the distance to an obstacle from the front of the vehicle; returns in cm.
float scanAhead(){
  unsigned long duration; // variable for the duration of sound wave travel
  float distanceCm; // variable for the distance measurement
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  distanceCm = duration * 0.034 / 2;
  serial.printf("Distance to obstacle is %f cm", distanceCm);
  return distanceCm;
}

void allStop(){
   digitalWrite(INML,0);
   digitalWrite(INMR,0);
   digitalWrite(PWML, 0);                                       //set speed
   digitalWrite(PWMR, 0);                                       //
}

void alert(){
    analogWrite(STATUS, 127);     //Bleep to indicate start of program
    delay(100);
    analogWrite(STATUS, LOW);
    delay(100);
    analogWrite(STATUS, 127);     //Bleep to indicate start of program
    delay(100);
    analogWrite(STATUS, LOW);
    delay(100);
    analogWrite(STATUS, 127);     //Bleep to indicate start of program
    delay(100);
    analogWrite(STATUS, LOW);
    delay(100);  
}

//Detects a change in a digital input from any pin. Returns a 0 on timeout and 1 if OK.
bool detectChange(int pin){
    unsigned long t = millis();
    bool currentState = digitalRead(pin);
    while((digitalRead(pin) == currentState) && ((millis() - t) < tt) );
    if( (millis() - t) >= tt) return(0);
       else return(1);
}

//Detects a slot by sensing 2x changes (toggle) on the IR input pin
//Returns 1 if all OK or 0 if a timeout occured.
bool detectSlot(){
    if(detectChange(IR) && detectChange(IR)) return (1);
        else return (0);
}

//Direction is left or right. Angle is any number from 1 to 180 in deg.
//Returns 1 if successful or 0 if timedout or wrong angle range entered.
bool turn(int Direction, unsigned char angle){
    unsigned char count = 0;
    if(angle > 180) return(0);                                         //abort function if angle > 180 deg.
    if(angle < _deg_per_slot) angle = 1;                            //min angle is 18 deg, equivalent to 1 slot count.
    unsigned char nSlots = angle/5.0;                               //In practice 1 slot is worth roughly 5 degrees.
    allStop();
    digitalWrite(INML,Direction);
    if (Direction){
    analogWrite(PWML, 255-100);     
    }
    else{
      analogWrite(PWML,100);
    }                                            //rotate left motor slowly
    while(count <= nSlots) {                                          //do till count = angle
      if (detectSlot()) count++;                                  //increase slot count if one is detected
            else return(0);                                         //failed to turn due to timeout
    }
    Serial.println(count);
    allStop();
    return(1);                                                     //return successful
}

//Returns a 1 if move was successful or 0 if timedout.
//cm must be minimum of 10
//Speed can be slow, normal or fast. Direction is left or right.
bool drive(bool Direction, unsigned long cm){
    if(cm<10) return 0;
    unsigned long count = 0;
    unsigned char Speed;
    cm = cm - 7;
    float distance = (float) ((cm * 10.0) / _mm_per_slot);         //number of mm to move
    unsigned long travel = (unsigned long) distance;               //number of slots to count for movement
    Serial.println("\nslots counted: ");
    Serial.println(travel);
    if(Direction) Speed = 180;
    // speed = 240 to 10
      else Speed = 255-180;
    digitalWrite(INML,Direction);
    digitalWrite(INMR,Direction);
    analogWrite(PWML, Speed);                                       //set speed
    analogWrite(PWMR, Speed);                                       //

    while(count <= travel) {                                         //do till count = travel in cm
        if (detectSlot()) count++;                                  //increase slot count if one is detected
            else return(0);                                         //failed to turn due to timeout
    }
    //Serial.print(count);
    allStop();
    return(1);                                                      //return successful
}


//************* STUDENTS USE THIS PART BELOW ONLY *****************

void loop() {
  alert();
  drive(forward,10);
  while(1);
}



