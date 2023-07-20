/*
===============================================================================
 Name        : Ejercicio_ADC_1.c
 Author      : Marclé, Emiliano
 Version     : 1.0
 Description :  Se configura el ADC para convertir por dos canales
				En la rutina de interrupción se apaga un led conectado en P0.22 si el valor
				convertido en el canal 0 es menor a 2048, y se enciende si es mayor a 2048.
				Si el valor convertido en el canal 1 es menor a 2048 se apaga un led conectado
				en el pin P0.0 y se mayor a 2048 se enciende.

				Frecuencia de CPU de 100MHz

				1) Frec. Trabajo = (CCLK/8)*(1/CLKDIV) = (100MHz/8)*(1/1)=12,5 MHz
	   	   	   	   Frec. Muestreo = ((Frec. Trabajo) / 64 ) = 195,312 KSample/s

				2) Frec. Muestreo para 2 canales = ((Frec. Muestreo) / 2) = 97,656 KSample/s
===============================================================================
*/

#include "LPC17xx.h"

/* ------------- Macros -------------- */
#define	LIMITE ((uint16_t) 2048)

/* ----- Prototipos de funciones ----- */
void confGPIO(void);
void confADC(void);

/* ------- Variables globales -------- */
uint16_t ADC_Val_0;
uint16_t ADC_Val_1;

/* -------- Programa principal ------- */
int main(void) {

	confGPIO();		// Configuración de GPIO.
	confADC();		// Configuración de ADC

	while(1) {

    }
    return 0 ;
}

/* Función de configuración de GPIO */
void confGPIO(void){
	LPC_GPIO0->FIODIR |= (1<<0);		// P0.0 como salida.
	LPC_GPIO0->FIOCLR |= (1<<0);		// Led apagado
	LPC_GPIO0->FIODIR |= (1<<22);		// P0.22 como salida.
	LPC_GPIO0->FIOSET |= (1<<22);		// Led apagado.
	return;
}

/*
 * Función de configuración del ADC
 * - Se configura el canal 0 y el 1 en modo burst.
 * - PCLK = CCLK/8
 * - Se habilita la generación de Interrupción
 */
void confADC(void){
	LPC_PINCON->PINSEL1		|= (1<<14) | (1<<16);			// P0.23 como AD0.0 y P0.24 como AD0.1
	LPC_PINCON->PINMODE1	|= (1<<15) | (1<<17);			// neither pull-down nor pull-up
	LPC_SC->PCONP    		|= (1<<12);						// Encendido del ADC
	LPC_SC->PCLKSEL0    	|= (3<<24);						// PCLK=CCLK/8
	LPC_ADC->ADCR       	|= (1<<0) | (1<<1);				// Habilita AD0.0 y AD0.1
	LPC_ADC->ADCR       	&=~(255<<8);   					// CLKDIV + 1 = 1
	LPC_ADC->ADCR       	|= (1<<16);						// Burst
	LPC_ADC->ADCR			&= ~((1<<24)|(1<<25)|(1<<26));	// START = 000 (No Start)
	LPC_ADC->ADCR       	|= (1<<21); 					// Habilita ADC
	LPC_ADC->ADINTEN   		&= ~(1<<8);						// Interrupción en canales individuales
	LPC_ADC->ADINTEN		|= 3;							// Habilita interrupcion en AD0.0 y AD0.1
	NVIC_EnableIRQ(ADC_IRQn);								// Habilita int. en NVIC.
	return;
}

/* Handler de Int. del ADC */
void ADC_IRQHandler(void){
	/* Verifica si ya termino la conversión en el canal 0 */
	if(LPC_ADC->ADDR0 & (1<<31)){
		ADC_Val_0 = ((LPC_ADC->ADDR0) >> 4) & (0xFFF);

		if(ADC_Val_0 > LIMITE){
			LPC_GPIO0->FIOCLR |= (1<<22);	// Enciende led en P0.22
		}
		else{
			LPC_GPIO0->FIOSET |= (1<<22);   // Apaga led en P0.22
		}
	}
	/* Verifica si ya termino la conversión en el canal 1 */
	else if(LPC_ADC->ADDR1 & (1<<31)){
		ADC_Val_1 = ((LPC_ADC->ADDR1) >> 4) & (0xFFF);
		if(ADC_Val_1 > LIMITE){
			LPC_GPIO0->FIOSET |= (1<<0);	// Enciende led en P0.0
		}
		else{
			LPC_GPIO0->FIOCLR |= (1<<0);    // Apaga led en P0.0
		}
	}

}
