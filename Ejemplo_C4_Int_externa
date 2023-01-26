/*
===============================================================================
 Alumno      : Marclé Emiliano
 Carrera     : Ing. Electronica
 Descripcion : Este programa realiza el parpadeo de un LED conectado en P0.22
 	 	 	   y mediante un boton conectado en P0.15 modifica la velocidad
 	 	 	   de parpadeo.
 	 	 	   Se utiliza interrupcion externa EINT0.
===============================================================================
*/

#include "LPC17xx.h"

uint32_t tiempo;

//Prototipos de funciones.
void retardo(uint32_t tiempo);
void configGPIO(void);
void configIntExt(void);
void EINT0_IRQHandler(void);

int main(void) {

	configGPIO();
	configIntExt();

    while(1) {
    	//Seleccion de tiempo de retardo.
    	//**Las variables dentro de los Handlers deben ser globales.

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
	LPC_GPIO0->FIODIR |= (1<<22); // Configura P0.22 como salida
	return;
}

void configIntExt(void){
	LPC_PINCON->PINSEL4 |= (1<<20);  //Se configura el P2.10 como EINT0.
	LPC_PINCON->PINSEL4 &= ~(1<<21);
	LPC_SC->EXTMODE     |= 1;		 //Selecciona interrupcion por flanco.
	LPC_SC->EXTPOLAR    |= 1;		 //Selecciona interrupcion por flanco de subida.
	LPC_SC->EXTINT 		|= 1;        //Limpia la bandera de interrupcion.
	NVIC_EnableIRQ(EINT0_IRQn);      //Habilita interrupciones externas.
	return;
}

void EINT0_IRQHandler(){
	static uint32_t inte = 0;
	inte++;
	if(inte%2){
		tiempo = 1000000;
	}
	else {
		tiempo = 9000000;
	}
	LPC_SC->EXTINT |= 1; //Limpia la bandera de interrupcion externa.
	return;
}
