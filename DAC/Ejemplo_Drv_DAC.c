/*
===============================================================================
 Name        : Ej_DAC.c
 Descripcion : Se sacan valores analógicos por pin P0.26 (AOUT) con un intervalo
  	  	  	   de tiempo definido por un retado.

 * Tiempo medido = 444ms
===============================================================================
*/

#include "LPC17xx.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_dac.h"

/* ---- Prototipos de funciones ---- */
void confPin(void);
void confDAC(void);
void delay(void);

/* ------- Programa Principal ------- */
int main(void) {
	uint32_t dac_value[11] = {0,123,223,323,423,523,623,723,823,923,1023};

	confPin();		// Configuración de pines
	confDAC();		// Configuración del DAC

    while(1) {

    	/* Se saca por el pin AOUT un valor
    	 * El retardo debe ser mayor o igual a 1us para lograr frec. de refresco menor a 1MHz.
    	 */
    	for(uint8_t conta = 0 ; conta<11 ; conta++){
    		DAC_UpdateValue(LPC_DAC, dac_value[conta]);
    		delay();
    	}
    }
    return 0 ;
}

/*
 * Función de configuración de Pines
 * - Configura P0.26 como AOUT
 */
void confPin(void){   //
	PINSEL_CFG_Type PinCfg;

	PinCfg.Portnum = PINSEL_PORT_0;
	PinCfg.Pinnum = PINSEL_PIN_26;
	PinCfg.Funcnum = PINSEL_FUNC_2;
	PinCfg.Pinmode = PINSEL_PINMODE_PULLUP;
	PinCfg.OpenDrain = PINSEL_PINMODE_NORMAL;

	PINSEL_ConfigPin(&PinCfg);
	return;
}

/*
 * Función de configuración del DAC
 * Se inicializa con máximo consumo (BIAS=0: 1us, 700uA)
 */
void confDAC(void){
	DAC_Init(LPC_DAC);
	return;
}

/* Función de Retardo */
void delay(void){
	for(uint32_t i=0 ; i<4000000 ; i++);
	return;
}
