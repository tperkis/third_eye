/*
  IR_vibro
  brightness of an led, and the speed of a vibro motor, is set by proximity, using a sharp
  IR range measurement sensor. 
 
 */

const int vibe_threshold = 20;
const int threshold = 20;

int led = 0; // PB0 = dip pin 5
int motor = 1;  // PB1 = dip pin 6
int sensor_in = 2; // ADC2 = PB4 (8pin dip pin 3)

void setup() {                
  // initialize the digital pin as an output.
  pinMode(led, OUTPUT);   
  pinMode(motor, OUTPUT);  
  
  // to minimize power consumption, make unused pins outputs and drive them somewhere.
  pinMode(3, OUTPUT);
  pinMode(2, OUTPUT);
  pinMode(5, OUTPUT);
  digitalWrite(3,HIGH);
  digitalWrite(2,HIGH);
  digitalWrite(5,HIGH);
}

int sensor_value = 0;
int sensor_value_in = 0;
int i;


void loop() {

  sensor_value_in =  analogRead(sensor_in); // 10 bit input, 5v = code 1023,
                                            // but sensor only goes up to about 2.4 v
  
  if(sensor_value_in > threshold) {   
    sensor_value = min(255, sensor_value_in / 2); // max is 255.
  }
  // brightness of LED proportional to proximity value.
  analogWrite(led, sensor_value);
  
  // vibe the motor, if above threshold
  if( sensor_value > vibe_threshold){
     analogWrite(motor, 255 - sensor_value);
  } else {
    analogWrite(motor, 255); // hi is OFF for motor.
  }
  delay(100);
}


