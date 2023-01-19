/*
 * Estudiante: Marclé Emiliano
 * Carrera: Ingenieria Electronica
 * Descripcion:
 * Este programa prende y apaga un led del pin P0.22 de forma intermitente
 * Ademas el tiempo de retardo es controlado por una señal digital proveniente del
 * pin P2.10 configurado como entrada con resistencia de pull-down.
*/

#include "LPC17xx.h"

/* Prototipos de las funciones */
void retardo(uint32_t tiempo);

/* Programa principal */

int main(void){
	uint32_t relojCPU= SystemCoreClock;

	uint32_t tiempo;
	LPC_PINCON->PINSEL1 &= ~(3<<12);  //Configura P0.22 como GPIO.
	LPC_PINCON->PINSEL4 &= ~(3<<20); //Configura P2.10 como GPIO.
 	LPC_GPIO0->FIODIR   |= (1<<22);  //Configura el pin P0.22 como salida
	LPC_GPIO2->FIODIR   &= ~(1<<10); //Configura el pin P2.10 como entrada
	LPC_PINCON->PINMODE4|= (3<<20);  //Asocia resistencia de Pull-Down al pin P2.10

	while(1){
		if ((LPC_GPIO2->FIOPIN)&(1<<10)){
			tiempo = 1000000;
		}
		else{
			tiempo = 4000000;
		}
		LPC_GPIO0->FIOCLR |= (1<<22); //Enciende el led
		retardo(tiempo);
		LPC_GPIO0->FIOSET |= (1<<22); //Apaga el led
		retardo(tiempo);
	}
	return 0;
}

void retardo(uint32_t tiempo){
	uint32_t contador;
	for(contador=0; contador<tiempo; contador++){};
}
