/*
===============================================================================
 Nombre : ADC_Timer
 Alumno : Marclé Emiliano
 Descripción : Programa que utiliza Timer para disparar ADC cuando hay
 	 	 	   flanco de bajada en MAT0.1.
 	 	 	   - Tiempo Match1 = 25us.
 	 	 	   - Tiempo Muestreo ADC = 50us.
 	 	 	   - Frec. Muestreo ADC = 20 KMuestras/seg

===============================================================================
*/
#include "LPC17xx.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_timer.h"
#include "lpc17xx_adc.h"

/* ----- Macros ------ */
#define PR_VAL  ((uint32_t) 100)
#define MR_VAL  ((uint32_t)  25)
#define PORT_0  ((uint8_t)    0)
#define OUTPUT  ((uint8_t)    1)

/* ----- Prototipos de funciones ------ */
void confGPIO(void);
void confADC(void);
void confTimer(void);

/* ------ Variables globales ------- */
__IO uint16_t ADC0Value = 0;
PINSEL_CFG_Type PinConfig;
TIM_TIMERCFG_Type TimerConfig;
TIM_MATCHCFG_Type MatchConfig;

/* -------- Programa principal --------- */
int main(void) {
	confGPIO();		// Configuración de GPIO
	confTimer();	// Configuración de Timer0
	confADC();		// Configuración de ADC

	/* Habilita Timer0 */
	TIM_Cmd(LPC_TIM0, ENABLE);

	/* Habilita la conversión en el canal 0 */
	ADC_ChannelCmd(LPC_ADC, ADC_CHANNEL_0, ENABLE);

    while(1) {}
    return 0 ;
}

/* -- Función de configuración de GPIO -- */
void confGPIO(void){
	/*P0.0 a P0.11 como GPIO con pullups. */
	PinConfig.Portnum = PINSEL_PORT_0;
	PinConfig.Funcnum = PINSEL_FUNC_0;
	PinConfig.Pinmode = PINSEL_PINMODE_PULLUP;
	PinConfig.OpenDrain = PINSEL_PINMODE_NORMAL;

	for(uint8_t pin=0 ; pin <12 ; pin++){
		PinConfig.Pinnum = pin;
		PINSEL_ConfigPin(&PinConfig);
	}

	/*P0.0 a P0.11 como salidas. */
	GPIO_SetDir(PORT_0, 0xFFF , OUTPUT);
	GPIO_ClearValue(PORT_0, 0xFFF);  // Limpia P0.0 a P0.11
	return;
}

/*
 * Función de configuración de ADC.
 * - Canal AD0.0
 * - No burst
 * - Dispara la conversión en flanco de bajada de MAT0.1 (cada 50 useg).
 * - Habilita interrupción
 */
void confADC(void){

	/* P0.23 como AD0.0 */
	PinConfig.Portnum = PINSEL_PORT_0;
	PinConfig.Pinnum = PINSEL_PIN_23;
	PinConfig.Pinmode = PINSEL_PINMODE_TRISTATE;
	PinConfig.OpenDrain = PINSEL_PINMODE_NORMAL;
	PinConfig.Funcnum = PINSEL_FUNC_1;

	PINSEL_ConfigPin(&PinConfig);

	/* Inicialización del ADC con tasa de adquisición de 200Kmuestras/s*/
	ADC_Init(LPC_ADC, 200000);
	ADC_BurstCmd(LPC_ADC, DISABLE);  // No burst

	/* Comenzar conversion en MAT0.1 con flanco de bajada. */
	ADC_StartCmd(LPC_ADC, ADC_START_ON_MAT01);
	ADC_EdgeStartConfig(LPC_ADC, ADC_START_ON_FALLING);

	/* Habilitación de interrupción */
	ADC_IntConfig(LPC_ADC, ADC_ADGINTEN, SET);
	NVIC_EnableIRQ(ADC_IRQn);

	return;
}

/*
 * Función de configuración del TIM0
 * - PCLK = CCLK
 * - Match1 cada 25 useg y 50 useg entre dos flancos de bajada en MAT0.1
 * - Toggle en Match1
 * - Sin interrupción
 */
void confTimer(void){

	/* Configuración del Timer */
	TimerConfig.PrescaleOption = TIM_PRESCALE_TICKVAL;
	TimerConfig.PrescaleValue = PR_VAL;

	TIM_Init(LPC_TIM0, TIM_TIMER_MODE, &TimerConfig);
	LPC_SC->PCLKSEL0 |= (1<<2); // PCLK = CCLK
	LPC_SC->PCLKSEL0 &= ~(1<<3); // PCLK = CCLK

	/* Configuración de Match0.1 */
	MatchConfig.MatchChannel = 1;
	MatchConfig.IntOnMatch = DISABLE;
	MatchConfig.StopOnMatch = DISABLE;
	MatchConfig.ResetOnMatch = ENABLE;
	MatchConfig.ExtMatchOutputType =TIM_EXTMATCH_TOGGLE;
	MatchConfig.MatchValue = (MR_VAL-1);

	TIM_ConfigMatch(LPC_TIM0, &MatchConfig);

	return;
}

/*
 * Handler de Int. en ADC
 */
void ADC_IRQHandler(void){

	/* Verificación de finalización de conversión en canal 0*/
	if(ADC_ChannelGetStatus(LPC_ADC, ADC_CHANNEL_0, ADC_DATA_DONE)){
		/* Guarda el valor convertido y lo saca por los pines P0.0 a P0.11 */
		ADC0Value = ADC_ChannelGetData(LPC_ADC, ADC_CHANNEL_0);
		LPC_GPIO0->FIOPIN = ADC0Value;
	}
	return;
}
