/*
 * 
 * 
 * 
 * 
0x2341 is the Vendor ID for Arduino
0x8036 is the Product ID for the Leonardo
0x8037 is the Product ID for the Micro
0x040 indicates the Multiple Input “quirk” should be used (i.e. HID_QUIRK_MULTI_INPUT)

To get this to work on Linux, you need to adjust a setting on the usbhid driver. This can be done by editing the /boot/cmdline.txt file. Add the following string to the end of the line of text in this file (there should only be one line of text in this file):
For the Arduino Leonardo

usbhid.quirks=0x2341:0x8036:0x040
For the Arduino Micro

usbhid.quirks=0x2341:0x8037:0x040



rmmod usbhid && modprobe usbhid quirks=0xVID:0xPID:0x40
 * 
 * 
 * 
 */


//NES Gamepad to HID device
//By Dylan Dean, built with code from BradsProjects

//Joystick1 has issues with being recognized, so we just use 1 joystick from this 2 Joystick library
 #include <Joystick2.h> //from https://github.com/MHeironimus/ArduinoJoystickLibrary/tree/version-1.0



//NES Connector Pins
byte NESData1 = 4;//Yellow       // this is the pin that the Data connection is connected to - 1k resistor between this pin and ground is required to eliminate noise
byte NESLatch1 = 3;//Orange         // this is the pin that the Latch (otherwise known as strobe) connection is connected to
byte NESClock1 = 2;//Red         // this is the pin that the Clock connection is connected to
//   VCC (5v)      //White
//   GND           //Brown

byte NESButtonData1;         // This is where we will store the received data that comes from the NES Control Pad

void setup() {              // let's get a few things setup before we get into the main code
    Serial.begin(9600);         // serial data will be sent at 9600bps
    pinMode(NESLatch1, OUTPUT);  // Latch connection is an output
    pinMode(NESClock1, OUTPUT);  // Clock connection is an output
    pinMode(NESData1, INPUT);    // Data connection is an Input (because we need to receive the data from the control pad)

    Joystick[0].begin();
}
 
void loop() {                       // we will run this code in a constant loop until power is removed
    GetNESControllerData1();         // This calls the function to grab the NES control pad data and it will store it in 'NESButtonData1'
    Serial.println();

    if(NESButtonData1!=0){
    writeState(0,NESButtonData1);
    }else{
      //Controller not connected
      NESButtonData1=255;
      writeState(0,NESButtonData1);
    }
}
void writeState(int joystick, byte data){
for(int i=0;i<8;i++){
  //update what the HID driver sees for each button
  Joystick[joystick].setButton(i,!bitRead(data,i));
}
  
}
void GetNESControllerData1(){            // this is where it all happens as far as grabbing the NES control pad data
    digitalWrite(NESLatch1, HIGH);       // we need to send a clock pulse to the latch (strobe connection)...
    delay(2);
    digitalWrite(NESLatch1, LOW);        // this will cause the status of all eight buttons to get saved within the 4021 chip in the NES control pad.
    for(int x=0; x<8; x++){         // Now we need to transmit the eight bits of data serially from the NES control pad to the Arduino
        bitWrite(NESButtonData1,x,digitalRead(NESData1)); // one by one, we will read from the NESData1 line and store each bit in the NESButtonData1 variable.
        digitalWrite(NESClock1, HIGH);                   // once each bit is saved, we send a clock pulse to the NES clock connection...
        digitalWrite(NESClock1, LOW);                    // this will now shift all bits in the 4021 chip in the NES control pad, so we can read the next bit.
        delay(3);
    }
}
