/*
======================================================================================================
 Name        : GPIO_entrada_1.c
 Author      : $Marclé Emiliano
 Version     : 1.0
 Description : Configurar el pin P0.4 como entrada digital con resistencia de pull down y utilizarlo
 	 	 	   para decidir si el valor representado por los pines P0.0 al P0.3 van a ser sumados o
 	 	 	   restados al valor guardado en la variable "acumulador".El valor inicial de "acumulador"
 	 	 	   es 0.
======================================================================================================
*/

#include "LPC17xx.h"

void config_GPIO(void);

int main(void) {

    uint32_t acumulador = 0; // Variable que acumula valores representados por P0.0 a P0.3
    uint16_t valor;

    config_GPIO();  // Configuracion de GPIO.

    while(1) {
    	valor = (LPC_GPIO0->FIOPIN0 & (0x0F)); // Lectura de P0.0 a P0.3.

        if(LPC_GPIO0->FIOPIN0 & (1<<4)){		// Si esta presionado P0.4 se resta el valor.
        	acumulador -= valor;
        }
        else{
        	acumulador += valor;			    // Si no esta presionado P0.4 se acumula el valor.
        }
    }
    return 0 ;
}

void config_GPIO(void){
	LPC_PINCON-> PINSEL0 &= ~(0x3FF);  				// P0.0 a P0.4 como GPIO.
	LPC_PINCON-> PINMODE0 |= (3<<8);   				// P0.4 con pull-down.
	LPC_GPIO0->FIOMASK0 |= (1<<7)|(1<<6)|(1<<5); 	// P0.5 a P0.7 enmascarados.
	return;
}
