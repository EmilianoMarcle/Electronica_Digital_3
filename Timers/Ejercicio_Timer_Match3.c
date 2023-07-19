/*
===============================================================================
 Name        : Ejercicio_Timer_Match3.c
 Author      : Emiliano Marclé
 Version     : 1.0
 Description : Escribir un programa para que por cada presión de un pulsador,
 	 	 	   la frecuencia de parpadeo disminuya a la mitad debido a la
 	 	 	   modificación del registro del Match 0. El pulsador debe producir
 	 	 	   una interrupción por EINT2 con flanco descendente.
===============================================================================
*/
#include "LPC17xx.h"
/* Macros */
#define MR_VAL  (uint32_t) 25000000
#define PR_VAL  (uint32_t) 1

/* Prototipos de funciones */
void confIntExt(void);
void confTimer(void);

/* Programa principal */
int main(void) {

	confIntExt();		// Configuración Int. por EINT2
	confTimer();		// Configuración TIM0

    while(1) {
    }
    return 0 ;
}

/* Función de configuración de EINT2*/
void confIntExt(void){
	LPC_PINCON->PINSEL4   |= (1<<24);	// EINT2 en P2.12
	LPC_SC->EXTMODE   	  |= (1<<2);    // Interrupción por flanco descendente.
	LPC_SC->EXTPOLAR 	  &= ~(1<<2);
	LPC_SC->EXTINT	      |=(1<<2);     // Limpia flag de Int. EINT2.
	NVIC_EnableIRQ(EINT2_IRQn);		// Habilita Int. en NVIC.
	return;
}

/* Función de configuración TIM3 con MR0
 * Se utiliza un pin externo de Match para conectar el led.
 * Se ajusta el Timer para un tiempo de 250ms con frecuencia de 100MHz
 * - PR=0 -> MR+1 = (250ms * 100MHz) = 25000000
 */
void confTimer(void){
	LPC_PINCON->PINSEL0 |= (3<<20);		  	// P0.10 como MAT3.0
	LPC_SC->PCONP       |= (1<<23);		  	// Enciende TIM3
	LPC_SC->PCLKSEL1    |= (1<<14);		  	// PCLK=CCLK=100MHz
	LPC_TIM3->EMR       |= (3<<4);			// Toggle en Match0
	LPC_TIM3->PR        = (PR_VAL-1);		// Carga valor de PR
	LPC_TIM3->MR0       = (MR_VAL-1);		// Carga valor para contar 250ms SI PR=0
	LPC_TIM3->MCR       &= ~(1<<0);			// Desactiva int. en MR0
	LPC_TIM3->MCR       |= (1<<1);			// Configura Reset en MR0.
	LPC_TIM3->TCR       |= 3;				// Habilita y resetea TC y PC
	LPC_TIM3->TCR		&= ~(1<<1);			// Saca del reset al TC y PC
	return;
}

/* Handler de EINT2
 * Cada vez que ingresa al handler, se duplica el valor de (MR+1) y se carga el MR0
 */
void EINT2_IRQHandler(void){
	static uint32_t match = MR_VAL;
	LPC_SC->EXTINT     |= (1<<2);			// Limpia bandera Int. EINT2
	LPC_TIM3->TCR      &= ~(1<<0);			// Deshabilita TC y PC
	match = 2*match;					// Calculo del match.
	LPC_TIM3->MR0 = (match - 1);		// Carga del MR0
	LPC_TIM3->TCR 		|= 3;			// Habilitacion y reset de TC y PC.
	LPC_TIM3->TCR 		&= ~(1<<1);		// Saca del reset a TC y PC.
	return;
}
