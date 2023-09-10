/*
===============================================================================
 Name        : Ejercicio_P1_Secuencia1.c
 Author      : $Marclé, Emiliano
 Version     : 1.0
 Description :
 Utilizando SysTick, hacer un programa que:
 - Cada vez que se produzca una interrupción por flanco ascendente en P2.2
  saque por el pin P2.1 la secuencia mostrada.
 - En caso de que se produzca una interrupción por flanco descendente en P2.2
  mientras se esta realizando la secuencia, se pondrá en 1 la salida P2.1 y
  se dará por finalizada la secuencia.
  - La entrada debe configurarse con resistencia de pull-down.
  - Suponer cclk = 40 MHz
===============================================================================
*/

#include "LPC17xx.h"

/* Definición de macros */
//#define TICKS_VAL ((uint32_t) 400000)
#define TICKS_VAL ((uint32_t) 10000000)

/* Variables globales */
uint8_t secActiva = 0;
uint8_t stop = 0; // Flag para detener la secuencia.
uint8_t sec[16]={0,1,0,1,0,1,0,0,0,0,1,0,1,0,1,0};
/* Prototipos de funciones */
void confGPIO(void);
void confGPIOInt(void);

/* Programa principal */
int main(void) {
	confGPIO();
	confGPIOInt();

    while(1) {}

    return 0;
}

/* Configuración GPIO */
void confGPIO(void){
	/* Configuración P2.1 */
	LPC_PINCON->PINSEL4 &= ~(3<<2);  // Configura P2.1 como GPIO
	LPC_PINCON->PINMODE4 &= ~(3<<2); // con pull-ups.
	LPC_GPIO2->FIODIR |= (1<<1); // P2.1 como salida.
	LPC_GPIO2->FIOCLR |= (1<<1); // Inicializa P2.1 en estado bajo.

	/* Configuración P2.2 */
	LPC_PINCON->PINSEL4 &= ~(3<<4);  // Configura P2.2 como GPIO
	LPC_PINCON->PINMODE4 |= (3<<4); // con pull-down.
	LPC_GPIO2->FIODIR |= (1<<1); // P2.2 como entrada.

	return;
}

void confGPIOInt(void){
	LPC_GPIOINT->IO2IntEnR |= (1<<2); // Habilita int. por flanco de subida en P2.2
	LPC_GPIOINT->IO2IntEnF |= (1<<2); // Habilita int. por flanco de bajada en P2.2
	LPC_GPIOINT->IO2IntClr |= (1<<2); // Limpia flag de interrupción.

	NVIC_EnableIRQ(EINT3_IRQn);
	return;
}

void EINT3_IRQHandler(void){
	/* Si la int. es por flanco de subida en P2.2
	 * - Habilita SysTick para generar secuencia.
	 * */
	if(LPC_GPIOINT->IO2IntStatR & (1<<2)){
		LPC_GPIOINT->IO2IntClr |= (1<<2); // Limpia el flag.
		SysTick_Config(TICKS_VAL-1); // Configura el SysTick
		secActiva = 1;
		NVIC_SetPriority(SysTick_IRQn, 0); // Mayor prioridad a SysTick
		return;
	}

	/* Si la int. es por flanco de bajada en P2.2
	 * - Habilita SysTick para generar secuencia.
	 * */
	if(LPC_GPIOINT->IO2IntStatF & (1<<2)){
		LPC_GPIOINT->IO2IntClr |= (1<<2); // Limpia el flag.
		if(secActiva){
			stop = 1; // Pone en 1 un flag para detener la secuencia si se está ejecutando.
		}
		return;
	}
}

void SysTick_Handler(void){
	static uint8_t counter = 0;  // Variable para contar int. de SysTick.

	SysTick->CTRL &= SysTick->CTRL; // Limpia flag.

	/* Verifica si llego int. por flanco de bajada */
	if(stop){
		LPC_GPIO2->FIOSET |= (1<<1); // Pone P2.1 en estado alto y detiene la secuencia.
		counter = 0;
		secActiva = 0;
		stop = 0;
		SysTick->CTRL &= ~(3<<0); // Detiene contador y desactiva int. de SysTick.
		return;
	}

	/* Ejecuta la secuencia */
	if(sec[counter]==1){
		LPC_GPIO2->FIOSET |= (1<<1);
	}
	else LPC_GPIO2->FIOCLR |= (1<<1);

	counter++;

	if(counter>15){
		counter = 0;
		secActiva = 0;
		SysTick->CTRL &= ~(3<<0); // Detiene contador y desactiva int. de SysTick.
	}

	return;
}


