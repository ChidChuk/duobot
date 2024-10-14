/* This code is for a self-driving automobile
 *  Code and mechanics from: Chidubem and Aidan
 */

// Note: try to switch the V0 pin on the left side of the mtor chip

const int TRIG_PIN = 7; // Arduino pin connected to Ultrasonic Sensor's TRIG pin
const int ECHO_PIN = 9; // Arduino pin connected to Ultrasonic Sensor's ECHO pin
const int RTRIG_PIN = 8;
const int RECHO_PIN = 5;
const int LTRIG_PIN = 13;
const int LECHO_PIN = 6;

int enablePin1 = 11;
int enablePin2 = 10;
int in1Pin = 12;
int in2Pin = 2;
int in3Pin = 3;
int in4Pin = 4;

// Define Input Connections
const int CH1 = A1;
const int CH2 = A2;
const int CH3 = A3;
const int CH6 = A0;

// Integers to represent values from sticks and pots
int ch1Value;
int ch2Value;
int ch3Value;
int ch6Value;

const int DISTANCE_THRESHOLD = 50; // centimeters
const int DISTANCE_THRESHOLD_SIDE = 50;

int botSpeed = 0;

// variables will change:
float duration_us, distance_cm, lduration_us, ldistance_cm, rduration_us, rdistance_cm;

// Manual control direction
bool reverse = LOW;

void setup() {
  Serial.begin (9600);       // initialize serial port
  pinMode(TRIG_PIN, OUTPUT); // set arduino pin to output mode
  pinMode(ECHO_PIN, INPUT);  // set arduino pin to input mode
  pinMode(RTRIG_PIN, OUTPUT); // set arduino pin to output mode
  pinMode(RECHO_PIN, INPUT);
  pinMode(LTRIG_PIN, OUTPUT); // set arduino pin to output mode
  pinMode(LECHO_PIN, INPUT);
  pinMode(in1Pin, OUTPUT);
  pinMode(in2Pin, OUTPUT);
  pinMode(in3Pin, OUTPUT);
  pinMode(in4Pin, OUTPUT);
  pinMode(enablePin1, OUTPUT);
  pinMode(enablePin2, OUTPUT);  // set arduino pin to input mode
  pinMode(CH1, INPUT);
  pinMode(CH2, INPUT);
  pinMode(CH3, INPUT);
  pinMode(CH6, INPUT);
}

void loop(){
  readChannels();

  // Read remote control
  botSpeed = ch3Value;
  
  if(ch6Value <= 255/2){
    manualControls();
  }else{
    autopilot();
  }

  // print the value to Serial Monitor
  refresh();
}

void autopilot() {
  findDistance(); // detect objects on all sides of the Spyder

  if (distance_cm >= DISTANCE_THRESHOLD && rdistance_cm >= DISTANCE_THRESHOLD_SIDE &&\
  ldistance_cm >= DISTANCE_THRESHOLD_SIDE){
    runMotorL(HIGH);
    runMotorR(HIGH);
  }
  
  if (distance_cm <= DISTANCE_THRESHOLD){
    runMotorL(LOW);
    runMotorR(LOW);
    delay(1000);
  }else{ 
    if (rdistance_cm <= DISTANCE_THRESHOLD_SIDE){
      runMotorR(HIGH);
      stopMotorL(); // turn on right motor
    }
    
    if (ldistance_cm <= DISTANCE_THRESHOLD_SIDE){
      runMotorL(HIGH);
      stopMotorR(); // turn on left motor
    }
  }
}

void manualControls(){
  if (ch2Value < -15 || ch2Value > 15){
    if (ch2Value > 15){
      reverse = HIGH;
    }
    if (ch2Value < -15){
      reverse = LOW;
    }
  }else{
    botSpeed = 0;
  }
    
    if (ch1Value > 15 || ch1Value < -15){
      if(ch1Value > 15){
      analogWrite(enablePin2, botSpeed);
      digitalWrite(in3Pin, !reverse);
      digitalWrite(in4Pin, reverse);
      stopMotorR();   
      }
      if(ch1Value < -15){
      analogWrite(enablePin1, botSpeed+20);
      digitalWrite(in1Pin, !reverse);
      digitalWrite(in2Pin, reverse); 
      stopMotorL();
      }
    }else{
      runMotorL(reverse);
      runMotorR(reverse);
    }
  }

void findDistance(){
  activate(TRIG_PIN);
  // measure duration of pulse from ECHO pin
  duration_us = pulseIn(ECHO_PIN, HIGH);
  // calculate the distance
  distance_cm = 0.017 * duration_us;

  activate(RTRIG_PIN);
  // measure duration of pulse from ECHO pin
  rduration_us = pulseIn(RECHO_PIN, HIGH);
  // calculate the distance
  rdistance_cm = 0.017 * rduration_us;

  activate(LTRIG_PIN);
  // measure duration of pulse from ECHO pin
  lduration_us = pulseIn(LECHO_PIN, HIGH);
  // calculate the distance
  ldistance_cm = 0.017 * lduration_us;
}

void activate(int trigger){
  // generate 10-microsecond pulse to TRIG pin
  digitalWrite(trigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigger, LOW);
}

// functions to control motor activation
void runMotorL(boolean reverse){
  analogWrite(enablePin2, botSpeed);
  digitalWrite(in3Pin, ! reverse);
  digitalWrite(in4Pin, reverse);
}

void runMotorR(boolean reverse){
  analogWrite(enablePin1, botSpeed+20);
  digitalWrite(in1Pin, ! reverse);
  digitalWrite(in2Pin, reverse);
}

void stopMotorL(){
  analogWrite(enablePin2, 0);
}

void stopMotorR(){
  analogWrite(enablePin1, 0);
}

// Read the number of a specified channel and convert to the range provided.
// If the channel is off, return the default value
int readChannel(int channelInput, int minLimit, int maxLimit, int defaultValue){
  int ch = pulseIn(channelInput, HIGH, 30000);
  if (ch < 100 && ch > -100){
    return defaultValue;
  }else{
    return map(ch, 1000, 2100, minLimit, maxLimit);
  }
}

void readChannels(){
    ch1Value = readChannel(CH1, -100, 100, 0);
    ch2Value = readChannel(CH2, -100, 100, 0);
    ch3Value = readChannel(CH3, 0, 255, 0);
    ch6Value = readChannel(CH6, 0, 255, 0);
}

// show sensor and controller inputs
void refresh(){
  Serial.println("distance: ");
  Serial.print(distance_cm);
  Serial.println(" cm");
  Serial.println("Left distance: ");
  Serial.print(ldistance_cm);
  Serial.println(" cm");
  Serial.println("Right distance: ");
  Serial.print(rdistance_cm);
  Serial.println(" cm");
  Serial.println("");
  Serial.println("Channel 1: ");
  Serial.println(ch1Value);
  Serial.println("Channel 2: ");
  Serial.println(ch2Value);
  Serial.println("Channel 3: ");
  Serial.println(ch3Value);
  Serial.println("Channel 6: ");
  Serial.println(ch6Value);
  Serial.println("");
  Serial.println(reverse);
}
