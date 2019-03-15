#include "FastLED.h"

// How many leds in your strip?
#define NUM_LEDS 111

// For led chips like Neopixels, which have a data line, ground, and power, you just
// need to define DATA_PIN.  For led chipsets that are SPI based (four wires - data, clock,
// ground, and power), like the LPD8806 define both DATA_PIN and CLOCK_PIN
#define DATA_PIN 2

// Order in which the Pi sends pixel colors
#define COLOR_ORDER GRB

// this version uses a start marker 254 and an end marker of 255
// it uses 253 as a special byte to be able to reproduce 253, 254 and 255
// it also sends data to the PC using the same system
// if the number of bytes is 0 the PC will assume a debug string and just print it to the screen

#define startMarker 254
#define endMarker 255
#define specialByte 253
#define maxMessage 64
#define serialRate 57600

// Define the array of leds
CRGB leds[NUM_LEDS];

// the program could be rewritten to use local variables instead of some of these globals
// however globals make the code simpler and simplify memory management

byte bytesRecvd = 0;
byte dataSentNum = 0; // the transmitted value of the number of bytes in the package i.e. the 2nd byte received
byte dataRecvCount = 0;

byte dataRecvd[maxMessage]; 
byte dataSend[maxMessage];  
byte tempBuffer[maxMessage];

byte dataSendCount = 0; // the number of 'real' bytes to be sent to the PC
byte dataTotalSend = 0; // the number of bytes to send to PC taking account of encoded bytes

boolean inProgress = false;
boolean startFound = false;
boolean allReceived = false;

//================

void setup() {
  FastLED.addLeds<WS2812B, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  pinMode(13, OUTPUT); // the onboard LED
  Serial.begin(serialRate);
  debugToPC("Arduino Ready from ArduinoPC.ino");
}

//================

void loop() {
  getSerialData();
  processData();
}

//================
void getSerialData() {
  // Receives data into tempBuffer[]
  //   saves the number of bytes that the PC said it sent - which will be in tempBuffer[1]
  //   uses decodeHighBytes() to copy data from tempBuffer to dataRecvd[]
  // the Arduino program will use the data it finds in dataRecvd[]
  if(Serial.available() > 0) {
    byte x = Serial.read();
    if (x == startMarker) { 
      bytesRecvd = 0; 
      inProgress = true;
      // blinkLED(2);
      // debugToPC("start received");
    }

    if(inProgress) {
      tempBuffer[bytesRecvd] = x;
      bytesRecvd++;
    }

    if (x == endMarker) {
      inProgress = false;
      allReceived = true;

      // save the number of bytes that were sent
      dataSentNum = tempBuffer[1];
  
      decodeHighBytes();
    }
  }
}

//============================
void processData() {
  // processes the data that is in dataRecvd[]
  if (allReceived) {
    // for demonstration just copy dataRecvd to dataSend
    dataSendCount = dataRecvCount;
    for (byte n = 0; n < dataRecvCount; n++) {
       dataSend[n] = dataRecvd[n];
    }
    dataToPC();

    // Populate LED colors

    // The serial message consists of the following byte sequence, total 28 bytes for 5 dryboxes
    // First three bytes: Default GRB color (for LEDs that are not above a particular drybox,
    //          or above a drybox whose humidity is not "high")
    // Five bytes for each drybox:
    //          Begin and end index of LEDs above that drybox, followed by GRB color for it

    // Clear memory of LED colors
    memset(leds, 0, NUM_LEDS * sizeof(struct CRGB));
    // Interpret input data byte-by-byte
    byte defaultG = dataRecvd[0]; // Default green color
    byte defaultR = dataRecvd[1]; // Default red color
    byte defaultB = dataRecvd[2]; // Default blue color
    byte firstStart = dataRecvd[3]; // Start index of Filament 1
    byte firstEnd = dataRecvd[4]; // End index of Filament 1, non-inclusive
    byte firstG = dataRecvd[5]; // Green for Filament 1
    byte firstR = dataRecvd[6]; // Red for Filament 1
    byte firstB = dataRecvd[7]; // Blue for Filament 1
    byte secondStart = dataRecvd[8]; // Start index of Filament 2
    byte secondEnd = dataRecvd[9]; // End index of Filament 2, non-inclusive
    byte secondG = dataRecvd[10]; // Green for Filament 2
    byte secondR = dataRecvd[11]; // Red for Filament 2
    byte secondB = dataRecvd[12]; // Blue for Filament 2
    byte thirdStart = dataRecvd[13]; // Start index of Filament 3
    byte thirdEnd = dataRecvd[14]; // End index of Filament 3, non-inclusive
    byte thirdG = dataRecvd[15]; // Green for Filament 3
    byte thirdR = dataRecvd[16]; // Red for Filament 3
    byte thirdB = dataRecvd[17]; // Blue for Filament 3
    byte fourthStart = dataRecvd[18]; // Start index of Filament 4
    byte fourthEnd = dataRecvd[19]; // End index of Filament 4, non-inclusive
    byte fourthG = dataRecvd[20]; // Green for Filament 4
    byte fourthR = dataRecvd[21]; // Red for Filament 4
    byte fourthB = dataRecvd[22]; // Blue for Filament 4
    byte fifthStart = dataRecvd[23]; // Start index of Filament 5
    byte fifthEnd = dataRecvd[24]; // End index of Filament 5, non-inclusive
    byte fifthG = dataRecvd[25]; // Green for Filament 5
    byte fifthR = dataRecvd[26]; // Red for Filament 5
    byte fifthB = dataRecvd[27]; // Blue for Filament 5

    for (byte i = 0; i < NUM_LEDS; i++) {
      if (i >= firstStart && i < firstEnd) {
        leds[i].g = firstG;
        leds[i].r = firstR;
        leds[i].b = firstB;
      }
      else if (i >= secondStart && i < secondEnd) {
        leds[i].g = secondG;
        leds[i].r = secondR;
        leds[i].b = secondB;
      }
      else if (i >= thirdStart && i < thirdEnd) {
        leds[i].g = thirdG;
        leds[i].r = thirdR;
        leds[i].b = thirdB;
      }
      else if (i >= fourthStart && i < fourthEnd) {
        leds[i].g = fourthG;
        leds[i].r = fourthR;
        leds[i].b = fourthB;
      }
      else if (i >= fifthStart && i < fifthEnd) {
        leds[i].g = fifthG;
        leds[i].r = fifthR;
        leds[i].b = fifthB;
      }
      else {
        leds[i].g = defaultG;
        leds[i].r = defaultR;
        leds[i].b = defaultB;
      }
    }

    // Show the LEDs
    FastLED.show();

    delay(100);
    allReceived = false; 
  }
}

//============================
void decodeHighBytes() {
  //  copies to dataRecvd[] only the data bytes i.e. excluding the marker bytes and the count byte
  //  and converts any bytes of 253 etc into the intended numbers
  //  Note that bytesRecvd is the total of all the bytes including the markers
  dataRecvCount = 0;
  for (byte n = 2; n < bytesRecvd - 1 ; n++) { // 2 skips the start marker and the count byte, -1 omits the end marker
    byte x = tempBuffer[n];
    if (x == specialByte) {
       // debugToPC("FoundSpecialByte");
       n++;
       x = x + tempBuffer[n];
    }
    dataRecvd[dataRecvCount] = x;
    dataRecvCount++;
  }
}

//====================
void dataToPC() {
    // expects to find data in dataSend[]
    //   uses encodeHighBytes() to copy data to tempBuffer
    //   sends data to PC from tempBuffer
    encodeHighBytes();

    Serial.write(startMarker);
    Serial.write(dataSendCount);
    Serial.write(tempBuffer, dataTotalSend);
    Serial.write(endMarker);
}

//============================
void encodeHighBytes() {
  // Copies to temBuffer[] all of the data in dataSend[]
  // and converts any bytes of 253 or more into a pair of bytes, 253 0, 253 1 or 253 2 as appropriate
  dataTotalSend = 0;
  for (byte n = 0; n < dataSendCount; n++) {
    if (dataSend[n] >= specialByte) {
      tempBuffer[dataTotalSend] = specialByte;
      dataTotalSend++;
      tempBuffer[dataTotalSend] = dataSend[n] - specialByte;
    }
    else {
      tempBuffer[dataTotalSend] = dataSend[n];
    }
    dataTotalSend++;
  }
}

//=========================
void debugToPC(char arr[]) {
    byte nb = 0;
    Serial.write(startMarker);
    Serial.write(nb);
    Serial.print(arr);
    Serial.write(endMarker);
}

//=========================
void debugToPC(byte num) {
    byte nb = 0;
    Serial.write(startMarker);
    Serial.write(nb);
    Serial.print(num);
    Serial.write(endMarker);
}

//=========================
void blinkLED(byte numBlinks) {
    for (byte n = 0; n < numBlinks; n ++) {
      digitalWrite(13, HIGH);
      delay(200);
      digitalWrite(13, LOW);
      delay(200);
    }
}

