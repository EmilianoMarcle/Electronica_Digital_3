/*
===============================================================================
 Alumno:  Marclé Emiliano
 Carrera: Ing. Electronica
 Descripcion : Utilizando el modo Capture, escribir un código en C  para que guarde
               en una variable ,llamada "shooter", el tiempo (en milisegundos) que
               le lleva a una persona presionar dos pulsadores con un único dedo.
===============================================================================
*/

#include "LPC17xx.h"

/* ----- Macros ------ */
#define PR_VAL ((uint32_t)  100000)  // Prescaler para contar 1ms con pclk=100MHz

/* ----- Prototipos de funciones ------ */
void confTimer(void);

/* ----- Variable globales ----- */
uint32_t val1 = 0;
uint32_t val2 = 0;
uint32_t shooter;
uint8_t count = 0;

/* ----- Programa principal ----- */
int main(void) {

	confTimer();	// Configuración de TIM0.
    while(1) {

    	/* Realiza el cálculo solo cuando count=1 */
    	if(count){
    		shooter = (val2-val1);
    		val1=0;
    		val2=0;
    		count = 0;
    	}
    }

    return 0 ;
}

/* Función de configuración TIM0 en modo capture */
void confTimer(void){

	LPC_PINCON->PINSEL3 |= (3<<20); // P1.26 como CAP0.0
	LPC_PINCON->PINSEL3 |= (3<<22); // P1.27 como CAP0.0
	LPC_SC->PCONP |= (1<<1);		// Enciende el TIM0.
	LPC_SC->PCLKSEL0 &= (1<<2);	    // PCLK=CCLK

	LPC_TIM0->PR = (PR_VAL-1); // Prescaler para incrementar TC cada 1ms.

	/* Captura en flanco de bajada de CAP0.0 y habilitación de interrupción */
	LPC_TIM0->CCR |= (1<<1)|(1<<2);

	/* Captura en flanco de bajada de CAP0.1 y habilitación de interrupción */
	LPC_TIM0->CCR |= (1<<4)|(1<<5);

	LPC_TIM0->TCR |= 3;	        // Habilitación de TC y PC.
	LPC_TIM0->TCR &= ~(1<<1);	// Saca del reset.

	NVIC_EnableIRQ(TIMER0_IRQn);  // Habilita interrupción en NVIC.

	return;
}

/* Handler TIM0 */
void TIMER0_IRQHandler(void){
	static uint8_t flag = 0;

	/* Interrupción por CR0 */
	if(LPC_TIM0->IR & (1<<4)){
		/* Se presiona primero pulsador en CAP0.0 */
		if(flag == 0){
			val1 = LPC_TIM0->CR0;
			flag = 2;
		}
		/* Se presiona segundo el pulsador en CAP0.0 */
		else if(flag == 1){
			val2 = LPC_TIM0->CR0;
			flag = 0;
			count = 1;
		}
		LPC_TIM0->IR |= (1<<4); // Limpia flag de interrupción.
		return;
	}

	/* Interrupción por CR1 */
	if(LPC_TIM0->IR & (1<<5)){
		/* Se presiona primero pulsador en CAP0.1 */
		if(flag == 0){
			val1 = LPC_TIM0->CR1;
			flag = 1;
		}
		/* Se presiona segundo el pulsador en CAP0.1 */
		else if(flag == 2){
			val2 = LPC_TIM0->CR1;
			flag = 0;
			count = 1;
		}
		LPC_TIM0->IR |= (1<<5); // Limpia flag de interrupción.
		return;
	}
}
