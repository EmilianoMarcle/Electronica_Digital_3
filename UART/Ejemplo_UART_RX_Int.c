/*
===============================================================================
 Name        : Ejemplo_UART_RX_Int.c
 Author      : Marclé Emiliano
 Version     : 1.0
 Description : Recepción UART2 usando interrupciones.
===============================================================================
*/
#include "LPC17xx.h"
#include "lpc17xx_uart.h"
#include "lpc17xx_pinsel.h"

/* ----- Prototipos de funciones ----- */
void confPin(void);
void confUart(void);

/* ------- Variables globales -------- */
uint8_t info[1] = "";   // Buffer para recibir datos.

/* -------- Programa principal ------- */
int main (void){
	confPin();		// Configuración de pines
	confUart();		// Configuración UART2

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

	/* Habilita interrupción por el RX del UART */
	UART_IntConfig(LPC_UART2, UART_INTCFG_RBR, ENABLE);

	/* Habilita interrupcion por el estado de la línea UART */
	UART_IntConfig(LPC_UART2, UART_INTCFG_RLS, ENABLE);

	/* Habilita interrupcion por UART2 en NVIC*/
	NVIC_EnableIRQ(UART2_IRQn);

	return;
}

/* ----- Handler de UART2 ----- */
void UART2_IRQHandler(void){
	uint32_t intsrc, tmp, tmp1;

	/* Determina la fuente de interrupción */
	intsrc = UART_GetIntId(LPC_UART2);
	tmp = intsrc & UART_IIR_INTID_MASK;

	/* Evalua si la interrupción fue por RLS */
	if (tmp == UART_IIR_INTID_RLS){

		/* Se lee el Line Status Register (LSR) y se guarda en una variable temporal
		 * Solo puede leerse una vez este registro, ya que algunos flags se limpian.
		 */
		tmp1 = UART_GetLineStatus(LPC_UART2);

		/* Si hay algún error en línea, se queda en un loop infinito.*/
		tmp1 &= (UART_LSR_OE | UART_LSR_PE | UART_LSR_FE | UART_LSR_BI | UART_LSR_RXFE);
		if (tmp1) {
			while(1){};
		}
	}

	/* Receive Data Available or Character time-out */
	if ((tmp == UART_IIR_INTID_RDA) || (tmp == UART_IIR_INTID_CTI)){
		UART_Receive(LPC_UART2, info, sizeof(info), NONE_BLOCKING);
	}
	return;
}
