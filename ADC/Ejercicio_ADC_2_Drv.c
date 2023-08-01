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
#include "lpc17xx_pinsel.h"
#include "lpc17xx_adc.h"

/* ----- Prototipos de funciones ----- */
void confADC(void);

/* ------- Variables globales -------- */
PINSEL_CFG_Type PinConfig;
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
	/* P0.23 como AD0.0 , P0.24 como AD0.1 , P0.25 como AD0.2 , P0.26 como AD0.3 */
	PinConfig.Portnum = PINSEL_PORT_0;
	PinConfig.Funcnum = PINSEL_FUNC_1;
	PinConfig.Pinmode = PINSEL_PINMODE_TRISTATE;
	PinConfig.OpenDrain = PINSEL_PINMODE_NORMAL;

	for(uint8_t i=23; i<27; i++){
		PinConfig.Pinnum = i;
		PINSEL_ConfigPin(&PinConfig);
	}

	/* Inicialización del ADC con 200KMuestras/seg (50 KMuestras/seg x canal) */
	ADC_Init(LPC_ADC, 200000);
	ADC_BurstCmd(LPC_ADC, ENABLE);
	ADC_StartCmd(LPC_ADC, ADC_START_CONTINUOUS);

	/* Configuración de interrupciones y habilitación de canales del ADC */
	ADC_IntConfig(LPC_ADC, ADC_ADGINTEN, RESET);

	for(uint8_t j=0 ; j<4 ; j++){
		ADC_IntConfig(LPC_ADC, j, SET);
		ADC_ChannelCmd(LPC_ADC, j, ENABLE);  // Habilita los canales AD0.0 a AD0.3
	}

	NVIC_EnableIRQ(ADC_IRQn);  // Habilita int. en NVIC.

	return;
}

/* Handler de Int. del ADC */
void ADC_IRQHandler(void){
	/* Verifica si ya termino la conversión en el canal 0 */
	if(ADC_ChannelGetStatus(LPC_ADC, ADC_CHANNEL_0, ADC_DATA_DONE)){
		ADC_Val[0] = ADC_ChannelGetData(LPC_ADC, ADC_CHANNEL_0);
	}

	/* Verifica si ya termino la conversión en el canal 1 */
	else if(ADC_ChannelGetStatus(LPC_ADC, ADC_CHANNEL_1, ADC_DATA_DONE)){
		ADC_Val[1] = ADC_ChannelGetData(LPC_ADC, ADC_CHANNEL_1);
	}

	/* Verifica si ya termino la conversión en el canal 2 */
	else if(ADC_ChannelGetStatus(LPC_ADC, ADC_CHANNEL_2, ADC_DATA_DONE)){
		ADC_Val[2] = ADC_ChannelGetData(LPC_ADC, ADC_CHANNEL_2);
	}

	/* Verifica si ya termino la conversión en el canal 3 */
	else if(ADC_ChannelGetStatus(LPC_ADC, ADC_CHANNEL_3, ADC_DATA_DONE)){
		ADC_Val[3] = ADC_ChannelGetData(LPC_ADC, ADC_CHANNEL_3);
	}

	return;
}
