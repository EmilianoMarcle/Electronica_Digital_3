/*
=====================================================================================================
 Name        : GPIO_salida_2.c
 Author      : $Marclé, Emiliano
 Version     : 1.0
 Description : En los pines P2.0 a P2.7 se encuentra conectado un display de 7 segmentos.
 	 	 	   Utilizando la variable:
 	 	 	   numDisplay [10] ={0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x67}
 	 	 	   que codifica los números del 0 a 9 para ser mostrados en el display, realizar un
 	 	 	   programa que muestre indefinidamente la cuenta de 0 a 9 en dicho display.
 	 	 	   El display es de cátodo común.
======================================================================================================
*/

#include "LPC17xx.h"

// Definiciones
#define DIGITOS 			(uint16_t)   10
#define TIEMPO_RETARDO		(uint32_t)   5000000

// Prototipos
void config_GPIO(void);
void retardo(uint32_t tiempo);

// Variables globales
uint16_t numDisplay[DIGITOS] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F};

// Programa principal.
int main(void) {
	uint32_t i;

	config_GPIO();

    while(1) {
    	for(i=0 ; i<DIGITOS ; i++){
    		LPC_GPIO2 -> FIOCLR0 |= 0xFF;
    		LPC_GPIO2 -> FIOPIN0 = numDisplay[i];
			retardo(TIEMPO_RETARDO);
    	}
    }
    return 0 ;
}

void config_GPIO(void){
	LPC_PINCON-> PINSEL4 &= ~(0xFFFF); 	// P2.0 a P2.7 como GPIO.
	LPC_GPIO2 -> FIOMASK0 |= (1<<7);    // Enmascara P2.7
	LPC_GPIO2 -> FIODIR0 |= 0xFF;       // P2.0 a P2.7 como salida.
	LPC_GPIO2 -> FIOCLR0 |= 0xFF;		// Limpia P2.0 a P2.7
	return;
}

void retardo(uint32_t tiempo){
	uint32_t conta;
	for(conta=0 ; conta < tiempo ; conta++ ){
	}
	return;
}
