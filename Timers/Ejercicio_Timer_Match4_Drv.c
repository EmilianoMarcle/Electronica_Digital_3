/*
===============================================================================
 Nombre        : Marclé, Emiliano
 Carrera	   : Ing. Electronica
 Descripcion   : Utilizando los 4 registros match del Timer 0 y las salidas
 	 	 	 	 P0.0, P0.1, P0.2 y P0.3, realizar un programa en C que permita
 	 	 	 	 obtener las formas de ondas adjuntas, donde los pulsos en alto
 	 	 	 	  tienen una duración de 5 mseg.
 	 	 	 	 Un pulsador conectado a la entrada EINT3, permitirá elegir entre
 	 	 	 	 las dos secuencias mediante una rutina de servicio a la interrupción.
 	 	 	 	 La prioridad de la interrupción del Timer tiene que ser mayor que
 	 	 	 	 la del pulsador.
===============================================================================
*/

#include "LPC17xx.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_exti.h"
#include "lpc17xx_timer.h"


/* ----- Macros ------ */
//#define PR_VAL  ((uint32_t) 1000) // 5 mseg
#define PR_VAL  ((uint32_t) 100000) // 0,5 seg para poder visualizar secuencia.
#define MR0_VAL ((uint8_t)  5)
#define MR1_VAL ((uint8_t)  10)
#define MR2_VAL ((uint8_t)  15)
#define MR3_VAL ((uint8_t)  20)
#define PORT_0  ((uint8_t)  0)
#define OUTPUT  ((uint8_t)  1)
#define PIN_0   ((uint8_t)  (1<<0))
#define PIN_1   ((uint8_t)  (1<<1))
#define PIN_2   ((uint8_t)  (1<<2))
#define PIN_3   ((uint8_t)  (1<<3))

/* ----- Prototipos de funciones ---- */
void confGPIO(void);
void confIntExt(void);
void confTimer(void);
void antirebote(void);

/* ----- Variables globales ----- */
uint8_t sec = 0;    // Flag de secuencia (sec=0->Secuencia 1, sec=1->Secuencia 2)
PINSEL_CFG_Type PinConfig;
TIM_TIMERCFG_Type TimerConfig;
TIM_MATCHCFG_Type MatchConfig;

/* ----- Programa principal ----- */
int main(void) {

	confGPIO();		// Configuración de GPIO.
	confIntExt();   // Configuración EINT3.
	confTimer();    // Configuración TIM0

	while(1){}

    return 0 ;
}

/* Función de configuración GPIO */
void confGPIO(void){

	/* Configura P0.0 a P0.3 como GPIO con PULLUP */
	PinConfig.Portnum = PINSEL_PORT_0;
	PinConfig.Funcnum = PINSEL_FUNC_0;
	PinConfig.Funcnum = PINSEL_PINMODE_PULLUP;
	PinConfig.OpenDrain = PINSEL_PINMODE_NORMAL;

	for(uint8_t i=0 ; i<4 ; i++){
		PinConfig.Pinnum = i;
		PINSEL_ConfigPin(&PinConfig);
	}

	/* P0.0 a P0.3 como salidas */
	GPIO_SetDir(PORT_0, 0xF, OUTPUT);
	GPIO_ClearValue(PORT_0, 0xF);

	return;
}

/* Función de configuración EINT3 */
void confIntExt(void){

	/* Pin 2.13 como EINT3 */
	PinConfig.Portnum = PINSEL_PORT_2;
	PinConfig.Pinnum = PINSEL_PIN_13;
	PinConfig.Funcnum = PINSEL_FUNC_1;
	PinConfig.Pinmode = PINSEL_PINMODE_PULLUP;
	PinConfig.OpenDrain = PINSEL_PINMODE_NORMAL;
	PINSEL_ConfigPin(&PinConfig);

	/* EINT3 por flanco de bajada */
	EXTI_SetMode(EXTI_EINT3, EXTI_MODE_EDGE_SENSITIVE);
	EXTI_SetPolarity(EXTI_EINT3, EXTI_POLARITY_LOW_ACTIVE_OR_FALLING_EDGE);
	EXTI_ClearEXTIFlag(EXTI_EINT3);

	/* Configura EINT3 con prioridad mas baja y habilita la interrupción */
	NVIC_SetPriority(EINT3_IRQn, 10);
	NVIC_EnableIRQ(EINT3_IRQn);

	return;
}

/* Función de configuración Timer */
void confTimer(void){

	/* Configuració de TIM0*/
	TimerConfig.PrescaleOption = TIM_PRESCALE_USVAL;
	TimerConfig.PrescaleValue = PR_VAL;

	/* Configuración del Match */
	MatchConfig.IntOnMatch = ENABLE;
	MatchConfig.StopOnMatch = DISABLE;
	MatchConfig.ExtMatchOutputType = TIM_EXTMATCH_NOTHING;

	/* MR0, MR1 y MR2 sin ResetOnMatch*/
	for(uint8_t i=0 ; i<3 ; i++){
		MatchConfig.MatchChannel = i;
		MatchConfig.ResetOnMatch = DISABLE;
		MatchConfig.MatchValue = (MR0_VAL-1) + i*(MR0_VAL); // MR0=4, MR1=9, MR2=14.
		TIM_ConfigMatch(LPC_TIM0, &MatchConfig);
	}

	MatchConfig.MatchChannel = 3;
	MatchConfig.ResetOnMatch = ENABLE;
	MatchConfig.MatchValue = (MR3_VAL-1);  // MR3=19.
	TIM_ConfigMatch(LPC_TIM0, &MatchConfig);

	/* Inicialización de Timer */
	TIM_Init(LPC_TIM0, TIM_TIMER_MODE, &TimerConfig);

	/* Configura int. por TIM0 con prioridad mas alta y la habilita. */
	NVIC_SetPriority(TIMER0_IRQn , 0);
	NVIC_EnableIRQ(TIMER0_IRQn);

	/* Habilita el Timer */
	TIM_Cmd(LPC_TIM0, ENABLE);

	return;
}

/* Handler EINT3*/
void EINT3_IRQHandler(void){
	EXTI_ClearEXTIFlag(EXTI_EINT3);  // Limpia flag de int. por EINT3.
	sec ^= 1;   // Cambia el estado del flag de secuencia.
	antirebote(); // Retardo antirebote;
	return;
}

/*Handler TIM0 */
void TIMER0_IRQHandler(void){

	switch(sec){
		/* Secuencia 1 */
		case 0:
			if(TIM_GetIntStatus(LPC_TIM0, TIM_MR0_INT)){
				FIO_ByteSetValue(PORT_0, 0 , PIN_0);
				FIO_ByteClearValue(PORT_0, 0 , ~PIN_0);
				TIM_ClearIntPending(LPC_TIM0, TIM_MR0_INT);
				break;
			}
			else if(TIM_GetIntStatus(LPC_TIM0, TIM_MR1_INT)){
				FIO_ByteSetValue(PORT_0, 0 , PIN_1);
				FIO_ByteClearValue(PORT_0, 0 , ~PIN_1);
				TIM_ClearIntPending(LPC_TIM0, TIM_MR1_INT);
				break;
			}
			else if(TIM_GetIntStatus(LPC_TIM0, TIM_MR2_INT)){
				FIO_ByteSetValue(PORT_0, 0 , PIN_2);
				FIO_ByteClearValue(PORT_0, 0 , ~PIN_2);
				TIM_ClearIntPending(LPC_TIM0, TIM_MR2_INT);
				break;
			}
			else if(TIM_GetIntStatus(LPC_TIM0, TIM_MR3_INT)){
				FIO_ByteSetValue(PORT_0, 0 , PIN_3);
				FIO_ByteClearValue(PORT_0, 0 , ~PIN_3);
				TIM_ClearIntPending(LPC_TIM0, TIM_MR3_INT);
				break;
			}

		/* Secuencia 2 */
		case 1:
			if(TIM_GetIntStatus(LPC_TIM0, TIM_MR0_INT)){
				FIO_ByteSetValue(PORT_0, 0 , (PIN_0|PIN_3));
				FIO_ByteClearValue(PORT_0, 0 , ~(PIN_0|PIN_3));
				TIM_ClearIntPending(LPC_TIM0, TIM_MR0_INT);
				break;
			}
			else if(TIM_GetIntStatus(LPC_TIM0, TIM_MR1_INT)){
				FIO_ByteSetValue(PORT_0, 0 , PIN_0|PIN_1);
				FIO_ByteClearValue(PORT_0, 0 , ~(PIN_0|PIN_1));
				TIM_ClearIntPending(LPC_TIM0, TIM_MR1_INT);
				break;
			}
			else if(TIM_GetIntStatus(LPC_TIM0, TIM_MR2_INT)){
				FIO_ByteSetValue(PORT_0, 0 , PIN_1|PIN_2);
				FIO_ByteClearValue(PORT_0, 0 , ~(PIN_1|PIN_2));
				TIM_ClearIntPending(LPC_TIM0, TIM_MR2_INT);
				break;
			}
			else if(TIM_GetIntStatus(LPC_TIM0, TIM_MR3_INT)){
				FIO_ByteSetValue(PORT_0, 0 , PIN_2|PIN_3);
				FIO_ByteClearValue(PORT_0, 0 , ~(PIN_2|PIN_3));
				TIM_ClearIntPending(LPC_TIM0, TIM_MR3_INT);
				break;
			}
	}

	return;
}

/* Función de retardo antirebote para pulsador */
void antirebote(void){
	uint32_t ticks = 10000;  // Valor ajustado para 1ms aproximadamente.
	for(uint32_t i=0 ; i<ticks ; i++){}
	return;
}
