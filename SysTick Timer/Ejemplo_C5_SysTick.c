/**
 ===============================================================================
 @Alumno      : Marclé Emiliano
 @Carrera     : Ing. Electronica
 @Descripcion : Este programa realiza el parpadeo de un LED conectado en P0.22
 	 	 y modifica mediante SysTick la frecuencia de parpadeo.
 ===============================================================================
 */

#include "LPC17xx.h"

uint32_t inte = 0;

/**
 * Prototipos de funciones
 */
void configGPIO(void);

/**
 * Programa principal
 */
int main(void) {

	configGPIO();

	if (SysTick_Config(SystemCoreClock/10)) {
		while (1); // ERROR.
	}

	while (1) {
			if (inte % 2) {
				LPC_GPIO0->FIOSET |= (1 << 22);
			}
			else {
				LPC_GPIO0->FIOCLR |= (1 << 22);
			}
		}
		return 0;
}

/**
 * Función de configuración de GPIO
 */
void configGPIO(void) {
	LPC_GPIO0->FIODIR |= (1 << 22); // Configura P0.22 como salida
	return;
}

/**
* Rutina de interrupción del SysTick
*/
void SysTick_Handler(void) {
	inte++;
	SysTick->CTRL &= SysTick->CTRL;
	return;
}
