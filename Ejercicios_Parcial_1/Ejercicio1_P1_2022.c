/*
===============================================================================
 Name        : Ejercicio1_P1_2022.c
 Author      : $Marclé, Emiliano
 Version     : 1.0
 Description : Realizar un programa que guarde mediante interrupción el dato presente
 en los pines de entrada P2.0 al P2.7 cada vez que se cambie su valor.
 Cada dato nuevo debe guardarse de forma consecutiva en una región de memoria que permita
 tener disponible siempre de los últimos 16 datos guardados.
===============================================================================
*/

#include "LPC17xx.h"

/* ---- Macros ---- */
#define SIZE (uint8_t) 16  // Tamaño del buffer

/* ---- Variables globales ---- */
uint8_t buffer[SIZE];
uint8_t dataFlag = 0;

/* ---- Prototipos de funciones ---- */
void confGPIO(void);
void confGPIOInt(void);

/* ---- Programa principal ---- */
int main(void) {

	confGPIO();
	confGPIOInt();

	/* Limpia el buffer */
	for(uint8_t i=0 ; i<(SIZE-1); i++){
		buffer[i]=0;
	}

    while(1) {

    	if(dataFlag){

    		/* Desplaza datos hacia abajo */
    		for(uint8_t j=SIZE-1 ; j>0 ; j--){
    			buffer[j]=buffer[j-1];
    		}
    		/* Guarda dato nuevo en la primer posición.*/
    		buffer[0] = LPC_GPIO2->FIOPIN0;
    		dataFlag = 0;
    	}

    }
    return 0 ;
}

/* ---- Función de configuración de GPIO ---- */
void confGPIO(void){
	LPC_GPIO2->FIODIR0 &= ~(0xFF); // P2.0 a P2.7 como salida
	LPC_GPIO2->FIOMASK |= ~(0xFF); // Enmascara los otros pines del puerto 2
	return;
}

/* ---- Función de configuración de int por GPIO ---- */
void confGPIOInt(void){
	/* Habilita int. por flanco de subida y bajada en pines P2.0 a P2.7 */
	LPC_GPIOINT->IO2IntEnF |= (0xFF);
	LPC_GPIOINT->IO2IntEnR |= (0xFF);
	LPC_GPIOINT->IO2IntClr |= (0xFF);

	NVIC_EnableIRQ(EINT3_IRQn);
	return;
}




/* ---- Handler EINT3 ---- */
void EINT3_IRQHandler(void){

	if((LPC_GPIOINT->IO2IntStatF & 0xFF) || (LPC_GPIOINT->IO2IntStatR & 0xFF)){
		LPC_GPIOINT->IO2IntClr |= 0xFF;
		dataFlag = 1;
	}

	return;

}
