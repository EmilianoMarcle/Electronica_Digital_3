/*
===============================================================================
 Alumno:  Marclé Emiliano
 Carrera: Ing. Electronica
 Descripcion : Utilizando el modo Capture, escribir un código en C  para que guarde
               en una variable ,llamada "shooter", el tiempo (en milisegundos) que
               le lleva a una persona presionar dos pulsadores con un único dedo.
===============================================================================
*/

#include "LPC17xx.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_timer.h"

/* ----- Macros ------ */
#define PR_VAL ((uint32_t)  100000)  // Prescaler para contar 1ms con pclk=100MHz

/* ----- Prototipos de funciones ------ */
void confTimer(void);

/* ----- Variable globales ----- */
uint32_t val1 = 0;
uint32_t val2 = 0;
uint32_t shooter;
uint8_t count = 0;
PINSEL_CFG_Type PinConfig;
TIM_TIMERCFG_Type TimerConfig;
TIM_CAPTURECFG_Type CaptureConfig;

/* ----- Programa principal ----- */
int main(void) {

	confTimer();	// Configuración de TIM0.
    while(1) {

    	/* Realiza el cálculo solo cuando count=1 */
    	if(count){
    		shooter = (val2-val1);
    		val1=0;
    		val2=0;
    		count = 0;
    	}
    }

    return 0 ;
}

/* Función de configuración TIM0 en modo capture */
void confTimer(void){

	/* Configuración de 1.26 como CAP0.0 y P1.27 como CAP0.1 */
	PinConfig.Portnum = PINSEL_PORT_1;
	PinConfig.Pinnum = PINSEL_PIN_26;
	PinConfig.Funcnum = PINSEL_FUNC_3;
	PinConfig.Pinmode = PINSEL_PINMODE_PULLUP;
	PinConfig.OpenDrain = PINSEL_PINMODE_NORMAL;
	PINSEL_ConfigPin(&PinConfig);

	PinConfig.Portnum = PINSEL_PORT_1;
	PinConfig.Pinnum = PINSEL_PIN_27;
	PinConfig.Funcnum = PINSEL_FUNC_3;
	PinConfig.Pinmode = PINSEL_PINMODE_PULLUP;
	PinConfig.OpenDrain = PINSEL_PINMODE_NORMAL;
	PINSEL_ConfigPin(&PinConfig);

	/* Configuración Timer */
	TimerConfig.PrescaleOption = TIM_PRESCALE_TICKVAL;
	TimerConfig.PrescaleValue = PR_VAL;

	/* Incialización del Timer */
	TIM_Init(LPC_TIM0, TIM_TIMER_MODE, &TimerConfig);
	LPC_SC->PCLKSEL0 &= (1<<2);  // pclk = cclk.

	/* Configuración CAP0 y CAP1
	 * - Captura en flanco de bajada.
	 * - Interrupción en la captura.
	 */
	CaptureConfig.CaptureChannel = 0;
	CaptureConfig.RisingEdge = DISABLE;
	CaptureConfig.FallingEdge = ENABLE;
	CaptureConfig.IntOnCaption = ENABLE;
	TIM_ConfigCapture(LPC_TIM0, &CaptureConfig);

	CaptureConfig.CaptureChannel = 1;
	TIM_ConfigCapture(LPC_TIM0, &CaptureConfig);

	/* Habilitación de Timer */
	TIM_Cmd(LPC_TIM0, ENABLE);

	NVIC_EnableIRQ(TIMER0_IRQn);  // Habilita interrupción en NVIC.

	return;
}

/* Handler TIM0 */
void TIMER0_IRQHandler(void){
	static uint8_t flag = 0;

	/* Interrupción por CR0 */
	if(TIM_GetIntStatus(LPC_TIM0, TIM_CR0_INT)){
		/* Se presiona primero pulsador en CAP0.0 */
		if(flag == 0){
			val1 = TIM_GetCaptureValue(LPC_TIM0, TIM_COUNTER_INCAP0);
			flag = 2;
		}
		/* Se presiona segundo el pulsador en CAP0.0 */
		else if(flag == 1){
			val2 = TIM_GetCaptureValue(LPC_TIM0, TIM_COUNTER_INCAP0);
			TIM_ResetCounter(LPC_TIM0); // Reset del TC y PC
			flag = 0;
			count = 1;
		}
		TIM_ClearIntPending(LPC_TIM0, TIM_CR0_INT); // Limpia flag de interrupción.
		return;
	}

	/* Interrupción por CR1 */
	if(TIM_GetIntStatus(LPC_TIM0, TIM_CR1_INT)){
		/* Se presiona primero pulsador en CAP0.1 */
		if(flag == 0){
			val1 = TIM_GetCaptureValue(LPC_TIM0, TIM_COUNTER_INCAP1);
			flag = 1;
		}
		/* Se presiona segundo el pulsador en CAP0.1 */
		else if(flag == 2){
			val2 = TIM_GetCaptureValue(LPC_TIM0, TIM_COUNTER_INCAP1);
			TIM_ResetCounter(LPC_TIM0); // Reset del TC y PC
			flag = 0;
			count = 1;

		}
		TIM_ClearIntPending(LPC_TIM0, TIM_CR1_INT); // Limpia flag de interrupción.
		return;
	}
}
