   /*
===============================================================================
 Alumno      : Marclé, Emiliano
 Descripción : Se configura el canal 0 del ADC, y se enciende o se apaga un led conenctado en P0.22
 	 	 	   de acuerdo al valor de entrada convertido.
===============================================================================
*/
#include "LPC17xx.h"

/* ------------- Macros -------------- */
#define	LIMITE	((uint16_t) 2046)

/* ----- Prototipos de funciones ----- */
void confGPIO(void);
void confADC(void);
void ADC_IRQHandlet(void);

/* ------- Variables globales ---------*/
uint16_t ADC0Value;

/* ------- Programa principal ---------*/
int main(void){
	confGPIO();		// Configuración de GPIO.
	confADC();		// Configuración del ADC.

    while(1) {
    };
    return 0 ;
}

/* Función de configuración de GPIO */
void confGPIO(void){
	LPC_GPIO0-> FIODIR |= (1<<22);		// P0.22 como salida.
	return;
}

/* Función de configuración del ADC */
void confADC(void){
	LPC_PINCON->PINSEL1 |= (1<<14);		 // P0.23 como AD0.0.
	LPC_PINCON->PINMODE1|= (1<<15);		 // Neither pull-up nor pull-down
	LPC_SC->PCONP       |= (1<<12); 	 // Enciende ADC
	LPC_SC->PCLKSEL0    |= (3<<24);      // CCLK/8
	LPC_ADC->ADCR       |= (1<<21);      // PDN=1 : Habilita ADC
	LPC_ADC->ADCR       &=~(255<<8);	 // CLKDIV=0; No hay division extra.
	LPC_ADC->ADCR		|= (1<<0);		 // Selecciona el canal 0.
	LPC_ADC->ADCR       |= (1<<16);      // BURST mode on.
	LPC_ADC->ADINTEN    &=~(1<<8);		 // Interrupciones en canales individuales
	LPC_ADC->ADINTEN    |= 1;			 // AD0.0 genera interrupcion cuando convierte.
	NVIC_EnableIRQ(ADC_IRQn);			 // Habilita interrupcion en NVIC.
	return;
}

/* Handler de interrupción del ADC */
void ADC_IRQHandler(void){
	float volt = 0;		// Variable de punto flotante para guardar valor de tensión.

	/* Verifica si el canal 0 termino de convertir DONE=1 */
	if(LPC_ADC->ADDR0 & (1<<31)){
		ADC0Value = ((LPC_ADC->ADDR0>>4) & 0xFFF);		// Guarda el valor convertido en una variable
		volt = (ADC0Value/4096)*3.3;					// Convierte el valor en una tensión (opcional).

		/* Enciende/Apaga el led rojo en P0.22 de acuerdo al valor convertido */
		if(ADC0Value < LIMITE){
			LPC_GPIO0->FIOSET |= (1<<22);
		}
		else{
			LPC_GPIO0->FIOCLR  |= (1<<22);
		}
	}

	return;
}
