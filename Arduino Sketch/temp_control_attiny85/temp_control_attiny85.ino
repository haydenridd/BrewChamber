/*
  Temp controlled environment for brewing Kombucha.
  Hayden Riddiford
 */
enum tempStates {
  OFF_FALL, // off with hysteresis
  OFF_RISE, // off without hysteresis
  ON, // temperature forcing
};

enum tempStates temp_mode = OFF_RISE;
#define NPT_AVG 16 // must be a 2^X value

int RISING_THRESH = 729;
int FALLING_THRESH = RISING_THRESH - 5;
int relayDriver_p = 0;
int tempSense_p = A1;
int tempCtrl_p = A2;
// int swPin = 1;
int temp_voltage_avg;
int temp_voltage_arr[NPT_AVG];
int temp_ctrl_val;
int counter = 0;
int sw_mode = 0;
int num_bit_shift = 1;
int test_bit_shift = NPT_AVG;
// the setup function runs once when you press reset or power the board
void setup() {
  
  // initialize digital pin 0 as a pwm output.
  pinMode(relayDriver_p, OUTPUT);
  // initialize analog pin 1, 2 as input 
  pinMode(tempSense_p, INPUT);
  pinMode(tempCtrl_p, INPUT);
    
  while( test_bit_shift != 2) {
    test_bit_shift = test_bit_shift >> 1;
    num_bit_shift +=1;
  }
  
}

// the loop function runs over and over again forever
void loop() {

  if(counter < NPT_AVG)
  {
    temp_voltage_arr[counter] = analogRead(tempSense_p);
    counter++;
    delay(500);
  }
  else
  {
    TempAvgUpdate();
    UpdateThresholds();
    
    switch (temp_mode) {
      
      case OFF_RISE:
        if (temp_voltage_avg < RISING_THRESH)
        {
          digitalWrite(relayDriver_p, HIGH);
          temp_mode = ON;
        }
      break;
          
      case ON:
        if (temp_voltage_avg > RISING_THRESH)
        {
          digitalWrite(relayDriver_p, LOW);
          temp_mode = OFF_FALL;
        }
      break;
      
      case OFF_FALL:
        if (temp_voltage_avg < FALLING_THRESH)
        {
          digitalWrite(relayDriver_p, HIGH);
          temp_mode = ON;
        }
      break;

    }

    delay(5000);
  }
  delay(10);
}

void TempAvgUpdate() {
  temp_voltage_avg = 0;
  for( int i = NPT_AVG-1; i>0; i--){
    temp_voltage_arr[i] = temp_voltage_arr[i-1];
    temp_voltage_avg += temp_voltage_arr[i];
  }
  temp_voltage_arr[0] = analogRead(tempSense_p);
  temp_voltage_avg += temp_voltage_arr[0];
  temp_voltage_avg = temp_voltage_avg >> num_bit_shift;
}

void UpdateThresholds() {
  temp_ctrl_val = analogRead(tempCtrl_p);
  RISING_THRESH = 695 + int(temp_ctrl_val/26);
  FALLING_THRESH = RISING_THRESH - 5;
}


