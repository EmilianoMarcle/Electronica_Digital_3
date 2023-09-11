/*
===============================================================================
 Name        : Ejercicio_P1_Secuencia3.c
 Author      : $Marclé, Emiliano
 Version     : 1.0
 Description :
 Utilizando SysTick, hacer un programa que:
 - Cada vez que se produzca una interrupción por nivel alto en EINT1
  saque por el pin P2.0 y P2.1 la secuencia mostrada.
 - En caso de que se produzca una nueva interrupción por EINT1
  mientras se esta realizando la secuencia, se vuelve a realizar desde el inicio,
  descartando la que se venía sacando por los pines.
  - Enmascarar los pines del puerto 2 que no se usan.
  - Suponer cclk = 20 MHz
===============================================================================
*/
#include "LPC17xx.h"

/* Definición de macros */
//#define TICKS_VAL ((uint32_t) 200000)
#define TICKS_VAL ((uint32_t) 10000000)
#define PASOS_SEC ((uint8_t) 6) // Cantidad de pasos de la secuencia

/* Variables globales */
uint8_t inte = 0;
uint8_t conta = 0; // Contador de intervalos de la secuencia.
uint8_t sec0[PASOS_SEC]={0,1,0,1,1,0};
uint8_t sec1[PASOS_SEC]={0,0,1,1,1,0};

/* Prototipos de funciones */
void confGPIO(void);
void confExtInt(void);

/* Programa principal */
int main(void) {
	confGPIO();
	confExtInt();

    while(1) {}

    return 0;
}

/* Función de configuración de GPIO */
void confGPIO(void){
	/* Configuración de P2.0 y P2.1 como GPIO de salida*/
	LPC_PINCON->PINSEL4 &= ~(0xF);  // P2.0 y P2.1 como GPIO con pull-ups.
	LPC_PINCON->PINMODE4 &= ~(0xF);
	LPC_GPIO2->FIODIR |= (1<<0)|(1<<1); // P2.0 y P2.1 como salida.
	LPC_GPIO2->FIOCLR |= (1<<0)|(1<<1); // Inicializa P2.0 y P2.1 en estado bajo.
	LPC_GPIO2->FIOMASKL |= ~ ((1<<0)|(1<<1)); // Enmascar los pines no usados del Puerto 2.

	return;
}

/* Función de configuración de EINT1 */
void confExtInt(void){
	LPC_PINCON->PINSEL4 |= (1<<22); // P2.11 como EINT1
	LPC_PINCON->PINMODE4 |= (3<<22); // pull-down
	LPC_SC->EXTMODE &= ~(1<<1); // Int. por nivel alto.
	LPC_SC->EXTPOLAR |= (1<<1);
	LPC_SC->EXTINT |= (1<<1); // Limpia flag de EINT1.

	// NVIC_SetPriority(EINT1_IRQn, 0); // Configura prioridad para EINT1.
	NVIC_EnableIRQ(EINT1_IRQn);  // Habilita EINT1 en NVIC.
	return;
}

/* Handler de EINT1 */
void EINT1_IRQHandler(void){

	LPC_SC->EXTINT |= (1<<1); // Limpia flag de EINT1.

	if(inte==0){
		SysTick_Config(TICKS_VAL);  // Configura el SysTick
		inte=1;
		return;
	}
	else if(inte){
		conta = 0; // Reinicia la secuencia.
	}

	return;
}

void SysTick_Handler(void){

	SysTick->CTRL &= SysTick->CTRL; // Limpia flag.

	/* Ejecuta la secuencia */
	if(sec0[conta]){
		LPC_GPIO2->FIOSET |= (1<<0);
	}
	else LPC_GPIO2->FIOCLR |= (1<<0);

	if(sec1[conta]){
			LPC_GPIO2->FIOSET |= (1<<1);
		}
		else LPC_GPIO2->FIOCLR |= (1<<1);

	conta++;

	if(conta > (PASOS_SEC-1)){
		conta = 0;  // Reinicia contador de pasos de la secuencia.
		inte=0;  // Habilita la generación de nueva secuencia.
		SysTick->CTRL &= ~(3<<0); // Detiene contador y desactiva int. de SysTick.
		SysTick->VAL=0; // Reinicia contador.
	}

	return;
}
