/*
*
* by Lewis Loflin www.bristolwatch.com lewis@bvu.net
* http://www.bristolwatch.com/rpi/i2clcd.htm
* Using wiringPi by Gordon Henderson
*
*
* Port over lcd_i2c.py to C and added improvements.
* Supports 16x2 and 20x4 screens.
* This was to learn now the I2C lcd displays operate.
* There is no warrenty of any kind use at your own risk.
*
*/

#include <wiringPiI2C.h>
#include <wiringPi.h>
#include <stdlib.h>
#include <stdio.h>

#include "../includes/system.h"

// Define some device parameters
#define I2C_ADDR   0x27 // I2C device address

// Define some device constants
#define LCD_CHR  1 // Mode - Sending data
#define LCD_CMD  0 // Mode - Sending command

#define LINE1  0x80 // 1st line
#define LINE2  0xC0 // 2nd line

#define LCD_BACKLIGHT   0x08  // On
// LCD_BACKLIGHT = 0x00  # Off

#define ENABLE  0b00000100 // Enable bit

void lcd_init(void);
void lcd_byte(int bits, int mode);
void lcd_toggle_enable(int bits);

// added by Lewis
void typeInt(int i);
void typeFloat(float myFloat);
void lcdLoc(int line); //move cursor
void ClrLcd(void); // clr LCD return home
void typeln(const char *s);
void typeChar(char val);
int fd;  // seen by all subroutines

// float to string
void typeFloat(float myFloat)   {
  char buffer[20];
  sprintf(buffer, "%4.1f",  myFloat);
  typeln(buffer);
}

// int to string
void typeInt(int i)   {
  char array1[20];
  sprintf(array1, "%d",  i);
  typeln(array1);
}

// clr lcd go home loc 0x80
void clrLcd(void)   {
  lcd_byte(0x01, LCD_CMD);
  lcd_byte(0x02, LCD_CMD);
}

// go to location on LCD
void lcdLoc(int line)   {
  lcd_byte(line, LCD_CMD);
}

// out char to LCD at current position
void typeChar(char val)   {

  lcd_byte(val, LCD_CHR);
}

// this allows use of any size string
void typeln(const char *s)   {

  while ( *s ) lcd_byte(*(s++), LCD_CHR);

}

void lcd_byte(int bits, int mode)   {

  //Send byte to data pins
  // bits = the data
  // mode = 1 for data, 0 for command
  int bits_high;
  int bits_low;
  // uses the two half byte writes to LCD
  bits_high = mode | (bits & 0xF0) | LCD_BACKLIGHT ;
  bits_low = mode | ((bits << 4) & 0xF0) | LCD_BACKLIGHT ;

  // High bits
  wiringPiI2CReadReg8(fd, bits_high);
  lcd_toggle_enable(bits_high);

  // Low bits
  wiringPiI2CReadReg8(fd, bits_low);
  lcd_toggle_enable(bits_low);
}

void lcd_toggle_enable(int bits)   {
  // Toggle enable pin on LCD display
  delayMicroseconds(500);
  wiringPiI2CReadReg8(fd, (bits | ENABLE));
  delayMicroseconds(500);
  wiringPiI2CReadReg8(fd, (bits & ~ENABLE));
  delayMicroseconds(500);
}


void lcd_init()   {
  if (wiringPiSetup () == -1) exit (1);

  fd = wiringPiI2CSetup(I2C_ADDR);
  // Initialise display
  lcd_byte(0x33, LCD_CMD); // Initialise
  lcd_byte(0x32, LCD_CMD); // Initialise
  lcd_byte(0x06, LCD_CMD); // Cursor move direction
  lcd_byte(0x0C, LCD_CMD); // 0x0F On, Blink Off
  lcd_byte(0x28, LCD_CMD); // Data length, number of lines, font size
  lcd_byte(0x01, LCD_CMD); // Clear display
  delayMicroseconds(500);
}

// C??digo autoral

void printTemperatures(float TI, float TR, float TE) {
  clrLcd();
  if (COOL_DOWN) {
    lcdLoc(LINE1);
    typeln("TI:");
    typeFloat(TI);
    typeln(" TE:");
    typeFloat(TE);
    lcdLoc(LINE2);
    typeln("Resfriando!");
  } 
  else {
    lcdLoc(LINE1);
    typeln("TI:");
    typeFloat(TI);
    typeln(" TR:");
    typeFloat(TR);
    lcdLoc(LINE2);
    if (START_TIMER == 1) {
      typeln("Tempo:");
      typeInt((int) timer);
      typeln(" min");
    } else {
      typeln("PRE AQUECENDO");
    }
  }
  delay(1000);
}

void printSystemOn() {
  lcdLoc(LINE1);
  typeln("SISTEMA LIGADO!");

  delay(1000);
}

void printSystemOff() {
  lcdLoc(LINE1);
  typeln("     SISTEMA    ");
  lcdLoc(LINE2);
  typeln("   DESLIGADO!   ");

  delay(1000);
}

void printFryingFinished() {
  lcdLoc(LINE1);
  typeln("   AQUECIMENTO  ");
  lcdLoc(LINE2);
  typeln("   FINALIZADO!  ");

  delay(1000);
}

void printMenuItem(char nome[10], float reference, int tempo) {
  clrLcd();
  lcdLoc(LINE1);
  typeln(nome);
  typeln(" TR:");
  typeFloat(reference);
  lcdLoc(LINE2);
  typeln("Tempo:");
  typeInt(tempo);

  delay(1000);
}