/*
===============================================================================
 Alumno		: Marclé Emiliano
 Carrera	: Ing. Electronica

 Descripcion : Algoritmo de antirrebote de un pulsador: Escribir un programa en C
  	  	  	  que ante la interrupción por flanco de subida del pin P0.1, configurado como
  	  	  	  entrada digital con pull-down interno, se incremente un contador de un dígito,
  	  	  	  se deshabilite esta interrupción y se permita la interrupción por systick cada
  	  	  	  20 milisegundos. En cada interrupción del systick se testeará una vez el pin P0.1.
  	  	  	  Solo para el caso de haber testeado 3 estados altos seguidos se sacará por los
  	  	  	  pines del puerto P2.0 al P2.7 el equivalente en ascii del valor del contador,
  	  	  	  se desactivará las interrupción por systick y se habilitará nuevamente la
  	  	  	  interrupción por P0.1. Por especificación de diseño se pide que los pines
   	   	   	   del puerto 2 que no sean utilizados deben estar enmascarados por hardware.
  	  	  	  Considerar que el CPU se encuentra funcionando con el oscilador interno RC (4Mhz).
===============================================================================
*/

#include "LPC17xx.h"

/* ------ Macros ------ */
#define	  TICKS	  		(uint32_t)  80000	  // TICKS = RELOAD+1 = 25ms*4MHz
//#define	  TICKS	  		(uint32_t)  2500000	  // TICKS = RELOAD+1 = 25ms*100MHz
#define   MAX_COUNT 	(uint16_t)  3       // Cantidad maxima de interrupciones de SysTick contadas.


/* ----- Prototipos funciones ------ */
void configGPIO(void);
void configIntGPIO(void);
void configSysTick(uint32_t);

/* ------ Variables globales ------- */
uint8_t digito = 0;			// Contador de un dígito (de 0 a 9).
uint8_t flagDigito = 0;

/* ------ Programa principal ------- */
int main(void){
	uint16_t valor = 0;

	configGPIO();			// Configuración de GPIO
	configIntGPIO();		// Configuración de Int. por GPIO.

	while(1){
		if(flagDigito){
			valor = (digito + 48);   // Convierte el digito a ASCII sumando 32 hexadecimal o 48 decimal.
			LPC_GPIO2->FIOPIN0 = valor;		  // Saca el valor por P2.0 a P2.7
			flagDigito = 0;
		}
	}

	return 0;
}

/* Función de configuración de GPIO */
void configGPIO(void){
	LPC_PINCON->PINSEL0  &= ~(3<<2);			// P0.1 como GPIO.
	LPC_PINCON->PINMODE0 |= (3<<2);				// P0.1 con pulldown
	LPC_GPIO0->FIODIR    &= ~(1<<1);			// P0.1 como entrada
	LPC_GPIO2->FIODIR0   |= 0xFF;				// P2.0 a P2.7 como salidas
	LPC_GPIO2->FIOCLR0   |= 0xFF;				// Limpia P2.0 a P2.7.
	LPC_GPIO2->FIOMASK1  |= 0x3F;				// Limpia P2.8 a P2.13. Los otros pines de P2 no estan implementados.
	return;
}

/* Función de configuración Int por GPIO */
void configIntGPIO(void){
	LPC_GPIOINT->IO0IntEnR |= (1<<1);			// Habilita interrupción por flanco de subida en P0.1
	LPC_GPIOINT->IO0IntClr |= (1<<1);			// Limpia flag de interrupción por flanco de subida en P0.1
	NVIC_SetPriority(SysTick_IRQn, 2);			// Configura una prioridad más baja que la int. por SysTick.
	NVIC_EnableIRQ(EINT3_IRQn);					// Habilita int. en NVIC.
	return;
}

/*
 * Función de configuración de SysTick.
 */
void configSysTick(uint32_t ticks){
	SysTick->LOAD = ticks-1;					// Carga valor de recarga.
	SysTick->VAL = 0;						    // Pone contador a cero.
	SysTick->CTRL |= (1<<0)|(1<<1)|(1<<2);		// Habilita el SysTick, la interrupción y selecciona cclk.
	return;
}

/* Handler de Int. por GPIO */
void EINT3_IRQHandler(void){
	/* Verifica si la interrupción fue por P0.1 */
	if((LPC_GPIOINT->IO0IntStatR) & (1<<1)){
		/* Incremento de contador de un dígito */
		digito++;
		if(digito > 9){
			digito = 0;
		}
		/* Desactivación Int. en P0.1 */
		LPC_GPIOINT->IO0IntEnR &= ~(1<<1);		// Deshabilita int. en P0.1
		LPC_GPIOINT->IO0IntClr |= (1<<1);		// Limpia flag de int.
		NVIC_DisableIRQ(EINT3_IRQn);			// Deshabilita int. en NVIC.
		/* Configuración SysTick */
		configSysTick(TICKS);					// Habilita SysTick para interrumpir cada 20ms.
	}
	return;
}

/*
 * Handler del SysTick
 * Cada 20ms se entra a la interrupción.
 * Si se tiene tres valores altos seguidos en P0.1:
 * - Se desactiva el SysTick
 * - Se configura de nuevo la int. en P0.1
 * - Se activa el flag para sacar el digito por P2.0 a P2.7
 * El SysTick solo de desactiva si se tienen 3 valores seguidos en P0.1
 * por lo que quedara entrando indefinidamente a el handler.
 * Hay que elegir correctamente el valor de MAX_COUNT.
 */
void SysTick_Handler(void){
	static uint8_t intCount = 0;                // Contador de interrupciones de SysTick.
	SysTick->CTRL &= SysTick->CTRL;				// Limpia flag int.
	if(LPC_GPIO0->FIOPIN & (1<<1)){
		intCount++;
		if(intCount == MAX_COUNT){
			SysTick->CTRL = 0;					// Desactiva el SysTick.
			configIntGPIO();					// Configura Int. en P0.1
			flagDigito = 1;                     // Activa el flag para sacar el digito por puerto.
			intCount = 0;						// Pone el contador de interrupciones a 0.
		}
	}
	else{
		intCount = 0;
	}
	return;
}

