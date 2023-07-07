/*
=======================================================================================================
 Name        : Int_GPIO_2.c
 Author      : $ Marclé Emiliano
 Version     : 1.0
 Description : Realizar un programa que configure el puerto P0.0 y P2.0 para que provoquen una
 	 	 	   interrupción por flanco de subida.
 	 	 	   Si la interrupción es por P0.0 guardar el valor binario 100111 en la variable "auxiliar",
 	 	 	   si es por P2.0 guardar el valor binario 111001011010110.
=======================================================================================================
*/

#include "LPC17xx.h"

void configGPIO(void);
void configGPIOInt(void);

uint16_t auxiliar;

int main(void) {
	configGPIO();
	configGPIOInt();

    while(1) {
    }
    return 0 ;
}

void configGPIO(void){
	LPC_PINCON->PINSEL0 &= ~(3<<0); // P0.0 y P2.0 como GPIO
	LPC_PINCON->PINSEL4 &= ~(3<<0);
	LPC_GPIO0->FIODIR &= ~(1<<0);	// P0.0 y P2.0 como entradas
	LPC_GPIO2->FIODIR &= ~(1<<0);
	return;
}

void configGPIOInt(void){
	LPC_GPIOINT->IO0IntEnR |= (1<<0);
	LPC_GPIOINT->IO2IntEnR |= (1<<0);
	LPC_GPIOINT->IO0IntClr |= (1<<0);
	LPC_GPIOINT->IO2IntClr |= (1<<0);
	NVIC_EnableIRQ(EINT3_IRQn);
	return;
}

void EINT3_IRQHandler(void){
	if(LPC_GPIOINT->IO0IntStatR & (1<<0)){
		auxiliar = 0b100111;
		LPC_GPIOINT->IO0IntClr |= (1<<0);
	}
	else if(LPC_GPIOINT->IO2IntStatR & (1<<0)){
		auxiliar = 0b111001011010110;
		LPC_GPIOINT->IO2IntClr |= (1<<0);
	}
	return;
}
