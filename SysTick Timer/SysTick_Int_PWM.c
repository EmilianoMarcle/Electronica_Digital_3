/*
===============================================================================
 Alumno		: Marclé Emiliano
 Carrera	: Ing. Electronica

 Descripcion : Se configura el Systick Timer de modo que genera una forma de
 	 	 	   onda llamada PWM.
 	 	 	   Esta señal se saca por el pin P0.22 y controla la intensidad de
 	 	 	   brillo del led.
 	 	 	   El periodo de la señal es de 10 mseg con un duty cycle de 10%.
 	 	 	   Cada vez que se entre en una rutina de interrupción externa EINT0
 	 	 	   el duty cycle se incrementa en un 10% (1 mseg). Esto se repite
 	 	 	   hasta llegar al 100%, luego, cuando se entra nuevamente a la
 	 	 	   interrupción externa, el duty cycle volverá al 10%.
			   Los niveles de prioridad se configuran para que la interrupción
			   por systick tenga mayor prioridad que la interrupción externa.
===============================================================================
*/
#include "LPC17xx.h"
/* -------- Prototipos -------- */
void configGPIO(void);
void configIntExt(void);
void SysTickConfig(void);
/* ---------- Macros ---------- */
#define  TICKS  (uint32_t) 100000  	// Ticks = RELOAD + 1 = 1ms * (1/100MHz);

/* ---- Variables globales ---- */
uint8_t dutyCycle = 1;

/* ---- Programa principal ---- */
int main(void) {
	configGPIO();					// Configuración GPIO
	configIntExt();					// Configuración EINT0
	SysTickConfig();				// Configuración SysTick
    while(1) {
    }
    return 0 ;
}

/* Función de configuración GPIO */
void configGPIO(void){
    LPC_GPIO0->FIODIR |= (1<<22);				// P0.22 como salida
    LPC_GPIO0->FIOSET |= (1<<22); 				// Led rojo en P0.22 apagado.
	return;
}

/*
 * Función de configuración interrupción externa EINT0.
 * Se configura por flanco de subida.
 */
void configIntExt(void){
    LPC_PINCON->PINSEL4 |= (1<<20);				// Configura P2.10 como EINT0
    LPC_SC->EXTMODE    |= 1;					// Configura EINT0 por flanco
    LPC_SC->EXTPOLAR   |= 1;					// Configura EINT0 por flanco de subida.
    LPC_SC->EXTINT	   |= 1;					// Limpia bandera de interrupcion.
    NVIC_SetPriority(EINT0_IRQn, 2);			// Configura menor prioridad para EINT0.
    NVIC_EnableIRQ(EINT0_IRQn);
	return;
}

/*
 * Función de configuración SysTick.
 * Se configura para generar una interrupción cada 1ms.
 */
void SysTickConfig(void){
    SysTick->LOAD = (TICKS-1);
    NVIC_SetPriority(SysTick_IRQn, 0); 			// Asigna una prioridad mayor a la de EINT0.
    SysTick->VAL = 0;							// Valor de contador en 0.
    SysTick->CTRL |= (1<<0)						// Habilita en SysTick
    		      |  (1<<1)						// Habilita la interrupción
				  |  (1<<2);                    // Selecciona el cclk como reloj del SysTick.
	return;
}

/*
 * Handler de EINT0
 * El duty cycle varia de 1 a 10.
 * Cada vez que se ingresa a la interrupción se incrementa la variable dutyCycle
 * y cuando llega a 10, el valor de la variable se reincia.
 * */
void EINT0_IRQHandler(){
    LPC_SC->EXTINT |= 1;
	dutyCycle++;
    if(dutyCycle>10){
    	dutyCycle =1;
    }
	return;
}

/*
 * Handler de SysTick
 * Se cuentan interrupciones de 1ms de 0 a 10.
 * Si el valor del contador es menor o igual al valor de dutyCycle se enciende el led,
 * de lo contrario, se apaga.
 */
void SysTick_Handler(void){
    static uint8_t counter = 1;
    if(counter <= dutyCycle){
    	LPC_GPIO0->FIOCLR |= (1<<22);		// Enciende el led rojo en P0.22
    }
    else{
    	LPC_GPIO0->FIOSET |= (1<<22);		// Apaga el led rojo en P0.22
    }
    counter++;
    if(counter>10){
    	counter = 1;
    }
	SysTick->CTRL &= SysTick->CTRL;			// Lee el registro para limpiar el flag de interrupción.
	return;
}
