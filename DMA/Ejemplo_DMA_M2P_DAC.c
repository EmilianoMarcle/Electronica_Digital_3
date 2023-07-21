/*
===============================================================================
 Name        : Ej_DAC_DMA.c
 Author      : Marcle Emiliano
 Version     : 1.0
 Description : En este programa se utiliza el DAC para sacar una onda senoidal de periodo 20 mseg (50hz).
 	 	 	   Mediante DMA se pasan las muestras guardadas en memoria al DAC (Transferencia M2P).
===============================================================================
*/

#include "LPC17xx.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpdma.h"
#include "lpc17xx_dac.h"

/* --------- Macros ---------- */
#define   DMA_SIZE  60
#define   NUM_SINE_SAMPLE  60
#define   SINE_FREQ_IN_HZ  50
#define   PCLK_DAC_IN_MHZ  25

/* ------- Prototipos -------- */
void confPin(void);
void confDMA(void);
void confDac(void);

/* ---- Variables globales ---- */
GPDMA_Channel_CFG_Type GPDMACfg;
uint32_t dac_sine_lut[NUM_SINE_SAMPLE]; // El compilador define en que lugar de memoria estará el arreglo.

/* ---- Programa principal ----- */
int main(void) {
	/* Variables locales */
	uint32_t i;
 	uint32_t sin_0_to_90_16_samples [16] = {		// Muestras del cuarto de onda senoidal
			0, 1045, 2079, 3090, 4067,
			5000, 5877, 6691, 7431, 8090,
			8660, 9135, 9510, 9781, 9945, 10000
	};

 	/* Configuración de pines. */
	confPin();

	/* Configuración de DAC. */
	confDac();

	/* Prepare DAC sine look up table
	 * La onda tiene un valor medio de 512 y un máximo de 1023
	 */

	for(i=0 ; i<NUM_SINE_SAMPLE ; i++){
		if (i<=15){
			dac_sine_lut[i] = (512 + 512*sin_0_to_90_16_samples[i]/10000);
			if(i==15) dac_sine_lut[i] = 1023;
		}
		else if(i<=30){
			dac_sine_lut[i] = (512 + 512*sin_0_to_90_16_samples[30-i]/10000);
		}
		else if(i<=45){
			dac_sine_lut[i] = (512 - 512*sin_0_to_90_16_samples[i-30]/10000);
		}
		else{
			dac_sine_lut[i] = (512 - 512*sin_0_to_90_16_samples[60-i]/10000);
		}
		dac_sine_lut[i] = (dac_sine_lut[i]<<6);
	}

	/* Configuración DMA */
	confDMA();

	/* Enable GPDMA channel 0. */
	GPDMA_ChannelCmd( 0 , ENABLE);

    while(1) {
    }
    return 0 ;
}

/* Función de configuración de pines */
void confPin(void){
	PINSEL_CFG_Type PinCfg;						// P0.26 como AOUT

	PinCfg.Funcnum = PINSEL_FUNC_2;
	PinCfg.Portnum = PINSEL_PORT_0;
	PinCfg.Pinnum  = PINSEL_PIN_26;
	PinCfg.Pinmode = PINSEL_PINMODE_PULLUP;
	PinCfg.OpenDrain = PINSEL_PINMODE_NORMAL;

	PINSEL_ConfigPin(&PinCfg);
	return;
}

/* Función de configuración del DAC.
 * - Configura el DAC para funcionar con el timer del DMA/Interupt
 * - El BIAS del DAC esta seteado en 1us 750uA*/
void confDac(void){
	uint32_t tmp;

	/* Configuración DACCTRL */
	DAC_CONVERTER_CFG_Type DAC_ConverterConfigStruct;

	DAC_ConverterConfigStruct.CNT_ENA = SET;
	DAC_ConverterConfigStruct.DMA_ENA = SET;

	DAC_ConfigDAConverterControl(LPC_DAC, &DAC_ConverterConfigStruct);

	/* Set Time Out for DMA
	 * Este es el intervalo de tiempo en el cual se envían datos de memoria al DAC
	 */
	tmp = (PCLK_DAC_IN_MHZ*1000000)/(SINE_FREQ_IN_HZ * NUM_SINE_SAMPLE);
	DAC_SetDMATimeOut(LPC_DAC,tmp);		// Tiempo desborde DAC.

	/* Inicialización DAC */
	DAC_Init(LPC_DAC);

	return;
}

/* Función de configuración GPDMA
 * - Se define una lista vinculada y luego se configura el DMA-
 * - Cada lista transfiere datos de un lugar de la memoria a otro de forma cotinua.
 */
void confDMA(void){

	/* Estructura para configuracción de LLI */
	GPDMA_LLI_Type DMA_LLI_Struct;

	/* Prepare DMA Linked List item structure */
	DMA_LLI_Struct.SrcAddr = (uint32_t) dac_sine_lut;
	DMA_LLI_Struct.DstAddr = (uint32_t) &(LPC_DAC->DACR); // Dirección de memoria del DAC.
	DMA_LLI_Struct.NextLLI = (uint32_t) & DMA_LLI_Struct; // La proxima lista a enviar es la misma. Forma onda periodica.
	DMA_LLI_Struct.Control = DMA_SIZE // Tamaño de la transferencia DMA.
						   | (2<<18)  // source width 32bits
						   | (2<<21)  // dest. width 32bits
						   | (1<<26); // source increment after each transfer.

	/*Initialize GPDMA Controller*/
	GPDMA_Init(); // Habilita Clock DMA, Resetea la configuración del canal, Limpia flags de int. por DMA y Error.

	/*Setup GPDMA Channel */
	// channel 0
	GPDMACfg.ChannelNum = 0;
	// source memory
	GPDMACfg.SrcMemAddr = (uint32_t) (dac_sine_lut);
	// Destination memory
	GPDMACfg.DstMemAddr = 0; // Como estoy usando un perisferico no se usa.
	// Transfer Size
	GPDMACfg.TransferSize = DMA_SIZE;
	// Transfer Width
	GPDMACfg.TransferWidth = 0;
	// Transfer Type
	GPDMACfg.TransferType = GPDMA_TRANSFERTYPE_M2P;
	//  Source connection
	GPDMACfg.SrcConn = 0;
	// Destination connection
	GPDMACfg.DstConn = GPDMA_CONN_DAC;
	// Linker List Item
	GPDMACfg.DMALLI = (uint32_t) &DMA_LLI_Struct;

	/* Seteo del canal con la configuración realizada */
	GPDMA_Setup(&GPDMACfg);

	return;
}
