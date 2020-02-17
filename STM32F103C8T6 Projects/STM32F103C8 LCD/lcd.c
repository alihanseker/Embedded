#include "lcd.h"
#include "lcd_io.h"

void LcdSendNibble(char data)
{
	Lcd_nibble(data);
	LCD_SET_E();
	delay_us(200);
	LCD_RST_E();
	delay_us(200);
}

void Lcd_komut(char cmd)
{
	LcdSendNibble(cmd>>4);
	LcdSendNibble(cmd);
}
void Lcd_karakter(char data)
{
	LCD_SET_RS();
	LcdSendNibble(data>>4);
	LcdSendNibble(data);
	LCD_RST_RS();
}

void LcdInit(void)
{
	LcdConfigIO();
	LCD_RST_E();
	LCD_RST_RS();
	delay_us(2000);
	LcdSendNibble(0x03);
	delay_us(5000);
	LcdSendNibble(0x03);
	delay_us(2000);
	LcdSendNibble(0x03);
	delay_us(2000);
	LcdSendNibble(0x02);
	delay_us(2000);
	Lcd_komut(0x28);
	delay_us(2000);
	Lcd_komut(0x0C);
	delay_us(2000);
	Lcd_komut(0x01);
	delay_us(2000);
	Lcd_komut(0x06);
	delay_us(2000);
	
}
void Lcd_yaz(const char *str)
{
	int i;
	for (i = 0; str[i] != 0; i++)
	{
		Lcd_karakter(str[i]);
	}
}
void LcdSetAddress(char addr)
{
	Lcd_komut(0x80 | addr);
}
void LcdClear(void)
{
	Lcd_komut(0x01);
	delay_us(2000);
}

 
