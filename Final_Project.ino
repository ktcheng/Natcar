// ==================== Final_Project Code ==================== //

// Pin Numbers
const int left_nslp_pin = 31;
const int right_nslp_pin = 11;

const int left_dir_pin = 29;
const int right_dir_pin = 30;

const int left_pwm_pin = 40;
const int right_pwm_pin = 39;

// Sensor Information
float sensorWeights[8] = {-1.75, -1.25, -0.75, -0.25, 0.25, 0.75, 1.25, 1.75};
int sensorOffsets[8] = {368, 391, 350, 415, 391, 414, 414, 414};
float sensorScalers[8] = {1.15, 1.15, 1.52, 1.21, 1.59, 1.49, 1.27, 1.26};
bool sensorValues[8]; // Contains the IR sensor readings
const int sensorPins[8] = {65, 48, 64, 47, 52, 68, 53, 69};
const int ERA = 56;

// Sensor Emitters
const int EMITEVEN = 61; 
const int EMITODD = 45;

// Distance Information
const float Kp = 67;
const float Kd = 15;
float error;
float previousError = 0.0;
float velocity;

// Positional Awareness
int counterStripe = 0;
int timefromStart = 0;

// Arduino Awareness Booleans
bool allWhite = true; // This means we are off the path entirely
bool allBlack = true; // This means we have hit the checkpoint

void setup()
{
  // ECE3_Init();
  pinMode(EMITEVEN, OUTPUT);
  pinMode(EMITODD, OUTPUT);

  digitalWrite(EMITEVEN, HIGH);
  digitalWrite(EMITODD, HIGH);

  pinMode(ERA, INPUT);

  pinMode(left_nslp_pin, OUTPUT);
  pinMode(right_nslp_pin, OUTPUT);

  pinMode(left_dir_pin, OUTPUT);
  pinMode(right_dir_pin, OUTPUT);

  pinMode(left_pwm_pin, OUTPUT);
  pinMode(right_pwm_pin, OUTPUT);

  // Sets motor direction to go forwards (HIGH will result in backwards motor direction)
  digitalWrite(left_dir_pin, LOW);
  digitalWrite(right_dir_pin, LOW);

  digitalWrite(left_nslp_pin, HIGH);
  digitalWrite(right_nslp_pin, HIGH);

  // Final Startup Conditions
  Serial.begin(9600); // Sets the data rate in bits per second
}

void loop()
{
  // Set our initial parameters
  allWhite = true;
  allBlack = true;
  error = 0.0;
  int blackCounter = 0;
  float delta = 0.0;
  
  // Sets the base motor speeds
  float leftSpd = 70.0;
  float rightSpd = 70.0;

 // Retrieve the digital sensor readings
 for( int i = 0; i < 8; i++)
  {
    pinMode( sensorPins[i] , OUTPUT);
    digitalWrite( sensorPins[i], HIGH);
  }
  
  delayMicroseconds(16); // Allows the sensors to process information

  for( int i = 0; i < 8; i++)
    pinMode( sensorPins[i] , INPUT);
  delayMicroseconds(800); // Solidifes sensor measurement accuracy

  // Assign all of the sensor readings
  for( int i = 0; i < 8; i++)
    sensorValues[i] = digitalRead(sensorPins[i]);

  // Calculate the error between this and previous time step
  previousError = error;
  for (int i = 0; i < 8; i++)
  {
    error += sensorWeights[i] * sensorValues[i];
  }

  // Count the number of sensors detecting the path
  for (int i = 0; i < 8; i++)
  {
    if (sensorValues[i] == 1)
    {
      blackCounter++;
    }
  }

  // Race Day Testing: LED flashes when a black stripe is detected
  if (blackCounter == 8)
  {
    allBlack = true;
    digitalWrite(41, HIGH);
    delay(10);
    digitalWrite(41, LOW);
  }
  else
  {
    allBlack = false;
  }

  /*
  // ========== Straight Path ========== //
  if (counterStripe == 0 && allBlack)
  {
    // Perform a doughnut turn
    digitalWrite(left_dir_pin, HIGH);
    analogWrite(left_pwm_pin, 100);
    analogWrite(right_pwm_pin, 100);
    delay(800);
    counterStripe++;
    digitalWrite(left_dir_pin, LOW);
  }
  else if (counterStripe == 1 && allBlack)
  {
    analogWrite(left_pwm_pin, 0);
    analogWrite(right_pwm_pin, 0);
    counterStripe = 10;
  }
  if (counterStripe != 10)
  {
    delta = (Kp * error + Kd * (error - previousError)) / 4;
    analogWrite(left_pwm_pin, leftSpd - delta);
    analogWrite(right_pwm_pin, rightSpd + delta);
  }
  // ========== Straight Path ========== // */

  // ========== Ribbon Path ========== // 
  if (counterStripe < 2 && allBlack)
  {
    counterStripe++;
    delay(200);
  }
  else if (counterStripe == 2 && allBlack)
  {
    // Perform a doughnut turn
    digitalWrite(left_dir_pin, HIGH);
    analogWrite(left_pwm_pin, 100);
    analogWrite(right_pwm_pin, 100);
    delay(1000);
    counterStripe++;
    digitalWrite(left_dir_pin, LOW);
  }
  else if (counterStripe < 5 && allBlack)
  {
    counterStripe++;
    delay(200);
  }
  else if (counterStripe == 5 && allBlack)
  {
    // Stop at the end of the race track
    analogWrite(left_pwm_pin, 0);
    analogWrite(right_pwm_pin, 0);
    counterStripe = 102;
  }

  if (counterStripe != 102)
  {
      delta = Kp * error + Kd * (error - previousError);
      delta /= 5;

      // Cap the delta value, if applicable
      if (delta > 70)
      {
        delta = 70;
      }
      else if (delta < -70)
      {
        delta = -70;
      }
      
      analogWrite(left_pwm_pin, leftSpd - delta);
      analogWrite(right_pwm_pin, rightSpd + delta);
  }
  // ========== Ribbon Path ========== // 
}
// ==================== Final_Project Code ==================== //
