/*
===============================================================================
 Nombre      : Marclé, Emiliano
 Descripcion : En este programa se configura el TIM0 para togglear P0.22
 	 	 	   cada vez que se produce un match en MAT0.0.
 	 	 	   Se genera una interrupción en el Match en la cual se implementa
 	 	 	   el parpadeo en el pin P0.22.
 	 	 	   En el pin P1.28 se tiene tambien la señal de salida cuadrada generada.
               por el toggle en MR0.
===============================================================================
*/
#include "LPC17xx.h"

/* Prototipos funciones */
void confGPIO(void);
void confTimer(void);

/* Programa principal */
int main(void) {
	confGPIO();
	confTimer();
	while(1){
    }
    return 0 ;
}

/* Función de configuración de GPIO */
void confGPIO(){
	LPC_GPIO0->FIODIR |= (1<<22);		// Configura P0.22 como salida.
	return;
}

/* Función de configuración de TIM0
 * - Configura toggle on match MAT0.0
 * - Carga valor de MR0
 * - Configura Reset en Match
 * - Habilita int. en Match
 */
void confTimer(){
	LPC_SC->PCONP  		|= (1<<1);  	// PCTIM0 = 1. Activa TIM0.
	LPC_SC->PCLKSEL0    |= (1<<2);  	// Clock=CCLK. Configuro divisor frecuencia.
	LPC_PINCON->PINSEL3 |= (3<<24); 	// Selecciona funcion MAT0.0 en P1.28. Este pin alimenta un led.
	LPC_TIM0->EMR       |= (3<<4);  	// Toggle en cada match de MAT0.0
	LPC_TIM0->MR0        = 70000000;	// Valor de Match. T=(1/100MHz)*(PR+1)*(MR+1) = 0,7seg.
	LPC_TIM0->MCR       |= (1<<1);  	// Reset en MR0.
	LPC_TIM0->MCR       |= (1<<0);  	// Habilita interrupción en Match
	LPC_TIM0->TCR		 = 3; 			// Habilita el contador, y pongo en reset el TC y PC.
	LPC_TIM0->TCR		&=~(1<<1);  	// Saca del reset al TC y PC.
	NVIC_EnableIRQ(TIMER0_IRQn);		// Se habilita la int. en NVIC.
	return;
}

/*
 * Handler del TIM0.
 * Se implemeta un parpadeo del led rojo en P0.22.
 */
void TIMER0_IRQHandler(void){
	static uint8_t i=0;
	if(i==0){
		LPC_GPIO0->FIOSET = (1<<22);	// Se apaga el led.
		i=1;
	}
	else if(i==1){
		LPC_GPIO0->FIOCLR = (1<<22);	// Se enciende el led.
		i=0;
	}
	LPC_TIM0->IR |= 1;					// Se limpia el flag de interrupción TIM0.
	return;
}
