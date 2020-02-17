/*****************************************************************\
LPC13XX UART HABERLESME YAZILIMI

16.04.2019 SALI 

ALIHAN SEKER 

BAUDRATE:115.200 
8-BIT, 1 STOP BIT, NO-PARITY BIT

\*****************************************************************/

#include "LPC13xx.h"                    // Device header
#include "string.h"
void UartInit(void);
void SerialPrint(char karakter);
void delay(uint32_t sure);
char karakter[12] ="Alihan Seker";
int i;
int main(){
	SystemCoreClockUpdate();  //72MHZ OLARAK ISLEMCI HIZI AYARLANDI
	LPC_SYSCON->SYSAHBCLKCTRL |= ( 1<<16); //IOCON ICIN SYSAHBCLKCTRL REGISTERI 16.BIT AKTIF EDILDI
	UartInit();
	while(1){
		serial_string("Merhaba, Dunya");
		delay(1000);

	}
}
void UartInit(void){
	LPC_IOCON->PIO1_6 = 0x1; //PIO1-6 PINI UART RX OLARAK AYARLANDI
	LPC_IOCON->PIO1_7 = 0x1; //PIO1-7 PINI UART TX OLARAK AYARLANDI
	LPC_SYSCON->SYSAHBCLKCTRL |= (1<<12); //SYSAHBCLKCTRL 12.BITI UARTCLK, UARTCLK IZIN VERILDI
	LPC_SYSCON->UARTCLKDIV = 0x6; //UARTCLKDIV, CCLK/6=12MHz CCLK=72MHZ  
	LPC_UART->LCR = 0x83; //8-BIT, 1 STOP BIT, NO PARITY, DLAB=1, DLAB BAUDRATE AYARI ICIN GEREKLI
	LPC_UART->DLM = 0;    //DLM=0X00 OLARAK AYARLANDI
	LPC_UART->DLL = 0x4;  //DLL=0X4 OLARAK AYARLANDI, BAUDRATE ICIN GEREKLI DLM VE DLL
	LPC_UART->FDR =0x85;  //FDR BITI DIVADDVAL VE MULVAL BAUDRATE ICIN GEREKLI, BAUDRATE=115.200 OLARAK AYARLANDI
	LPC_UART->LCR = 0x3;  //BAUDRATE SABIT TUTULDU, DLAB=0 YAPILDI
	LPC_UART->FCR = 0x7;	//FIFO CONTROL REGISTER, FIFO RX VE TX PINLERI ICIN AKTIF 
}

void SerialPrint(char karakter){
	while(!(LPC_UART->LSR & 0x20)); // LINE STATUS REGISTER, THRE = 0 OLANA KADAR GONDERIYOR.
	LPC_UART->THR = karakter;   //THR, TRANMISTTER REGISTER, GONDERILECEK KARAKTER
}
void serial_string(char* data)
{	
	int sayi;
	while (data[sayi])
	{
		SerialPrint(karakter[i]);
		sayi++;
	}
}
void delay(uint32_t sure){
	uint32_t i;
	for(i=0;i<18000*sure;i++);
}