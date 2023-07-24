/*
===============================================================================
 Name        : Ejercicio_Timer_Match3.c
 Author      : Emiliano Marclé
 Version     : 1.0
 Description : Escribir un programa para que por cada presión de un pulsador,
 	 	 	   la frecuencia de parpadeo disminuya a la mitad debido a la
 	 	 	   modificación del registro del Match 0. El pulsador debe producir
 	 	 	   una interrupción por EINT2 con flanco descendente.
===============================================================================
*/
#include "LPC17xx.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_exti.h"
#include "lpc17xx_timer.h"

/* ------ Macros ------ */
#define  PR_VAL  ((uint32_t)  250000)
#define  MR_VAL         ((uint8_t)  1)

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
	confTimer();		// Configuración de Timer2.

	/* Habilita el Timer2 */
	TIM_Cmd(LPC_TIM2, ENABLE);

    while(1) {
    }

    return 0 ;
}

/* Función de configuración EINT2 */
void confIntExt(void){

	/* Configura P2.12 como EINT2 */
	PinConfig.Portnum = PINSEL_PORT_2;
	PinConfig.Pinnum = PINSEL_PIN_12;
	PinConfig.Funcnum = PINSEL_FUNC_1;
	PinConfig.Pinmode = PINSEL_PINMODE_PULLUP;
	PinConfig.OpenDrain = PINSEL_PINMODE_NORMAL;
	PINSEL_ConfigPin(&PinConfig);

	/* Interrupción por flanco de bajada en EINT2. */
	EXTI_SetMode(EXTI_EINT2, EXTI_MODE_EDGE_SENSITIVE);
	EXTI_SetPolarity(EXTI_EINT2, EXTI_POLARITY_LOW_ACTIVE_OR_FALLING_EDGE);
	EXTI_ClearEXTIFlag(EXTI_EINT2);

	NVIC_EnableIRQ(EINT2_IRQn);

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
	TimerConfig.PrescaleValue = PR_VAL;  // 0,5 seg.

	MatchConfig.MatchChannel = 0;
	MatchConfig.IntOnMatch = DISABLE;
	MatchConfig.StopOnMatch = DISABLE;
	MatchConfig.ResetOnMatch = ENABLE;
	MatchConfig.ExtMatchOutputType = TIM_EXTMATCH_TOGGLE;
	MatchConfig.MatchValue = MR_VAL;

	/* Inicialización del TIM2 como Timer */
	TIM_Init(LPC_TIM2, TIM_TIMER_MODE, &TimerConfig);

	/* Configuración del Match0 */
	TIM_ConfigMatch(LPC_TIM2, &MatchConfig);

	return;
}

void EINT2_IRQHandler(void){
	static uint32_t match = MR_VAL;

	/* Disminuye la frecuencia del parpadeo a la mitad duplicando el MR0 */
	match *= 2;
	MatchConfig.MatchValue = match;
	TIM_ConfigMatch(LPC_TIM2, &MatchConfig);
	EXTI_ClearEXTIFlag(EXTI_EINT2);  // Limpia flag de int. por EINT1.

	return;
}
