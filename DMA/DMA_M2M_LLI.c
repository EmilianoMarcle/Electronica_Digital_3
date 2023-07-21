/*
===============================================================================
 Name        : DMA_M2M_LLI.c
 Author      : Marclé, Emiliano
 Version     : 1.0
 Description : El programa realiza transferencias DMA M2M utilizando listas enlazadas.
 	 	 	   La transferencia de Memoria a Memoria se dispara
===============================================================================
*/

#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpdma.h"
#include "lpc17xx_adc.h"

/* ----------- Macros -------------- */
#define DMA_SWAP_SIZE 10
#define TABLE_LEN 1034

/* ----- Prototipos de función ----- */
void confDMA(void);
void confADC(void);

/* ------- Variables globales ------ */

/* Estructuras para configuración DMA */
GPDMA_LLI_Type DMA_LLI_Struct1;
GPDMA_LLI_Type DMA_LLI_Struct2;
GPDMA_LLI_Type DMA_LLI_Struct3;
GPDMA_Channel_CFG_Type GPDMACfg;

uint32_t table[TABLE_LEN];	// Tabla con valores de 0 a 1034.
uint32_t random[2];			// Arreglo de dos valores aleatorios
uint32_t aux_buffer[DMA_SWAP_SIZE];	// Buffer auxiliar de 10 valores
uint32_t bitcount;

/* ------- Programa principal ------ */
int main(void)
{
	/* Inicialización del arreglo con valores aleatorios */
	random[0]=545;
	random[1]=433;
	/* Inicialización de variable bitcount */
	bitcount=0;

	/* Prepare the table */
	for(uint32_t i=0 ; i<TABLE_LEN ; i++)
	{
		table[i] = i;
	}

	/* ADC configuration */
	confADC();

	while (1);
	return 0;
}

/* --- DMA configuration function --- */
void confDMA(void){

	/* Prepare DMA linked list item structures */
	DMA_LLI_Struct1.SrcAddr= (uint32_t)table+4*random[0];
	DMA_LLI_Struct1.DstAddr= (uint32_t)aux_buffer;
	DMA_LLI_Struct1.NextLLI= (uint32_t)&DMA_LLI_Struct2;
	DMA_LLI_Struct1.Control= DMA_SWAP_SIZE
							| (2<<18) //source width 32 bit
							| (2<<21) //dest. width 32 bit
							| (1<<26) //source increment
							| (1<<27); //dest increment

	DMA_LLI_Struct2.SrcAddr= (uint32_t)table+4*random[1];
	DMA_LLI_Struct2.DstAddr= (uint32_t)table+4*random[0];
	DMA_LLI_Struct2.NextLLI= (uint32_t)&DMA_LLI_Struct3;
	DMA_LLI_Struct2.Control= DMA_SWAP_SIZE
							| (2<<18) //source width 32 bit
							| (2<<21) //dest. width 32 bit
							| (1<<26) //source increment
							| (1<<27); //dest increment

	DMA_LLI_Struct3.SrcAddr= (uint32_t)aux_buffer;
	DMA_LLI_Struct3.DstAddr= (uint32_t)table+4*random[1];
	DMA_LLI_Struct3.NextLLI= 0;
	DMA_LLI_Struct3.Control= DMA_SWAP_SIZE
							| (2<<18)  //source width 32 bit
							| (2<<21)  //dest. width 32 bit
							| (1<<26)  //source increment
							| (1<<27); //dest increment

	/* Initialize GPDMA controller */
	GPDMA_Init();

	/* Setup GPDMA channel */
	// Channel 7
	GPDMACfg.ChannelNum = 7;
	// Source memory
	GPDMACfg.SrcMemAddr = DMA_LLI_Struct1.SrcAddr;
	// Destination memory -
	GPDMACfg.DstMemAddr = DMA_LLI_Struct1.DstAddr;
	// Transfer size
	GPDMACfg.TransferSize = DMA_SWAP_SIZE;
	// Transfer width - word
	GPDMACfg.TransferWidth = GPDMA_WIDTH_WORD;
	// Transfer type
	GPDMACfg.TransferType = GPDMA_TRANSFERTYPE_M2M;
	// Source connection - unused
	GPDMACfg.SrcConn = 0;
	// Destination connection - unused
	GPDMACfg.DstConn = 0;
	// Linker List Item
	GPDMACfg.DMALLI = (uint32_t)&DMA_LLI_Struct2;
	// Setup channel with given parameter
	GPDMA_Setup(&GPDMACfg);

	return;
}

/* --- ADC Configuration function ---
 * Canal 0 con Frec. muestreo de aprox. 200kHz en modo Burst
 * Interrupción habilitada.
 */

void confADC(void){
	LPC_PINCON->PINSEL1 |= (1<<14); // Pin 0.23 como AD0.0
	LPC_SC->PCONP |= (1 << 12);  // Se enciende el ADC.
	LPC_ADC->ADCR |= (1 << 21);  // Habilitación del ADC. Bit PDN
	LPC_SC->PCLKSEL0 |= (3<<24); // CCLK/8
	LPC_ADC->ADCR &=~(255 << 8); // [15:8] CLKDIV
	LPC_ADC->ADCR |= (1 << 16);  // Burst mode
	LPC_PINCON->PINMODE1 |= (1<<15); //neither pull-up nor pull-down.
	LPC_ADC->ADINTEN &= ~(1<<8);	// Interrupción en canales individuales,
	LPC_ADC->ADINTEN = 1;	// Habilita interrupción en canal 0
	NVIC_SetPriority(ADC_IRQn, 3);
	NVIC_EnableIRQ(ADC_IRQn);
	return;
}

/* ------ ADC Handler ------ */
void ADC_IRQHandler(void) {
	static uint16_t ADC0Value = 0;	//Variable auxiliar para observar el valor del registro de captura

	/* Guarda el LSB del valor convertido en una variable */
	ADC0Value = ((LPC_ADC->ADDR0)>>4) & 0x001;

	if(bitcount< 10){
		/* Guarda 10 valores convertidos (0 o 1) en random[0] */
		if(bitcount==0){
			random[0]=0;
		}
		random[0]=(random[0]<<1)|ADC0Value;
		bitcount++;
	}else{
		/* Guarda 10 valores convertidos (0 o 1) en random[1] */
		if(bitcount==10){
			random[1]=0;
		}
		random[1]=(random[1]<<1)|ADC0Value;
		bitcount++;

		/* Si ya se guardaron 10 valores en random[0] y random[1] (20 conversiones)
		 * se reinicia el bitcount, y calcula el modulo 1024 de los 10 bits.
		 * Si todos los bits convertidos fueron 1, el modulo 1024 dará resultado 0
		 * Si no todos los bis fueron 0, el modulo 1024 será distinto a 0.
		 * Se configura el DMA y se habilita el canal, comenzando la transferencia de Memoria a Memoria.
		 *
		 */
		if(bitcount==20){
			bitcount=0;
			random[1]=random[1]%1024;
			random[0]=random[0]%1024;

			/* Disable DMA Interrupt */
			NVIC_DisableIRQ(DMA_IRQn);

			/* DMA Configuration */
			confDMA();

			/* Enable GPDMA channel 7 */
			GPDMA_ChannelCmd(7, ENABLE);
		}
	}
	return;
}
