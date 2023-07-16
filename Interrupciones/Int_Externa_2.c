/*
======================================================================================================
 Name        : Int_Externa_2.c
 Author      : $ Marclé Emiliano
 Version     : 1.0
 Description : Escribir un programa en C que permita sacar por los pines P1.16 al P1.23 el equivalente
  	  	  	   en ascii de "a" si la interrupción se ha realizado por interrupción de EINT1
  	  	  	   y no hay interrupción pendiente de EINT0.
  	  	  	   Para el caso en el cual se produzca una interrupción por EINT1 y exista una interrupción
  	  	  	   pendiente de EINT0 sacar por el  puerto el equivalente en ascii de "A".
  	  	  	   La interrupción de EINT1 es por el flanco de subida producido por un pulsador
  	  	  	   identificado como "tecla a/A".
  	  	  	   La interrupción EINT0 es por el nivel alto de un pulsador etiquetado
  	  	  	   como "Activación de Mayusculas".
======================================================================================================
*/

#include "LPC17xx.h"

/* ---- Prototipos de funciones ---- */
void configGPIO(void);
void configIntExt(void);

/* ------ Variables globales --------*/
uint8_t mayuscula = 0;   // Flag de mayusculas que cambia entre 0 y 1 cada vez que ocurre EINT1.
uint8_t estado = 0;      // Flag para identificar la primer entrada a la interrupcion EINT0.
char caracter = 'a';     // Caracter a sacar por pines de salida.
uint16_t valor = 0;      // Variable para monitorear el valor de los pines P1.16 a P1.23

/* ------ Programa principal ------- */
int main(void) {

	configGPIO();		// Configura GPIO
	configIntExt();		// Configura Int. Externas EINT1 por flanco de bajada y EINT2 por flanco de subida.

    while(1) {
    }
    return 0 ;
}

/* Configuración GPIO */
void configGPIO(void){
	LPC_GPIO1->FIODIR2 |= 0xFF; 	   // P1.16 a P1.23 como salidas.
	LPC_GPIO1->FIOCLR2  |= 0xFF; 	   // Pone en 0 los pines de salida.
	LPC_PINCON->PINMODE4 &= ~(3<<20);  // Pull-Down en P2.10 para aseguran nivel bajo en el pin.
	LPC_PINCON->PINMODE4 &= ~(3<<22);  // Pull-Down en P2.11
	return;
}

/* Función de configuración de Interrupciones externas */
void configIntExt(void){
	/* Configuracion EINT0*/
	LPC_PINCON->PINSEL4 |= (1<<20);    // P2.10 como EINT0
	LPC_SC-> EXTMODE    &= ~(1);    // Int. por nivel
	LPC_SC-> EXTPOLAR   |= 1;          // Int. por nivel alto
	LPC_SC-> EXTINT     |= 1;          // Limpia flag int. por EINT0
	NVIC_SetPriority(EINT0_IRQn, 2);   // Configura EINT0 con menor prioridad
	NVIC_EnableIRQ(EINT0_IRQn);        // Habilita int. en NVIC.
	/* Configuración EINT1 */
	LPC_PINCON->PINSEL4 |= (1<<22);    // P2.11 como EINT1
	LPC_SC-> EXTMODE    |= (1<<1);     // Int. por flanco.
	LPC_SC-> EXTPOLAR   |= (1<<1);     // Flanco de subida.
	NVIC_SetPriority(EINT1_IRQn, 0);   // Configura EINT1 con mayor prioridad.
									   // Si hay interrupción pendiente de EINT0, lo mismo se tiene que atender EINT1.
	NVIC_EnableIRQ(EINT1_IRQn);        // Habilita int. en NVIC.
	return;
}

/*
 * Handler de interrupción EINT0
 * Se entra al handler hasta que el nivel del pin P2.10 sea bajo (al dejar de pulsar) y se pueda limpiar el flag.
 */
void EINT0_IRQHandler(void){
	// Intercambia el estado del flag mayuscula solo la primera vez que entra.
	// Si se mantiene apretado el pulsador el bit en EXTINT estara en uno porque no se pudo limpiar.
	if(estado == 0){
		mayuscula ^= 1;
		estado = 1;
	}
	LPC_SC->EXTINT |= 1;            // En modo sensible a nivel, el flag solo se limpia si el estado del pin es bajo.
	if((LPC_SC->EXTINT & 1) == 0){	// Si se logró limpiar el flag de interrupción, cambiar el estado.
		estado = 0;
	}
	return;
}

/*
 * Handler de interrupción EINT1
 * Si el flag esta en 1 (mayuscula) o esta pendiente (pulsador apretado) se saca una "A".
 * Si el flag esta en 0 (minuscula), se saca una "a".
 */
void EINT1_IRQHandler(void){
	if(mayuscula || NVIC_GetPendingIRQ(EINT0_IRQn)){
		caracter = 'A';
	}
	else{
		caracter = 'a';
	}
	LPC_GPIO1->FIOPIN2 = caracter;  // Saca por P1.16 a P1.23 un caracter a/A.
	valor = LPC_GPIO1->FIOPIN2;     // Guarda valor de P1.16 a PI.23 en variable.
	LPC_SC ->  EXTINT |= (1<<1);    // Limpia flag de int. EINT1.
	return;
}
