/*
ALIHAN SEKER TARAFINDAN YAZILMIS BIR UYGULAMADIR
STM32F103C8T6 MIKRODENETLEYICISI ILE USART HABERLESME
PERIPHERAL'I KULLANILARAK BIR UART(ASYNCHRONOUS MOD) CALISMASI 
YAPILMISTIR.
*/

#include "stm32f10x.h"                  // Device header
#include "stdio.h"

void clock_init(void);
void char_send(char karakter);
void string_send(char *metin);
char c_read(void);
char data;
int main(){
	clock_init();
	while(1){
		data=c_read();
		if(data=='1'){
			GPIOC->BSRR=(1<<29);
			string_send("LED KAPALI");

		}
		if(data=='0'){
			GPIOC->BSRR=(1<<13);
			string_send("LED AÇIK");
		}
	}
}

void clock_init(void){
	RCC->CR|=RCC_CR_HSEON;//High Speed External Osilatoru aktif hale getir
	while(!(RCC->CR&RCC_CR_HSERDY));//HSE stabil olana kadar bekle
	RCC->CIR|=RCC_CIR_HSERDYC;//HSE Interrupt Clean (HSE Interrupt'ini kapat)
	RCC->CR|=RCC_CR_CSSON;//Clock Security System Aktif Hale getirildi
	RCC->CIR|=RCC_CIR_CSSC;//CSS Interrupt Clear

	RCC->CFGR|=RCC_CFGR_PLLMULL9;//System Clock 9 ile çarpildi, 8MHz HSE System Clock*PLL MULL Degeri, 8*9=72 MHz
	/*
	XTAL FREQUENCY=8MHz
	PLL MULTIPLICATION = 9
	APB2 DIV=PLL/1
	APB1 DIV=PLL/4
	ADC DIV=PLL/2
	PLLSRC=HSE(8 MHz Crsytal)
	AHB DIV=HSE/1
	USB DIV=HSE/1
	Peripheral Clock Ayarlari, Bu ayarlamalar sayesinde hangi BUS hangi CLOCK frekansinda calisacak
	onun ayarlamasi yapiliyor. Bu mikrodenetleyicinin haberlesme BUS'larini ve veri BUS'larini
	kullanabilmesi icin gerekli bir ayarlama.
	*/
	RCC->CFGR|= RCC_CFGR_PPRE2_DIV2|RCC_CFGR_PPRE1_DIV4|RCC_CFGR_ADCPRE_DIV2|
							RCC_CFGR_PLLSRC|RCC_CFGR_HPRE_DIV1|RCC_CFGR_USBPRE;
							
	RCC->CR|=RCC_CR_PLLON;//PLL Aktif Hale getirildi
	while(!(RCC->CR&=RCC_CR_PLLRDY));//PLL Clock stabil olana kadar bekle
	RCC->CFGR |=RCC_CFGR_SW_PLL;//System Clock olarak PLL olarak ayarlandi
	SystemCoreClockUpdate();//Clock Update yaparak System Clock'u 72MHz'e yükseltildi.
	/*GPIO A ve GPIO B BUS'larina Clock Sinyali gitmesi icin APB2EN Register'inda bunlari aktif hale getirilmeli
	BAUDRATE=9600 olarak ayarlandi.
	BRR Register'i Baudrate ayarinin yapildigi Register
											Fck
	TX/RX Baud=-------------------------- Hesaplamasi ile Baudrate icin gerekli hesaplamalar yapilabilir.
									(16*USARTDIV)
	Fck->BUS Clock Frekansi, USART1 kullanildigi icin APB2 Bus Clock Frekansi =36MHz
	*/
	RCC->APB2ENR|=RCC_APB2ENR_IOPAEN|RCC_APB2ENR_IOPCEN;
	GPIOC->CRH=0x200000;//
	GPIOA->CRH= 0x000004B0;
	RCC->APB2ENR|=RCC_APB2ENR_USART1EN;//USART1 CLOCK Aktif Hale getirildi
	
	USART1->BRR=0xEA6;//BRR Baudrate Hesaplamasinda kullanilacak, gerekli bilgi Reference Manual'da bulunabilir.

	USART1->CR1|=USART_CR1_UE|USART_CR1_TE|USART_CR1_RE;//USART1 Aktif hale getirildi, TX aktif hale getirildi, RX Aktif hale getirildi.
}

void char_send(char karakter){
	while(!(USART1->SR & USART_SR_TXE));//TX Müsait oldugu zaman veriyi Gonder
	USART1->DR = karakter;//DR Register'i gonderilecek datanin tutuldugu Register
}

void string_send(char *metin){
	uint32_t sayi = 0;
	while (metin[sayi])
	{
		while(!(USART1->SR & USART_SR_TXE));
		USART1->DR = metin[sayi];
		sayi++;
	}
}

char c_read(void){
	while(!(USART1->SR & USART_SR_RXNE));
	return USART1->DR;
}

