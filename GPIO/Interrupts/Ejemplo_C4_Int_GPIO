/*
===============================================================================
 Nombre      : Ejemplo_Int_GPIO.c
 Alumno      : Marclé, Emiliano
 Carrera     : Ing. Electrónica
 Description : Este programa realiza el parpadeo de un LED conectado en P0.22
 	 	 	         y mediante un boton conectado en P0.15 modifica la velocidad
 	 	 	          de parpadeo.
 	 	 	         Se utiliza interrupcion por GPIO por flanco de subida.
===============================================================================
*/

#include "LPC17xx.h"

//Prototipos de funciones.
void retardo(uint32_t tiempo);
void configGPIO(void);
void configIntGPIO(void);

//Variables Globales.
uint32_t inte = 0;

int main(void) {
	uint32_t tiempo;

	configGPIO();
	configIntGPIO();

    while(1) {
    	//Seleccion de tiempo de retardo.
    	//**Las variables dentro de los Handlers deben ser globales.
    	if(inte%2){
    		tiempo = 1000000;
    	}
    	else {
    		tiempo = 6000000;
    	}
    	//Parpadeo del LED.
    	LPC_GPIO0->FIOSET |= (1<<22);
    	retardo(tiempo);
    	LPC_GPIO0->FIOCLR |= (1<<22);
    	retardo(tiempo);
    }
    return 0 ;
}

void retardo(uint32_t tiempo){
	uint32_t conta;
	for(conta=0 ; conta<tiempo ; conta++){}
	return;
}

void configGPIO(void){
	// PINSEL SE DEJA POR DEFECTO EN GPIO.
	LPC_PINCON->PINMODE0 |= (3<<30); //PullDown en P0.15.
	LPC_GPIO0->FIODIR |= (1<<22); // Configura P0.22 como salida
	LPC_GPIO0->FIODIR &= ~(1<<15);// Configura P0.15 como entrada
	return;
}

void configIntGPIO(void){
	LPC_GPIOINT->IO0IntEnR |= (1<<15); // Habilita la interrupcion por flanco de subida en P0.15.
	LPC_GPIOINT->IO0IntClr |= (1<<15); // Limpia la bandera de interrupcion en P0.15.
	NVIC_EnableIRQ(EINT3_IRQn);		   // Habilita la interrupcion en el NVIC.
	return;
}

void EINT3_IRQHandler(){
	if((LPC_GPIOINT->IO0IntStatR) & (1<<15)){
		inte++;
		LPC_GPIOINT-> IO0IntClr |= (1<<15);
	}
	return;
}
