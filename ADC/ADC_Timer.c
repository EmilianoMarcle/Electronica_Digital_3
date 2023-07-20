/*
===============================================================================
 Nombre : ADC_Timer
 Alumno : Marclé Emiliano
 Descripción : Programa que utiliza Timer para disparar ADC cuando hay
 	 	 	   flanco de bajada en MAT0.1.
 	 	 	   - P0.22 parpadea continuamente
 	 	 	   - P0.9 se enciende de acuerdo al valor convertido.
 	 	 	   - Tiempo TIM0 = 1 seg
 	 	 	   - Tiempo Muestreo ADC = 2 seg
 	 	 	   - Frec. Muestreo ADC = 0,5 Muestras/seg

===============================================================================
*/
#include "LPC17xx.h"

/* ----- Prototipos de funciones ------ */
void confGPIO(void);
void confADC(void);
void confTimer(void);
void retardo(void);

/* -------- Programa principal --------- */
int main(void) {
	confGPIO();		// Configuración de GPIO
	confTimer();	// Configuración de Timer0
	confADC();		// Configuración de ADC

    while(1) {
    	LPC_GPIO0->FIOSET |= (1<<22);
    	retardo();
    	LPC_GPIO0->FIOCLR |= (1<<22);
    	retardo();
    }
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
	LPC_GPIO0->FIODIR |= (1<<22) | (1<<9);	// P0.22 y P0.9 como salida.
	return;
}

/*
 * Función de configuración de ADC.
 * - Canal AD0.0
 * - No burst
 * - Dispara la conversión en flanco de bajada de MAT0.1 (cada 2 segundos).
 * - Habilita interrupción
 */
void confADC(void){
	LPC_PINCON->PINSEL1  |= (1<<14); // P0.23 como AD0.0
	LPC_PINCON->PINMODE1 |= (1<<15); // neither pullup nor pulldown
	LPC_SC->PCONP |= (1<<12);	 	 // Enciende ADC
	LPC_SC->PCLKSEL0 |= (3<<24); 	 // CLK/8
	LPC_ADC->ADCR |= (1<<21);	 	 // Habilita ADC
	LPC_ADC->ADCR &= ~(255 << 8);	 // CLKDIV = 0;
	LPC_ADC->ADCR &= ~(1 << 16);     // No burst
	// Comenzar conversion en MAT0.1 con flanco de bajada.
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
 * - Cuenta 1 seg
 * - Toggle en Match1
 * - Sin interrupción
 */
void confTimer(void){
	LPC_SC->PCONP |= (1<<1);
	LPC_SC->PCLKSEL0 |= (1<<2); // CCLK
	LPC_TIM0->EMR |= (1<<1);	// MR1
	LPC_TIM0->EMR |= (3<<6);    // Toggle en Match1
	LPC_TIM0->MR1 = 100000000;	// Valor de MR para contar 1seg.
	LPC_TIM0->MCR &= ~(1<<3);   // Deshabilita Int. en Match1
	LPC_TIM0->MCR |= (1<<4);    // Reset en Match1
	LPC_TIM0->TCR |= 3;			// Habilita y resetea TC y PC
	LPC_TIM0->TCR &= ~ (1<<1);  // saca del reset
	return;
}

/*
 * Handler de Int. en ADC
 * Si el valor convertido es mayor a 2055 enciende/apaga un led en P0.9
 */
void ADC_IRQHandler(void){
	static uint16_t ADC0Value = 0;
	float volt = 0;

	/* Guarda el valor convertido */
	ADC0Value = ((LPC_ADC->ADDR0) >> 4) & 0xFFF;

	if(ADC0Value > 2055)
		LPC_GPIO0-> FIOSET |= (1<<9);
	else
		LPC_GPIO0-> FIOCLR |= (1<<9);

	volt = (ADC0Value/4096)*3.3;
	return;
}
