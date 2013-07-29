/*
  This code controls the handlebar simulator on the Arduino.
  
  It samples the ADC connected to the potentiometer and sends the result via serial.
  
  It also reads in a byte of data that controls which motors are on.
 
 */

// These constants won't change.  They're used to give names
// to the pins used:
const int analogInPin = A0;  // Analog input pin that the potentiometer is attached to

// The pin configuration 
byte NUM_MOTOR_PINS_LEFT = 4;
byte NUM_MOTOR_PINS_RIGHT = 4;
//byte MOTOR_PINS_RIGHT[4] = {9, 8, 7, 6};
//byte MOTOR_PINS_LEFT[4] = {5, 4, 3, 2};
byte MOTOR_PINS_RIGHT[4] = {9, 8, 7, 6};
byte MOTOR_PINS_LEFT[4] = {5, 4, 3, 2};

// Buffer to store commands
byte MOTOR_CMD_LEFT[4] = {0, 0, 0, 0};
byte MOTOR_CMD_RIGHT[4] = {0, 0, 0, 0};

const byte DEBUG_PIN = 13;
const byte ON_CMD = 255;

unsigned long timeout = 0;
byte pulseOut=0;

void setup() {
   // Set all of the motor pins to output mode
  for(byte i = 0; i < NUM_MOTOR_PINS_LEFT; i++) {
      pinMode(MOTOR_PINS_LEFT[i], OUTPUT);
  }
  for(byte i = 0; i < NUM_MOTOR_PINS_RIGHT; i++) {
      pinMode(MOTOR_PINS_RIGHT[i], OUTPUT);
  }
  
  // For debug
  pinMode(DEBUG_PIN, OUTPUT);
  
  // initialize serial communications at 115200 bps:
  Serial.begin(115200); 
}

void setPosition(int pos) {
 for(byte i = 0; i < NUM_MOTOR_PINS_LEFT; i++) {
   MOTOR_CMD_LEFT[i] = 0;
 } 
 for(byte i = 0; i < NUM_MOTOR_PINS_RIGHT; i++) {
   MOTOR_CMD_RIGHT[i] = 0; 
 }
 
 if (pos > 0) {
   if (pos <= NUM_MOTOR_PINS_RIGHT) {
      MOTOR_CMD_RIGHT[pos - 1] = ON_CMD;
   } 
 } else if (pos < 0) {
   pos = abs(pos);
   if (pos <= NUM_MOTOR_PINS_LEFT) {
      MOTOR_CMD_LEFT[pos - 1] = ON_CMD; 
   }
 } else {
  // Pos is centered - do nothing 
 }
 
 dispatchCommands();
 
}


/*
Send the buffered commands to the digital pins (via PWM)
//old technique, req'd 8 pins just for the motors
*//*
void dispatchCommands() {
   for(int i = 0; i < NUM_MOTOR_PINS_LEFT; i++) {
     analogWrite(MOTOR_PINS_LEFT[i], MOTOR_CMD_LEFT[i]);
   }
   for(int i = 0; i < NUM_MOTOR_PINS_RIGHT; i++) {
     analogWrite(MOTOR_PINS_RIGHT[i], MOTOR_CMD_RIGHT[i]);
   }
   
   analogWrite(DEBUG_PIN, MOTOR_CMD_RIGHT[0]);
   
}*/

/** 
 Process an input byte, which in our case
 is an 8-bit bitmask representing which of the 
 8 motors should be on (MSB is left-most motor)
 */
void processInputByte(int b) {
  for(int i = 0; i < 4; i++) {
    if (b & (1 << i)) {
       // Turn on this motor
      MOTOR_CMD_RIGHT[i] = ON_CMD;
    } else {
      MOTOR_CMD_RIGHT[i] = 0;
    }
  }
  
  for(int i = 4; i < 8; i++) {
     if (b & (1 << i)) {
       // Turn on this motor
      MOTOR_CMD_LEFT[i - 4] = ON_CMD; 
     } else {
       MOTOR_CMD_LEFT[i - 4] = 0;
     }
  }
  
  // Now that we're done processing, dispatch.
  dispatchCommands();
  
}


void loop() {
  // read the analog in value:
  int sensorValue = analogRead(analogInPin);            
      
  // print the results to the serial monitor:                   
  Serial.println(sensorValue);      
  
  if (Serial.available() > 0) {
     int in = Serial.read();
     processInputByte(in);
     timeout = millis();
  }else if(millis()-timeout>3000){
     if(pulseOut>150){
        pulseOut=0;
     }else{
        pulseOut+=3;
     }
     for(int i = 0; i < NUM_MOTOR_PINS_LEFT; i+=2) {
     MOTOR_CMD_LEFT[i]=pulseOut;
   }
   for(int i = 0; i < NUM_MOTOR_PINS_RIGHT; i+=2) {
     MOTOR_CMD_RIGHT[i]=pulseOut;
   }
   dispatchCommands();
  }
     

//  if (Serial.available() > 0) {
//     int in = Serial.read();
//     Serial.println(in); 
//  } else {
//      Serial.println(0);
//  }

  // wait 10 milliseconds before the next loop
  // for the analog-to-digital converter to settle
  // after the last reading:
  delay(10);                     
}
