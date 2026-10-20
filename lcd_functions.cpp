//LCD CODES
#include "lcd.h"

void LCD_Cmd(unsigned char cmd){
	/*Sending the first nibble of data (Higher 4 bits)*/
	LCD_Port = (cmd & 0xF0);/* Sending upper nibble */
	LCD_Port &= ~ (1<<RS);
	LCD_Port |= (1<<EN);
	_delay_us(1);
	LCD_Port &= ~ (1<<EN);
	_delay_us(200);
	
	/*Sending the second nibble of data (Lower 4 bits)*/
	LCD_Port = (cmd << 4);/* Sending lower nibble */
	LCD_Port &= ~ (1<<RS);
	LCD_Port |= (1<<EN);
	_delay_us(1);
	LCD_Port &= ~ (1<<EN);
	_delay_ms(2);
}
/*LCD data write function */
void LCD_Char (unsigned char char_data){
	/*Sending the first nibble of data (Higher 4 bits)*/
	LCD_Port =(char_data & 0xF0);/* Sending upper nibble */
	LCD_Port |= (1<<RS);
	LCD_Port |= (1<<EN);
	_delay_us(1);
	LCD_Port &= ~ (1<<EN);
	_delay_us(200);
	//_delay_ms(500);
	/*Sending the second nibble of data (Lower 4 bits)*/
	LCD_Port =(char_data << 4);  /* Sending lower nibble */
	LCD_Port |= (1<<RS);
	LCD_Port |= (1<<EN);
	_delay_us(1);
	LCD_Port &= ~ (1<<EN);
	_delay_ms(2);
}
/*LCD Initialize function */
void LCD_Init (void){
	LCD_Dir = 0xFF; /* Make LCD command port direction as output pins*/
	_delay_ms(20); /* LCD Power ON delay always > 15ms */
	LCD_Cmd(0x02); /* Return display to its home position */
	LCD_Cmd(0x28); /* 2 line 4bit mode */
	LCD_Cmd(0x0C); /* Display ON Cursor OFF */
	LCD_Cmd(0x06); /* Auto Increment cursor */
	LCD_Cmd(0x01); /* Clear display */
	_delay_ms(2);
}


void LCD_String (char *str){
	int i;
	/* Send each char of string till the NULL */
	for(i=0;str[i]!=0;i++){
		LCD_Char(str[i]);
	}
}




void LCD_Clear()
{
	LCD_Cmd(0x01);
	_delay_ms(2);
	LCD_Cmd(0x02);
	_delay_us(50);
}
