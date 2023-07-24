/*
===============================================================================
 Nombre        : Marclé, Emiliano
 Carrera	   : Ing. Electronica
 Descripcion   : Utilizando los 4 registros match del Timer 0 y las salidas
 	 	 	 	 P0.0, P0.1, P0.2 y P0.3, realizar un programa en C que permita
 	 	 	 	 obtener las formas de ondas adjuntas, donde los pulsos en alto
 	 	 	 	  tienen una duración de 5 mseg.
 	 	 	 	 Un pulsador conectado a la entrada EINT3, permitirá elegir entre
 	 	 	 	 las dos secuencias mediante una rutina de servicio a la interrupción.
 	 	 	 	 La prioridad de la interrupción del Timer tiene que ser mayor que
 	 	 	 	 la del pulsador.
===============================================================================
*/

#include "LPC17xx.h"

/* ----- Macros ------ */
//#define PR_VAL  ((uint32_t) 25000) // 5 mseg
#define PR_VAL  ((uint32_t) 2500000) // 0,5 seg para poder visualizar secuencia.
#define MR0_VAL ((uint8_t)  5)
#define MR1_VAL ((uint8_t)  10)
#define MR2_VAL ((uint8_t)  15)
#define MR3_VAL ((uint8_t)  20)

/* ----- Prototipos de funciones ---- */
void confGPIO(void);
void confIntExt(void);
void confTimer(void);
void antirebote(void);

/* ----- Variables globales ----- */
uint8_t sec = 0;    // Flag de secuencia (sec=0->Secuencia 1, sec=1->Secuencia 2)

/* ----- Programa principal ----- */
int main(void) {

	confGPIO();		// Configuración de GPIO.
	confIntExt();   // Configuración EINT3.
	confTimer();    // Configuración TIM0

	while(1){}

    return 0 ;
}

/* Función de configuración GPIO */
void confGPIO(void){
	LPC_GPIO0->FIODIR0 |= (0xF);  // P0.0 a P0.3 como salida.
	LPC_GPIO0->FIOCLR0 |= (0xF);  // Limpia P0.0 a P0.3.
	LPC_GPIO0->FIOMASK0 |= ~(0xF);  // Enmascara P0.4 a P0.7
	return;
}

/* Función de configuración EINT3 */
void confIntExt(void){
	LPC_PINCON->PINSEL4 |= (1<<26);    // P2.13 como EINT3.
	LPC_PINCON->PINMODE4 &= ~(3<<26);  // P2.13 con pull-ups.

	/* EINT por flanco de bajada */
	LPC_SC->EXTMODE |= (1<<3);
	LPC_SC->EXTPOLAR &= ~(1<<3);
	LPC_SC->EXTINT |= (1<<3);

	/* Configura EINT3 con prioridad mas baja y habilita la interrupción */
	NVIC_SetPriority(EINT3_IRQn, 10);
	NVIC_EnableIRQ(EINT3_IRQn);

	return;
}

/* Función de configuración Timer */
void confTimer(void){
	LPC_SC->PCONP |= (1<<1);  // Enciende TIM0.
	LPC_SC->PCLKSEL0 &= ~(3<<2);  // PCLK=CCLK/4;
	LPC_TIM0->PR=(PR_VAL-1);
	LPC_TIM0->MR0 = (MR0_VAL-1);
	LPC_TIM0->MR1 = (MR1_VAL-1);
	LPC_TIM0->MR2 = (MR2_VAL-1);
	LPC_TIM0->MR3 = (MR3_VAL-1);
	LPC_TIM0->MCR |= (1<<0)|(1<<3)|(1<<6)|(1<<9); // Habilita int. en MR0, .., MR3.
	LPC_TIM0->MCR |= (1<<10); // Reset del TC y PC en MR3

	LPC_TIM0->TCR |= 3;  // Reset del TC y PC.
	LPC_TIM0->TCR &= ~(1<<1);  // Saca del reset a TC y PC.

	/* Configura int. por TIM0 con prioridad mas alta y la habilita. */
	NVIC_SetPriority(TIMER0_IRQn , 0);
	NVIC_EnableIRQ(TIMER0_IRQn);

	return;
}

/* Handler EINT3*/
void EINT3_IRQHandler(void){
	LPC_SC->EXTINT |= (1<<3);  // Limpia flag de int. por EINT3.
	sec ^= 1;   // Cambia el estado del flag de secuencia.
	antirebote(); // Retardo antirebote;
	return;
}

/*Handler TIM0 */
void TIMER0_IRQHandler(void){

	switch(sec){
		/* Secuencia 1 */
		case 0:
			if(LPC_TIM0->IR & (1<<0)){
				LPC_GPIO0->FIOSET0 |= (1<<0);
				LPC_GPIO0->FIOCLR0 |= ~(1<<0);
				LPC_TIM0->IR |= (1<<0);
				break;
			}
			else if(LPC_TIM0->IR & (1<<1)){
				LPC_GPIO0->FIOSET0 |= (1<<1);
				LPC_GPIO0->FIOCLR0 |= ~(1<<1);
				LPC_TIM0->IR |= (1<<1);
				break;
			}
			else if(LPC_TIM0->IR & (1<<2)){
				LPC_GPIO0->FIOSET0 |= (1<<2);
				LPC_GPIO0->FIOCLR0 |= ~(1<<2);
				LPC_TIM0->IR |= (1<<2);
				break;
			}
			else if(LPC_TIM0->IR & (1<<3)){
				LPC_GPIO0->FIOSET0 |= (1<<3);
				LPC_GPIO0->FIOCLR0 |= ~(1<<3);
				LPC_TIM0->IR |= (1<<3);
				break;
			}

		/* Secuencia 2 */
		case 1:
			if(LPC_TIM0->IR & (1<<0)){
				LPC_GPIO0->FIOSET0 |= (1<<0)|(1<<3);
				LPC_GPIO0->FIOCLR0 |= ~((1<<0)|(1<<3));
				LPC_TIM0->IR |= (1<<0);
				break;
			}
			if(LPC_TIM0->IR & (1<<1)){
				LPC_GPIO0->FIOSET0 |= (1<<0)|(1<<1);
				LPC_GPIO0->FIOCLR0 |= ~((1<<0)|(1<<1));
				LPC_TIM0->IR |= (1<<1);
				break;
			}
			if(LPC_TIM0->IR & (1<<2)){
				LPC_GPIO0->FIOSET0 |= (1<<1)|(1<<2);
				LPC_GPIO0->FIOCLR0 |= ~((1<<1)|(1<<2));
				LPC_TIM0->IR |= (1<<2);
				break;
			}
			if(LPC_TIM0->IR & (1<<3)){
				LPC_GPIO0->FIOSET0 |= (1<<2)|(1<<3);
				LPC_GPIO0->FIOCLR0 |= ~((1<<2)|(1<<3));
				LPC_TIM0->IR |= (1<<3);
				break;
			}
	}

	return;
}

/* Función de retardo antirebote para pulsador */
void antirebote(void){
	uint32_t ticks = 10000;  // Valor ajustado para 1ms aproximadamente.
	for(uint32_t i=0 ; i<ticks ; i++){}
	return;
}
