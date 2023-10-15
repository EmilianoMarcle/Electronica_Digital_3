/*
===============================================================================
 Name        : Ejercicio_Integracion_2.c
 Author      : Marclé, Emiliano
 Version     : 1.0
 Description :
 Filtrar las muestras provenientes del ADC con un filtro promediador móvil de N muestras,
 donde N inicialmente es igual a 2 y puede ser incrementado en dos unidades cada vez que se presiona
 un pulsador conectado en P0.6 hasta llegar a un valor de 600.
 A su vez, utilizando el bit de overrun y un timer, complete el código realizado para que en caso de
 producirse el evento de pérdida de datos por parte del ADC, se deje de muestrear y se saque por
 el pin MATCH2.1 una señal cuadrada de 440 Hz.
 Considerar una frecuencia de cclk de 60 Mhz y configurar el ADC para obtener una frecuencia
 de muestreo de 5 Kmuestras/seg.
===============================================================================
*/

#include "LPC17xx.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_timer.h"
#include "lpc17xx_adc.h"

/* ------- Macros ------- */
#define    PORT_0       ((uint8_t) 0)
#define    PIN_6        ((uint32_t) (1<<6))
#define    PIN_NUM_6    ((uint32_t) 6)
#define    OUTPUT       ((uint8_t) 1)
#define    RISING_EDGE ((uint8_t) 0)
#define    ADC_RATE     ((uint32_t) 115380)
#define    PR_VAL_0     ((uint32_t) 1)      // Prescaler para contar 1 us
#define    MR_VAL_0     ((uint32_t) 1000)    // Cuenta 100us.
#define    PR_VAL_2     ((uint32_t) 250)    // Prescaler para contar 250 us
#define    MR_VAL_2     ((uint32_t) 5)      // Cuenta 1,25ms.
#define    BUFFER_SIZE  ((uint16_t)  600)   // Máximo tamaño del buffer

/* ----- Variables Globales ----- */
PINSEL_CFG_Type PinConfig;
TIM_TIMERCFG_Type TimerCfg;
TIM_MATCHCFG_Type MatchCfg;
uint32_t buffer[BUFFER_SIZE]={0};
__IO uint16_t ADCValue = 0;
uint16_t samples = 2;
uint32_t suma = 0;
uint32_t promedio = 0;
uint32_t TIM2Flag = 0;

/* ----- Prototipos de funciones ------ */
void confGPIO(void);
void confADC(void);
void confTimer0(void);
void confTimer2(void);

/* ----- Programa principal ------ */
int main(void) {

	confGPIO();  // Configuración de GPIO
	confADC();   // Configuración de ADC
	confTimer0(); // Configuración de Timer0
	confTimer2();

	/* Habilitación del TIM0 */
	TIM_Cmd(LPC_TIM0, ENABLE);

	/* Habilitación del canal 0 del ADC */
	ADC_ChannelCmd(LPC_ADC, ADC_CHANNEL_0, ENABLE);

	while(1) {
		if(TIM2Flag){
			TIM_Cmd(LPC_TIM2, ENABLE);
			TIM2Flag = 0;
		}
	}

    return 0 ;
}

/* Función de configuración */
void confGPIO(void){

	/* P0.6 como entrada con pull-up */
	PinConfig.Portnum = PINSEL_PORT_0;
	PinConfig.Pinnum = PINSEL_PIN_6;
	PinConfig.Funcnum = PINSEL_FUNC_0;
	PinConfig.Pinmode = PINSEL_PINMODE_PULLUP;
	PinConfig.OpenDrain = PINSEL_PINMODE_NORMAL;
	PINSEL_ConfigPin(&PinConfig);

	GPIO_SetDir(PORT_0, PIN_6, OUTPUT);
	GPIO_ClearValue(PORT_0, PIN_6);

	/* Interrupción por GPIO en P0.6 por flanco de subida */
	GPIO_IntCmd(PORT_0, PIN_6, RISING_EDGE);
	GPIO_ClearInt(PORT_0, PIN_6);
	NVIC_EnableIRQ(EINT3_IRQn);

	return;
}

/* Función de configuración del ADC
 * - ADC_RATE = ADC_CLOCK/65 = (CCLK/8)/65=115385
 */
void confADC(void){
	/* P0.23 como AD0.0 */
	PinConfig.Portnum = PINSEL_PORT_0;
	PinConfig.Pinnum = PINSEL_PIN_23;
	PinConfig.Funcnum = PINSEL_FUNC_1;
	PinConfig.Pinmode = PINSEL_PINMODE_TRISTATE;
	PinConfig.OpenDrain = PINSEL_PINMODE_NORMAL;
	PINSEL_ConfigPin(&PinConfig);

	/* Inicialización del ADC en modo No Burst */
	ADC_Init(LPC_ADC, ADC_RATE);
	ADC_BurstCmd(LPC_ADC, ENABLE);

	/* Conversión en flanco de sudibda en Match0.1 */
	ADC_StartCmd(LPC_ADC, ADC_START_ON_MAT01);
	ADC_EdgeStartConfig(LPC_ADC, ADC_START_ON_RISING);

	/* Habilitación de interrupción del ADC  */
	ADC_IntConfig(LPC_ADC, ADC_ADINTEN0, SET);
	NVIC_EnableIRQ(ADC_IRQn);

	return;
}

/* Función de configuración de Timer0
 * - Controla el muestreo del ADC
 * - Toggle en match
 * - Tiempo de Match de 100 us
 * - Interrupción en Match desactivada
 */
void confTimer0(void){
	TIM_TIMERCFG_Type TimerCfg;
	TIM_MATCHCFG_Type MatchCfg;

	/* Configuración Timer0 */
	TimerCfg.PrescaleOption = TIM_PRESCALE_USVAL;
	TimerCfg.PrescaleValue = PR_VAL_0;

	/* Configuración Match 0.1 */
	MatchCfg.MatchChannel = 1;
	MatchCfg.IntOnMatch = DISABLE;
	MatchCfg.ResetOnMatch = ENABLE;
	MatchCfg.StopOnMatch = DISABLE;
	MatchCfg.ExtMatchOutputType = TIM_EXTMATCH_TOGGLE;
	MatchCfg.MatchValue = (MR_VAL_0 - 1);

	/* Inicialización del Timer0 */
	TIM_Init(LPC_TIM0, TIM_TIMER_MODE, &TimerCfg);
	TIM_ConfigMatch(LPC_TIM0, &MatchCfg);

	return;
}

/* Función de configuración de Timer2
 * - Genera onda cuadrada de 400Hz
 * - Toggle en match
 * - Tiempo de Match de 250us
 * - Fseñal = 400Hz, Tseñal = 2,5ms
 * - T_TIM2 = 1,25ms = (1/pclk)*(PR+1)*(MR+1)= 250us*5
 * - Interrupción en Match desactivada
 */
void confTimer2(void){

	/* P0.7 como MAT2.1 */
	PinConfig.Portnum = PINSEL_PORT_0;
	PinConfig.Pinnum = PINSEL_PIN_7;
	PinConfig.Funcnum = PINSEL_FUNC_3;
	PinConfig.Pinmode = PINSEL_PINMODE_PULLUP;
	PinConfig.OpenDrain = PINSEL_PINMODE_NORMAL;
	PINSEL_ConfigPin(&PinConfig);

	/* Configuración Timer2 */
	TimerCfg.PrescaleOption = TIM_PRESCALE_USVAL;
	TimerCfg.PrescaleValue = PR_VAL_2;

	/* Configuración Match 2.1 */
	MatchCfg.MatchChannel = 1;
	MatchCfg.IntOnMatch = DISABLE;
	MatchCfg.ResetOnMatch = ENABLE;
	MatchCfg.StopOnMatch = DISABLE;
	MatchCfg.ExtMatchOutputType = TIM_EXTMATCH_TOGGLE;
	MatchCfg.MatchValue = (MR_VAL_2 - 1);

	/* Inicialización del Timer2 */
	TIM_Init(LPC_TIM2, TIM_TIMER_MODE, &TimerCfg);
	TIM_ConfigMatch(LPC_TIM2, &MatchCfg);

	return;
}

void EINT3_IRQHandler(void){
	if(GPIO_GetIntStatus(PORT_0, 6, RISING_EDGE)){

		/* Incrementa el número de muestras de a 2 hasta llegar a 600 */
		if(samples < 600){
			samples = samples + 2;
		}
		else{
			samples = 600;
		}
	}

	GPIO_ClearInt(PORT_0, PIN_6);  // Limpia flag de interrupción.

	return;
}

void ADC_IRQHandler(void){

    if(ADC_ChannelGetStatus(LPC_ADC, ADC_CHANNEL_0, ADC_DATA_DONE)){

		/* Almacena el valor convertido */
		ADCValue = ADC_ChannelGetData(LPC_ADC, ADC_CHANNEL_0);

		/* Inicialización de variable suma */
		suma = 0;
		promedio = 0;

		/* Descarta la muestra más vieja del buffer */
		for(uint32_t i=0 ; i<(samples-1) ; i++){
			buffer[i] = buffer[i+1];
			suma += buffer[i];
		}

		/* Carga el nuevo valor en la última posición del buffer*/
		buffer[samples-1] = ADCValue;
		suma += buffer[samples-1];

		/* Calcula el promedio de N muestras*/
		promedio = suma/samples;
	}

    /* Verifica OVERRUN en channel 0 */
    else if(LPC_ADC->ADDR0 & (1<<30)){
        /* Desactiva el ADC */
        ADC_ChannelCmd(LPC_ADC, ADC_CHANNEL_0, DISABLE);
    	NVIC_DisableIRQ(ADC_IRQn);

        /* Habilitación del TIM2 que genera onda cuadrada de 400 Hz */
    	TIM2Flag = 1;
    }

	return;
}


