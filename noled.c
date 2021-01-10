/*
  Newhaven character OLED library for displays like
  NHD-0216CW-AY3 - and other colour/size variants
  https://www.newhavendisplay.com/specs/NHD-0216CW-AY3.pdf

   Adapted from software provided by Newhaven Display International
   and updates made by Pasquale D'Antini.
   https://newhavendisplay.com/app_notes.html
   https://www.newhavendisplay.com/NHD_forum/index.php?topic=914.0

   Further updated by Andy4495 to package into Arduino library format
   and allow simpler selection of Arduino control pin numbers.
   https://github.com/Andy4495/NewhavenOLED

    This version by MWWorks, https://mwworks.uk
    Simplifies the code a little
    Ports it from C++ to C, so as to be able to compile into a QMK-based keyboard

    This is bitbanging SPI over 2 pins so doesn't require any SPI libraries. Wiring guide for this setup:
     1 GND
     2 5v
     3 5v
     4-6 N/C
     7 Clock
     8 Data
     9-14 N/C
     15 CS - tie to GND if you are not multiplexing the clock and data pins
     16 Reset - tie to 5v if you do not need this
     17-20 GND (these must all be tied to GND)
     See https://imgur.com/gallery/J3RqDzu for a pic of wiring to a pro micro

     Requires defines for the clock and data pins
     Could only get this working well via port commands rather than direct pins
     So the definition should follow this format, eg for pins F4 and F5

    #define NOLED_PORT_SCK  PORTF
    #define NOLED_PIN_SCK      PF4
    #define NOLED_DDR_SCK    DDRF
    #define NOLED_DPIN_SCK    DDF4

    #define NOLED_PORT_MOSI PORTF
    #define NOLED_PIN_MOSI     PF5
    #define NOLED_DDR_MOSI   DDRF
    #define NOLED_DPIN_MOSI   DDF5

     Somebody better informed can probably bitwise these in this library from just the 2 pin definitions!

     Also requires defines for display size:
    #define NOLED_LINES 2
    #define NOLED_COLS 16

    This is packaged with a keyboard firmware build so refer to that for usage example

*/

#pragma once
#include <util/delay.h>
#include <util/atomic.h>
#include <string.h>

//----------

static uint8_t row_bit;
static uint8_t row_address[4];


/*-------------- PRIVATE METHODS -------------------------------------------*/

// the other commands reformat bytes as required - this one actually pulses the bits
// original method references timing of digitalwrite and the us delay
// now using port registers but you could tinker with the us from the original 3
static void noledSendBits(uint8_t byte){

  //make sure no interrupts etc interfere with this
  ATOMIC_BLOCK(ATOMIC_FORCEON);
  for (uint8_t pos=0; pos<8 ; pos++) {
      if(byte & (1 << pos)){
        NOLED_PORT_MOSI |= (1 << NOLED_PIN_MOSI);
      }else{
        NOLED_PORT_MOSI &= ~(1 << NOLED_PIN_MOSI);
      }
      NOLED_PORT_SCK &= ~(1 << NOLED_PIN_SCK);     //pulse low
      _delay_us(3);                                //Waits >1 us (required for timing purpose)
      NOLED_PORT_SCK |= (1 << NOLED_PIN_SCK);      //then back to high
      _delay_us(3);                                //Waits >1 us (required for timing purpose)
  }
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE);

}

// Package the byte and transmit to OLED
// A byte is split into upper and lower nybbles, with 4 low bits filling the transfer
// In other words: low data nybble, 4 low bits, high data nybble, 4 low bits
static void noledSendByte(uint8_t tx_b)
{
  //send lower nybble, and 4 filler
  noledSendBits(tx_b & 0b00001111);
  //shift upper into lower, and the same filler
  noledSendBits((tx_b >> 4) & 0b00001111);
}

/* command(c)
   Send a command byte to the OLED
   Start Byte: 5 high bits, followed by:
   - R/W control bit: 0 = write, 1 = read
   - Data/Control bit: 0 = control, 1 = data
   - End bit: 0
   So in the case of a command, the sequence is 5 high bits followed by 3 low bits
*/
void noledCommand(uint8_t c)
{
  noledSendBits(0b00011111);
  noledSendByte(c);                      // Transmits the byte
}

/* data(c)
   Send a data byte to the OLED
   Start Byte: 5 high bits, followed by:
   - R/W control bit: 0 = write, 1 = read
   - Data/Control bit: 0 = control, 1 = data
   - End bit: 0
   So in the case of a command, the sequence is 5 high bits followed by low, high, low bits
*/
void noledData(uint8_t d)
{
  noledSendBits(0b01011111);
  noledSendByte(d);                      // Transmits the byte
}

/* begin()
   Initialize the control signals and display hardware.
   Note that this function has 252 ms of blocking delays
*/
void noledBegin(void)
{
  if ((NOLED_LINES == 2) || (NOLED_LINES == 4)) {
    row_bit = 0x08;                    // Display mode: 2/4 lines
  }else{
    row_bit = 0x00;                    // Display mode: 1/3 lines
  }

  row_address[0] = 0x80;
  row_address[1] = 0xA0;
  row_address[2] = 0xC0;
  row_address[3] = 0xE0;
  if (NOLED_LINES == 2) row_address[1] = 0xC0;

  //set pins to output
  NOLED_DDR_MOSI  |= (1 << NOLED_DPIN_MOSI);
  NOLED_DDR_SCK   |= (1 << NOLED_DPIN_SCK);

  //and default states, MOSI low CLK high
  NOLED_PORT_MOSI &= ~(1 << NOLED_PIN_MOSI);
  NOLED_PORT_SCK  |= (1 << NOLED_PIN_SCK);

  noledCommand(0x22 | row_bit); // Function set: extended command set (RE=1), lines #
  noledCommand(0x71);        // Function selection A:
  noledData(0x5C);           //  enable internal Vdd regulator at 5V I/O mode (def. value) (0x00 for disable, 2.8V I/O)
  noledCommand(0x20 | row_bit); // Function set: fundamental command set (RE=0) (exit from extended command set), lines #
  noledCommand(0x08);        // Display ON/OFF control: display off, cursor off, blink off (default values)
  noledCommand(0x22 | row_bit); // Function set: extended command set (RE=1), lines #
  noledCommand(0x79);        // OLED characterization: OLED command set enabled (SD=1)
  noledCommand(0xD5);        // Set display clock divide ratio/oscillator frequency:
  noledCommand(0x70);        //  divide ratio=1, frequency=7 (default values)
  noledCommand(0x78);        // OLED characterization: OLED command set disabled (SD=0) (exit from OLED command set)

  if (NOLED_LINES > 2){
  noledCommand(0x09);  // Extended function set (RE=1): 5-dot font, B/W inverting disabled (def. val.), 3/4 lines
  }else{
  noledCommand(0x08);  // Extended function set (RE=1): 5-dot font, B/W inverting disabled (def. val.), 1/2 lines
  }

  noledCommand(0x06);        // Entry Mode set - COM/SEG direction: COM0->COM31, SEG99->SEG0 (BDC=1, BDS=0)
  noledCommand(0x72);        // Function selection B:
  noledData(0x0A);           //  ROM/CGRAM selection: ROM C, CGROM=250, CGRAM=6 (ROM=10, OPR=10)
  noledCommand(0x79);        // OLED characterization: OLED command set enabled (SD=1)
  noledCommand(0xDA);        // Set SEG pins hardware configuration:
  noledCommand(0x10);        //  alternative odd/even SEG pin, disable SEG left/right remap (default values)
  noledCommand(0xDC);        // Function selection C:
  noledCommand(0x00);        //  internal VSL, GPIO input disable
  noledCommand(0x81);        // Set contrast control:
  noledCommand(0x7F);        //  contrast=127 (default value)
  noledCommand(0xD9);        // Set phase length:
  noledCommand(0xF1);        //  phase2=15, phase1=1 (default: 0x78)
  noledCommand(0xDB);        // Set VCOMH deselect level:
  noledCommand(0x40);        //  VCOMH deselect level=1 x Vcc (default: 0x20=0,77 x Vcc)
  noledCommand(0x78);        // OLED characterization: OLED command set disabled (SD=0) (exit from OLED command set)
  noledCommand(0x20 | row_bit); // Function set: fundamental command set (RE=0) (exit from extended command set), lines #
  noledCommand(0x01);        // Clear display
  _delay_ms(2);             // After a clear display, a minimum pause of 1-2 ms is required
  noledCommand(0x80);        // Set DDRAM address 0x00 in address counter (cursor home) (default value)
  noledCommand(0x0C);        // Display ON/OFF control: display ON, cursor off, blink off
  _delay_ms(250);           // Waits 250 ms for stabilization purpose after display on
}

/* end()
   Turn off the OLED and set the control pins to default values for the OLED
*/
void noledEnd(void)
{
  noledCommand(0x20 | row_bit); // RE=0 (exit from extended command set), lines #
  noledCommand(0x08);        // display off, cursor off, blink off (default values)

  //and default states, MOSI low CLK high
  NOLED_PORT_MOSI &= ~(1 << NOLED_PIN_MOSI);
  NOLED_PORT_SCK  |= (1 << NOLED_PIN_SCK);
}

/* clear()
   Clear the OLED display: move cursosr to top left, and clear display RAM
   Note that this function has a 2 ms blocking delay
*/
void noledClear(void)
{
  noledCommand(0x01);        // Clear display
  _delay_ms(2);             // After a clear display, a minimum pause of 1-2 ms is required
}

void noledWriteChar(uint8_t row, uint8_t col, char c) {
  if (col < NOLED_COLS && row < NOLED_LINES) {
    // Write the character
    noledCommand(row_address[row] + col);
    noledData(c);
  }
}

void noledWriteChars(uint8_t row, uint8_t col, char* c) {
    for(uint8_t pos=0; pos<strlen(c); pos++){
      noledWriteChar(row, col+pos, c[pos]);
    }
}

void noledWriteBlock(uint8_t row, uint8_t col, char* c, uint8_t length, bool ljust) {
    uint8_t i;

    if(!ljust){
      for(i=0; i<(length-strlen(c)); i++){
        noledWriteChar(row, col, ' ');
        col++;
      }
    }

    noledWriteChars(row, col, c);
    col += strlen(c);

    if(ljust){
      for(i=0; i<(length-strlen(c)); i++){
        noledWriteChar(row, col, ' ');
        col++;
      }
    }
}

