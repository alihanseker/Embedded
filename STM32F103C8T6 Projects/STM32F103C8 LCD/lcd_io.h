#include "stm32f10x.h"                  // Device header

static inline void LCD_SET_RS(void)
{
	GPIOA->BSRR = 1;
}
static inline void LCD_SET_E(void)
{
	GPIOA->BSRR = 2;
}
static inline void LCD_RST_RS(void)
{
	GPIOA->BRR = 1;
}
static inline void LCD_RST_E(void)
{
	GPIOA->BRR = 2;
}

static inline void LcdConfigIO(void)
{
	GPIOA->CRL = (GPIOA->CRL & 0xFF000000) | 0x00222222;
}
static inline void Lcd_nibble(char data)
{
	GPIOA->BRR  = ~(data<<2) & 0b00111100;
	GPIOA->BSRR =  (data<<2) & 0b00111100;
}

 
