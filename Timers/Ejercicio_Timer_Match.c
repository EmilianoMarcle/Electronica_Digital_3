/*
===============================================================================
 Alumno      : Marclé Emiliano
 Carrera     : Ing. Electrónica
 Descripcion : Utilizando el Timer 1, escribir un código en C para que por cada
               presión de un pulsador, la frecuencia de parpadeo de un led disminuya
               a la mitad debido a la modificación del reloj que llega al periférico.
               El pulsador debe producir una interrupción por GPIO0 con flanco descendente.
===============================================================================
*/
#include "LPC17xx.h"

/* Macros */
#define MR_VAL  (uint32_t) 25000000

/* Prototipos de funciones */
void confIntGPIO(void);
void confTimer(void);



/* Programa principal */
int main(void) {

	confIntGPIO();		// Configuración Int. por GPIO
	confTimer();		// Configuración TIM0

    while(1) {
    }
    return 0 ;
}

/* Función de configuración GPIO*/
void confIntGPIO(void){
	LPC_GPIO0->FIODIR 	   &= ~(1<<0);		// P0.0 como entrada.
	LPC_GPIOINT->IO0IntEnF |= (1<<0);   	// Habilita int. por flanco de bajada en P0.0
	LPC_GPIOINT->IO0IntClr |= (1<<0);		// Limpia flag de Int. por GPIO.
	NVIC_EnableIRQ(EINT3_IRQn);				// Habilita Int. en NVIC.
	return;
}

/* Función de configuración TIM1
 * Se utiliza un pin externo de Match para conectar el led.
 * Se ajusta el Timer para un tiempo de 250ms con frecuencia de 100MHz
 * - PR=0 -> MR+1 = (250ms * 100MHz) = 25000000
 */
void confTimer(void){
	LPC_PINCON->PINSEL3 |= (3<<12);		// Configura P1.22 como MAT1.0
	LPC_SC->PCONP |= (1<<2);		    // Enciende el TIM1
	LPC_SC->PCLKSEL0 |= (1<<4);		    // Configura PCLK=CCLK;
	LPC_TIM1->EMR |= (3<<4);		    // Togglea en MR0.
	LPC_TIM1->PR = 0;					// Prescale Register = 0
	LPC_TIM1->MR0 = (MR_VAL-1);		    // Se carga valor de MR para contar 250ms
	LPC_TIM1->MCR &= ~(1<<0);			// Desactiva interrupción en MR0
	LPC_TIM1->MCR |= (1<<1);			// Configura Reset en MR0.
	LPC_TIM1->TCR |= 3;					// Habilitacion y reset de TC y PC.
	LPC_TIM1->TCR &= ~(1<<1);			// Saca del reset a TC y PC.
	return;
}

/* Handler EINT3 */
void EINT3_IRQHandler(void){
	static uint8_t conta = 0;

	/* Se verifica que la interrupción sea por flanco descendente en P0.0*/
	if(LPC_GPIOINT->IO0IntStatF && (1)){

		LPC_GPIOINT->IO0IntClr |= 1;	// Limpia flag de Int. por GPIO;

		conta++;
		if(conta>3){
			conta=0;
		}

		switch(conta){
		/* PCLK = CCLK/2 (PCLKSEL = 10) */
		case 1:
			LPC_SC->PCLKSEL0 &= ~(1<<4);
			LPC_SC->PCLKSEL0 |= (1<<5);
			break;
		/* PCLK = CCLK/4 (PCLKSEL = 00) */
		case 2:
			LPC_SC->PCLKSEL0 &= ~(1<<4);
			LPC_SC->PCLKSEL0 &= ~(1<<5);
			break;
		/* PCLK = CCLK/8 (PCLKSEL = 11) */
		case 3:
			LPC_SC->PCLKSEL0 |= (1<<4);
			LPC_SC->PCLKSEL0 |= (1<<5);
			break;
		/* PCLK = CCLK (PCLKSEL = 01) */
		default:
			LPC_SC->PCLKSEL0 |= (1<<4);
			LPC_SC->PCLKSEL0 &= ~(1<<5);
			break;
		}
	}
	return;
}
