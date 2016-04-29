
// TICC Time interval Counter based on TICC Shield using TDC7200
// version -0.5 6 March 2016
// refactor to rename Ch0 to ChA and Ch 1 to ChB, combine duplicate
// subroutiones
// Copyright John Ackermann N8UR 2016
// Portions Copyright George Byrkit K9TRV 2016
// Licensed under BSD 2-clause license

#include "TICC.h"     //Register and structure definitions

// the TI TDC7200 communicates using SPI, so include the library:
#include <SPI.h>

volatile unsigned long long coarseCount = 0;

// Enumerate the TDC7200 channel structures
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))
static tdc7200Channel channels[] = {
	tdc7200Channel('A', ENABLE_A, INTB_A, CSB_A, STOP_A),
	tdc7200Channel('B', ENABLE_B, INTB_B, CSB_B, STOP_B),
};

// properties of the TDC7200 chip:
// Most Significant Bit is clocked first (MSBFIRST)
// clock is low when idle
// data is clocked on the rising edge of the clock (seems to be SPI_MODE0)
// max clock speed: 20 mHz
// Using TDC7200 timing mode 1...
void setup() {
  int i;
  // start the SPI library:
  SPI.begin();

  pinMode(intPin, INPUT);

  for(i = 0; i < ARRAY_SIZE(channels); ++i)
    channels[i].setup();

  attachInterrupt(interrupt, coarseTimer, RISING);

  delay(10);

  for(i = 0; i < ARRAY_SIZE(channels); ++i)
      channels[i].ready_next();

  Serial.begin(115200);
  Serial.println("Starting...");
}

void loop() {
  int i;

  for(i = 0; i < ARRAY_SIZE(channels); ++i) {
    if(channels[i].STOP)
      channels[i].stopTime = coarseCount;

    if(channels[i].INTB) {
      channels[i].result = channels[i].calc();
      channels[i].ready_next();
    }
  }

   // if we have both channels, subtract channel 0 from channel 1, print result, and reset vars
  if (channels[0].result && channels[1].result) {
    output_ti();
    for(i = 0; i < ARRAY_SIZE(channels); ++i)
    	channels[i].reset();
  }
}

// ISR for timer. NOTE: uint_64 rollover would take
// 62 million years at 100 usec interrupt rate.

void coarseTimer() {
  coarseCount++;
}

// Constructor
tdc7200Channel::tdc7200Channel(char id, int enable, int intb, int csb, int stop):
	ID(id), ENABLE(enable), INTB(intb), CSB(csb), STOP(stop) {

	pinMode(ENABLE,OUTPUT);
	pinMode(INTB,INPUT);
	pinMode(CSB,OUTPUT);
	pinMode(STOP,INPUT);
};

// Initial config for TDC7200

void tdc7200Channel::setup() {
  digitalWrite(ENABLE, HIGH);
}

void tdc7200Channel::reset() {
	result = 0;
	stopTime = 0;
}

// Fetch and calculate results from TDC7200
int tdc7200Channel::calc() {
    read();
    // calc the values (John...)
}

// Read TDC for channel
void tdc7200Channel::read() {
  byte inByte = 0;
  int timeResult = 0;
  int clockResult = 0;
  int calResult = 0;

  // read the TIMER1 register
  // take the chip select low to select the device:
  digitalWrite(CSB, LOW);

  SPI.transfer(TIME1);
  inByte = SPI.transfer(0x00);
  timeResult |= inByte;
  inByte = SPI.transfer(0x00);
  timeResult = timeResult<<8 | inByte;
  inByte = SPI.transfer(0x00);
  timeResult = timeResult<<8 | inByte;

  digitalWrite(CSB, HIGH);

  // read the CLOCK1 register
  // take the chip select low to select the device:
  digitalWrite(CSB, LOW);

  SPI.transfer(CLOCK_COUNT1);
  inByte = SPI.transfer(0x00);
  clockResult |= inByte;
  inByte = SPI.transfer(0x00);
  clockResult = clockResult<<8 | inByte;
  inByte = SPI.transfer(0x00);
  clockResult = clockResult<<8 | inByte;

  digitalWrite(CSB, HIGH);

  // read the CALIBRATION1 register
  // take the chip select low to select the device:
  digitalWrite(CSB, LOW);

  SPI.transfer(CALIBRATION1);
  inByte = SPI.transfer(0x00);
  calResult |= inByte;
  inByte = SPI.transfer(0x00);
  calResult = calResult<<8 | inByte;
  inByte = SPI.transfer(0x00);
  calResult = calResult<<8 | inByte;

  digitalWrite(CSB, HIGH);

  timeResult = timeResult;
  clockResult = clockResult;
  calResult = calResult;
  return;
}

// Enable next measurement cycle
void tdc7200Channel::ready_next() {
    // needs to set the enable bit (START_MEAS in CONFIG1)
    write(CONFIG1, 0x03);  // measurement mode 2 ('01')
}

// Calculate and print time interval to serial
void output_ti() {
}

void tdc7200Channel::write(byte address, byte value) {
  // take the chip select low to select the device:
  digitalWrite(CSB, LOW);

  SPI.transfer(address);
  SPI.transfer(value);

  digitalWrite(CSB, HIGH);
}

/* byte readTDC7200(struct chanTDC7200 *channel, byte address) {
  byte inByte = 0;

  // take the chip select low to select the device:
  digitalWrite(channel->CSB, LOW);

  SPI.transfer(address);
  inByte = SPI.transfer(0x00);

  digitalWrite(channel->CSB, HIGH);

  return inByte;
} */
