/*
===============================================================================
 Name        : Ejemplo_UART.c
 Author      : Marclé Emiliano
 Version     : 1.0
 Description :
===============================================================================
*/
#include "LPC17xx.h"
#include "lpc17xx_uart.h"
#include "lpc17xx_pinsel.h"

/* ----- Prototipos de funciones ----- */
void confPin(void);
void confUart(void);

/* -------- Programa principal ------- */
int main (void){

	uint8_t info[] = "Hola mundo\t-\tElectrónica Digital 3\t-\tFCEFyN-UNC \n\r";
	//uint8_t info[]={0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39};

	confPin();	// Configuración de pines
	confUart();	// Configuración UART2

	while(1){
		/* Transmisión de datos
		 * BLOCKING: Envia datos en bloques y mantiene el uso del Core hasta finalizar la transmisión.
		 */
		UART_Send(LPC_UART2, info, sizeof(info), BLOCKING);
	}
	return 0;
}

/* ----- Función de configuración de Pines ----- */
void confPin(void){

	/* Configuración pin de TXD2 (P0.10) y RXD2 (P0.11) */
	PINSEL_CFG_Type PinCfg;

	PinCfg.Funcnum = 1;
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	PinCfg.Pinnum = 10;
	PinCfg.Portnum = 0;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 11;
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
	return;
}
