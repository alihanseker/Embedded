#include "stm32f10x.h"                  // Device header
#include "delay.h"

void Init(void);
void servoWrite(int data);
int derece(int deger);
int main(){
	Init();
	DelayInit();
	//DelayMs(1);
	//TIM4->CCER &= ~TIM_CCER_CC1E;
	uint16_t i=0;

	
	while(1){
	for(i=6;i<30;i++){
		//derece=(uint16_t)orantila(i, 0,180,5,25);
		TIM4->CCR1 = i;
		DelayMs(1000);
	}
	}
}

void Init(void){
	RCC->CR |= (1<<16);// HSEON BIT, 8MHZ HSE Crystal Enabled 
	while(!(RCC->CR & (1<<17)));//Wait until HSE Clock Source is ready for use 
	RCC->CR |= (1<<19);//Clock Security System Enabled 
	RCC->CFGR |=(1<<16);//HSE Crystal is selected for PLL Clock Source
	
	//PLL Clock Multiplication Factor is 9 so Main Clock equals to 72MHz 
	RCC->CFGR |=((1<<18) | (1<<19) | (1<<20)); 
	RCC->CFGR &= (unsigned int)~(1<<21);
	
	//AHB Prescaler Value is selected /2
	RCC->CFGR |=(1<<7);
	RCC->CFGR &= (unsigned int)~((1<<6)| (1<<5) |(1<<4));
	
	RCC->CR |= (1<<24);
	while(!(RCC->CR & (1<<25)));
	
	//PLL is switched as SYSTEM CLOCK 
	RCC->CFGR &= (unsigned int)~(1<<0);
	RCC->CFGR |= (1<<1);
	
	//Wait Until System Clock is switched to PLL 
	while(!(RCC->CFGR & (1<<3)));
	
	/*
	TIMx Clock is depent on APB1 prescaler value. It means, if APB1 Prescaler equals to 1 then TIM4 CLK frequency equals PCLK*1.
	But APB1 Prescaler value doesn't equal to 1 then TIM4CLK Frequency equals to PCLK*2.
	TIM4 CLK => 36MHz
  */
	SystemCoreClockUpdate();
	
	
	RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
	RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;
	
	GPIOB->CRL 	|=	GPIO_CRL_MODE6;
	GPIOB->CRL	|=	GPIO_CRL_CNF6_1;
	GPIOB->CRL	&=	~GPIO_CRL_CNF6_0;
	
	GPIOB->CRL 	|=	GPIO_CRL_MODE7;
	GPIOB->CRL	|=	GPIO_CRL_CNF7_1;
	GPIOB->CRL	&=	~GPIO_CRL_CNF7_0;
	
	TIM4->PSC = 3600;	//TIM_CLK is 1kHz
	TIM4->ARR = 200;
	TIM4->CCMR1 |= ((1<<6) | (1<<5));
	//TIM4->CCR1 = 18 ;

	TIM4->CCMR1 |= TIM_CCMR1_OC1PE;

	TIM4->CCER |= TIM_CCER_CC1E;
	TIM4->CR1 |= TIM_CR1_CEN;
	
	
}

