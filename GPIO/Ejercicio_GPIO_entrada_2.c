/*
=====================================================================================================
 Name        : GPIO_entrada_2.c
 Author      : $Marclé Emiliano
 Version     : 1.0
 Description : Considerando pulsadores normalmente abiertos conectados en un extremo a masa y en el
 	 	 	   otro directamente a las entradas P0.0 y p0.1. Realizar un programa que identifique
 	 	 	   en una variable cual o cuales pulsadores han sido presionados. Las identificaciones
 	 	 	   posibles a implementar en esta variable van a ser "ninguno", "pulsador 1",
 	 	 	   "pulsador 2", "pulsador 1 y 2".
====================================================================================================
*/

#include "LPC17xx.h"
#include "string.h"

void config_GPIO(void);
void retardo(void);

char presionado[15];

int main(void) {
	uint8_t flag0=0, flag1=0, flag2=0;

	config_GPIO();

    while(1) {

    	if(!(LPC_GPIO0->FIOPIN & (1<<0)) && (LPC_GPIO0->FIOPIN & (1<<1)) && !flag0){
    		retardo();
    		strcpy(presionado , "pulsador 1     ");
    		flag0 = 1;
    		flag1 = 0;
    		flag2 = 0;
    	}
    	else if((LPC_GPIO0->FIOPIN & (1<<0)) && !(LPC_GPIO0->FIOPIN & (1<<1)) && !flag1 ){
    		retardo();
    		strcpy(presionado , "pulsador 2     ");
    		flag1 = 1;
    		flag0 = 0;
    		flag2 = 0;
    	}
    	else if(!(LPC_GPIO0->FIOPIN & (1<<0)) && !(LPC_GPIO0->FIOPIN & (1<<1)) && !flag2 ){
    		retardo();
    		strcpy(presionado , "pulsador 1 y 2");
    	    flag2 = 1;
    	    flag0 = 0;
    	    flag1 = 0;
    	}
    	else if(!flag0 && !flag1 && !flag2){
    		retardo();
    		strcpy(presionado , "ninguno");
    	}

    }
    return 0 ;
}

void config_GPIO(void){
	LPC_PINCON->PINSEL0 &= ~(0xF);  // P0.0 y P0.1 como GPIO.
	LPC_PINCON->PINMODE0 &= ~(0xF);	// P0.0 y P0.1 con pull-ups.
	LPC_GPIO0-> FIODIR0 &= ~(3<<0); // P0.0 y P0.1 como entrada.
	return;
}

void retardo(void){
	for(uint32_t i = 0 ; i<10000 ; i++){}
	return;
}
