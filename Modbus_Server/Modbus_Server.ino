/*
  ModbusMonitor.com (c) 2022 ModbusMonitor.com

  Sample Project to use Arduino with Modbus Monitor Adavanced 

  modified 5 Sept 2022
  by ModbusMonitor.com Team
  
  This example code is in the public domain.

  Based On
  https://github.com/arduino-libraries/ArduinoModbus
*/

#include <ArduinoModbus.h>
#include <ArduinoModbus.h>


//Debug
const int ledPin = LED_BUILTIN;

//Modbus Image Size
const int numCoils = 10;
const int numDiscreteInputs = 10;
const int numHoldingRegisters = 10;
const int numInputRegisters = 10;
const int SlaveID = 1;
const int ModbusBaudRate = 9600;

enum class SERVER_STATUS_TYPE { BOOT, INIT, STARTING, RUNNING, FAIL, MAX};

SERVER_STATUS_TYPE ServerStatus = SERVER_STATUS_TYPE::BOOT;


// the setup function runs once when you press reset or power the board
void setup() {
    
  StatusBlink(SERVER_STATUS_TYPE::INIT);
  
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(ledPin, OUTPUT);
  
 
  Serial.begin(ModbusBaudRate);
  //while (!Serial);  //Let's not hang
  if(!Serial) {
    StatusBlink(SERVER_STATUS_TYPE::FAIL);
  }
  

  // start the Modbus RTU server, with (slave) id slave_id = 1
  if (!ModbusRTUServer.begin(SlaveID, ModbusBaudRate)) {
    //Serial.println("Failed to start Modbus RTU Server!");
    //while (1);  //Let's Not Hang
    //delay(100);
    //setup_ok = false;  //Modbus Server Problem
    StatusBlink(SERVER_STATUS_TYPE::FAIL);  
  }

  // configure coils at address 0x00
  ModbusRTUServer.configureCoils(0x00, numCoils);

  // configure discrete inputs at address 0x00
  ModbusRTUServer.configureDiscreteInputs(0x00, numDiscreteInputs);

  // configure holding registers at address 0x00
  ModbusRTUServer.configureHoldingRegisters(0x00, numHoldingRegisters);

  // configure input registers at address 0x00
  ModbusRTUServer.configureInputRegisters(0x00, numInputRegisters);
  
  StatusBlink(SERVER_STATUS_TYPE::STARTING);
  //StatusBlink(SERVER_STATUS_TYPE::FAIL);  
}

// the loop function runs over and over again forever
unsigned long lastPolled = 0;
void loop() {
  
  StatusBlink(SERVER_STATUS_TYPE::RUNNING); //Just Blink Last State
  
  int packetReceived;
  // poll for Modbus RTU requests
  ModbusRTUServer.poll();
  
  // map the coil values to the discrete input values
  for (int i = 0; i < numCoils; i++) {
    int coilValue = ModbusRTUServer.coilRead(i);

    ModbusRTUServer.discreteInputWrite(i, coilValue);
  }

  // map the holding register values to the input register values
  for (int i = 0; i < numHoldingRegisters; i++) {
    long holdingRegisterValue = ModbusRTUServer.holdingRegisterRead(i);

    ModbusRTUServer.inputRegisterWrite(i, holdingRegisterValue);
  }  
}


void StatusBlink(SERVER_STATUS_TYPE ServerSt) {

    static bool bLED = false;
   
    //Current State = Max => Blink LED ONLY
    //Past State = FAIL => Don't Update status but blink LED
    if ( (ServerSt == SERVER_STATUS_TYPE::MAX) || (ServerStatus == SERVER_STATUS_TYPE::FAIL) )  {
      //Keep Current State and just bink LED
    } else {
      ServerStatus = ServerSt;  
    }


    //BOOT, INIT, STARTING, RUNNING, FAIL, MAX
    switch(ServerStatus) {
      case SERVER_STATUS_TYPE::BOOT:
        digitalWrite(ledPin, HIGH);   
        break;
  
      case SERVER_STATUS_TYPE::INIT:    
          digitalWrite(ledPin, HIGH);   
          delay(500);
         digitalWrite(ledPin, LOW);    
      break;

      case SERVER_STATUS_TYPE::STARTING:    
          digitalWrite(ledPin, HIGH);   
          delay(1000);                  
          digitalWrite(ledPin, LOW);    
          delay(100);
          digitalWrite(ledPin, HIGH);   
          delay(1000);                  
          digitalWrite(ledPin, LOW);    
      break;
  
      case SERVER_STATUS_TYPE::RUNNING:      
      if ((millis() - lastPolled) > 500) {
          lastPolled = millis();
          //printDelay();
  
          bLED = !bLED;
          digitalWrite(ledPin, bLED);   //       
      }
      break;

  
      case SERVER_STATUS_TYPE::FAIL:    
          digitalWrite(ledPin, HIGH);   
          delay(100);                   
          digitalWrite(ledPin, LOW);    
          delay(100);                   
          digitalWrite(ledPin, HIGH);   
          delay(100);                   
          digitalWrite(ledPin, LOW);              
      break;
      
      default:
        //ignore
        break;
    }  
  
}
