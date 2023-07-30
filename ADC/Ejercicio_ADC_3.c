/*
===============================================================================
 Nombre : ADC_Timer
 Alumno : Marclé Emiliano
 Descripción : Programa que utiliza Timer para disparar ADC cuando hay
 	 	 	   flanco de bajada en MAT0.1.
 	 	 	   - Tiempo Match1 = 25us.
 	 	 	   - Tiempo Muestreo ADC = 50us.
 	 	 	   - Frec. Muestreo ADC = 20 KMuestras/seg

===============================================================================
*/
#include "LPC17xx.h"

/* ----- Macros ------ */
#define PR_VAL  ((uint32_t) 100)
#define MR_VAL  ((uint32_t) 25)

/* ----- Prototipos de funciones ------ */
void confGPIO(void);
void confADC(void);
void confTimer(void);
void retardo(void);

/* ------ Variables globales ------- */
__IO uint16_t ADC0Value = 0;

/* -------- Programa principal --------- */
int main(void) {
	confGPIO();		// Configuración de GPIO
	confTimer();	// Configuración de Timer0
	confADC();		// Configuración de ADC

    while(1) {}
    return 0 ;
}

/* ----------Función de retardo --------- */
void retardo(void){
	uint32_t conta;
	for(conta=0 ; conta<1000000 ; conta++){}
	return;
}

/* -- Función de configuración de GPIO -- */
void confGPIO(void){
	/*P0.0 a P0.11 como GPIO con pullups. */
	LPC_PINCON->PINSEL0 &= ~(0xFFFFFF);
	LPC_PINCON->PINMODE0 &= ~(0xFFFFFF);

	/*P0.0 a P0.11 como salidas. */
	LPC_GPIO0->FIODIR0 |= 0xFF;
	LPC_GPIO0->FIODIR1 |= 0xF;
	LPC_GPIO0->FIOMASK1 |= (1<<15); // Enmascara P0.15
	LPC_GPIO0->FIOCLRL |= (0xFFF);  // Limpia P0.0 a P0.11
	LPC_GPIO0->FIOCLRL |= (1<<15);  // Limpia P0.15
	return;
}

/*
 * Función de configuración de ADC.
 * - Canal AD0.0
 * - No burst
 * - Dispara la conversión en flanco de bajada de MAT0.1 (cada 50 useg).
 * - Habilita interrupción
 */
void confADC(void){
	LPC_PINCON->PINSEL1  |= (1<<14); // P0.23 como AD0.0
	LPC_PINCON->PINMODE1 |= (1<<15); // neither pullup nor pulldown
	LPC_SC->PCONP |= (1<<12);	 	 // Enciende ADC
	LPC_SC->PCLKSEL0 |= (3<<24); 	 // CLK/8
	LPC_ADC->ADCR |= (1<<0);         // ADC canal 0.
	LPC_ADC->ADCR |= (1<<21);	 	 // Habilita ADC
	LPC_ADC->ADCR &= ~(255 << 8);	 // CLKDIV = 0;
	LPC_ADC->ADCR &= ~(1 << 16);     // No burst

	/* Comenzar conversion en MAT0.1 con flanco de bajada. */
	LPC_ADC->ADCR |= (1<<26);
	LPC_ADC->ADCR &= ~(3<<24);
	LPC_ADC->ADCR |= (1<<27);
	LPC_ADC->ADINTEN |= (1<<0);
	NVIC_EnableIRQ(ADC_IRQn);
	return;
}

/*
 * Función de configuración del TIM0
 * - PCLK = CCLK
 * - Match1 cada 25 useg y 50 useg entre dos flancos de bajada en MAT0.1
 * - Toggle en Match1
 * - Sin interrupción
 */
void confTimer(void){
	LPC_SC->PCONP |= (1<<1);
	LPC_SC->PCLKSEL0 |= (1<<2); // CCLK
	LPC_TIM0->EMR |= (1<<1);	// MR1
	LPC_TIM0->EMR |= (3<<6);    // Toggle en Match1
	LPC_TIM0->PR= PR_VAL-1;     // Prescales para incrementar TC cada 1useg.
	LPC_TIM0->MR1 = MR_VAL-1;	// Valor de MR para contar 25us.
	LPC_TIM0->MCR &= ~(1<<3);   // Deshabilita Int. en Match1
	LPC_TIM0->MCR |= (1<<4);    // Reset en Match1
	LPC_TIM0->TCR |= 3;			// Habilita y resetea TC y PC
	LPC_TIM0->TCR &= ~ (1<<1);  // saca del reset
	return;
}

/*
 * Handler de Int. en ADC
 */
void ADC_IRQHandler(void){

	/* Verificación de finalización de conversión en canal 0*/
	if(LPC_ADC->ADSTAT & (1<<0)){
		/* Guarda el valor convertido y lo saca por los pines P0.0 a P0.11 */
		ADC0Value = (LPC_ADC->ADDR0 >> 4) & 0xFFF;
		LPC_GPIO0->FIOPINL = ADC0Value;
	}
	return;
}
