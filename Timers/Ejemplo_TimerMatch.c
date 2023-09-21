/*
===============================================================================
 Nombre      : Marclé, Emiliano
 Descripcion : En este programa se configura el TIM0 para togglear una salida
 	 	 	   cada vez que se produce un match en MAT0.0.
 	 	 	   No se genera interrupción cuando se produce el match.
 	 	 	   Para comprobar el funcionamiento se conecta un led en P1.28
===============================================================================
*/
#include "LPC17xx.h"
/* Macros */
#define MR_VAL ((uint32_t) 70000000)  // Valor de Match. T=(1/100MHz)*(PR+1)*(MR+1)=0,7seg.
/* Prototipos de funciones */
void confGPIO(void);
void confTimer(void);

/* Programa principal */
int main(void) {
	confTimer();	// Configuración del TIM0.

	while(1){
    }

    return 0 ;
}

/* Configuración TIM0 */
void confTimer(){
	LPC_SC->PCONP  		|= (1<<1);  // PCTIM0 = 1. Activa TIM0.
	LPC_SC->PCLKSEL0    |= (1<<2);  // Clock=CCLK. Configuro divisor frecuencia.
	LPC_PINCON->PINSEL3 |= (3<<24); // Selecciona funcion MAT0.0 en P1.28
	LPC_TIM0->EMR       |= (3<<4);  // Toggle en cada match de MAT0.0
	LPC_TIM0->MR0        = (MR_VAL-1);
	LPC_TIM0->MCR       |= (1<<1);  // Reset en MR0.
	LPC_TIM0->MCR       &=~(1<<0);  // Deshabilita interrupcion en Match
	LPC_TIM0->TCR		 = 3; 		// Habilita el contador, y pongo en reset el TC y PC.
	LPC_TIM0->TCR		&=~(1<<1);  // Saca del reset al TC y PC.
	return;
}
