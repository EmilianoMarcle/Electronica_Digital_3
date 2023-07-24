/*
===============================================================================
 Name        : Ejercicio_Drv_Timer_Match1.c
 Author      : Marclé, Emiliano
 Version     : 1.0
 Description : Escribir el código que configure el timer0 para cumplir con las especificaciones
               dadas en la figura adjunta. (Pag 510 Figura 115 del manual de usuario del LPC 1769).
 	 	 	  - Frecuencia de cclk de 100 Mhz y una división de reloj de periférico de 2.
===============================================================================
*/

#include "LPC17xx.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_timer.h"

/* ---------- Macros ---------- */
#define PORT_0  ((uint8_t) 0)
#define PIN_22  ((uint32_t) (1<<22))
#define OUTPUT  ((uint8_t) 1)

/* -------- Prototipos ---------*/
void confGPIO(void);
void confTimer(void);

/* ---- Programa Principal ---- */
int main(void) {
	confGPIO();
	confTimer(); // Configuración de TIM0.

	while(1){
	}

	return 0;
}

void confGPIO(void){
	PINSEL_CFG_Type PinCfg;
	PinCfg.Portnum = PINSEL_PORT_0;
	PinCfg.Pinnum = PINSEL_PIN_22;
	PinCfg.Funcnum = PINSEL_FUNC_0;
	PinCfg.Pinmode = PINSEL_PINMODE_PULLUP;
	PinCfg.OpenDrain = PINSEL_PINMODE_NORMAL;
	PINSEL_ConfigPin(&PinCfg);

	GPIO_SetDir(PORT_0, PIN_22, OUTPUT);
	GPIO_SetValue(PORT_0, PIN_22);
	return;
}
void confTimer(void){

	/* Estructura de configuracion del TIM0 */
	TIM_TIMERCFG_Type TimerStructCfg;
	TimerStructCfg.PrescaleOption = TIM_PRESCALE_TICKVAL;
	TimerStructCfg.PrescaleValue = 3;  // PrescaleValue = TICKVAL = PR + 1;

	/* Estructura de configuración del Match0
	 * - Reset en Match0
	 * - Interrupción en Match0
	 */
	TIM_MATCHCFG_Type MatchStructCfg;
	MatchStructCfg.MatchChannel = 0;
	MatchStructCfg.IntOnMatch = ENABLE;
	MatchStructCfg.StopOnMatch = DISABLE;
	MatchStructCfg.ResetOnMatch = ENABLE;
	MatchStructCfg.ExtMatchOutputType = TIM_EXTMATCH_NOTHING;
	MatchStructCfg.MatchValue = 6;  // MR = 6;

	/* Inicialización el TIM0
	 * Enciende periférico
	 * PCLK = CCLK/4
	 */
	TIM_Init(LPC_TIM0, TIM_TIMER_MODE, &TimerStructCfg);

	// Cambia el Clock a PCLK = CCLK/2 (10);
	LPC_SC->PCLKSEL0 &= ~ (1<<2) ;
	LPC_SC->PCLKSEL0 |= (1<<3) ;

	/* Configuración del Match */
	TIM_ConfigMatch(LPC_TIM0, &MatchStructCfg);

	/* Resetea y Habilita el TC y PC */
	TIM_Cmd(LPC_TIM0, ENABLE);


	/* Habilitación de interrupción en NVIC */
	NVIC_EnableIRQ(TIMER0_IRQn);

	return;
}

void TIMER0_IRQHandler(void){
	static uint8_t count = 0;

	if(TIM_GetIntStatus(LPC_TIM0, TIM_MR0_INT)){

		TIM_ClearIntPending(LPC_TIM0, TIM_MR0_INT);		// Limpia interrupción por Match0.

		if(count){
			GPIO_SetValue(PORT_0, PIN_22);
			}
			else{
				GPIO_ClearValue(PORT_0, PIN_22);
			}
			count ^= 1;
	}

	return;
}
