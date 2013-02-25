/*
GuitarButtons v0.3
Enables use of Squier guitar buttons with XBox 360 MIDI Adapter

2011-09-05
WDO

2011-09-24
JAH

2012-10-28
BME
*/

// I/O pins used for button outputs
// These are dependent on which Aruino outputs you
// wired to each controller button circuit
const int BTN_FWD = 2;
const int BTN_BCK = 3;

const int DPAD_UP = 4;
const int DPAD_RT = 5;
const int DPAD_DN = 6;
const int DPAD_LT = 7;

const int BTN_GRN = 8;
const int BTN_BLU = 9;
const int BTN_YLW = 10;
const int BTN_RED = 11;

const int BTN_XBX = 12;

const int STATUS_LED = 13;

const int MSG_MAX_LENGTH = 20;
const int MSG_TIMEOUT = 2000;

byte MsgBuffer[MSG_MAX_LENGTH];
int msglength;
bool XBoxButtonIsPressed = false;
bool forwardPressed = false;
bool forwardWaiting = false;
bool backwardPressed = false;
bool backwardWaiting = false;

const unsigned long HOLD_TIME = 5;
unsigned long forwardPressedTime = 0;
unsigned long backwardPressedTime = 0;

void Press(int Pin) {
  pinMode(Pin, OUTPUT);
}

void Release(int Pin) {
  pinMode(Pin, INPUT);
}

void setup() {
  //  Set MIDI baud rate:
  Serial.begin(31250);

  // Set button pins to iput mode to start
  // (should be already, anyway)
  reset();

  // While in INPUT mode, setting the pin LOW disables pullup, so it won't interfere with the MIDI adapter's own pullup
  // While in OUTPUT mode, LOW simulates a button press
  // Once set to LOW, these don't need to be changed.  Button presses are simulated by switching from INPUT to OUTPUT mode.
  digitalWrite(BTN_BLU, LOW);
  digitalWrite(BTN_GRN, LOW);
  digitalWrite(BTN_RED, LOW);
  digitalWrite(BTN_YLW, LOW);
  digitalWrite(BTN_FWD, LOW);
  digitalWrite(BTN_BCK, LOW);
  digitalWrite(BTN_XBX, LOW);
  digitalWrite(DPAD_UP, LOW);
  digitalWrite(DPAD_DN, LOW);
  digitalWrite(DPAD_LT, LOW);
  digitalWrite(DPAD_RT, LOW);

  pinMode(STATUS_LED, OUTPUT);
  digitalWrite(STATUS_LED, LOW);
}

void reset() {
    Release(BTN_BLU);
    Release(BTN_GRN);
    Release(BTN_RED);
    Release(BTN_YLW);
    Release(BTN_FWD);
    Release(BTN_BCK);
    Release(BTN_XBX);
    Release(DPAD_UP);
    Release(DPAD_DN);
    Release(DPAD_LT);
    Release(DPAD_RT);
    XBoxButtonIsPressed = false;
    forwardWaiting = false;
    backwardWaiting = false;
    forwardPressed = false;
    backwardPressed = false;
}

void loop() {
  byte inbyte; 
  byte clrstate, dirstate, padstate;
  int msglength = 0;
  int bytecount = 0;
  unsigned long StartTime = millis();

  // wait for a SysEx message, or time out
  while (msglength == 0 && StartTime + MSG_TIMEOUT >  millis()) {
    if (Serial.available() > 0) {
      inbyte = Serial.read(); 
      if (inbyte == 0xF0) {  // start of a SysEx message
        StartTime = millis();
        bytecount = 0;
        // digitalWrite(STATUS_LED, LOW);
      }
      MsgBuffer[bytecount] = inbyte;
      bytecount++;
      if (inbyte == 0xF7 || bytecount == MSG_MAX_LENGTH - 1) {
         msglength = bytecount;
      }
    }
    
    
    if(backwardWaiting && backwardPressedTime + HOLD_TIME < millis()) {
      Press(BTN_BCK);
      backwardWaiting = false;
    }
    
    if(forwardWaiting && forwardPressedTime + HOLD_TIME < millis()) {
      Press(BTN_FWD);
      forwardWaiting = false;
    }
  }
  digitalWrite(STATUS_LED, LOW);
      
  // message format:  F0 08 40 08 08 0x 0y 4z 00 F7
  
  if ( msglength==10 && MsgBuffer[1]==0x08 && MsgBuffer[2]==0x40 && MsgBuffer[3]==0x08 && MsgBuffer[4]==0x08 ) {  

    digitalWrite(STATUS_LED, HIGH);
    
    clrstate = MsgBuffer[5] & 0x0F; // colored buttons
    if (clrstate & 0x01) Press(BTN_BLU); else Release(BTN_BLU);
    if (clrstate & 0x02) Press(BTN_GRN); else Release(BTN_GRN);
    if (clrstate & 0x04) Press(BTN_RED); else Release(BTN_RED);
    if (clrstate & 0x08) Press(BTN_YLW); else Release(BTN_YLW);
      
    dirstate = MsgBuffer[6] & 0x03; // forward/back buttons
    if (XBoxButtonIsPressed) {
      if(!(dirstate & 0x01 && dirstate & 0x02)) {
        // At least one button has been released
        Release(BTN_XBX);
      }    
      if(!(dirstate & 0x01 || dirstate & 0x02)) {
        // Both buttons have been released
        XBoxButtonIsPressed = false;
      }
    } else {
      if (dirstate & 0x01) {
        if(!backwardPressed) {
          backwardPressed = true;
          backwardWaiting = true;
          backwardPressedTime = millis();
        } 
      } else {
        if(backwardPressed) {
          backwardPressed = false;
          backwardWaiting = false;
          Release(BTN_BCK);
        }
      }
      
      if(dirstate & 0x02) {
        if(!forwardPressed) {
          forwardPressed = true;
          forwardWaiting = true;
          forwardPressedTime = millis();
        }
      } else {
        if(forwardPressed) {
          forwardPressed = false;
          forwardWaiting = false;
          Release(BTN_FWD);
        }
      }
        
      if(forwardWaiting && backwardWaiting) {
        // both buttons pressed within HOLD_TIME
        Press(BTN_XBX);
        XBoxButtonIsPressed = true;
        forwardWaiting = false;
        backwardWaiting = false;
        forwardPressed = false;
        backwardPressed = false;
      }
    }

    padstate = MsgBuffer[7] & 0x0F; // D-Pad
    if (padstate == 0 || padstate == 1 || padstate == 7) Press(DPAD_UP); else Release(DPAD_UP);
    if (padstate == 1 || padstate == 2 || padstate == 3) Press(DPAD_RT); else Release(DPAD_RT);
    if (padstate == 3 || padstate == 4 || padstate == 5) Press(DPAD_DN); else Release(DPAD_DN);
    if (padstate == 5 || padstate == 6 || padstate == 7) Press(DPAD_LT); else Release(DPAD_LT);
    
  } else if (msglength == 0) {
    // no heartbeat msg -- something is wrong (guitar is off or disconnected)
    // so clear all buttons
    reset();
  }
}
