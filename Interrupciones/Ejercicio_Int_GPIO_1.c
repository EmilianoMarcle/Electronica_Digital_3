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

#define SIZE (uint8_t)  9

void configGPIO(void);
void configGPIOInt(void);
void retardo(void);

uint8_t secA[SIZE] = {0, 1, 0, 0, 1, 1, 0, 1, 0};
uint8_t secB[SIZE] = {0, 1, 1, 1, 0, 0, 1, 1, 0};
uint8_t flag0 = 0, flag1 = 0;

int main(void) {

	configGPIO();
	configGPIOInt();

    while(1) {
    	if(flag0){
    		for(uint8_t i = 0 ; i<SIZE ;i++){
    			if(secA[i] == 1){
    				LPC_GPIO0->FIOSET |= (1<<0);
    				retardo();
    			}
    			else LPC_GPIO0->FIOCLR |= (1<<0);
    		}
    		flag0 = 0;
    	}
    	else if(flag1){
    		for(uint8_t j = 0; j<SIZE ; j++){
    			if(secB[j] == 1){
    			    LPC_GPIO0->FIOSET |= (1<<1);
    			    retardo();
    			}
    			else LPC_GPIO0->FIOCLR |= (1<<1);
    		}
    		flag1 = 0;
    	}
    }
    return 0 ;
}

void configGPIO(void){
	LPC_PINCON->PINSEL4 &= ~(0x0F);
	LPC_PINCON->PINSEL0 &= ~(0x0F);
	LPC_GPIO0->FIODIR |= (1<<0) | (1<<1); // P0.0 y P0.1 como salidas.
	return;
}

void configGPIOInt(void){
	LPC_GPIOINT->IO2IntEnR |= (1<<0);
	LPC_GPIOINT->IO2IntEnF |= (1<<1);
	LPC_GPIOINT->IO2IntClr |= (3<<0);
	NVIC_EnableIRQ(EINT3_IRQn);
	return;
}

void EINT3_IRQHandler(void){
	if(LPC_GPIOINT->IO2IntStatR & (1<<0)){
		LPC_GPIOINT->IO2IntClr |= (1<<0);
		flag0++;
	}
	if(LPC_GPIOINT->IO2IntStatF & (1<<1)){
		LPC_GPIOINT->IO2IntClr |= (1<<1);
		flag1++;
	}
	return;
}

void retardo(void){
	for(uint32_t conta = 0 ; conta < 100000 ; conta++){};
	return;
}
