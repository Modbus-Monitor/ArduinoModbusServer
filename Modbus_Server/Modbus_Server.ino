/*
  ModbusMonitor.com (c) 2022 ModbusMonitor.com

  Sample Project to use Arduino with Modbus Monitor Adavanced 

  modified 5 Sept 2022
  by ModbusMonitor.com Team
  
  This example code is in the public domain.

  Based On: https://www.arduino.cc/reference/en/libraries/arduinomodbus/
  Library:  https://github.com/arduino-libraries/ArduinoModbus

  Tested on:
   BN: Arduino Uno
   VID: 2341
   PID: 0043
   SN: 95XXXXXXXXXXXXXXXXE0
*/

#include <ArduinoModbus.h>
#include <ArduinoModbus.h>


//Debug: Blink LED to show Run/Error Status

const int ledPin = LED_BUILTIN;   //Pin 13

//Server Status
enum class SERVER_STATUS_TYPE { BOOT, INIT, STARTING, RUNNING, FAIL, MAX};


//Modbus Configuration
const int SlaveID = 1;
const int ModbusBaudRate = 9600;
const int numCoils = 14;          
const int numDiscreteInputs = 14; 

const int numHoldingRegisters = 10;
const int numInputRegisters = 10;

//local variables 
SERVER_STATUS_TYPE ServerStatus = SERVER_STATUS_TYPE::BOOT;


//Setup function runs once when you press reset or power the board
void setup() {

  //We are in Setup
  StatusBlink(SERVER_STATUS_TYPE::INIT);
  
  //initialize digital pin LED_BUILTIN as an output.
  pinMode(ledPin, OUTPUT);      //Pin 13 = LED with 1k Register
  pinMode(12, INPUT_PULLUP);    //Pin 12  = DI    //Connect GND To Turn OFF 
  pinMode(11, INPUT_PULLUP);    //Pin 11  = DI    //Connect GND To Turn OFF 
  pinMode(10, INPUT_PULLUP);    //Pin 10  = DI    //Connect GND To Turn OFF 
  pinMode(9, INPUT_PULLUP);     //Pin 09  = DI    //Connect GND To Turn OFF 
  pinMode(8, INPUT_PULLUP);     //Pin 08  = DI    //Connect GND To Turn OFF
  
  pinMode(7, INPUT);     //Pin 07  = DI           //Connect +5V To Turn ON
  pinMode(6, INPUT);     //Pin 06  = DI           //Connect +5V To Turn ON
  pinMode(5, INPUT);     //Pin 05  = DI           //Connect +5V To Turn ON
  pinMode(4, INPUT);     //Pin 04  = DI           //Connect +5V To Turn ON
  pinMode(3, INPUT);     //Pin 03  = DI           //Connect +5V To Turn ON
  pinMode(2, INPUT);     //Pin 02  = DI           //Connect +5V To Turn ON
  //pinMode(01, INPUT_PULLUP);    //Pin 01  = Already Set as TX
  //pinMode(00, INPUT_PULLUP);    //Pin 00  = Already Set as RX

  //Serial Port Config
  Serial.begin(ModbusBaudRate);
  //while (!Serial);  //Let's not hang when error or timeout
  if(!Serial) {
    StatusBlink(SERVER_STATUS_TYPE::FAIL);
  }
  

  //Start the Modbus RTU server, with (slave) id slave_id = 1
  if (!ModbusRTUServer.begin(SlaveID, ModbusBaudRate)) {
    //Serial.println("Failed to start Modbus RTU Server!");
    //while (1);  //Let's Not Hang when error       
    StatusBlink(SERVER_STATUS_TYPE::FAIL);    //Modbus Server Problem
  }

  //Configure Modbus PLC Image
  //[0x] configure coils at address 0x00    
  ModbusRTUServer.configureCoils(0x00, numCoils);

  //[1x] configure discrete inputs at address 0x00
  ModbusRTUServer.configureDiscreteInputs(0x00, numDiscreteInputs);

  //[3x] configure input registers at address 0x00
  ModbusRTUServer.configureInputRegisters(0x00, numInputRegisters);

  //[4x] configure holding registers at address 0x00
  ModbusRTUServer.configureHoldingRegisters(0x00, numHoldingRegisters);
 
  StatusBlink(SERVER_STATUS_TYPE::STARTING);
  //Debug: StatusBlink(SERVER_STATUS_TYPE::FAIL);  
}

// the loop function runs over and over again forever
void loop() {
  
  int packetReceived;
  int pinval;
  int coilValue;

  //Just Blink Last State or Set to RUNNING
  StatusBlink(SERVER_STATUS_TYPE::RUNNING); 
    
  //Poll for Modbus RTU requests
  ModbusRTUServer.poll();
  
  //Map the coil values to the discrete input values
  for (int i = 0; i < numCoils; i++) {

    //Modbus Map    
    //00001 = LED_BUILTIN   DO (Digital OUT)
    //00002 = NOT USED      xxx
    //00003 = LED_BUILTIN   DI (Digital IN)
    //00004 = LED_BUILTIN   DI (Digital IN)
    
    
    //Map IO to Modbus Coils & Discrete Inputs starting with Modbus Address: 0 [000001]
    switch(i) {      
      case 0: //Modbus Address=0, ModbusMonitor Address (1-based)= 000001
          //RX 
        break;

       case 1: //Modbus Address=1, ModbusMonitor Address (1-based)= 000002
           //TX
        break;

        case 2: //Modbus Address=2,   ModbusMonitor Address (1-based)= 000003
        case 3: //Modbus Address=3,   ModbusMonitor Address (1-based)= 000004
        case 4: //Modbus Address=4,   ModbusMonitor Address (1-based)= 000005
        case 5: //Modbus Address=5,   ModbusMonitor Address (1-based)= 000006
        case 6: //Modbus Address=6,   ModbusMonitor Address (1-based)= 000007
        case 7: //Modbus Address=7,   ModbusMonitor Address (1-based)= 000008
        case 8: //Modbus Address=8,   ModbusMonitor Address (1-based)= 000009
        case 9: //Modbus Address=9,   ModbusMonitor Address (1-based)= 000010
        case 10: //Modbus Address=10, ModbusMonitor Address (1-based)= 000011
        case 11: //Modbus Address=11, ModbusMonitor Address (1-based)= 000012
        case 12: //Modbus Address=12, ModbusMonitor Address (1-based)= 000013
        case 13: //Modbus Address=13, ModbusMonitor Address (1-based)= 000014

          //UNO Pin 00 - Pin 01 => Already used as TX/RX (Do not use)
          //UNO Pin 02 - Pin 13 => Modbus Address Mapped as follows
          coilValue = digitalRead(i);
          if(ModbusRTUServer.coilWrite(i,coilValue)) {
            //success
          } else {
            //failed
          } 
        break;
        
      break;

      default:
              
      break;
    }
    
     coilValue = ModbusRTUServer.coilRead(i);           //0x R/W
    ModbusRTUServer.discreteInputWrite(i, coilValue);   //1x RO
  }


  long holdingRegisterValue;
  // map the holding register values to the input register values
  for (int i = 0; i < numHoldingRegisters; i++) {
    


   //Map IO to Modbus Coils & Discrete Inputs starting with Modbus Address: 0 [000001]
    switch(i) {      
      
      case 0: //A0 = Modbus Address=0, ModbusMonitor Address (1-based)= 300001,  0 - 1023 for 0 to +5V
      case 1: //A1 = Modbus Address=1, ModbusMonitor Address (1-based)= 300002,  0 - 1023 for 0 to +5V
      case 2: //A2 = Modbus Address=2, ModbusMonitor Address (1-based)= 300003,  0 - 1023 for 0 to +5V
      case 3: //A3 = Modbus Address=3, ModbusMonitor Address (1-based)= 300004,  0 - 1023 for 0 to +5V
      case 4: //A4 = Modbus Address=4, ModbusMonitor Address (1-based)= 300005,  0 - 1023 for 0 to +5V
      case 5: //A5 = Modbus Address=5, ModbusMonitor Address (1-based)= 300006,  0 - 1023 for 0 to +5V
         holdingRegisterValue = analogRead(i);  //AI A0-A5, 0 to 1023 
        break;

      default:        
        holdingRegisterValue  = (long)ServerStatus;                
      break;
      
    }
    
    if(ModbusRTUServer.holdingRegisterWrite(i, holdingRegisterValue)) {
      //success
    } else {
      //failed
    }
        
    holdingRegisterValue = ModbusRTUServer.holdingRegisterRead(i);  //4x R/W
    ModbusRTUServer.inputRegisterWrite(i, holdingRegisterValue);    //3x RO
  }  
}


unsigned long lastPolled = 0;
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
        
        //Let's use timer/timeout instead of delay to improve perfomance
        if ((millis() - lastPolled) > 500) {    //One Second LED Blink => 500ms On, 500ms Off
            lastPolled = millis();
            
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
