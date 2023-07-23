/*
===============================================================================
 Name        : Ejemplo_UART_TX_Int.c
 Author      : Marclé Emiliano
 Version     : 1.0
 Description :  Transmisión UART2 usando interrupción por THRE. 
===============================================================================
*/

#include "LPC17xx.h"
#include "lpc17xx_uart.h"
#include "lpc17xx_pinsel.h"

/* ----- Prototipos de funciones ----- */
void confPin(void);
void confUart(void);
void UART2_IRQHandler(void);

/* ------- Variables globales -------- */
uint8_t info1[] = {0xC};		// Page break.
uint8_t info2[] = "FCEFyN - UNC 2021 - Digital 3\n\r";    // 32 caracteres.

/* -------- Programa principal ------- */
int main (void){
	confPin();		// Configuración de pines
	confUart();		// Configuración UART2

	/* Transmite el caracter Page break.
	 * - NONE_BLOCKING : Solo se envía si esta vacía la THR FIFO
	 */
	UART_Send(LPC_UART2, info1, sizeof(info1), NONE_BLOCKING);

	while(1){}

	return 0;
}

/* ----- Función de configuración de Pines ----- */
void confPin(void){
	/* Configuración pin de TXD2 (P0.10) y RXD2 (P0.11) */
	PINSEL_CFG_Type PinCfg;
	PinCfg.Funcnum = PINSEL_FUNC_1;
	PinCfg.OpenDrain = PINSEL_PINMODE_NORMAL;
	PinCfg.Pinmode = PINSEL_PINMODE_PULLUP;
	PinCfg.Pinnum = PINSEL_PIN_10;
	PinCfg.Portnum = PINSEL_PORT_0;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = PINSEL_PIN_11;
	PINSEL_ConfigPin(&PinCfg);
	return;
}

/* ----- Función de configuración UART ----- */
void confUart(void){

	/* Configuración e inicialización del UART por defecto
	 * - BR = 9600
	 * - 8 bits
	 * - No Parity
	 * - 1 stop bit
	 */
	UART_CFG_Type UARTConfigStruct;
	UART_ConfigStructInit(&UARTConfigStruct);
	UART_Init(LPC_UART2, &UARTConfigStruct);

	/* Configuración e inicialización de la FIFO
	 * - DMA Disable
	 * - FIFO TRGLVL0 (1 character)
	 * - Reset FIFO TX Buffer
	 * - Reset FIFO RX Buffer
	 */
	UART_FIFO_CFG_Type UARTFIFOConfigStruct;
	UART_FIFOConfigStructInit(&UARTFIFOConfigStruct);
	UART_FIFOConfig(LPC_UART2, &UARTFIFOConfigStruct);

	/* Habilita transmisión en pin TXD */
	UART_TxCmd(LPC_UART2, ENABLE);

	/* Habilita interrucpción THRE */
	UART_IntConfig(LPC_UART2, UART_INTCFG_THRE, ENABLE);

	/* Habilita interrupcion por UART2 en NVIC*/
	NVIC_EnableIRQ(UART2_IRQn);

	return;
}

/* ----- Handler de UART2 ----- */
void UART2_IRQHandler(void){
	uint32_t intsrc;
	uint16_t bSent;				// Variable que guarda cantidad de bytes enviados.
	static uint16_t conta = 0;  // Contador de bytes enviados.

	/* Determina la fuente de interrupcion */
	intsrc = UART_GetIntId(LPC_UART2) & UART_IIR_INTID_MASK;  // intsrc * 0b1110;

	/* Evalúa si Transmit Holding Register está vacio
	 * Si la interrupción fue por TRHE, el THR está vacío y se puede transmitir la cadena.
	 */
	if (intsrc == UART_IIR_INTID_THRE){
		/* Se transmiten los 32 bytes de info 2.
		 * En cada transmisión se envían 16 bytes.
		 * Luego de enviar los 32 bytes, se desactiva la interrupción por THRE
		 */
		if(conta < sizeof(info2)){
			bSent = UART_Send(LPC_UART2, info2+conta, sizeof(info2), NONE_BLOCKING);
			conta += bSent;
		}
		else{
			UART_IntConfig(LPC_UART2, UART_INTCFG_THRE, DISABLE);
		}
	}

	return;
}
