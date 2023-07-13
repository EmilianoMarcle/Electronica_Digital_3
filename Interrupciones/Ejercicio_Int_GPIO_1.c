/*
====================================================================================================
 Name        : Int_GPIO_1.c
 Author      : $ Marclé Emiliano
 Version     : 1.0
 Description : Realizar un programa que configure el puerto P2.0 y P2.1 para que provoquen una
 	 	 	   interrupción por flanco de subida para el primer pin y por flanco de bajada para el
 	 	 	   segundo.
 	 	 	   Cuando la interrupción sea por P2.0 se enviará por el pin P0.0 la secuencia
 	 	 	   de bits 010011010.
 	 	 	   Si la interrupción es por P2.1 se enviará por el pin P0.1 la secuencia 011100110.
 	 	 	   Las secuencias se envían únicamente cuando se produce una interrupción, en caso
 	 	 	   contrario la salida de los pines tienen valores 1 lógicos.
====================================================================================================
*/

#include "LPC17xx.h"

/* Macros*/
#define SIZE (uint8_t)  9

/* Prototipos de las funciones */
void configGPIO(void);
void configGPIOInt(void);
void retardo(void);

/* Variables locales*/
uint8_t secA[SIZE] = {0, 1, 0, 0, 1, 1, 0, 1, 0};
uint8_t secB[SIZE] = {0, 1, 1, 1, 0, 0, 1, 1, 0};
uint8_t flag = 0;  // Flag de activación de secuencia: bit 1 (secB) , bit 0 (secA)

/* Programa principal */
int main(void) {

	configGPIO();
	configGPIOInt();

    while(1) {

    	/* Si se presionó P2.0 se saca la secuencia A por P0.0 */
    	if(flag & (1<<0)){
    		for(uint8_t i = 0 ; i<SIZE ;i++){
    			if(secA[i] == 1){
    				LPC_GPIO0->FIOSET |= (1<<0);
    				retardo();
    			}
    			else {
    				LPC_GPIO0->FIOCLR |= (1<<0);
    				retardo();
    			}
    		}
    		LPC_GPIO0->FIOSET   |= (1<<0); // Luego de la secuencia pone el P0.0 en 1.
    		flag &= ~(1<<0); 			   // Limpia flag de activación de secuencia A.

    	}
    	/* Si se presionó P2.1 se saca la secuencia B por P0.1 */
    	else if(flag & (1<<1)){
    		for(uint8_t j = 0; j<SIZE ; j++){
    			if(secB[j] == 1){
    			    LPC_GPIO0->FIOSET |= (1<<1);
    			    retardo();
    			}
    			else {
    				LPC_GPIO0->FIOCLR |= (1<<1);
    				retardo();
    			}
    		}
    		LPC_GPIO0->FIOSET   |= (1<<1); // Luego de la secuencia pone el P0.1 en 1.
    		flag &= ~(1<<1); 			   // Limpia flag de activación de secuencia B.
    	}
    }
    return 0 ;
}

/* Función de configuración de GPIO */
void configGPIO(void){
	LPC_PINCON->PINSEL4 &= ~(0x0F);   	// P2.0 y P2.1 como GPIO con pull-down
	LPC_PINCON->PINMODE4 |= 0xF;
	LPC_PINCON->PINSEL0 &= ~(0x0F);	  	// P0.0 y P0.1 como GPIO de salida y en estado activo.
	LPC_GPIO0->FIODIR   |= (3<<0);
	LPC_GPIO0->FIOSET   |= (3<<0);
	return;
}

/* Función de configuración de interrupciones por GPIO */
void configGPIOInt(void){
	LPC_GPIOINT->IO2IntEnR |= (1<<0);	// Habilita interrupcion por flanco de subida en P2.0.
	LPC_GPIOINT->IO2IntEnF |= (1<<1);   // Habilita interrupcion por flanco de subida en P2.1.
	LPC_GPIOINT->IO2IntClr |= (3<<0);   // Limpia flags de interrupción en P2.0 y P2.1
	NVIC_EnableIRQ(EINT3_IRQn);			// Habilita interrupciones de GPIO en el NVIC.
	return;
}

/* ISR de Interrupcion por puerto */
void EINT3_IRQHandler(void){
	if(LPC_GPIOINT->IO2IntStatR & (1<<0)){
		LPC_GPIOINT->IO2IntClr |= (1<<0);
		flag |= (1<<0);						// Pone en 1 el bit 0 del flag para habilitar secuencia A.
	}
	if(LPC_GPIOINT->IO2IntStatF & (1<<1)){
		LPC_GPIOINT->IO2IntClr |= (1<<1);   // Pone en 1 el bit 1 del flag para habilitar secuencia B.
		flag |= (1<<1);
	}
	return;
}

/* Retardo ajustado para poder visualizar secuencia.*/
void retardo(void){
	for(uint32_t conta = 0 ; conta < 1000000 ; conta++){};
	return;
}
