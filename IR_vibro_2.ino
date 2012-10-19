/*
  IR_vibro
  brightness of an led, and the speed of a vibro motor, is set by proximity, using a sharp
  IR range measurement sensor. 
  
  Bit by bit I've added signal processing to the input signal from the sensor to give the kind 
  of response curve I want.
  
  Signal from sensor is a voltage that varies between 0.0 and 2.4v, at the nominal supply voltage
  of 5.0 volts. We are using 3 AAA batteries, so the supply voltage is actually
  somewhere btw 4.0 and 4.5v.
  
  First I filter the signal with an exponential filter, which, at each sample time, returns an
  average of the previous filtered value and the new value; proportion of new to old at each
  sample is adjustable, but best performance seems to be when using 80% to 90% of the old value.
  
  The signal is then "auto-ranged", by keeping a running max and min value, and scaling the current
  input as a proportion within the range set by these actively acquired max/min values. 
  
  I then straighten out the native exponential response of the sensor to something more linear by
  taking (1 - ( (1-x) * (1-x))), which, since the processed input signal is now in the bounds 0.0 -> 1.0, will
  return a result within the same bounds.
   
   Then the value is scaled to 0,255.0, (and clipped at 0 and 255), turned into an int, and
   used to set the vibrator and led outputs.
 
 */

// constants
const int vibe_threshold = 5;
const int loop_delay = 25; // control sample rate, ms delay.

const float ff_sensin_new = 0.2; // factor in exponential filtering of input.
const float ff_sensin_old = 0.8; // should always be 1.0 - ff_sensin_new

// pins
int led = 0; // PB0 = dip pin 5
int motor = 1;  // PB1 = dip pin 6
int sensor_in = 2; // ADC2 = PB4 (8pin dip pin 3)

// sensor values
float raw_sensin_f = 0.0;
float max_sensin_f = 0.0; // keep max input to adjust sensor gain actively.
float min_sensin_f = 1000.0;
float sensin_f  = 0.0; // filtered sensor input value.
float adj_sensin_f = 0.0; // gain adjusted 
float range_f = 0.0;
int sensin = 0;  // int version of above.

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

void loop() {

   raw_sensin_f=  (float)analogRead(sensor_in); // 10 bit input, 5v = code 1023,
                                            // but sensor only goes up to about 2.4 v,
                                            // so effectively 0.0 <--> 500.0 approx.

  // exp filter on sensor value input to smooth it, fight chatter
  sensin_f = (ff_sensin_new * raw_sensin_f) + (ff_sensin_old * sensin_f);
  
  // get max/min of sensor input values
  max_sensin_f = max(sensin_f, max_sensin_f);
  min_sensin_f = min(sensin_f, min_sensin_f);
  range_f = max_sensin_f - min_sensin_f;
 
  // scale input value within active range to 0.0 -> 1.0.
  adj_sensin_f = ((sensin_f - min_sensin_f) / range_f);
  
  // sensor input curve is non linear. flip, square, flip to straighten curve out.
  // extends effective sensor range.
  adj_sensin_f = 1.0 - adj_sensin_f;
  adj_sensin_f = adj_sensin_f * adj_sensin_f;
  adj_sensin_f = 1.0 - adj_sensin_f;
  
  //scale and clip at 255, 0.
  adj_sensin_f = min(255.0, (255.0 * adj_sensin_f));
  adj_sensin_f = max(0.0, (adj_sensin_f - min_sensin_f));
  
  // to int
  sensin = (int)adj_sensin_f;
 
  // brightness of LED proportional to proximity value.
  analogWrite(led, sensin);
  
  // vibe the motor, if above threshold
  if( sensin > vibe_threshold){
    analogWrite(motor, 255 - sensin); // motor is wired to be active LOW.
  } else {
    analogWrite(motor, 255); // hi is OFF for motor.
  }
  delay(loop_delay); 
}

/*
void loop(){
  heartbeat();
}


uint8_t hbval=128;
int8_t hbdelta=8;
void heartbeat() {
  if (hbval < 8) hbdelta = -hbdelta;
  if (hbval > 250) { 
    hbdelta = -hbdelta;
  }
  hbval += hbdelta;
  analogWrite(led, hbval);
  delay(30);
}
*/


