#define F_CPU 8000000UL
#include <time.h>
#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

//definitions for LCD
#define  RS PD0
#define  EN PD1
#define LCD_Dir DDRD
#define LCD_Port PORTD
#define PIN PC2

//DEF FOR KEYPAD



void LCD_Cmd(unsigned char cmd);//to pass LCD command [like go to which line or go home] also used to initialize
void LCD_Char(unsigned char char_data);// write the data to lcd
void LCD_Init(void);//initialize LCD with basic commands
void LCD_String(char *str);// write data[str] to LCD character by character
void LCD_write_xy(char x, char y, char *str);//write data but with specified xition
void LCD_Clear();