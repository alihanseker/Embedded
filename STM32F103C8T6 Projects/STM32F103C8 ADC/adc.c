/*

ALIHAN SEKER TARAFINDAN YAZILMISTIR

2 EKSENLI BIR JOYSTICK ILE X EKSENI VE Y EKSENI
OLCUMU ANALOG OLARAK YAPILIP SERI PORT UZERINDEN 
GONDERILMEKTEDIR. 

STM32F103C8 ADC Register'i ile ADC Sample Time 1.5 us
olarak ayarlanmistir. Her 1.5 us de analog giris sinyali 
örnekleniyor, 12-Bit'lik cozunurlukte bir donusum yapmakta ve 
ADC-DR register'ina veriyi yazmaktadir.

Analog 0 kanali A0  ve Analog 1 kanali A1 GPIO Portundan okunmaktadir.
Okunan veri x_ekseni, y_ekseni degiskenlerine aktarilmaktadir.


*/
#include "stm32f10x.h"                  // Device header
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
void clock(void);
void serial_print(char *metin);
void serial_println(char *metin);
char serial_read(void);
void delay(unsigned int time);
char pt[40];
uint16_t analog_read(int kanal);
int main(){
	clock();
	float x_ekseni,y_ekseni;
	int i;
	while(1){
		y_ekseni=(1994-analog_read(0))/110.0;
		x_ekseni=(2062-analog_read(1))/110.0;	
		sprintf(pt, "X= %1.5f \tY= %1.5f", x_ekseni, y_ekseni);
		serial_println(pt);
		delay(5);
	}
}

void clock(void){
	RCC->CR|=RCC_CR_HSEON;
	while(!(RCC->CR & RCC_CR_HSERDY));
	RCC->CIR|=RCC_CIR_HSERDYC;
	RCC->CR|=RCC_CR_CSSON;
	RCC->CIR|=RCC_CIR_CSSC;
	
	RCC->CFGR|=RCC_CFGR_PLLMULL9;
	RCC->CFGR|=RCC_CFGR_PPRE2_DIV2|RCC_CFGR_PPRE1_DIV4|
						 RCC_CFGR_ADCPRE_DIV6|RCC_CFGR_PLLSRC|RCC_CFGR_HPRE_DIV1|RCC_CFGR_USBPRE;
	RCC->CR|=RCC_CR_PLLON;
	while(!(RCC->CR & RCC_CR_PLLRDY));
	RCC->CFGR|=RCC_CFGR_SWS_PLL;
	SystemCoreClockUpdate();
	SysTick->CTRL = SysTick_CTRL_ENABLE;
	RCC->APB2ENR|=RCC_APB2ENR_ADC1EN;
	RCC->APB2ENR|=RCC_APB2ENR_IOPAEN;
	RCC->APB2ENR|=RCC_APB2ENR_IOPCEN;
	RCC->APB2ENR|=RCC_APB2ENR_AFIOEN;
	GPIOC->CRH=0x200000;
	GPIOA->CRH= 0x000004B0;
	RCC->APB2ENR|=RCC_APB2ENR_USART1EN;
	
	USART1->BRR=0xEA6;
	USART1->CR1|=USART_CR1_UE|USART_CR1_TE|USART_CR1_RE;
	ADC1->CR2 = ADC_CR2_ADON;
	ADC1->CR2|=ADC_CR2_TSVREFE;
	ADC1->CR2 |= ADC_CR2_CAL;
	while(ADC1->CR2 & ADC_CR2_CAL);
}

void serial_print(char *metin){
	uint32_t sayi = 0;
	while (metin[sayi])
	{
		while(!(USART1->SR & USART_SR_TXE));
		USART1->DR = metin[sayi];
		sayi++;
	}
}
void serial_println(char *metin)
{
	uint32_t sayi = 0;
	while (metin[sayi])
	{
		while(!(USART1->SR & USART_SR_TXE));
		USART1->DR = metin[sayi];
		sayi++;
	}
	while(!(USART1->SR & USART_SR_TXE));
	USART1->DR = '\n';
}

char serial_read(void)
{
	while(!(USART1->SR & USART_SR_RXNE));
	return USART1->DR;
}

void delay(unsigned int time)
{
	SysTick->LOAD = time;

	SysTick->VAL = 0;
	
	while(!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk));
}

uint16_t analog_read(int kanal){
	int sayac=0;
	int deger=0;
	ADC1->SQR1=ADC_SQR1_L;
	ADC1->SQR3=kanal;
	ADC1->CR2 |= ADC_CR2_ADON;
	while((ADC1->SR & ADC_SR_EOC) == 0);
	for(sayac=0;sayac<32;sayac++){
			deger+=ADC1->DR;
	}
	deger=(deger>>5);
	return deger;
}

