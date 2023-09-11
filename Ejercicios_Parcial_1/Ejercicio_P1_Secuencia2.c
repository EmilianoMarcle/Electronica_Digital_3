/*
===============================================================================
 Name        : Ejercicio_P1_Secuencia2.c
 Author      : $Marclé, Emiliano
 Version     : 1.0
 Description :
 Utilizando SysTick, hacer un programa que:
 - Cada vez que se produzca una interrupción por flanco descendente en EINT2
  saque por el pin P2.4 la secuencia mostrada.
 - En caso de que se produzca una nueva interrupción por EINT2
  mientras se esta realizando la secuencia, se pondrá en 1 la salida P2.4 y
  se dará por finalizada la secuencia.
  - Enmascarar los pines del puerto 2 que no se usan.
  - Suponer cclk = 60 MHz
===============================================================================
*/

#include "LPC17xx.h"

/* Definición de macros */
//#define TICKS_VAL ((uint32_t) 600000)
#define TICKS_VAL ((uint32_t) 8000000)
#define PASOS_SEC ((uint16_t) 15) // Cantidad de pasos de la secuencia

/* Variables globales */
uint8_t inte = 0;
uint8_t stop = 0; // Flag para detener la secuencia.
uint8_t sec[PASOS_SEC]={0,0,0,0,1,0,1,0,1,0,1,0,0,0,0};

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
	/* Configuración P2.4 */
	LPC_GPIO2->FIODIR |= (1<<4); // P2.4 como salida.
	LPC_GPIO2->FIOCLR |= (1<<4); // Inicializa P2.4 en estado bajo.
	LPC_GPIO2->FIOMASKL |= ~ (1<<4); // Enmascar los pines no usados del Puerto 2.

	return;
}

/* Función de configuración de EINT2 */
void confExtInt(void){
	LPC_PINCON->PINSEL4 |= (1<<24); // P2.12 como EINT2
	LPC_SC->EXTMODE |= (1<<2); // Int. por flanco de bajada
	LPC_SC->EXTPOLAR &= ~(1<<2); //
	LPC_SC->EXTINT |= (1<<2); // Limpia flag de EINT2.

	NVIC_SetPriority(EINT2_IRQn, 0); // Configura prioridad más alta para EINT2.
	NVIC_EnableIRQ(EINT2_IRQn);
	return;
}

/* Handler de EINT2 */
void EINT2_IRQHandler(void){

	LPC_SC->EXTINT |= (1<<2); // Limpia flag de EINT2.

	if(inte==0){
		SysTick_Config(TICKS_VAL);  // Configura el SysTick
		inte++;
		return;
	}
	else if(inte){
		stop=1; // Activa flag para detener la secuencia.
	}

	return;
}

void SysTick_Handler(void){
	static uint8_t i = 0;  // Variable para contar int. de SysTick.

	SysTick->CTRL &= SysTick->CTRL; // Limpia flag.

	/* Verifica si llego int. por flanco de bajada */
	if(stop){
		LPC_GPIO2->FIOSET |= (1<<4); // Pone P2.1 en estado alto y detiene la secuencia.
		i = 0;
		inte = 0;
		stop = 0;
		SysTick->CTRL &= ~(3<<0); // Detiene contador y desactiva int. de SysTick.
		return;
	}

	/* Ejecuta la secuencia */

	if(sec[i]){
		LPC_GPIO2->FIOSET |= (1<<4);
	}
	else LPC_GPIO2->FIOCLR |= (1<<4);

	i++;

	if(i > PASOS_SEC-1){
		i = 0;
		inte=0;
		SysTick->CTRL &= ~(3<<0); // Detiene contador y desactiva int. de SysTick.
	}

	return;
}
