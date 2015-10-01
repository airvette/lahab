/*
+Define battVoltage pin
+Define low power voltage threshold (lowV_thresh) as 3.3V
+Define the variable for the time of the last measured voltage over 3.3V (last_over_33v)
+Define the max time to wait below 3.3V to be 1 second?
+Define off power voltage threshold (offV_thresh) as 3.1V
+Define the variable for the time of the last measured voltage over 3.1V (last_over_31v)
+Define the max time to wait below 3.1V to be 1 second?
-Define fuelGauge pin
+Define HackHD switch
-Define I2C pins
*/

const int battV_pin = 0; // analog input 0
int battV; // variable that holds the current battery voltage
const float lowV_thresh = 3.3; // low battery range is 3.1-3.3V
const float offV_thresh = 3.1; // the range for which to turn the battery off is 0-3.1V
// Note that the function micros() returns an unsigned long which is why the following variables are created with this data type
unsigned long last_over_lowV; // the time that the battery was last measured to be above the low range
unsigned long last_over_offV; // the time that the battery was last measured to be above the off range
unsigned long voltSettle = 1E6; // time in us that the voltage needs to be to initiate a state change in voltage control 

const int cameraSwitch = 4; // pin for payload camera
int cameraOffFlag = 0; // flag that is set to one once the camera is shut off.  This is to prevent camera cycling

void setup() {
  // put your setup code here, to run once:
  /*
  +Setup hackHD switch
  -Setup battery gauge
  -Setup fuel gauge
  -Setup I2C interface
  -Setup interrupt w/ attachinterrupt(0 (pin 2), call I2C message function, on change of pin value)
  */
    
  pinMode(cameraSwitch, OUTPUT); // set's the digital pin for the camera to output
  digitalWrite(cameraSwitch, HIGH); // set the camera's initial pin value to high
} // end setup

void loop() {
  // put your main code here, to run repeatedly:
  battV = analogRead(battV_pin); // read the current voltage
  //convert to voltage
  if (battV < lowV_thresh && battV >= offV_thresh) // if battery is in the low voltage range
  {
    // update the times of the last measured voltages above the offV thresholds
    last_over_offV = micros(); // update the time the voltage was last measured over the off voltage range
    
    if ((micros()-last_over_lowV) > voltSettle && cameraOffFlag == 0) //compare time of last voltage above the low range
    {
      //if the settle time is exceeded and the camera was not previously shut off, switch to low power mode
      digitalWrite(cameraSwitch, LOW); // set the camera pin to low to signal to the camera to turn off
      delay(500); // pause the program for .5 seconds so the camera registers the low pin value
      digitalWrite(cameraSwitch, HIGH); // set the camera pin back to high
      cameraOffFlag = 1; // set the flag to 1 to prevent camera cycling
    } // end if
    
    else if (battV < offV_thresh) // if battery is in the off voltage range
    {
      if ((micros()-last_over_offV) > voltSettle)
      {
        //put system to sleep
        /* insert sleep code from JeeLabs (http://jeelabs.org/2011/12/13/developing-a-low-power-sketch/)
         * or narcoleptic code (https://code.google.com/p/narcoleptic/)
         * or low-power (http://www.rocketscream.com/blog/2011/07/04/lightweight-low-power-arduino-library/)
         * max sleep time is 8s, so set input to 8000
         * need to set a sleep flag and have the arduino stay in a sleep loop with period 8s
        */
      } // end if
    } // end else if
    
    else // if the battery is in the normal operating range
    {
      // update the times the voltages were measured within in the normal operating thresholds
      last_over_lowV = micros(); // update the time the voltage was last measured over the low voltage range
      last_over_offV = micros(); // update the time the voltage was last measured over the off voltage range
    } // end else
  }
  /*
  Check battery voltage pin
  if voltage is < lowV_thresh && >= offV_thresh // if battery voltage is less than 3.3V
    // update the times of the last measured voltages above the offV thresholds
    last_over_31v = micros(); // update the time the voltage was last measured over 3.1V
    if voltage has been lower than the lowV_thresh for a certain amount of time //compare time of last voltage above 3.3V
      //switch to low power mode
      turn off camera by setting the camera pin to ground
      send a message to the Rpi to power off non-essential functions
  else if voltage is < offV_thresh // if batter voltage is less than 3.1V
    if voltage has been lower than the offV_thresh for a certain amount of time //compare time of last voltage above 3.1V
      switch to off
  else voltage is >= 3.3V // else the voltage is in the normal operating range
    // update the times of the last measured voltages above the lowV and offV thresholds 
    last_over_31v = micros(); // update the time the voltage was last measured over 3.1V
    last_over_33v = last_over_31v ; // update the time the voltage was last measured over 3.3V
  */
}
