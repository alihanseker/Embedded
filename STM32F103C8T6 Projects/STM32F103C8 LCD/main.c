#include "stm32f10x.h"                  // Device header
#include "lcd.h"
#include "string.h"
char pt[16];
void delay_us(unsigned int time)
{
	SysTick->LOAD = time;
	SysTick->VAL = 0;
	while(!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk));
}
void clock(void);
int main()
{
	clock();
	delay_us(1000000);
	LcdInit();

	while(1)
	{
		LcdSetAddress(0x80);
		sprintf(pt, "Merhaba, Dunya!");
		Lcd_yaz(pt);
		LcdSetAddress(0x40);
		Lcd_yaz("STM32F103C8T6");
		
	}
}

void clock(void)
{
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN |
									RCC_APB2ENR_IOPCEN | RCC_APB2ENR_IOPDEN | RCC_APB2ENR_AFIOEN;
	AFIO->MAPR = AFIO_MAPR_SWJ_CFG_JTAGDISABLE;
	SysTick->CTRL = SysTick_CTRL_ENABLE_Msk;
}

 

 	