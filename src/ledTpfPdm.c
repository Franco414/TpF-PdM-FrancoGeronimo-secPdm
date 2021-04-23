/*
 * ledTpfPdm.c
 *
 *  Created on: Apr 21, 2021
 *      Author: franco-unt
 */
#include "sapi.h"
#include "ledTpfPdm.h"
#include "secTpfPdm.h"

float intenFloat=0;
float duracFloat=0;

static delay_t delayLed1;
static delay_t delayLed2;
static delay_t delayLed3;

static bool_t bloqLed1d=LOW;
static bool_t bloqLed2d=LOW;
static bool_t bloqLed3d=LOW;

static volatile uint8_t contadorLed1=0;
static volatile uint8_t contadorLed2=0;
static volatile uint8_t contadorLed3=0;

static volatile bool_t auxDimLed1=1;
static volatile bool_t auxDimLed2=1;
static volatile bool_t auxDimLed3=1;

static volatile uint8_t intAntLed1=0; //defino la presente variable para guardar la intensidad del paso anterior
static volatile uint8_t intAntLed2=0;
static volatile uint8_t intAntLed3=0;

static volatile uint8_t auxPWMLed1=0;
static volatile uint8_t auxPWMLed2=0;
static volatile uint8_t auxPWMLed3=0;

static volatile uint8_t aux2PWMLed1=0;
static volatile uint8_t aux2PWMLed2=0;
static volatile uint8_t aux2PWMLed3=0;

static volatile uint8_t distPWMLed1=0; //defino la presente variable para guardar la diferencia de intensidades entre el paso anterior y el paso dimerizable que se procese en ese momento
static volatile uint8_t distPWMLed2=0;
static volatile uint8_t distPWMLed3=0;

static volatile uint32_t auxIntPWMLed1=0;
static volatile uint32_t auxIntPWMLed2=0;
static volatile uint32_t auxIntPWMLed3=0;

static struct secLed led1; //defino una estructura para el led1 que almacenará la información de un determinado paso. (Intensidad, modo y duración)
static struct secLed led2;
static struct secLed led3;

void apagarSec(){
	pwmWrite( PWM7, 0 );
	pwmWrite( PWM8, 0 );
	pwmWrite( PWM9, 0 );
}

void actLed1(){
		switch(led1.modo){
			case 1: // modo apagar
				pwmWrite( PWM7, 0); //apaga el led
				intAntLed1=0; // actualiza el valor de la intensidad del led, en este caso como se apaga el mismo debe igualarse a cero
				delayWrite( &delayLed1, led1.duracion ); // Actualiza la duración del tiempo de rebalse del delay, de acuerdo a la duración del paso entregado por recibirPasoLed1()
				break;

			case 3: // modo encender con intensidad constante
				intenFloat=led1.intensidad*2.55; //Como la intensidad se expresó como un número entre 0 a 100, con esta línea se obtiene un número flotante entre 0 a 255
				auxPWMLed1=intenFloat/1;		//Obtiene la parte entera del número flotante.
				if(auxPWMLed1>255)auxPWMLed1=255; //por seguridad si el valor ingresado es mayor a 255 lo limitamos a 255
				intAntLed1=auxPWMLed1; 			  // actualiza el valor de la intensidad del led, con el valor entero obtenido en la línea anterior.
				pwmWrite( PWM7, auxPWMLed1 );
				delayWrite( &delayLed1, led1.duracion );
				break;

			case 2: // modo encender gradualmente o de manera dimerizable.

				if(auxDimLed1){
					bloqLed1d=HIGH; //Ingresa a esta rama del IF cuando por primera vez ingresa al modo dimerizable. Esta sección busca obtener el modo en que debe graduar
					auxDimLed1=LOW; //la luz, es decir si debe aumentar o disminuir los micropasos para aplicar al módulo PWM, como así también determina la duración de tiempo
					intenFloat=led1.intensidad*2.55; // que debe tener estos micropasos.
					auxPWMLed1=intenFloat/1;
					if(auxPWMLed1>255)auxPWMLed1=255;
					if(intAntLed1==auxPWMLed1){ //pregunta si el valor final de la intensidad del paso dimerizable es igual a la intensidad del paso anterior
						aux2PWMLed1=HIGH; // establece que la intensidad se mantendrá constante dado que se cumplió la condición anterior. No necesita graduar.
									}else{
										aux2PWMLed1=LOW;  // si son distintas las intensidades del paso anterior con el de la dimerización, entonces ingresa en esta sección del If.
										if(intAntLed1>auxPWMLed1){ //determina cuál de las dos cantidades es mayor para establecer el orden de la diferencia entre ellos.
											distPWMLed1=intAntLed1-auxPWMLed1;
										}else{
											distPWMLed1=auxPWMLed1-intAntLed1;
										}//obtiene la diferencia entre la intensidad del paso anterior y la de este paso para obtener la distancia que hay entre ella para conocer cuantas veces debe incrementar o decrementar para
										//que la intensidad actual iguale a la intensidad fijada en este paso dimerizable.
										duracFloat=led1.duracion/distPWMLed1; //divide la duración de este modo entre la diferencia de pasos obtenida en la línea anterior para obtener el tiempo de cada micropaso.
										auxIntPWMLed1=duracFloat/1;
										delayWrite( &delayLed1, auxIntPWMLed1 ); //actualiza el valor del tiempo del delay con el valor del micropaso obtenido anteriormente
									}
				}

				if(aux2PWMLed1){
					intenFloat=led1.intensidad*2.55; //ingresa en esta rama del if, si la intensidad del modo dimerizable es igual a la intensidad del paso anterior
					auxPWMLed1=intenFloat/1;
					if(auxPWMLed1>255)auxPWMLed1=255;
					pwmWrite( PWM7, auxPWMLed1 ); //carga el valor de la intensidad fijada por el modo dimerizable
					delayWrite( &delayLed1, led1.duracion ); //actualiza el valor del delay para le LED1, con la duración fijada en este paso.
				}else{
					if(intAntLed1==auxPWMLed1){
						//entra en esta rama del if si después de graduar el valor de la intensidad del Led1, igual a la intensidad fijada inicialmente en este paso
						auxDimLed1=HIGH; //prepara las variables de inicio de modo dimerizable y el bloque del mismo paso a fin de tenerla lista si el usuario escogió otro paso dimerizable
						bloqLed1d=LOW;   //termina este paso
					}else{
						if(intAntLed1>auxPWMLed1){
							intAntLed1--; //como la intensidad del paso anterior es mayor que la intensidad del paso dimerizable entonces decrementa la intensidad y la carga en el módulo PWM
							pwmWrite( PWM7, intAntLed1 );
						}else{
							intAntLed1++;//como la intensidad del paso anterior es menor que la intensidad del paso dimerizable entonces incrementa la intensidad y la carga en el módulo PWM
							pwmWrite( PWM7, intAntLed1 );
						}
					}
				}

				break;

			default:
				break;
			}
}

void actLed2(){

		switch(led2.modo){
			case 1:
				pwmWrite( PWM8, 0);
				intAntLed2=0;
				delayWrite( &delayLed2, led2.duracion );
				break;

			case 3:
				intenFloat=led2.intensidad*2.55;
				auxPWMLed2=intenFloat/1;
				if(auxPWMLed2>255)auxPWMLed2=255;
				intAntLed2=auxPWMLed2;
				pwmWrite( PWM8, auxPWMLed2 );
				delayWrite( &delayLed2, led2.duracion );
				break;

			case 2:

				if(auxDimLed2){
					bloqLed2d=HIGH;
					auxDimLed2=LOW;
					intenFloat=led2.intensidad*2.55;
					auxPWMLed2=intenFloat/1;
					if(auxPWMLed2>255)auxPWMLed2=255;
					if(intAntLed2==auxPWMLed2){
						aux2PWMLed2=HIGH;
									}else{
										aux2PWMLed2=LOW;
										if(intAntLed2>auxPWMLed2){
											distPWMLed2=intAntLed2-auxPWMLed2;
										}else{
											distPWMLed2=auxPWMLed2-intAntLed2;
										}
										duracFloat=led2.duracion/distPWMLed2;
										auxIntPWMLed2=duracFloat/1;
										delayWrite( &delayLed2, auxIntPWMLed2 );
									}
				}

				if(aux2PWMLed2){
					intenFloat=led2.intensidad*2.55;
					auxPWMLed2=intenFloat/1;
					if(auxPWMLed2>255)auxPWMLed2=255;
					pwmWrite( PWM8, auxPWMLed2 );
					delayWrite( &delayLed2, led2.duracion );
				}else{

					if(intAntLed2==auxPWMLed2){
						auxDimLed2=HIGH;
						bloqLed2d=LOW;
					}else{
						if(intAntLed2>auxPWMLed2){
							intAntLed2--;
							pwmWrite( PWM8, intAntLed2 );
						}else{
							intAntLed2++;
							pwmWrite( PWM8, intAntLed2 );
						}
					}
				}

				break;

			default:
				break;
			}

}

void actLed3(){

		switch(led3.modo){
			case 1:
				pwmWrite( PWM9, 0);
				intAntLed3=0;
				delayWrite(&delayLed3,led3.duracion);
				break;

			case 3:
				intenFloat=led3.intensidad*2.55;
				auxPWMLed3=intenFloat/1;
				if(auxPWMLed3>255)auxPWMLed3=255;
				intAntLed3=auxPWMLed3;
				pwmWrite( PWM9, auxPWMLed3 );
				delayWrite( &delayLed3, led3.duracion );
				break;

			case 2:

				if(auxDimLed3){
					bloqLed3d=HIGH;
					auxDimLed3=LOW;
					intenFloat=led3.intensidad*2.55;
					auxPWMLed3=intenFloat/1;
					if(auxPWMLed3>255)auxPWMLed3=255;
					if(intAntLed3==auxPWMLed3){
						aux2PWMLed3=HIGH;
									}else{
										aux2PWMLed3=LOW;
										if(intAntLed3>auxPWMLed3){
											distPWMLed3=intAntLed3-auxPWMLed3;
										}else{
											distPWMLed3=auxPWMLed3-intAntLed3;
										}
										duracFloat=led3.duracion/distPWMLed3;
										auxIntPWMLed3=duracFloat/1;
										delayWrite( &delayLed3, auxIntPWMLed3 );
									}
				}

				if(aux2PWMLed3){
					intenFloat=led3.intensidad*2.55;
					auxPWMLed3=intenFloat/1;
					if(auxPWMLed3>255)auxPWMLed3=255;
					pwmWrite( PWM9, auxPWMLed3 );
					delayWrite( &delayLed3, led3.duracion );
				}else{

					if(intAntLed3==auxPWMLed3){
						auxDimLed3=HIGH;
						bloqLed3d=LOW;
					}else{
						if(intAntLed3>auxPWMLed3){
							intAntLed3--;
							pwmWrite( PWM9, intAntLed3 );
						}else{
							intAntLed3++;
							pwmWrite( PWM9, intAntLed3 );
						}
					}
				}

				break;

			default:
				break;
			}

}

void resetSecLed(){
	contadorLed1=0;
	contadorLed2=0;
	contadorLed3=0;

	intAntLed1=0;
	intAntLed2=0;
	intAntLed3=0;

	auxDimLed1=1;
	auxDimLed2=1;
	auxDimLed3=1;

	bloqLed1d=LOW; //Con este valor permite que la función actLed() actualice la secuencia del Led1
	bloqLed2d=LOW;
	bloqLed3d=LOW;

	aux2PWMLed1=0;
	aux2PWMLed2=0;
	aux2PWMLed3=0;

	led1.duracion=1000;
	led1.intensidad=0;
	led1.modo=1;
	led2.duracion=1000;
	led2.intensidad=0;
	led2.modo=1;
	led3.duracion=1000;
	led3.intensidad=0;
	led3.modo=1;

	led1=recibirPasoLed1(&contadorLed1,1);
	led2=recibirPasoLed2(&contadorLed2,1);
	led3=recibirPasoLed3(&contadorLed3,1);

	delayWrite(&delayLed1,led1.duracion );
	delayWrite(&delayLed2,led2.duracion );
	delayWrite(&delayLed3,led3.duracion );
}

void actSecLeds(bool_t *resetActSec){
	if(*resetActSec){
		*resetActSec=LOW; //Ingresa en esta sección del if cuando se solicita el reinicio de la visualización de la secuencia mediante los leds, por medio del puntero *resetActSec cuando el valor de la variable a la que apunta es TRUE.
		resetSecLed();
	}
	if(delayRead(&delayLed1)){
		if(bloqLed1d==LOW)led1=recibirPasoLed1(&contadorLed1,LOW); //El bloqueo que hace la variable bloqLed1d funciona solo para el modo dimerizable, en donde prohíbe que avance con la secuencia hasta que no termine de iluminar gradualmente
		actLed1();	//una vez que tiene el valor de la secuencia a mostrar para el LED1 invoca a la función actLed1 para actualizar el estado del LED1
	}
	if(delayRead(&delayLed2)){
		if(bloqLed2d==LOW)led2=recibirPasoLed2(&contadorLed2,LOW);
		actLed2();
	}
	if(delayRead(&delayLed3)){
		if(bloqLed3d==LOW)led3=recibirPasoLed3(&contadorLed3,LOW);
		actLed3();
	}

}

void configDelay(){
	   delayConfig( &delayLed1, 5000 ); //Inicializa el delay para el Led1 con un intervalo de tiempo de 5 segundos.
	   delayConfig( &delayLed2, 5000 );
	   delayConfig( &delayLed3, 5000 );
	   bloqLed1d=LOW;
	   bloqLed2d=LOW;
	   bloqLed3d=LOW;
}
