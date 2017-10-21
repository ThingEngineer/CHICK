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
  * CDS
  * Relay

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
const int manualButton = 4;     // Manual button
const int hoursButton = 5;      // Hours button
const int setButton = 6;        // Set button

const int cdsInPin = 0;   // CDS light sensor

int lightReading = 0;      // value read from the CDS/Photocell sensor

void setup() {
  
  // initialize serial communications at 9600 bps:
  Serial.begin(9600);
  
  pinMode(relayPin, OUTPUT);
  
}

void loop() {

  // read the analog in value of the CDS sensor:
  lightReading = analogRead(cdsInPin);

  if (lightReading > 1000)
  {
    digitalWrite(relayPin, LOW);
  }
  else
  {
    digitalWrite(relayPin, HIGH); 
  }

  // print the results to the serial monitor/plotter:
  Serial.print("sensor = ");
  Serial.print(lightReading);
  Serial.print("\n");

  delay(100);
}
