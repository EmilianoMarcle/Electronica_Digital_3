/*
===============================================================================
 Name        : Ejercicio2_P1_2022.c
 Author      : $Marclé, Emiliano
 Version     : 1.0
 Description : Utilizando interrupciones por SysTick y por eventos externos EINT,
 realizar un programa que permita habilitar y deshabilitar el temporizador por flanco ascendente en el pin P2.11
 El temporizador debe desbordar cada 10 ms utilizando un clock de cclk=62MHz.
 Por cada interrupción del SysTick, se debe mostrar por el puerto P0 el promedio de los valores guardados en uint8_t values[8].
 Detallar cálculos para obtener el valor de RELOAD y asegurar que la interrupción por SysTick sea de mayor prioridad que EINT.

===============================================================================
*/

#include "LPC17xx.h"

/* ---- Macros ---- */
//#define TICKS ((uint32_t) 620000) // Valor de RELOAD+1
#define TICKS ((uint32_t) 62000000) // Valor de RELOAD+1
#define SIZE  ((uint8_t)        8 ) // Cantidad de numeros a promediar

/* ---- Variables globales ---- */
uint8_t values[8]={8,8,8,8,8,8,8,8};
uint8_t timerFlag = 0;    // Flag para activar/desactivar el SysTick
uint8_t promedioFlag = 0; // Flag para calcular promedio y sacar por P0 el valor.
uint8_t promedio;
uint16_t suma;

/* ---- Prototipos de funciones ---- */
void confGPIO(void);
void confIntExt(void);


/* ---- Programa principal ---- */
int main(void) {

	confGPIO();  // Configuración de GPIO
	confIntExt(); // Configuración de EINT1

    while(1) {

    	if(promedioFlag){
    		suma = 0;
    		promedio = 0;
    		for(uint32_t i=0 ; i<SIZE ; i++){
    			suma += values[i];
    		}

    		promedio = (suma/SIZE);
    		LPC_GPIO0->FIOPIN0 = promedio & 0xFF;
    		promedioFlag = 0;
    	}

    }
    return 0 ;
}

/* ---- Función de configuración de GPIO ---- */
void confGPIO(void){
	LPC_PINCON->PINSEL0 &= ~(0xFFFF);  // P0.0 a P0.7 como GPIO
	LPC_PINCON->PINMODE0 &= ~(0xFFFF); // pull-ups.
	LPC_GPIO0->FIODIR0 |= (0xFF); // P0.0 a P0.7 como salida.
	LPC_GPIO0->FIOCLR0 |= (0xFF);
	LPC_GPIO0->FIOMASK |= ~(0xFF); // Enmascara los otros pines del Puerto 0.

	return;
}

/* ---- Función de configuración de EINT1 ---- */
void confIntExt(void){
	LPC_PINCON->PINSEL4 |= (1<<22); // P2.11 como EINT1
	LPC_SC->EXTMODE |= (1<<1);  // Int. por flanco de subida.
	LPC_SC->EXTPOLAR |= (1<<1);
	LPC_SC->EXTINT |= (1<<1);

	NVIC_SetPriority(EINT1_IRQn, 5);
	NVIC_EnableIRQ(EINT1_IRQn);

	return;
}

/* ---- Función de configuración de SysTick ---- */
void confSysTick(uint32_t ticks){
	SysTick->CTRL=0;
	SysTick->LOAD = (ticks-1);
	SysTick->VAL = 0;
	SysTick->CTRL |= (1<<2)|(1<<1)|(1<<0);

	NVIC_SetPriority(SysTick_IRQn, 0);
	return;
}

/* ---- Handler EINT1 ---- */
void EINT1_IRQHandler(void){
	LPC_SC->EXTINT |= (1<<1); // Limpia flag EINT1.
	if(timerFlag){
		timerFlag = 0;
		SysTick->CTRL = 0;
		return;
	}

	timerFlag = 1;
	confSysTick(TICKS);

	return;
}

/* ---- Handler SysTick ---- */
void SysTick_Handler(void){
	SysTick->CTRL &= SysTick->CTRL; // Limpia flag SysTick.
	promedioFlag = 1;
	return;
}
