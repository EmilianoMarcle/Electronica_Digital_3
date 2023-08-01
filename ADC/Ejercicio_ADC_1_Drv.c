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
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_adc.h"

/* ------------- Macros -------------- */
#define	LIMITE ((uint16_t) 2048)
#define PORT_0 ((uint8_t)  0)
#define PIN_0  ((uint32_t) (1<<0))
#define PIN_22 ((uint32_t) (1<<22))
#define OUTPUT ((uint8_t)  1)
/* ----- Prototipos de funciones ----- */
void confGPIO(void);
void confADC(void);

/* ------- Variables globales -------- */
PINSEL_CFG_Type PinConfig;
uint16_t ADC_Val_0;
uint16_t ADC_Val_1;

/* -------- Programa principal ------- */
int main(void) {

	confGPIO();		// Configuración de GPIO.
	confADC();		// Configuración de ADC

	/* Habilita canal 0 y 1 del ADC */
	ADC_ChannelCmd(LPC_ADC, ADC_CHANNEL_0, ENABLE);
	ADC_ChannelCmd(LPC_ADC, ADC_CHANNEL_1, ENABLE);

	while(1) {

    }
    return 0 ;
}

/* Función de configuración de GPIO */
void confGPIO(void){
	/* Configura P0.0 y P0.22 como GPIO de salida */
	PinConfig.Portnum = PINSEL_PORT_0;
	PinConfig.Pinnum = PINSEL_PIN_0;
	PinConfig.Funcnum = PINSEL_FUNC_0;
	PinConfig.Pinmode = PINSEL_PINMODE_PULLUP;
	PinConfig.OpenDrain = PINSEL_PINMODE_NORMAL;
	PINSEL_ConfigPin(&PinConfig);

	PinConfig.Pinnum = PINSEL_PIN_22;
	PINSEL_ConfigPin(&PinConfig);

	GPIO_SetDir(PORT_0, PIN_0, OUTPUT);  // P0.0 como salida.
	GPIO_ClearValue(PORT_0, PIN_0);	     // Led apagado
	GPIO_SetDir(PORT_0, PIN_22, OUTPUT); // P0.22 como salida.
	GPIO_SetValue(PORT_0, PIN_22);  // Led apagado.

	return;
}

/*
 * Función de configuración del ADC
 * - Se configura el canal 0 y el 1 en modo burst.
 * - PCLK = CCLK/8
 * - Se habilita la generación de Interrupción
 */
void confADC(void){
	/* P0.23 como AD0.0 y P0.24 como AD0.1 */
	PinConfig.Portnum = PINSEL_PORT_0;
	PinConfig.Pinnum = PINSEL_PIN_23;
	PinConfig.Funcnum = PINSEL_FUNC_1;
	PinConfig.Pinmode = PINSEL_PINMODE_TRISTATE;
	PinConfig.OpenDrain = PINSEL_PINMODE_NORMAL;
	PINSEL_ConfigPin(&PinConfig);

	PinConfig.Pinnum = PINSEL_PIN_24;
	PINSEL_ConfigPin(&PinConfig);

	/* Inicializa el ADC con los parámetros del ejemplo visto */
	LPC_SC->PCONP    	|= (1<<12);						// Encendido del ADC
	LPC_SC->PCLKSEL0    |= (3<<24);						// PCLK=CCLK/8
	LPC_ADC->ADCR       |= (1<<21); 					// Habilita ADC
	LPC_ADC->ADCR       &=~(255<<8);   					// CLKDIV + 1 = 1

	ADC_BurstCmd(LPC_ADC, ENABLE);
	ADC_StartCmd(LPC_ADC, ADC_START_CONTINUOUS);

	/* Habilita interrupción en AD0.0 y AD0.1 */
	ADC_IntConfig(LPC_ADC, ADC_ADINTEN0, ENABLE);
	ADC_IntConfig(LPC_ADC, ADC_ADINTEN1, ENABLE);
	ADC_IntConfig(LPC_ADC, ADC_ADGINTEN, DISABLE);
	NVIC_EnableIRQ(ADC_IRQn);		// Habilita int. en NVIC.
	return;
}

/* Handler de Int. del ADC */
void ADC_IRQHandler(void){

	/* Verifica si ya termino la conversión en el canal 0 */
	if(ADC_ChannelGetStatus(LPC_ADC, ADC_CHANNEL_0, ADC_DATA_DONE)){
		ADC_Val_0 = ADC_ChannelGetData(LPC_ADC, ADC_CHANNEL_0);

		if(ADC_Val_0 > LIMITE){
			GPIO_ClearValue(PORT_0, PIN_22);	// Enciende led en P0.22
			return;
		}
		else{
			GPIO_SetValue(PORT_0, PIN_22);   // Apaga led en P0.22
			return;
		}
	}

	/* Verifica si ya termino la conversión en el canal 1 */
	else if(ADC_ChannelGetStatus(LPC_ADC, ADC_CHANNEL_1, ADC_DATA_DONE)){
		ADC_Val_1 = ADC_ChannelGetData(LPC_ADC, ADC_CHANNEL_1);
		if(ADC_Val_1 > LIMITE){
			GPIO_SetValue(PORT_0, PIN_0);	// Enciende led en P0.0
			return;
		}
		else{
			GPIO_ClearValue(PORT_0, PIN_0);    // Apaga led en P0.0
			return;
		}
	}

}
