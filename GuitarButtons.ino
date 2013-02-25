/*
GuitarButtons v0.1
Enables use of Squier guitar buttons with XBox 360 MIDI Adapter

2011-09-05
WDO

2011-09-24
JAH
*/

// I/O pins used for button outputs
const int BTN_BLU = 8;
const int BTN_GRN = 9;
const int BTN_RED = 10;
const int BTN_YLW = 11;
const int BTN_XBX = 12;
const int BTN_FWD = 3;  // FWD & BCK swapped JAH
const int BTN_BCK = 2;
const int DPAD_UP = 5;  // UP & DN  swapped JAH
const int DPAD_DN = 4;
const int DPAD_LT = 7;  // LT & RT swapped JAH
const int DPAD_RT = 6;
const int STATUS_LED = 13;

const int MSG_MAX_LENGTH = 20;
const int MSG_TIMEOUT = 2000;

byte MsgBuffer[MSG_MAX_LENGTH];
int msglength;
bool XBoxButtonIsPressed = false;

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
  pinMode(BTN_BLU, INPUT);
  pinMode(BTN_GRN, INPUT);
  pinMode(BTN_RED, INPUT);
  pinMode(BTN_YLW, INPUT);
  pinMode(BTN_FWD, INPUT);
  pinMode(BTN_BCK, INPUT);
  pinMode(BTN_XBX, INPUT);
  pinMode(DPAD_UP, INPUT);
  pinMode(DPAD_DN, INPUT);
  pinMode(DPAD_LT, INPUT);
  pinMode(DPAD_RT, INPUT);

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

void loop() {
  byte inbyte; 
  byte clrstate, dirstate, padstate;
  int msglength = 0;
  int bytecount = 0;
  unsigned long StartTime = millis();

  // wait for a SysEx message, or time out
  while (msglength == 0 && StartTime > millis() - MSG_TIMEOUT ) {
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
  }  
  digitalWrite(STATUS_LED, LOW);
      
  // message format:  F0 08 40 08 08 0x 0y 4z 00 F7
  
  if ( msglength==10 && MsgBuffer[1]==0x08 && MsgBuffer[2]==0x40 && MsgBuffer[3]==0x08 && MsgBuffer[4]==0x08 ) {  

    digitalWrite(STATUS_LED, HIGH);
    
    clrstate = MsgBuffer[5] & 0x0F; // colored buttons
    if (clrstate & 0x01) Press(BTN_BLU); else Release(BTN_BLU);
    if (clrstate & 0x02) Press(BTN_GRN); else Release(BTN_GRN);
   
    if (clrstate & 0x04 && clrstate & 0x08) {
      // handle red and yellow pressed together as XBox button 
      // (This will probably pick up a stray single button press.  Filtering this would require complicated timing logic,
      // so hopefully it will be ignored or benign.)
      Release(BTN_RED);
      Release(BTN_YLW);
      Press(BTN_XBX);  
      XBoxButtonIsPressed = true;
    }
    else {
      if (XBoxButtonIsPressed) {
        // At lesat one of the buttons has been released, so release XBox button,
        // but don't handle the remaining button normally this time
        Release(BTN_XBX);
        XBoxButtonIsPressed = false;
      }
      else { 
        // handle red and yellow normally
        if (clrstate & 0x04) Press(BTN_RED); else Release(BTN_RED);
        if (clrstate & 0x08) Press(BTN_YLW); else Release(BTN_YLW);
      }
    }
        
    dirstate = MsgBuffer[6] & 0x03; // forward/back buttons
    if (dirstate & 0x01) Press(BTN_FWD); else Release(BTN_FWD);
    if (dirstate & 0x02) Press(BTN_BCK); else Release(BTN_BCK);

    padstate = MsgBuffer[7] & 0x0F; // D-Pad
    if (padstate == 0 || padstate == 1 || padstate == 7) Press(DPAD_DN); else Release(DPAD_DN);
    if (padstate == 1 || padstate == 2 || padstate == 3) Press(DPAD_LT); else Release(DPAD_LT);
    if (padstate == 3 || padstate == 4 || padstate == 5) Press(DPAD_UP); else Release(DPAD_UP);
    if (padstate == 5 || padstate == 6 || padstate == 7) Press(DPAD_RT); else Release(DPAD_RT);

    // turn on status LED if any buttons are being pressed
    // if (clrstate == 0 && dirstate == 0 && padstate == 8) digitalWrite(STATUS_LED, LOW); else digitalWrite(STATUS_LED, HIGH);
  }
  else if (msglength == 0) {
    // no heartbeat msg -- something is wrong (guitar is off or disconnected)
    // so clear all buttons
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
    // digitalWrite(STATUS_LED, LOW);
  }
}
