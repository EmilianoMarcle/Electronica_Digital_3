/*
=====================================================================================================
 Name        : GPIO_salida_1.c
 Author      : $Marclé, Emiliano
 Version     : 1.0
 Copyright   : $(copyright)
 Description : Una famosa empresa de calzados a incorporado a sus zapatillas 10 luces leds
 	 	 	   comandadas por un microcontrolador LPC1769 y ha pedido a su grupo de ingenieros que
 	 	 	   diseñen 2 secuencias de luces que cada cierto tiempo se vayan intercalando
 	 	 	  (secuencia A - secuencia B- secuencia A- ... ).
 	 	 	  Como todavía no se ha definido la frecuencia a la cual va a funcionar el CPU del
 	 	 	  microcontrolador, las funciones de retardos que se incorporen deben tener como
 	 	 	  parametros de entrada variables que permitan modificar el tiempo de retardo que se
 	 	 	  vaya a utilizar finalmente.
 	 	 	  Los leds se encuentran conectados en los puertos P0,0 al P0.9.
=====================================================================================================
*/

#include "LPC17xx.h"

// Includes

// Definiciones y declaraciones
#define PASOS_SEC        (uint32_t)  5
#define REPETICIONES_SEC (uint32_t)  25
#define TICKS_RETARDO   (uint32_t)  750000

// Prototipos
void config_GPIO (void);
void retardo(uint32_t ticksRetardo);

// Programa principal
int main(void) {
	uint8_t flagSec = 0;
	uint32_t conta = 0;
	uint16_t secA[PASOS_SEC] = {0b0000000011, 0b0000001100, 0b0000110000, 0b0011000000, 0b1100000000};
	uint16_t secB[PASOS_SEC] = {0b1100000011, 0b0011001100, 0b0000110000, 0b0011001100, 0b1100000011};

	// Configuracion GPIO
	config_GPIO();

    while(1) {
    	switch(flagSec){
    		case 0:
    			for(uint32_t i=0 ; i<PASOS_SEC ; i++){
    				LPC_GPIO0->FIOPINL = secA[i];
    				retardo(TICKS_RETARDO);
    			}
    			break;
    		case 1:
       			for(uint32_t j=0 ; j<PASOS_SEC ; j++){
        			LPC_GPIO0->FIOPINL = secB[j];
        			retardo(TICKS_RETARDO);
        		}
        		break;
    	}

    	conta++;
    	if(conta>REPETICIONES_SEC){
    		conta=0;
    		flagSec ^= 1;
    	}
    }

    return 0 ;
}

void config_GPIO (void){
	LPC_PINCON -> PINSEL0 &= ~(0xFFFFF);
	LPC_GPIO0 -> FIOMASKL |= ~(0x3FF);
	LPC_GPIO0 -> FIOMASKH |= 0xFFFF;
	LPC_GPIO0 -> FIODIRL  |= 0x3FF;
	LPC_GPIO0 -> FIOCLRL |= 0x3FF;
	return;
}

void retardo(uint32_t ticksRetardo){
	for(uint32_t k=0 ; k<ticksRetardo ; k++){
	}
	return;
}
