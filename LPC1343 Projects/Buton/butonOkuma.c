#include "LPC13xx.h"                    // Device header

void osc_init(void){
	LPC_SYSCON->SYSOSCCTRL = 0x00; // krsital baglama araligi 1-20MHz + Osilat�r pllye g�nderilmez.
 	LPC_SYSCON->PDRUNCFG &=~(1<<5); // sistem osilat�r�n� a�
	LPC_SYSCON->MAINCLKSEL = 1; // main clock = sistem osilat�r�
	LPC_SYSCON->MAINCLKUEN = 0;	// main clock se�me
	LPC_SYSCON->MAINCLKUEN = 1;	// g�ncellemesi
}
int durum;

int main(void)
{
	osc_init();
	LPC_GPIO1->DIR &= ~((1<<8)) ; //config PIO1_5 as Input
	LPC_GPIO2->DIR |= (1<<7); //config PIO0_7 as Output

	LPC_GPIO2->DATA &= ~(1<<7); //drive PIO0_7 LOW initially

	while(1)
	{
		durum=LPC_GPIO1->DATA & (1<<8);
		if(durum) 
		{
			LPC_GPIO2->DATA |= (1<<7); 
		
		}
		if(!durum){
			LPC_GPIO2 ->DATA &= ~(1<<7);
		}
	}

}