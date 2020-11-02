//Illuminating System
//Declaring the pins as constants
const int aPin = A0;    //photoresistor
const int ledr1 = 12;   //red leds
const int ledr2 = 11;
const int ledr3 = 10;
const int ledr4 = 9;
const int ledg1 = 8;    //green leds
const int ledg2 = 7;
const int ButtonPin1 = 2;   //Buttons
const int ButtonPin2 = 3;
const int G = 4;            //RGB green led
const int B = 5;            //RGB blue led
const int R = 6;            //RGB red led
const int MinVal = 20;      //Value of the sensitivity for the photoresistor

//Global variables declared

int brightness = 0;     //brightness of the leds
int fadeAmount = 20;    //fade value
int nr2 = 0;            //number of times the 2nd button is pressed
int nr1 = 0;            //number of times the 1st button is pressed
bool ButtonState1 = false;   //state of the first button 
bool ButtonState2 = false;   //state of the second button
bool ok = false;             //control variable

//Declaring the functions

bool DebounceButton ();     //Debounce function for buttons
void setup();               //setup function for the pins
void Photodiode ();         //photodiode-related code
void EnvironmentalMode ();  //Environmental System related code
void MovingMode ();         //Moving Mode related code
void StopLEDs ();           //Stops all LED's (except RGB)
void NormalMode ();         //Normal Mode code
void Button1 ();            //Counts the number of times button 1 is pressed
void Button2 ();            //Counts the number of times button 2 is pressed
void StopLEDsRGB ();        //Stops the RGB led
void ModeSelect ();         //Selects the mode
void ButtonCounter ();      //Starts the counting for the buttons at the same time

void loop() {               //Main function
  ButtonCounter ();         //starts the button counter
  ModeSelect ();            //starts the mode select
}
void Button1 ()
{
  if (DebounceButton (ButtonState1, 1) == true && ButtonState1 == false)  //if the debounced value of the button is true (on) and the current state of the button 1 is false (off), proceed
  {
    ButtonState1 = true;                //current state of the button 1 becomes true (on)
    nr1++;                              //counts the pressing
  }
  if (DebounceButton (ButtonState1, 1) == false && ButtonState1 == true) //if the debounced value of the button is false (off) and the current state of the button 1 is true (on), proceed
    ButtonState1 = false;               //the current state of the button 1 becomes false (off)
 
}
void ButtonCounter ()                   //simply calls the functions to count each button's pressings
{
  Button1 ();
  Button2 ();
}
void Button2 ()                         //exact same mechanism as for button 1
{
  if (DebounceButton (ButtonState2, 2) == true && ButtonState2 == false)
  {
    ButtonState2 = true;
    nr2++;
  }
  if (DebounceButton (ButtonState2, 2) == false && ButtonState2 == true)
    ButtonState2 = false;
}
void ModeSelect ()                     //identifies the current mode
{
  bool ok;                             //tracking variable 
  Serial.println (nr2%3);                //displays the mode the system is currently in (0, 1 or 2) 
  if (nr2 % 3 == 0) {                  // mode 0: normal mode - because the number of pressings is counted, by dividing it to 3, we can determine which mode the drone needs to operate in
    ok = true;                         // tracks that mode 0 is the last mode used
    NormalMode();                      //starts the normal mode of the drone
    Photodiode();                      //starts the photodiode LED
  }
  else if (nr2 % 3 == 1)               // mode 1: environmental mode
  {
    if (ok == true) {                  // if the previous mode was mode 0, stops the LEDs - this is necessary because otherwise the function will just kill all the LED's every loop
      StopLEDs();                      // calls the function to stop LED's
      ok = false;                      // refreshes the tracker
    }
    StopLEDsRGB ();                    // stops the photodiode
    EnvironmentalMode ();              // starts the environmental mode
  }
  else {StopLEDs (); nr1 = 0;}         // mode 2: stops everything, refreshes the counter for button 1
}
void NormalMode ()
{
  if (nr1 % 2 == 0) {                  // by dividing the number of times button 1 is pressed by 2, the current state of the drone can be identified
    if (!ok) StopLEDs ();              // kills all the lights if this is the previous mode was the moving mode (necessary because the green LED's need to be turned off)
    EnvironmentalMode ();              // starts the environmental mode
    ok = true;                         // tracks that the last active mode is the environmental mode
  }
  else {
    if (ok == true) {                 // if the last active mode is the environmental mode, the new last mode becomes the moving mode, necessary to refresh the LED's just once, otherwise 2 extra red led's would be open
      ok = false;                     //tracks that the last active mode is the moving mode
      StopLEDs ();                    //stops all the LED's
    }
    MovingMode ();                    // starts the moving mode
  }
}
void StopLEDsRGB ()                  //function to stop the RGB LED
{
  digitalWrite (R, LOW);
  digitalWrite (G, LOW);
  digitalWrite (B, LOW);
}
void Photodiode ()                  //function for the Photodiode
{
  //220 ohm resistor used
  int val = analogRead(aPin);       //registers the photodiode value
  //Serial.println(val);
  delay(100);                       //sets a delay
  if (val < MinVal) {               // if the photodiode value is below the set limit, the RGB LED starts
    digitalWrite (R, HIGH);
    digitalWrite (G, HIGH);
    digitalWrite (B, HIGH);
  }
  else {                            //otherwisSe it stops
    digitalWrite (R, LOW);
    digitalWrite (G, LOW);
    digitalWrite (B, LOW);
  }
}
void StopLEDs ()                    //stops all the LED's (except RGB) and refreshes the brightness and the fade amount
{
  analogWrite (ledr1, 0);
  analogWrite (ledr2, 0);
  analogWrite (ledr3, 0);
  analogWrite (ledr4, 0);
  analogWrite (ledg1, 0);
  analogWrite (ledg2, 0);
  brightness = 0;
  fadeAmount = 20;
}
void MovingMode ()                //Moving mode, alternative blinking between 2 front green LED's and 2 back red LED's
{
  analogWrite (ledg1, 0);         
  analogWrite (ledg2, 0);
  digitalWrite (ledr2, HIGH);
  digitalWrite (ledr3, HIGH);
  delay (250);
  digitalWrite (ledg1, HIGH);
  digitalWrite (ledg2, HIGH);
  analogWrite (ledr2, 0);
  analogWrite (ledr3, 0);
  delay (250);
}
void EnvironmentalMode ()     //Environmental mode, 4 red LED's with a fade type blinking
{
  //330 ohm Resistors were used
  analogWrite (ledr1, brightness);
  analogWrite (ledr2, brightness);
  analogWrite (ledr3, brightness);
  analogWrite (ledr4, brightness);
  digitalWrite (ledg1, 0);
  digitalWrite (ledg2, 0);
  delay (500);
  brightness += fadeAmount;
  if (brightness <= 0 || brightness >= 235)
  {
    fadeAmount = -fadeAmount;
  }
}
void setup() {
  // The analog pins are always input, so no need to set them here
  Serial.begin(9600);
  pinMode (ledr1, OUTPUT);
  pinMode (ledr2, OUTPUT);
  pinMode (ledr3, OUTPUT);
  pinMode (ledr4, OUTPUT);
  pinMode (ledg1, OUTPUT);
  pinMode (ledg2, OUTPUT);
  pinMode (ButtonPin1, INPUT);
  pinMode (ButtonPin2, INPUT);
  pinMode (R, OUTPUT);
  pinMode (G, OUTPUT);
  pinMode (B, OUTPUT);
}
boolean DebounceButton (boolean state, int index)   //debouncing function, the index variable helps tracking which button the function applies to
{
  bool stateNow;
  if (index == 1) stateNow = digitalRead(ButtonPin1);
  else stateNow = digitalRead(ButtonPin2);
  if (state != stateNow)                            //debounces the value with a 10 milisecond range
  {
    delay(10);
    if (index == 1)stateNow = digitalRead(ButtonPin1);
    else stateNow = digitalRead(ButtonPin2);
  }
  return stateNow;                                  //returns the debounced value
}




