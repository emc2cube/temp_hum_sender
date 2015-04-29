/*
Temperature and humidity sensor / sender

Uses a DHT series temperature and humidity sensor and an ASK RF transmitter

Requires the following Arduino libraries to be installed in the libraries directory:
- DHT from https://github.com/markruys/arduino-DHT
- WT450Sender from https://github.com/GraemeWieden/WT450Sender
- JeeLib from https://github.com/jcw/jeelib
*/

#include <DHT.h>
#include <WT450Sender.h>
#include <Ports.h> // From JeeLib
ISR(WDT_vect) { Sleepy::watchdogEvent(); } 

// set up the hardware pins
byte sensorPin = 6; // digital pin for DHT sensor
byte sensorGroundPin = 8; // Ground Pin for DHT sensor
byte sensor5VPin = 5; // 5V Pin for DHT sensor
byte txPin = 12; // digital pin for RF transmitter
byte txGroundPin = 10; // Ground Pin for RF transmitter
byte tx5VPin = 11; // 5V Pin for RF transmitter
byte ledPin = 13; // digital pin for LED

// DEBUG mode, turn Serial monitor ON or OFF, + Blink LED when Tx
//const bool DEBUG = true; // set to true to enable serial monitoring
const bool DEBUG = false;

// define our desired house and channel codes
byte houseCode = 15; // from 1 to 15
byte channelCode = 4; // from 1 to 4

// define how long to sleep between two transmissions
byte SleepTime = 15; // Time in minutes

DHT sensor; // create the sensor object
WT450Sender sender; // create the sender object

void setup()
{  
  // Use adjacent pins to provide 5V and GND.
  pinMode(sensorGroundPin, OUTPUT);
  pinMode(sensor5VPin, OUTPUT);
  pinMode(txGroundPin, OUTPUT);
  pinMode(tx5VPin, OUTPUT);
  pinMode(ledPin, OUTPUT);  
  digitalWrite(sensorGroundPin, LOW);
  digitalWrite(txGroundPin, LOW);
  
  // DHT init and setup
  digitalWrite(sensor5VPin, HIGH);
  delay(sensor.getMinimumSamplingPeriod()); // The DHT requires some time to wake up, only once no need for Sleepy
  sensor.setup(sensorPin);
  sender.setup(txPin, houseCode, channelCode);
  digitalWrite(tx5VPin, LOW);
  
  if (DEBUG) {
  Serial.begin(9600);
  Serial.println("Temperature and Humidity Sender");
  Serial.print("Status");
  Serial.print("\t");
  Serial.print("Temperature");
  Serial.print("\t");
  Serial.println("Humidity");
  Serial.flush();
  }
}

void loop()
{
  digitalWrite(sensor5VPin, HIGH); // turn DHT sensor on
  Sleepy::loseSomeTime(sensor.getMinimumSamplingPeriod()); // The DHT requires some time to wake up
  
  // Read temperature and humidity
  double t = sensor.getTemperature();
  double h = sensor.getHumidity();

  digitalWrite(sensor5VPin, LOW); // turn DHT sensor off
  
  digitalWrite(tx5VPin, HIGH); // Turn 433MHz module on
  sender.send(h, t); // send the sensor readings
  digitalWrite(tx5VPin, LOW); // Turn 433MHz module off
  
  if (DEBUG) {
  Serial.print(sensor.getStatusString());
  Serial.print("\t");
  Serial.print(t); // output readings to the serial monitor
  Serial.print("C ");
  Serial.print("\t\t");
  Serial.print(h);
  Serial.println("%");
  blinkLed(); // flash the led
  Sleepy::loseSomeTime(10000); // send every 10 seconds
  }
  else
  for (byte i = 0; i < SleepTime; ++i) // repeat 60s sleep loop "SleepTime" times
  Sleepy::loseSomeTime(60000); // send every 60 seconds
}

void blinkLed()
{
  digitalWrite(ledPin, HIGH);
  delay(30);   
  digitalWrite(ledPin, LOW);
}

