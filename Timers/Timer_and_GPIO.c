/*
===============================================================================
 Nombre      : Marclé Emiliano
 Descripcion :
===============================================================================
*/

#include "LPC17xx.h"

//Include de libreria
#include "lpc17xx_timer.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"

//Definiciones
#define INPUT		((uint8_t)	0)
#define OUTPUT		((uint8_t)	1)
#define PIN_22		((uint32_t)(1<<22))
#define PORT_ZERO	((uint8_t)  0)
#define PORT_ONE	((uint8_t)  1)
#define PORT_TWO	((uint8_t)  2)
#define PORT_THREE  ((uint8_t)  3)

//Prototipos de funciones
void config_GPIO(void);
void config_timer(void);

//Programa Principal
int main(void) {

	config_GPIO();
	config_timer();
	GPIO_SetValue(PORT_ZERO,PIN_22);

    while(1) {
    }
    return 0 ;
}

void config_GPIO(void){
	/* Variable de tipo estructura PINSEL */
	PINSEL_CFG_Type pin_configuration;

	/* Configuración de los miembros de la estructura */
	pin_configuration.Portnum   = PINSEL_PORT_0;
	pin_configuration.Pinnum    = PINSEL_PIN_22;
	pin_configuration.Pinmode   = PINSEL_PINMODE_PULLUP;
	pin_configuration.Funcnum   = PINSEL_FUNC_0;
	pin_configuration.OpenDrain = PINSEL_PINMODE_NORMAL;

	/* Carga de valores en la estructura*/
	PINSEL_ConfigPin(&pin_configuration);

	/* Configura P0.22 como salida. */
	GPIO_SetDir(PORT_ZERO, PIN_22, OUTPUT);

	return;
}

void config_timer(void){
	TIM_TIMERCFG_Type struct_config;			// Estructura TIMERCFG
	TIM_MATCHCFG_Type struct_match;				// Estructura MATCH

	struct_config.PrescaleOption = TIM_PRESCALE_USVAL;	// Configuracion prescaler en us
	struct_config.PrescaleValue  = 100;					// 100us

	struct_match.MatchChannel    	= 0;
	struct_match.IntOnMatch      	= ENABLE;
	struct_match.ResetOnMatch    	= ENABLE;
	struct_match.StopOnMatch     	= DISABLE;
	struct_match.ExtMatchOutputType = TIM_EXTMATCH_NOTHING;
	struct_match.MatchValue			= 9999;

	TIM_Init(LPC_TIM0, TIM_TIMER_MODE, &struct_config); // Con esta funcion se enciende
														// el Timer.
	//LPC_SC->PCONP |= (0<<1); //Apaga TIM0.
	TIM_ConfigMatch(LPC_TIM0, &struct_match);

	TIM_Cmd(LPC_TIM0,ENABLE);

	NVIC_EnableIRQ(TIMER0_IRQn);

	return;
}

void TIMER0_IRQHandler(void){
	TIM_ClearIntPending(LPC_TIM0,TIM_MR0_INT);

	if((GPIO_ReadValue(PORT_ZERO)) & PIN_22){
		GPIO_ClearValue(PORT_ZERO, PIN_22);
	}
	else{
		GPIO_SetValue(PORT_ZERO, PIN_22);
	}

	return;
}
