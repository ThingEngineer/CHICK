/*
  CHICK - Chicken House Illumination Control boKs

  CHICK is an open source, simple to use, minimalistic approach 
  daylight automated 12 volt powered LED light controller. 
  Its purpose is to extend daylight hours in a chicken house/coup, 
  thus promoting greater egg production. 
  You can read more info about that here: 
  https://poultrykeeper.com/general-chickens/light-for-layers/

  GitHub Project: https://github.com/joshcam/CHICK
  Thingiverse files for CHICK: https://www.thingiverse.com/thing:2595492

  Functions:
  - Memorize current light level as the trigger on point.
  - Remain on for 1-5 hours.
  - With manual On/Off

 The circuit:
  * 3 buttons connected from ground to digital pins X-X control 
    memorization of light level, BREAK OUT EACH BUTTON
  * 5 LEDs connected from +5V to digital pins 6-10 indicate 
    the number of hours the main lighting LEDs will remain on
    after they are triggered on by the CDS sensor level event
  * CDS Photocell
  * Relay > DC-DC Buck Converter

 Created 18 October 2017
 Modified 1 November 2017
 By Josh Campbell

 This work is licensed under a Creative Commons Attribution-ShareAlike 4.0 International License.
 */

#include <EEPROM.h>       // library to access the EEPROM read write functions

// pin the Relay is connected to
const int relayPin = 5;   // LED control relay

// pins the LEDs are connected to
const int led1Pin = 6;    // Hour 1
const int led2Pin = 7;    // Hour 2
const int led3Pin = 8;    // Hour 3
const int led4Pin = 9;    // Hour 4
const int led5Pin = 10;   // Hour 5

// pins the buttons are connected to
const int setButtonPin = 2;        // Set button
const int hoursButtonPin = 3;      // Hours button
const int manualButtonPin = 4;     // Manual button

const int cdsPin = 0;           // CDS light sensor

int manualButtonState = 0;      // variable for reading the manual button status
int hoursButtonState = 0;       // variable for reading the hours button status
int setButtonState = 0;         // variable for reading the set button status
int initialLightReading = 0;    // the initial value read from the CDS/Photocell sensor
int lightReading = 0;           // value read from the CDS/Photocell sensor
const int lightDeltaMax = 10;   // maximum deviation from initial and current light level readings
const int triggerLevelAddr = 0; // address where the saved trigger light level is saved in EEPROM
int triggerLevel = 0;           // level at which the LEDs are triggered ON
byte triggerCounter = 0;        // number of valid light level triggers (current reading is less than or equal to triggerLevel)
const byte triggerCount = 5;    // number of valid light level triggers required to activate LEDs
byte shutOffDelayCounter = 0;   // count down timer to shut LEDs off
bool LEDstate = false;          // flag to indicate LED state (true = ON, false = OFF)
bool nightFlag = false;         // flag set at night that must be reset at dawn before LEDs can be retriggered

int currentButtonPressed = 0;   // the current button pressed
int lastButtonPressed = 0;      // the last button pressed
int buttonCheck = 0;            // button press checksum
bool buttonValid = false;       // valid button press flag

unsigned long lastDebounceTime = 0;   // the last time the output pin was toggled
unsigned long debounceDelay = 150;    // the debounce time delay

long lastTime = 0;              // last saved millisecond count
byte minutesCounter = 0;        // minutes counter
byte hoursCounter = 0;          // hours counter
bool minuteFlag = false;        // minute flag - triggered once each minute
bool hourFlag = false;          // hour flag - triggered once each hour

const byte shutOffDelayAddr = 2;// address where the saved shut off delay is saved in EEPROM
byte shutOffDelay = 0;          // number of hours that the LED will remain on before being shut off

void setup() {

  // load light trigger level from EEPROM
  triggerLevel = EEPROMReadInt(triggerLevelAddr);
  
  // load shut off delay from EEPROM
  shutOffDelay = EEPROMReadInt(shutOffDelayAddr);
  
  // initialize serial communications at 9600 bps:
  //Serial.begin(9600);

  // Set pin modes
  pinMode(manualButtonPin, INPUT_PULLUP);
  pinMode(hoursButtonPin, INPUT_PULLUP);
  pinMode(setButtonPin, INPUT_PULLUP);
  pinMode(led1Pin, OUTPUT);
  pinMode(led2Pin, OUTPUT);
  pinMode(led3Pin, OUTPUT);
  pinMode(led4Pin, OUTPUT);
  pinMode(led5Pin, OUTPUT);
  pinMode(relayPin, OUTPUT);

  // Ensure LEDs are off (active LOW)
  digitalWrite(led1Pin, HIGH);
  digitalWrite(led2Pin, HIGH);
  digitalWrite(led3Pin, HIGH);
  digitalWrite(led4Pin, HIGH);
  digitalWrite(led5Pin, HIGH);
  // Ensure relay is off (active HIGH)
  digitalWrite(relayPin, LOW);
}

void loop() {

  // Read current button states
  manualButtonState = digitalRead(manualButtonPin);
  hoursButtonState = digitalRead(hoursButtonPin);
  setButtonState = digitalRead(setButtonPin);

  // ****************************************************
  // Button debouncing and validation
  buttonCheck = manualButtonState + hoursButtonState + setButtonState;  // allows ignoring pressing multiple buttons at once
  
  if (manualButtonState == LOW) currentButtonPressed = 1;
  if (hoursButtonState == LOW) currentButtonPressed = 2;
  if (setButtonState == LOW) currentButtonPressed = 3;

  // if a button was previously validated
  if (buttonValid == true)
  {
    lastButtonPressed = 0;      //erase button states to prevent button code from running again
    currentButtonPressed = 0;
  }

  // if only one button is pressed, no button was previously pressed and no button is currently valid
  if (buttonCheck == 2 && lastButtonPressed == 0 && buttonValid == false)
  {
    lastButtonPressed = currentButtonPressed;   // save current button pressed
    lastDebounceTime = millis();                // save current time
  }
  // if no button is currently valid, only one button is pressed, the last button pressed is the current button pressed and we are over the minimum debouse time delay
  else if (buttonValid == false && buttonCheck == 2 && lastButtonPressed == currentButtonPressed && ((millis() - lastDebounceTime) > debounceDelay))
  {
    buttonValid = true;
  }
  // if a debounce has occured AND, a single (one) button is not pressed OR last and current buttons do not match
  else if (lastDebounceTime !=0 && buttonCheck != 2 || lastButtonPressed != currentButtonPressed)
  {
    lastDebounceTime = 0;       // clear debouce timer
    currentButtonPressed = 0;   // clear current button pressed
    lastButtonPressed = 0;      // clear last button pressed
    buttonValid = false;        // clear button validation flag

    refreshLEDs();              // clear LEDs and turn the corresponding LED to indicate hours remaning in the countdown timer
  }
  // END Button debouncing and validation
  // ****************************************************

  // ****************************************************
  // Manual Button Press
  if (buttonValid == true && currentButtonPressed == 1)
  {
    digitalWrite(relayPin, digitalRead(relayPin) ^ 1);    // toggle the relay pin (does not interfere with timer on/off state)
  }
  // END Manual Button Press
  // ****************************************************

  // ****************************************************
  // Hours Button Press
  if (buttonValid == true && currentButtonPressed == 2)
  {
    shutOffDelay++;
    if (shutOffDelay >= 6) shutOffDelay = 1;
    
    if (shutOffDelay == 1) digitalWrite(led1Pin, LOW);      // LED indication of shutOffDelay selection in hours
    if (shutOffDelay == 2) digitalWrite(led2Pin, LOW);
    if (shutOffDelay == 3) digitalWrite(led3Pin, LOW);
    if (shutOffDelay == 4) digitalWrite(led4Pin, LOW);
    if (shutOffDelay == 5) digitalWrite(led5Pin, LOW);
    delay(50);  // short delay for inidcator LED
    
    // save this value as the new shut off delay
    EEPROMWriteInt(shutOffDelayAddr, shutOffDelay);
  }
  // END Hours Button Press
  // ****************************************************

  // ****************************************************
  // Set Button Press
  if (buttonValid == true && currentButtonPressed == 3)
  {
    saveLightLevel();
  }
  // END Set Button Press
  // ****************************************************

  if(millis()-lastTime > 2000)  // if 60 seconds have elasped
  {
    minutesCounter++;     // increment minutes counter
    minuteFlag = true;    // set minute flag
    lastTime = millis();  // save current millisecond as lastTime
  }
  
  if(minutesCounter > 2)  // if 60 minutes have elapsed
  {
    hoursCounter++;       // increment hours counter
    hourFlag = true;      // set hour flag
    minutesCounter = 0;   // reset minutes counter
  }

  // Code run on the minute
  if (minuteFlag == true)
  {
    if (LEDstate == false)                  // only run this code if the LEDstate is false
    {
      lightReading = analogRead(cdsPin);    // read the analog in value of the CDS sensor

      if (lightReading < triggerLevel && nightFlag == false)      // if current reading is less than or equal to triggerLevel and it is no longer nightime
      {
        triggerCounter++;                   // increment triggerCounter
        if (triggerCounter == 1) digitalWrite(led1Pin, LOW);      // LED indication of trigger counter stage
        if (triggerCounter == 2) digitalWrite(led2Pin, LOW);
        if (triggerCounter == 3) digitalWrite(led3Pin, LOW);
        if (triggerCounter == 4) digitalWrite(led4Pin, LOW);
      }
      else if (lightReading > triggerLevel &&  nightFlag == false)
      {
        triggerCounter = 0;                 // or reset triggerCounter
        refreshLEDs();
      }

      if (triggerCounter == triggerCount && nightFlag == false)   // if triggerCounter hits the threshold at it is not nightime activate LEDs and countdown timer
      {
        digitalWrite(relayPin, HIGH);       // turn LED relay on
        nightFlag = true;                   // set nighttime flag
        LEDstate = true;                    // indicate LED relay is on
        shutOffDelayCounter = shutOffDelay; // load shutOffDelay to the shutOffDelayCounter
        refreshLEDs();                      // turn the corresponding LED to indicate hours remaning in the countdown timer
        minutesCounter = 0;                 // reset minutes
        hoursCounter = 0;                   //   and hours to ensure a fresh counter start
        hourFlag = false;                   // clear just incase an hour was triggered this minute
      }

      if (lightReading > triggerLevel && nightFlag == true)
      {
        if (triggerCounter == 0)            // LED indication of trigger counter stage
        {
          digitalWrite(led1Pin, LOW);
          digitalWrite(led2Pin, LOW);
          digitalWrite(led3Pin, LOW);
          digitalWrite(led4Pin, LOW);
        }
        triggerCounter++;                   // increment triggerCounter
        if (triggerCounter == 2) digitalWrite(led4Pin, HIGH);     // LED indication of trigger counter stage
        if (triggerCounter == 3) digitalWrite(led3Pin, HIGH);
        if (triggerCounter == 4) digitalWrite(led2Pin, HIGH);
        if (triggerCounter == 5) digitalWrite(led1Pin, HIGH);
      }
      else if (lightReading < triggerLevel && nightFlag == true)
      {
        triggerCounter = 0;                 // or reset triggerCounter
        refreshLEDs();
      }

      if (triggerCounter == triggerCount && nightFlag == true)   // if triggerCounter hits the threshold and it is nightime
      {
        nightFlag = false;                  // reset nighttime flag
      }
      
    }
    minuteFlag = false;                     // reset minute flag
  }

  // Code run on the hour
  if (hourFlag == true)
  {
    if (LEDstate == true)                   // only run this code if the LEDstate is true
    {
      shutOffDelayCounter--;                // decrement the shutOffDelayCounter
      if (shutOffDelayCounter == 0)
      {
        digitalWrite(relayPin, LOW);        // turn LED relay off
        LEDstate = false;                   // indicate LED relay is off
      }
      refreshLEDs();                        // turn the corresponding LED to indicate hours remaning in the countdown timer
    }

    hourFlag = false;  // reset hour flag
  }
  

  // cds test code
//  lightReading = analogRead(cdsPin);  // read the analog in value of the CDS sensor
//  if (lightReading > triggerLevel + 5)   // if sensor level is above triggerLevel
//  {
//    digitalWrite(relayPin, LOW);  // relay off
//  }
//  else if (lightReading < triggerLevel - 5) // if sensor level is at or below the triggerLevel
//  {
//    digitalWrite(relayPin, HIGH); // relay on
//  }
//  Serial.println(lightReading); // print cds value to serial plotter:
//  delay(200);
}

void saveLightLevel()
{
  // sample light level over time and show LEDs to indicate progress
  sampleLight:
  initialLightReading = analogRead(cdsPin);
  delay(500);
  digitalWrite(led1Pin, LOW);
  lightReading = analogRead(cdsPin);
  if ( levelDeltaCheck(initialLightReading, lightReading) ) goto restart;
  delay(500);
  if (digitalRead(setButtonPin)) return;
  digitalWrite(led2Pin, LOW);
  lightReading = analogRead(cdsPin);
  if ( levelDeltaCheck(initialLightReading, lightReading) ) goto restart;
  delay(500);
  if (digitalRead(setButtonPin)) return;
  digitalWrite(led3Pin, LOW);
  lightReading = analogRead(cdsPin);
  if ( levelDeltaCheck(initialLightReading, lightReading) ) goto restart;
  delay(500);
  if (digitalRead(setButtonPin)) return;
  digitalWrite(led4Pin, LOW);
  lightReading = analogRead(cdsPin);
  if ( levelDeltaCheck(initialLightReading, lightReading) ) goto restart;
  delay(500);
  if (digitalRead(setButtonPin)) return;
  digitalWrite(led5Pin, LOW);
  lightReading = analogRead(cdsPin);
  if ( levelDeltaCheck(initialLightReading, lightReading) ) goto restart;
  delay(500);
  lightReading = analogRead(cdsPin);
  if ( levelDeltaCheck(initialLightReading, lightReading) ) goto restart;
  triggerLevel = lightReading;

  // save this value as the ON triger level
  EEPROMWriteInt(triggerLevelAddr, triggerLevel);
  
  // flash LEDs to indicate the new trigger level was saved
  digitalWrite(relayPin, HIGH); // relay on
  digitalWrite(led1Pin, HIGH);
  digitalWrite(led2Pin, HIGH);
  digitalWrite(led3Pin, HIGH);
  digitalWrite(led4Pin, HIGH);
  digitalWrite(led5Pin, HIGH);
  delay(300);
  digitalWrite(relayPin, LOW); // relay off
  digitalWrite(led1Pin, LOW);
  digitalWrite(led2Pin, LOW);
  digitalWrite(led3Pin, LOW);
  digitalWrite(led4Pin, LOW);
  digitalWrite(led5Pin, LOW);
  delay(300);
  return;

  // Max delta range error, clear LEDs and restart sampling
  restart:
  delay(25);   // small delay before clearing to help indicate what is happening
  digitalWrite(led1Pin, HIGH);
  digitalWrite(led2Pin, HIGH);
  digitalWrite(led3Pin, HIGH);
  digitalWrite(led4Pin, HIGH);
  digitalWrite(led5Pin, HIGH);
  goto sampleLight;
}

// This function first clears the LEDs turns on the corresponding LED to indicate hours remaning in the countdown timer
void refreshLEDs()
{
  // clear LEDs
  digitalWrite(led1Pin, HIGH);
  digitalWrite(led2Pin, HIGH);
  digitalWrite(led3Pin, HIGH);
  digitalWrite(led4Pin, HIGH);
  digitalWrite(led5Pin, HIGH);

  if (LEDstate == true)    // turn on hour indicator LED only if LEDstate is true (on)
  {
    if (shutOffDelayCounter == 5) digitalWrite(led5Pin, LOW);  // turn on LED #5
    if (shutOffDelayCounter == 4) digitalWrite(led4Pin, LOW);  // turn on LED #4
    if (shutOffDelayCounter == 3) digitalWrite(led3Pin, LOW);  // turn on LED #3
    if (shutOffDelayCounter == 2) digitalWrite(led2Pin, LOW);  // turn on LED #2
    if (shutOffDelayCounter == 1) digitalWrite(led1Pin, LOW);  // turn on LED #1
  }
  
  return;
}

bool levelDeltaCheck(int p_initialLightReading, int p_lightReading)
{
  if ( (initialLightReading - lightDeltaMax > lightReading) || (initialLightReading + lightDeltaMax < lightReading) ) return true;
  return false;
}

// this function will write a 2 byte integer to the eeprom at the specified address and address + 1
void EEPROMWriteInt(int p_address, int p_value)
{
  byte lowByte = ((p_value >> 0) & 0xFF);
  byte highByte = ((p_value >> 8) & 0xFF);

  EEPROM.write(p_address, lowByte);
  EEPROM.write(p_address + 1, highByte);
}

// this function will read a 2 byte integer from the eeprom at the specified address and address + 1
unsigned int EEPROMReadInt(int p_address)
{
  byte lowByte = EEPROM.read(p_address);
  byte highByte = EEPROM.read(p_address + 1);

  return ((lowByte << 0) & 0xFF) + ((highByte << 8) & 0xFF00);
}
