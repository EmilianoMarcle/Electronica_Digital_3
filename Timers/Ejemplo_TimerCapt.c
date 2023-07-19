/*
===============================================================================
 Name        : timerCapt.c
 Author      : Marclé Emiliano
 Description : Este programa utiliza TIM0 en modo capture con generación de interrupción.
===============================================================================
*/

#include "LPC17xx.h"

/* Prototipos de función */
void confGPIO(void);
void confTimer(void);

/* Variables globales */
uint32_t aux=0;

/* Programa principal */
int main(void) {
	confGPIO();
	confTimer();
    while(1) {
    }
    return 0 ;
}

/* Función de configuración de GPIO */
void confGPIO(void){
	LPC_GPIO0->FIODIR |= (1<<22);	// Configura P0.22 como salida.
	return;
}

/* Función de configuración de Timer
 * - Se configura pin de captura CAP0.0
 * - Captura en flanco de bajada
 * - Habilita generación de interrupción cuando ocurre evento de captura
 *   con flanco de bajada.
 */
void confTimer(void){
	LPC_SC->PCONP       |= (1<<1);  // Enciende TIM0.
	LPC_SC->PCLKSEL0    |= (1<<2);  // Configuracion del clock del peroférico PCLK=CCLK=100MHz.
	LPC_PINCON->PINSEL3 |= (3<<20); // Pin 1.26 como CAP0.0
	LPC_TIM0->CCR       |= (1<<1)   // Captura en flanco de bajada en CAP0.0
						|  (1<<2);	// Habilita interrupcion en CAP0.0
	LPC_TIM0->TCR       |= 3;		// Habilita y resetea el TC y PC.
	LPC_TIM0->TCR       &= ~(1<<1); // Saca al TC y PC del estado de reset
	NVIC_EnableIRQ(TIMER0_IRQn);    // Activa interrupcion en NVIC.
	return;
}

/* Handler de TIM0 */
void TIMER0_IRQHandler(void){
	static uint8_t i=0;
	// Si la interrupción se genero por CAP0 prende/apaga el led y guarda el valor capturado en un avariable.
	if(LPC_TIM0->IR & (1<<4)){
		aux = LPC_TIM0->CR0; // Variable auxiliar para observar el valor del reg. captura
		if(i==0){
			LPC_GPIO0->FIOSET = (1<<22);
			i=1;
		}
		else if(i==1){
			LPC_GPIO0->FIOCLR = (1<<22);
			i=0;
		}
		LPC_TIM0->IR |=4;    // Limpia bandera de interrupcion.
	}

	return;
}
