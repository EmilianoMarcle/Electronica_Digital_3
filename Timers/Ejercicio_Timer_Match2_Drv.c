/*
===============================================================================
 Nombre        : Marclé, Emiliano
 Carrera	   : Ing. Electronica
 Descripción   : Escribir un programa para que por cada presión de un pulsador,
  	  	  	     la frecuencia de parpadeo de un led disminuya a la mitad debido
  	  	  	     a la modificación del pre-escaler del Timer 2. El pulsador debe
  	  	  	     producir una interrupción por EINT1 con flanco descendente.
===============================================================================
*/

#include "LPC17xx.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_exti.h"
#include "lpc17xx_timer.h"

/* ------ Macros ------ */
#define  PRE_VAL  ((uint32_t)  250000)

/* ------ Prototipos de funciones ------ */
void confIntExt(void);
void confTimer(void);

/* ----- Variables globales ------ */
PINSEL_CFG_Type PinConfig;
TIM_TIMERCFG_Type TimerConfig;
TIM_MATCHCFG_Type MatchConfig;

/* ------ Programa principal ------ */
int main(void) {

	confIntExt();		// Configuración de interrupción externa.
	confTimer();		// Configuración de Timer.

	/* Habilita el Timer2 */
	TIM_Cmd(LPC_TIM2, ENABLE);

    while(1) {
    }

    return 0 ;
}

/* Función de configuración EINT1 */
void confIntExt(void){

	/* Configura P2.11 como EINT1 */
	PinConfig.Portnum = PINSEL_PORT_2;
	PinConfig.Pinnum = PINSEL_PIN_11;
	PinConfig.Funcnum = PINSEL_FUNC_1;
	PinConfig.Pinmode = PINSEL_PINMODE_PULLUP;
	PinConfig.OpenDrain = PINSEL_PINMODE_NORMAL;
	PINSEL_ConfigPin(&PinConfig);

	/* Interrupción por flanco de bajada. */
	EXTI_SetMode(EXTI_EINT1, EXTI_MODE_EDGE_SENSITIVE);
	EXTI_SetPolarity(EXTI_EINT1, EXTI_POLARITY_LOW_ACTIVE_OR_FALLING_EDGE);
	EXTI_ClearEXTIFlag(EXTI_EINT1);

	NVIC_EnableIRQ(EINT1_IRQn);

	return;
}

/* Función de configuración de TIM2*/
void confTimer(void){

	/* Configura P0.6 como MAT2.0*/
	PinConfig.Portnum = PINSEL_PORT_0;
	PinConfig.Pinnum = PINSEL_PIN_6;
	PinConfig.Funcnum = PINSEL_FUNC_3;
	PinConfig.Pinmode = PINSEL_PINMODE_PULLUP;
	PinConfig.OpenDrain = PINSEL_PINMODE_NORMAL;
	PINSEL_ConfigPin(&PinConfig);

	/* Configuración TIM2
	 * - Tiempo match inicial de 500ms.
	 * - Reset en Match0.
	 * - Toggle en Match0.
	 * - PCLK = CCLK/4;
	 */
	TimerConfig.PrescaleOption = TIM_PRESCALE_USVAL;
	TimerConfig.PrescaleValue = PR_VAL;  // 250 mseg.

	MatchConfig.MatchChannel = 0;
	MatchConfig.IntOnMatch = DISABLE;
	MatchConfig.StopOnMatch = DISABLE;
	MatchConfig.ResetOnMatch = ENABLE;
	MatchConfig.ExtMatchOutputType = TIM_EXTMATCH_TOGGLE;
	MatchConfig.MatchValue = 1;

	/* Inicialización del TIM2 como Timer */
	TIM_Init(LPC_TIM2, TIM_TIMER_MODE, &TimerConfig);

	/* Configuración del Match */
	TIM_ConfigMatch(LPC_TIM2, &MatchConfig);

	return;
}

void EINT1_IRQHandler(void){
	static uint32_t prescaler = PR_VAL;

	/* Disminuye la frecuencia del parpadeo a la mitad duplicando el PRESCALER */
	prescaler *= 2;
	TimerConfig.PrescaleValue = prescaler;
	TIM_Cmd(LPC_TIM2, DISABLE);  // Deshabilita el Timer2
	TIM_Init(LPC_TIM2, TIM_TIMER_MODE, &TimerConfig);  // Se cargan los nuevos valores y se resetea el TC y PC.
	TIM_Cmd(LPC_TIM2, ENABLE);  // Se habilita el Timer2

	EXTI_ClearEXTIFlag(EXTI_EINT1);  // Limpia flag de int. por EINT1.

	return;
}


