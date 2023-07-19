/*
===============================================================================
 Nombre        : Marclé, Emiliano
 Carrera	   : Ing. Electronica
 Descripcion   : Escribir el código que configure el timer0 para cumplir con las
 	 	 	 	 especificaciones dadas en la figura adjunta. (Pag 510 Figura 115
 	 	 	 	 del manual de usuario del LPC 1769).
 	 	 	 	 Considerar una frecuencia de cclk de 100 Mhz y una división de
 	 	 	 	 reloj de periférico de 2.
===============================================================================
*/

#include "LPC17xx.h"
/* -------- Prototipos ---------*/
void confTimer(void);

/* ---- Programa Principal ---- */
int main(void) {
	confTimer(); // Configuración de TIM0.

	while(1){
	}

	return 0;
}

void confTimer(void){
	LPC_SC->PCONP |= (1<<0);		// Enciende TIM0
	LPC_SC->PCLKSEL0 &= ~(1<<2);	// PCLK = CCLK/2
	LPC_SC->PCLKSEL0 |= (1<<3);
	LPC_TIM0->PR = 2;
	LPC_TIM0->MR0 = 6;
	LPC_TIM0->MCR |= 3;				// Habilita Int. y Reset on Match.
	LPC_TIM0->TCR |= 3;				// Habilitación y reset de TC y PC
	LPC_TIM0->TCR &= ~(1<<1);		// Saca de reset a TC y PC.
	NVIC_EnableIRQ(TIMER0_IRQn);	// Habilita int. en NVIC.
	return;
}

void TIMER0_IRQHandler(void){
	LPC_TIM0->IR |= 1;	// Limpia bandera de interrupcion por match con MR0.
	return;
}

