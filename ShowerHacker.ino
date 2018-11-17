/*
 * Copyright (c) 2016 Intel Corporation.  All rights reserved.
 * See the bottom of this file for the license terms.
 */

#include <CurieBLE.h>
#include <Wire.h>
#include <Adafruit_MotorShield.h>
//#include <stdlib.h> 

// Create the motor shield object with the default I2C address
Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 
// Or, create it with a different I2C address (say for stacking)
// Adafruit_MotorShield AFMS = Adafruit_MotorShield(0x61); 

// Connect a stepper motor with 200 steps per revolution (1.8 degree)
// to motor port #2 (M3 and M4)
Adafruit_StepperMotor *myMotor = AFMS.getStepper(200, 2);

const int ledPin = 13; // set ledPin to on-board LED
const int buttonPin = 4; // set buttonPin to digital pin 4

BLEService ledService("19B10010-E8F2-537E-4F6C-D104768A1214"); // create service


// create switch characteristic and allow remote device to read and write
BLECharCharacteristic ledCharacteristic("19B10011-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);
// create button characteristic and allow remote device to get notifications
BLECharCharacteristic buttonCharacteristic("19B10012-E8F2-537E-4F6C-D104768A1214", BLERead | BLENotify); // allows remote device to get notifications


int left_counter = 0;



void setup() {
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT); // use the LED on pin 13 as an output
  pinMode(buttonPin, INPUT); // use button pin 4 as an input

  // begin initialization
  BLE.begin();
  AFMS.begin();  // create with the default frequency 1.6KHz
  //AFMS.begin(1000);  // OR with a different frequency, say 1KHz
  myMotor->setSpeed(75);  // 10 rpm   
  // set the local name peripheral advertises
  BLE.setLocalName("TOPZ");
  // set the UUID for the service this peripheral advertises:
  BLE.setAdvertisedService(ledService);

  // add the characteristics to the service
  ledService.addCharacteristic(ledCharacteristic);
  ledService.addCharacteristic(buttonCharacteristic);

  // add the service
  BLE.addService(ledService);

  ledCharacteristic.setValue(0);
  buttonCharacteristic.setValue(0);

  // start advertising
  BLE.advertise();

  Serial.println("Bluetooth device active, waiting for connections...");
}

void loop() {
  // poll for BLE events
  BLE.poll();

  // read the current button pin state
  char buttonValue = digitalRead(buttonPin);

  // has the value changed since the last read
  boolean buttonChanged = (buttonCharacteristic.value() != buttonValue);

  //reset to default position


  if (buttonChanged) {
    // button state changed, update characteristics
    ledCharacteristic.setValue(buttonValue);
    buttonCharacteristic.setValue(buttonValue);
  }

  if (ledCharacteristic.written() || buttonChanged) {
    //int val = atoi((const char *)ledCharacteristic.value());
    //Serial.println(val);
    // update motor, either central has written to characteristic or button state has changed
    if (ledCharacteristic.value()==0x01) {
      if (left_counter==0){
        Serial.println("motor forward");
        myMotor->step(100, FORWARD, MICROSTEP); 
        left_counter+=100;
      }else{
        Serial.println("Already turned 100");
      }
      
    } else if(ledCharacteristic.value()==0x02) {
     if (left_counter >0){
      Serial.println("motor reset, backward by");
      Serial.println(left_counter);
      myMotor->step(left_counter, BACKWARD, MICROSTEP); 
      left_counter=0;}
      else{
         Serial.println("Already at 0");
      }
    } else if(ledCharacteristic.value()==0x03) {
      if (left_counter+10 <=100){
      Serial.println("motor forward 10%");
       myMotor->step(10, FORWARD, MICROSTEP); 
      left_counter+=10;}
      else{
        Serial.println("Already turned 100");
      }
    } else if(ledCharacteristic.value()==0x04) {
       if (left_counter-10 >=0){
      Serial.println("motor backward 10%");
       myMotor->step(10, BACKWARD, MICROSTEP); 
      left_counter-=10;}
      else{
         Serial.println("Already at 0");
      }
    } 


    }
 

}

  
