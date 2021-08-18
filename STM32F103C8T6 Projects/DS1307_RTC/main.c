#include "stm32f10x.h"                  // Device header
#include "stdio.h"


#define  tmYearToCalendar(Y) ((Y) + 1970)  // full four digit year 
#define  CalendarYrToTm(Y)   ((Y) - 1970)
#define  tmYearToY2k(Y)      ((Y) - 30)    // offset is from 2000
#define  y2kYearToTm(Y)      ((Y) + 30)   

#define		SECONDS_ADDR	0x00
#define		MINUTES_ADDR	0x01
#define 	HOUR_ADDR			0x02
#define		DAY_ADDR			0x03
#define		DATE_ADDR			0x04
#define		MONTH_ADDR		0x05
#define		YEAR_ADDR			0x06

typedef struct  { 
  uint8_t Second; 
  uint8_t Minute; 
  uint8_t Hour; 
  uint8_t Wday;   // day of week, sunday is day 1
  uint8_t Day;
  uint8_t Month; 
  uint8_t Year;   // offset from 1970; 
} 	tmElements_t, TimeElements, *tmElementsPtr_t;

tmElements_t tm;

void clockSettings(void);
void putC(uint8_t data);
void printString(char *data);
void DS1307_Write(uint8_t wordAddr, uint8_t data);
uint8_t DS1307_Read(uint8_t wordAddr);
uint8_t dec2bcd(uint8_t num);
uint8_t bcd2dec(uint8_t num);
void  send_char(uint8_t data);
uint8_t data;
void SetTime(void);
void ReadTime(void);
int main(){
	clockSettings();

	char data[50];
	
	tm.Minute=59;
	tm.Hour=16; 
	tm.Wday=3;  
	tm.Day=7;
	tm.Month=7; 
	tm.Year=21;  
	
	//SetTime();
	
	while(1){
		ReadTime();
		sprintf(data, "%.2d:%.2d:%.2d\t%.2d.%.2d.%.2d\n", tm.Hour, tm.Minute, tm.Second, tm.Day, tm.Month, tmYearToCalendar(tm.Year));
		printString(data);
	}
}

void clockSettings(void){	
	
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

	SystemCoreClockUpdate();
	
	RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
	RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
	RCC->APB2ENR|=RCC_APB2ENR_IOPAEN;//IOPAEN Resgiter is set.
	RCC->APB2ENR|=RCC_APB2ENR_USART1EN;//USART1 CLOCK Aktif Hale getirildi
	
	GPIOA->CRH 	|= 	(1<<4) | (1<<5);//PA9 OUTPUT MAX 50MHZ
	GPIOA->CRH	&=	~(1<<6);
	GPIOA->CRH	|=	(1<<7);	//[7:6] => 1 0 Alternate OUTPUT
	
	GPIOA->CRH	&= ~((1<<8) | (1<<9));//PA10 INPUT 
	GPIOA->CRH	|=	(1<<10);
	GPIOA->CRH	&=	~(1<<11); //[11:10]=> 0 1 FLOATING INPUT 
	
	
	USART1->BRR=0xEA6;//BRR Baudrate Hesaplamasinda kullanilacak, gerekli bilgi Reference Manual'da bulunabilir.

	USART1->CR1|=USART_CR1_UE|USART_CR1_TE|USART_CR1_RE;//USART1 Aktif hale getirildi, TX aktif hale getirildi, RX Aktif hale getirildi.
	
	GPIOB->CRL |= ((1<<24) | (1<<25));
	GPIOB->CRL |= ((1<<26) | (1<<27));
	
	GPIOB->CRL |= ((1<<28) | (1<<29));
	GPIOB->CRL |= ((1<<30) | (1<<31));
	
	GPIOB->CRL |= GPIO_CRL_MODE5;
	GPIOB->CRL &= ~GPIO_CRL_CNF5;
	
	I2C1->CR2 |= (36<<0);
	I2C1->CCR = (0xB4<<0);
	I2C1->TRISE = 0x25;
	I2C1->CR1 |= I2C_CR1_PE;
	
}

void putC(uint8_t data){
	USART1->DR = data;//DR Register'i gonderilecek datanin tutuldugu Register
	while(!(USART1->SR & USART_SR_TXE));//TX Müsait oldugu zaman veriyi Gonder
	while(!(USART1->SR & USART_SR_TC));
}
void printString(char *data){
	int i=0;

	while (data[i])
	{
		while(!(USART1->SR & USART_SR_TXE));
		USART1->DR = data[i];
		i++;
	}

}

void DS1307_Write(uint8_t wordAddr, uint8_t data){
	I2C1->CR1 |= I2C_CR1_START;
	I2C1->CR1 |= I2C_CR1_ACK;
	while(!(I2C1->SR1 & I2C_SR1_SB));
	uint8_t temp=I2C1->SR1;
	
	I2C1->DR = 0x68<<1 | 0x00;
	while(!(I2C1->SR1 & I2C_SR1_ADDR));
	temp= I2C1->SR1 | I2C1->SR2;
	
	while(!(I2C1->SR1 & I2C_SR1_TXE));
	I2C1->DR = wordAddr;
	while(!(I2C1->SR1 & I2C_SR1_TXE));
	
	while(!(I2C1->SR1 & I2C_SR1_TXE));
	I2C1->DR = data;
	while(!(I2C1->SR1 & I2C_SR1_TXE));
	while(!(I2C1->SR1 & I2C_SR1_BTF));
	I2C1->CR1 |= I2C_CR1_STOP;
	
	
}

uint8_t DS1307_Read(uint8_t wordAddr){
	uint8_t data;
	I2C1->CR1 |= I2C_CR1_START;
	I2C1->CR1 |= I2C_CR1_ACK;	
	while(!(I2C1->SR1 & I2C_SR1_SB));
	uint8_t temp=I2C1->SR1;
	
	I2C1->DR = 0x68<<1;
	while(!(I2C1->SR1 & I2C_SR1_ADDR));
	temp= I2C1->SR1 | I2C1->SR2;
	
	while(!(I2C1->SR1 & I2C_SR1_TXE));
	I2C1->DR = wordAddr;
	while(!(I2C1->SR1 & I2C_SR1_TXE));

	I2C1->CR1 |= I2C_CR1_START;
	while(!(I2C1->SR1 & I2C_SR1_SB));
	temp=I2C1->SR1;
	
	I2C1->DR = 0x68<<1 | (0x01);
	while(!(I2C1->SR1 & I2C_SR1_ADDR));
	I2C1->CR1 &= ~I2C_CR1_ACK;	
	temp= I2C1->SR1 | I2C1->SR2;
	I2C1->CR1 |= I2C_CR1_STOP;
	
	while(!(I2C1->SR1 & I2C_SR1_RXNE));
	data= I2C1->DR;
	I2C1->CR1 |= I2C_CR1_STOP;
	return data;
	
	
}

uint8_t dec2bcd(uint8_t num)
{
    unsigned int ones = 0;
    unsigned int tens = 0;
    unsigned int temp = 0;

    ones = num%10; // 65535%10 = 5
    temp = num/10; // 65535/10 = 6553
    tens = temp<<4;  // what's displayed is by tens is actually the lower
                     // 4 bits of tens, so tens is 6553%16=9
    return (tens + ones);// so the result is 95
}

// Convert Binary Coded Decimal (BCD) to Decimal
uint8_t bcd2dec(uint8_t num)
{
  return ((num/16 * 10) + (num % 16));
}

void ReadTime(void){
	tm.Second=DS1307_Read(SECONDS_ADDR);
	tm.Second=bcd2dec(tm.Second);

	tm.Minute=DS1307_Read(MINUTES_ADDR);
	tm.Minute=bcd2dec(tm.Minute);

	tm.Hour=DS1307_Read(HOUR_ADDR);
	tm.Hour=bcd2dec(tm.Hour);

	tm.Wday=DS1307_Read(DATE_ADDR);
	tm.Wday=bcd2dec(tm.Wday);

	tm.Day=DS1307_Read(DAY_ADDR);
	tm.Day=bcd2dec(tm.Day);

	tm.Month=DS1307_Read(MONTH_ADDR);
	tm.Month=bcd2dec(tm.Month);

	tm.Year=DS1307_Read(YEAR_ADDR);
	tm.Year=bcd2dec(tm.Year);
	
}


void SetTime(void){
	DS1307_Write(SECONDS_ADDR, dec2bcd(tm.Second));
	DS1307_Write(MINUTES_ADDR, dec2bcd(tm.Minute));
	DS1307_Write(HOUR_ADDR, dec2bcd(tm.Hour));
	DS1307_Write(DATE_ADDR, dec2bcd(tm.Wday));
	DS1307_Write(DAY_ADDR, dec2bcd(tm.Day));
	DS1307_Write(MONTH_ADDR, dec2bcd(tm.Month));
	tm.Year=y2kYearToTm(tm.Year);
	DS1307_Write(YEAR_ADDR,dec2bcd(tm.Year));
}

