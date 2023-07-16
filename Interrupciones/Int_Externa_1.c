/*
======================================================================================================
 Name        : Int_Externa_1.c
 Author      : $ Marclé Emiliano
 Version     : 1.0
 Description : Configurar la interrupción externa EINT1 para que interrumpa por flanco de bajada y la
  	  	  	  interrupción EINT2 para que interrumpa por flanco de subida.
  	  	  	  En la interrupción por flanco de bajada configurar el systick para desbordar cada 25 mseg,
  	  	  	  mientras que en la interrupción por flanco de subida configurarlo para que desborde cada
  	  	  	  60 mseg.
  	  	  	  Considerar que EINT1 tiene mayor prioridad que EINT2.
  	  	  	  Para verificar el programa se utiliza el led rojo de la placa (P0.22).
======================================================================================================
*/

#include "LPC17xx.h"

/* ---- Prototipos de funciones ---- */
void configGPIO(void);
void configIntExt(void);

/* ----------- Macros -------------- */
#define	TICKS1 (uint32_t) 2500000		// Ticks = (RELOAD+1) = (1/100MHz)*25ms
#define TICKS2 (uint32_t) 6000000		// Ticks = (RELOAD+1) = (1/100MHz)*25ms

/* ------ Variables globales --------*/
uint8_t	inte = 0;			// Flag de interrupción por Systick.

/* ------ Programa principal ------- */
int main(void) {

	configGPIO();		// Configura GPIO
	configIntExt();		// Configura Int. Externas EINT1 por flanco de bajada
						// y EINT2 por flanco de subida.
	SysTick->CTRL=0;    // Inicializa el SysTick deactivado.

    while(1) {
    	/*
    	 * Parpadeo de led rojo.
    	 * Cuando se configura SysTick, el flag inte se intercambiara a 1
    	 * con lo que el led se encenderá y luego se apagará cuando desborde el SysTick.
    	 */
    	if(inte){
    		LPC_GPIO0->FIOCLR |= (1 << 22);  // Enciende el led rojo.
    	}
    	else{
    		LPC_GPIO0->FIOSET |= (1 << 22);  // Apaga el led rojo.
    	}
    }
    return 0 ;
}

/* Configuración GPIO */
void configGPIO(void){
	LPC_GPIO2->FIODIR &= ~(1<<11);	// P2.11 como entrada.
	LPC_GPIO2->FIODIR &= ~(1<<12);	// P2.12 como entrada.
	LPC_GPIO0->FIODIR |= (1<<22); 	// Configura P0.22 como salida (Led rojo de la placa).
	LPC_GPIO0->FIOSET |= (1<<22); 	// Apaga el led rojo de la placa.
	return;
}

/* Función de configuración de Interrupciones externas */
void configIntExt(void){
	/* Configuracion EINT1 */
	LPC_PINCON -> PINSEL4 |= (1<<22);      // Selecciona función EINT1 en P2.11
	LPC_SC-> EXTMODE      |= (1<<1);       // Interrupción por flanco
	LPC_SC-> EXTPOLAR     &= ~(1<<1);      // Flanco de bajada.
	LPC_SC-> EXTINT       |= (1<<1);       // Limpia flag de int. en EINT1.
	NVIC_EnableIRQ(EINT1_IRQn);            // Habilita interrupción en NVIC

	/* Configuracion EINT2 */
	LPC_PINCON -> PINSEL4 |= (1<<24);      // Selecciona función EINT2 en P2.12
	LPC_SC-> EXTMODE 	  |= (1<<2);       // Interrupción por flanco
	LPC_SC-> EXTPOLAR 	  |= (1<<2);       // Flanco de subida
	LPC_SC-> EXTINT       |= (1<<2);       // Limpia flag de int. en EINT2.
	NVIC_SetPriority(EINT2_IRQn, 10);      // Configura una prioridad menor para EINT2.
	NVIC_EnableIRQ(EINT2_IRQn);			   // Habilita interrupción en NVIC
	return;
}

/*
 * Handler de interrupción EINT1
 * Cuando se detecta el flanco de bajada se configura el SysTick para 25ms.
 */
void EINT1_IRQHandler(void){
	SysTick_Config(TICKS1);		// Configura SysTick para interrumpir cada 25ms.
	LPC_SC->EXTINT |= (1<<1);	// Limpia flag de int. en EINT1.
	return;
}

/*
 * Handler de interrupción EINT2
 * Cuando se detecta el flanco de bajada se configura el SysTick para 60ms.
 */
void EINT2_IRQHandler(void){
	SysTick_Config(TICKS2);		// Configura SysTick para interrumpir cada 60ms.
	LPC_SC->EXTINT |= (1<<2);	// Limpia flag de int. en EINT2.
	return;
}

void SysTick_Handler(void){
	inte ^= 1;  // intercambia el estado del flag.
	SysTick->CTRL  &= SysTick-> CTRL; 	//Se lee el registro STCTRL para limpiar el flag de desborde COUNTFLAG.
	return;
}
