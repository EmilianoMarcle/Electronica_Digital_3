/*
===============================================================================
 Name        : Ejercicio_Timer_Match2.c
 Author      : Emiliano Marclé
 Version     :
 Description : Escribir un programa para que por cada presión de un pulsador,
 	 	 	   la frecuencia de parpadeo de un led disminuya a la mitad debido
 	 	 	   a la modificación del pre-escaler del Timer 2.
 	 	 	   El pulsador debe producir una interrupción por EINT1 con flanco descendente.
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

	confIntExt();		// Configuración Int. por GPIO
	confTimer();		// Configuración TIM0

    while(1) {
    }
    return 0 ;
}

/* Función de configuración de EINT1*/
void confIntExt(void){
	LPC_PINCON->PINSEL4 |= (1<<22); // P2.11 como EINT1
	LPC_SC->EXTMODE  |= (1<<1);		// Interrupción por flanco.
	LPC_SC->EXTPOLAR &= ~(1<<1);	// Flanco de bajada.
	LPC_SC->EXTINT 	 |= (1<<1);		// Limpia flag de int.
	NVIC_EnableIRQ(EINT1_IRQn);		// Habilita Int. en NVIC.
	return;
}

/* Función de configuración TIM2 con MR0
 * Se utiliza un pin externo de Match para conectar el led.
 * Se ajusta el Timer para un tiempo de 250ms con frecuencia de 100MHz
 * - PR=0 -> MR+1 = (250ms * 100MHz) = 25000000
 */
void confTimer(void){
	LPC_PINCON->PINSEL0 |= (3<<12);		// Configura P0.6 como MAT2.0
	LPC_SC->PCONP       |= (1<<22);		// Enciende el TIM2
	LPC_SC->PCLKSEL1    |= (1<<12);		// Configura PCLK=CCLK;
	LPC_TIM2->EMR       |= (3<<4);		// Togglea en MR0.
	LPC_TIM2->PR 		 = (PR_VAL-1);	// Prescale Register = 0
	LPC_TIM2->MR0 		 = (MR_VAL-1);  // Se carga valor de MR para contar 250ms
	LPC_TIM2->MCR 		&= ~(1<<0);	    // Desactiva interrupción en MR0
	LPC_TIM2->MCR 		|= (1<<1);		// Configura Reset en MR0.
	LPC_TIM2->TCR 		|= 3;			// Habilitacion y reset de TC y PC.
	LPC_TIM2->TCR 		&= ~(1<<1);		// Saca del reset a TC y PC.
	return;
}

/* Handler de EINT1
 * Cada vez que ingresa al handler, se duplica el valor de (PR+1) y se carga el PR
 */
void EINT1_IRQHandler(void){
	static uint32_t prescaler = PR_VAL;
	LPC_SC->EXTINT |= (1<<1);			// Limpia bandera Int. EINT1
	LPC_TIM2->TCR  &= ~(1<<0);			// Deshabilita TC y PC
	prescaler = 2*prescaler;			// Calculo del prescaler.
	LPC_TIM2->PR = (prescaler - 1);		// Carga del precaler
	LPC_TIM2->TCR 		|= 3;			// Habilitacion y reset de TC y PC.
	LPC_TIM2->TCR 		&= ~(1<<1);		// Saca del reset a TC y PC.
	return;
}
