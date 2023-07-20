/*
===============================================================================
 Name        : Ejercicio_ADC_2.c
 Author      : Marclé, Emiliano
 Version     : 1.0
 Description :
 - Se configura el ADC para convertir por 4 canales en modo Burst con Fm=50Kmuestras/s cada uno.
 - Las conversiones se guardan en una variable.
 - Frecuencia de CPU de 100MHz
 - Frec. Muestreo por canal = 50Ksamples/s
 - Frec. Muestreo total = 4 * 50Ksamples/s = 200 Ksamples/s
 - Frec. Trabajo = 64 ciclos * 200 Ksamples/s = 12,8 MHz
 - Frec. Trabajo aproximada = (CCLK/8)*(1/CLKDIV) = (100MHz/8)*(1/1)=12,5 MHz
===============================================================================
*/

#include "LPC17xx.h"

/* ----- Prototipos de funciones ----- */
void confGPIO(void);
void confADC(void);

/* ------- Variables globales -------- */
__IO uint16_t ADC_Val [4]; // Arreglo de valores para guardar conversiones de los 4 canales.

/* -------- Programa principal ------- */
int main(void) {

	confADC();		// Configuración de ADC

	while(1) {

    }
    return 0 ;
}

/*
 * Función de configuración del ADC
 * - Se configuran 1os canales 1,2,3 y 4 en modo burst.
 * - PCLK = CCLK/8
 * - Se habilita la generación de Interrupción
 */
void confADC(void){
	// P0.23 como AD0.0 , P0.24 como AD0.1 , P0.25 como AD0.2 , P0.26 como AD0.3
	LPC_PINCON->PINSEL1		|= (1<<14)|(1<<16)|(1<<18)|(1<<20);
	// neither pull-down nor pull-up
	LPC_PINCON->PINMODE1	|= (1<<15)|(1<<17)|(1<<19)|(1<<21);
	// Encendido del ADC y PCLK=CCLK/8
	LPC_SC->PCONP    		|= (1<<12);
	LPC_SC->PCLKSEL0    	|= (3<<24);
	// Habilita AD0.0 a AD0.3
	LPC_ADC->ADCR       	|= (1<<0)|(1<<1)|(1<<2)|(1<<3);
	LPC_ADC->ADCR       	&=~(255<<8);	// CLKDIV + 1 = 1
	// Modo Burst
	LPC_ADC->ADCR       	|= (1<<16);
	// START = 000 (No Start)
	LPC_ADC->ADCR			&= ~((1<<24)|(1<<25)|(1<<26));
	// Habilita ADC: bit PDN
	LPC_ADC->ADCR       	|= (1<<21);
	// Interrupción en canales individuales AD0.0 a AD0.3
	LPC_ADC->ADINTEN   		&= ~(1<<8);
	LPC_ADC->ADINTEN		|= (1<<0)|(1<<1)|(1<<2)|(1<<3);
	// Habilita int. en NVIC.
	NVIC_EnableIRQ(ADC_IRQn);
	return;
}


/* Handler de Int. del ADC */
void ADC_IRQHandler(void){
	/* Verifica si ya termino la conversión en el canal 0 */
	if(LPC_ADC->ADDR0 & (1<<31)){
		ADC_Val[0] = ((LPC_ADC->ADDR0) >> 4) & (0xFFF);
	}

	/* Verifica si ya termino la conversión en el canal 1 */
	else if(LPC_ADC->ADDR1 & (1<<31)){
		ADC_Val[1] = ((LPC_ADC->ADDR1) >> 4) & (0xFFF);
	}

	/* Verifica si ya termino la conversión en el canal 2 */
	else if(LPC_ADC->ADDR2 & (1<<31)){
		ADC_Val[2] = ((LPC_ADC->ADDR2) >> 4) & (0xFFF);
	}

	/* Verifica si ya termino la conversión en el canal 3 */
	else if(LPC_ADC->ADDR3 & (1<<31)){
		ADC_Val[3] = ((LPC_ADC->ADDR3) >> 4) & (0xFFF);
	}

	return;
}
