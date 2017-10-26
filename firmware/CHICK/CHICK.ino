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

 Created 18 Oct. 2017
 Modified 20 Oct. 2017
 By Josh Campbell

 This work is licensed under a Creative Commons Attribution-ShareAlike 4.0 International License.
 */

// pin the Relay is connected to
const int relayPin = 5;

// pins the LEDs are connected to
const int led1Pin = 6;    // Hour 1
const int led2Pin = 7;    // Hour 2
const int led3Pin = 8;    // Hour 3
const int led4Pin = 9;    // Hour 4
const int led5Pin = 10;   // Hour 5

// pins the buttons are connected to
const int manualButtonPin = 2;     // Manual button
const int hoursButtonPin = 3;      // Hours button
const int setButtonPin = 4;        // Set button

const int cdsPin = 0;     // CDS light sensor

int manualButtonState = 0;  // variable for reading the manual button status
int hoursButtonState = 0;   // variable for reading the hours button status
int setButtonState = 0;     // variable for reading the set button status
int lightReading = 0;       // value read from the CDS/Photocell sensor

void setup() {
  
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

  // Debouce
  //TODO

  if (manualButtonState == LOW)
  {
    digitalWrite(led1Pin, LOW);
    delay(1000);
  }

  if (hoursButtonState == LOW)
  {
    digitalWrite(led2Pin, LOW);
    delay(1000);
  }

  if (setButtonState == LOW)
  {
    digitalWrite(led3Pin, LOW);
    digitalWrite(led4Pin, LOW);
    digitalWrite(led5Pin, LOW);
    delay(1000);
  }

  digitalWrite(led1Pin, HIGH);
  digitalWrite(led2Pin, HIGH);
  digitalWrite(led3Pin, HIGH);
  digitalWrite(led4Pin, HIGH);
  digitalWrite(led5Pin, HIGH);

  // read the analog in value of the CDS sensor:
  lightReading = analogRead(cdsPin);

  if (lightReading > 1000)
  {
    digitalWrite(relayPin, LOW);
  }
  else
  {
    digitalWrite(relayPin, HIGH); 
  }

  // print the results to the serial monitor/plotter:
  //Serial.print("sensor = ");
  //Serial.println(lightReading);

  delay(100);
}
