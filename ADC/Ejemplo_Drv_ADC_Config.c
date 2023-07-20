/*
===============================================================================
 Alumno      : Marcle, Emiliano
 Carrera     : Ing. Electrónica
 Descripcion : Se configura el ADC para disparar por software (No Burst), conversiones en el canal 2.
 	 	 	   La frecuencia de muestreo es de 200KHz.

===============================================================================
*/

#include "LPC17xx.h"
#include "lpc17xx_adc.h"
#include "lpc17xx_pinsel.h"

/* ----- Macros ------ */
#define _ADC_INT		    ADC_ADINTEN2
#define _ADC_CHANNEL		ADC_CHANNEL_2

/* ------ Variables globales ------ */
__IO uint32_t adc_value;	// Variable para guardar conversión.

/* ---- Protoipos de funciones ---- */
void ADC_IRQHandler(void);
void configPin(void);
void configADC(void);

/* ------- Programa Pincipal ------ */
int main (void)
{
	uint32_t tmp;

	configPin();
	configADC();

	while(1)
	{
		// Start conversion
		ADC_StartCmd(LPC_ADC,ADC_START_NOW);

		/* Enable ADC in NVIC */
		NVIC_EnableIRQ(ADC_IRQn);

		/* Retardo entre cada conversión */
		for(tmp = 0; tmp < 1000000; tmp++);
	}

	return 0;
}

/* Función de configuración de pines */
void configPin(){
	PINSEL_CFG_Type PinCfg;
	/*
	 * Init ADC pin connect
	 * AD0.2 on P0.25
	 */
	PinCfg.Funcnum = PINSEL_FUNC_1;
	PinCfg.OpenDrain = PINSEL_PINMODE_NORMAL;
	PinCfg.Pinmode = PINSEL_PINMODE_TRISTATE;
	PinCfg.Pinnum = PINSEL_PIN_25;
	PinCfg.Portnum = PINSEL_PORT_0;
	PINSEL_ConfigPin(&PinCfg);
return;
}

/* Función de configuración de ADC*/
void configADC(){

	/* Configuration for ADC:
	 *  select: ADC channel 2
	 *  ADC conversion rate = 200KHz
	 */
	ADC_Init(LPC_ADC, 200000);
	ADC_IntConfig(LPC_ADC,_ADC_INT, SET);
	ADC_BurstCmd(LPC_ADC, DISABLE);
	ADC_ChannelCmd(LPC_ADC,_ADC_CHANNEL,ENABLE);

	NVIC_SetPriority(ADC_IRQn, (9));
	return;
}

/* Handler del ADC */
void ADC_IRQHandler(void)
{
	adc_value = 0;

	/* Verifica la finalización de la conversión en el canal 2 */
	if (ADC_ChannelGetStatus(LPC_ADC,_ADC_CHANNEL,ADC_DATA_DONE))
	{
		/* Guarda el valor convertido */
		adc_value =  ADC_ChannelGetData(LPC_ADC,_ADC_CHANNEL);
		/* Desactiva la interrupción */
		NVIC_DisableIRQ(ADC_IRQn);
	}
}
