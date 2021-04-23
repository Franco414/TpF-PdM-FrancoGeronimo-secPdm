/*
 * secTpfPdm.c
 *
 *  Created on: Apr 21, 2021
 *      Author: franco-unt
 */
#include "utilFeg.h"
#include "secTpfPdm.h"
#include "sapi.h"
#include "string.h"

static volatile bool_t resetActSec=LOW;

static gpioMap_t ledSec;

static uint8_t auxAnt=0;

static bool_t banLedSec=LOW;
static bool_t banPasoSec=LOW;

volatile char datoRecSec=0;

static bool_t inicioPaso=HIGH;

static bool_t ErrorFlagPaso=LOW;

static struct secLed secLED1[10];
static struct secLed secLED2[10];
static struct secLed secLED3[10];

static struct secLed secAuxUlt[10];
static volatile uint8_t secAuxTam=0;
static volatile uint8_t secAuxID=0;

static volatile uint8_t numPasosLED1=10;
static volatile uint8_t numPasosLED2=10;
static volatile uint8_t numPasosLED3=10;
static volatile bool_t EnableSec=LOW;

static volatile estadoMefSec estActSec=InicioSec;
static volatile evento3Mef evenSecAct=ledSep;

static estNameSec nameLed=letraLed;

struct auxPaso auxPasoMef;  //esta estructura permite guardar temporalmente un paso de la secuencia;
static pasoEstMef pasoEstado=inicioPasoMef;

static volatile uint8_t conSecLED1=0;
static volatile uint8_t conSecLED2=0;
static volatile uint8_t conSecLED3=0;

volatile struct tranSec tablaEventoSec[4][6]={
		{{fncLedSec,InicioSec},{fncPasoSec,esperaSec},{fncErrorSec,errorSec},{fncErrorSec,errorSec},{fncErrorSec,errorSec},{fncErrorSec,errorSec}},
		{{fncErrorSec,errorSec},{fncErrorSec,errorSec},{fncPasoSec,esperaSec},{fncGuardarSec,guardarSec},{fncFinSec,InicioSec},{fncErrorSec,errorSec}},
		{{fncErrorSec,errorSec},{fncErrorSec,errorSec},{fncPasoSec,esperaSec},{fncErrorSec,errorSec},{fncFinSec,InicioSec},{fncErrorSec,errorSec}},
		{{fncLedSec,InicioSec},{fncErrorSec,errorSec},{fncErrorSec,errorSec},{fncErrorSec,errorSec},{fncErrorSec,errorSec},{fncConfSec,InicioSec}},
};

struct recupSec{
	bool_t enableLed;
	bool_t modifSecLed;
};

static struct recupSec recuLed1;
static struct recupSec recuLed2;
static struct recupSec recuLed3;

void SecDefault(){
	secLED1[0].duracion=5000;
	secLED1[0].intensidad=100;
	secLED1[0].modo=3;

	secLED1[1].duracion=8000;
	secLED1[1].intensidad=20;
	secLED1[1].modo=2;

	secLED1[2].duracion=6000;
	secLED1[2].intensidad=0;
	secLED1[2].modo=1;

	secLED1[3].duracion=3000;
	secLED1[3].intensidad=50;
	secLED1[3].modo=3;

	secLED1[4].duracion=8000;
	secLED1[4].intensidad=70;
	secLED1[4].modo=3;

	secLED1[5].duracion=6000;
	secLED1[5].intensidad=10;
	secLED1[5].modo=3;

	secLED1[6].duracion=5000;
	secLED1[6].intensidad=0;
	secLED1[6].modo=1;

	secLED1[7].duracion=8000;
	secLED1[7].intensidad=90;
	secLED1[7].modo=2;

	secLED1[8].duracion=15000;
	secLED1[8].intensidad=0;
	secLED1[8].modo=2;

	secLED1[9].duracion=2000;
	secLED1[9].intensidad=50;
	secLED1[9].modo=3;
}

void borraAuxPaso(){
	char aux=strlen(auxPasoMef.auxIntensidad);
	for(uint8_t i=0;i<aux;i++)auxPasoMef.auxIntensidad[i]=0;
	aux=strlen(auxPasoMef.auxTiempo);
	for(uint8_t i=0;i<aux;i++)auxPasoMef.auxTiempo[i]=0;
	auxPasoMef.modo=0;
	auxPasoMef.contDigInt=0;
	auxPasoMef.contDigTiempo=0;
}

void fncLedSec(){
	bool_t repLoop=LOW; // se usa esta variable en caso de que aparezca un error en la carga de datos mediante la presente función que necesite volver a ingresar al switch para entrar en el estado errorLed

	do{
		repLoop=LOW; //se inicia el valor en LOW que corresponde cuando no hay errores. En caso de error este valor cambia a uno y se repite el ciclo por la estructura do-while

		switch(nameLed){
		case iniNameLed:
			nameLed=letraLed;
			break;

		case letraLed:
			if(datoRecSec=='l'|| datoRecSec=='L'){ //Para referirse al nombre del led acepta el formato L1 o l1, L2 o l2, L3 o l3.
				resetActSec=LOW;
				nameLed=numeroLed;
			}else{
				//si se recibe otro caracter que no sea l o L, ingresa en esta sección.
				repLoop=HIGH; //cambia su valor de LOW a HIGH para repetir el ciclo y así ingresar al estado errorLed
				nameLed=errorLed;
			}
			break;

		case numeroLed: //en este caso espera como carácter el 1, 2, 3, a fin de terminar de definir el led de la secuencia a cargar.
			banLedSec=HIGH;
			switch(datoRecSec){
			case '1':
				ledSec=LED1; //identifica al LED1 al cual debe guardar la secuencia. Por tal fin lo almacena en la variable ledSec del tipo gpioMap_t.
				conSecLED1=0;//inicia el contador para el led1, que se usa para participar en la carga de su correspondiente secuencia.
				recuLed1.modifSecLed=HIGH; //cargar el valor a TRUE de la presente variable permitirá conocer que secuencia se está modificando en los pasos posteriores.
				if(recuLed1.enableLed){  //la condición de la variable recuLed1.enableLed=TRUE, indica que previamente se cargó exitosamente una secuencia en el LEd1 y por ende debe almacenarla en una
					guardarUltSec(1,numPasosLED1); // secuencia auxiliar
				}
				break;

			case '2':
				ledSec=LED2;
				conSecLED2=0;
				recuLed2.modifSecLed=HIGH;
				if(recuLed2.enableLed){
					guardarUltSec(2,numPasosLED2);
				}
				break;

			case '3':
				ledSec=LED3;
				conSecLED3=0;
				recuLed3.modifSecLed=HIGH;
				if(recuLed3.enableLed){
					guardarUltSec(3,numPasosLED3);
				}
				break;

			default:	//que llegue a este caso significa que se recibió otro carácter distinto a 1,2,3
				repLoop=HIGH;
				nameLed=errorLed;
				break;
			}
			break;

			case errorLed:
				uartWriteString(UART_USB,"Error Led \r\n");
				repLoop=LOW; //cambia su valor de HIGH a LOW para dejar de repetir el do-while
				ErrorFlagPaso=HIGH;//indica que ocurrio un fallo en la carga de la secuencia
				nameLed=letraLed;
				break;
		}

	}while(repLoop);

}


void fncPasoSec(){
	bool_t repPaso=LOW;// se usa esta variable en caso de que aparezca un error en la carga de datos mediante la presente función que necesite volver a ingresar al switch para entrar en el estado errorPaso
	do{
		repPaso=LOW;
		switch(pasoEstado){

		case inicioPasoMef: //ingresa en este caso cuando se recibe una coma.
			pasoEstado=tiempoPaso;
			break;

		case tiempoPaso:
			if(datoRecSec>=48 && datoRecSec<=57){ // analiza si el dato recibido por la interfaz serial es un carácter numérico.
				banPasoSec=HIGH;
				auxAnt=auxPasoMef.contDigTiempo;  // Toma el valor del indice de la cadena auxPasoMef.auxTiempo para poder cargar el tiempo del paso mediante caracteres
				auxPasoMef.auxTiempo[auxAnt]=datoRecSec; //guarda el caracter recibido de la interfaz uart en la cadena auxPasoMef.auxTiempo
				auxAnt++; //aumenta la variable auxiliar para manejar el indice de la cadena auxPasoMef.auxTiempo
				auxPasoMef.contDigTiempo=auxAnt; //actualiza la cadena auxPasoMef.auxTiempo
			}else{
				pasoEstado=modoPaso; //ingresa en esta sección si se recibe un carácter que no es numérico.
				repPaso=HIGH; // repite el cilo do-while para ingresar al estado modoPaso y analizar este caracter no numérico recibido
			}
			break;

		case modoPaso:
			if(banPasoSec){
				pasoEstado=intensPaso;
				switch(datoRecSec){
				case 97: //'a'
					uartWriteString(UART_USB,"modo apagar \r\n");
					auxPasoMef.modo=1; // indica que el modo de encendido del paso es apagado
					break;

				case 100: //'d'
					uartWriteString(UART_USB,"modo dimmer \r\n");
					auxPasoMef.modo=2; // indica que el modo de encendido del paso es luz dimerizable
					break;

				case 101: // 'e'
					uartWriteString(UART_USB,"modo encender \r\n");
					auxPasoMef.modo=3; // indica que el modo de encendido del paso es encender
					break;

				default:
					auxPasoMef.modo=0;  // entra en default si el datoRecibido es distinto de 'a','e','d'
					pasoEstado=errorPaso;
					repPaso=HIGH;
					break;
				}
			}else{
				pasoEstado=errorPaso;
				repPaso=HIGH;
			}


			break;

			case intensPaso:
				if(datoRecSec>=48 && datoRecSec<=57){
					auxAnt=auxPasoMef.contDigInt;
					auxPasoMef.auxIntensidad[auxAnt]=datoRecSec;
					auxAnt++;
					auxPasoMef.contDigInt=auxAnt;
				}else{
					if(datoRecSec!=',' && datoRecSec!='.'){
						pasoEstado=errorPaso;
						repPaso=HIGH;
					}
				}
				break;

			case errorPaso:
				uartWriteString(UART_USB,"Error paso \r\n");
				banPasoSec=LOW;
				repPaso=LOW; //cambia su valor de HIGH a LOW para dejar de repetir el do-while
				ErrorFlagPaso=HIGH;//indica que ocurrio un fallo en la carga de la secuencia
				break;

		}
	}while(repPaso);

}

void fncGuardarSec(){
	EnableSec=LOW; //Indica a la capa main que detenga la presentación de la secuencia mediante los leds
	escEstSec(); //Invoca a la siguiente función escEstSec(); para guardar el paso valido ingresado
	pasoEstado=tiempoPaso; // prepara el estado de la mef que controla la carga del paso para recibir una nueva carga empezando por recibi el tiempo del paso
	borraAuxPaso(); //borra la estructura auxiliar a fin de preparala para recibir un nuevo paso de la secuencia
}

void escEstSec(){
	uartWriteString(UART_USB,"ingreso un paso valido \r\n");
	uartWriteString(UART_USB,"El paso es: \r\n");
	uartWriteString(UART_USB,"Nombre del led: ");
	switch(ledSec){ // basándonos en el valor de ledSec que contiene el led al cual se realiza la modificación de su secuencia, se analiza sus posibles casos
	case LED1:
		if(conSecLED1<10){//controla que no se carguen mas elementos de acuerdo al tamaño maximo que tiene la secuencia
			secLED1[conSecLED1].duracion=myAtoi(auxPasoMef.auxTiempo); //convierte la cadena de caracteres que contiene la duración del paso almacenado en caracteres a un número entero y lo guarda en la secuencia
			secLED1[conSecLED1].intensidad=myAtoi(auxPasoMef.auxIntensidad); //convierte la cadena de caracteres que contiene la intensidad del paso almacenado en caracteres a un número entero y lo guarda en la secuencia
			secLED1[conSecLED1].modo=auxPasoMef.modo; //guarda en la secuencia correspondiente al led1 el modo que fue almacenado en la estructura auxiliar auxPasoMef
			conSecLED1++; //aumenta el indice de la secuencia del led1
			numPasosLED1=conSecLED1; //actualiza el tamaño de la secuencia Led1 según el valor del índice actual de la secuencia del led1
		}
		uartWriteString(UART_USB," LED1\r\n");
		break;
	case LED2:
		if(conSecLED2<10){
			secLED2[conSecLED2].duracion=myAtoi(auxPasoMef.auxTiempo);
			secLED2[conSecLED2].intensidad=myAtoi(auxPasoMef.auxIntensidad);
			secLED2[conSecLED2].modo=auxPasoMef.modo;
			conSecLED2++;
			numPasosLED2=conSecLED2;
		}
		uartWriteString(UART_USB," LED2\r\n");
		break;
	case LED3:
		if(conSecLED3<10){
			secLED3[conSecLED3].duracion=myAtoi(auxPasoMef.auxTiempo);
			secLED3[conSecLED3].intensidad=myAtoi(auxPasoMef.auxIntensidad);
			secLED3[conSecLED3].modo=auxPasoMef.modo;
			conSecLED3++;
			numPasosLED3=conSecLED3;
		}
		uartWriteString(UART_USB," LED3\r\n");
		break;
	}
	uartWriteString(UART_USB,"tiempo= ");// a partir de aquí hasta el final de la presente función, se dedica a mostrar el paso guardado vía interfaz serial.
	uartWriteString(UART_USB,auxPasoMef.auxTiempo);
	uartWriteString(UART_USB,"\r\n");
	uartWriteString(UART_USB,"intensidad= ");
	uartWriteString(UART_USB,auxPasoMef.auxIntensidad);
	uartWriteString(UART_USB,"\r\n");
	uartWriteString(UART_USB,"modo= ");
	char aux2=auxPasoMef.modo+48;
	uartTxWrite(UART_USB,aux2);
	uartWriteString(UART_USB,"\r\n");
}
void fncErrorSec(){
	//limpia las variables utilizadas durante la carga de los datos
	auxAnt=0;
	borraAuxPaso();
	banLedSec=LOW;
	datoRecSec=0;
	inicioPaso=HIGH;
	ErrorFlagPaso=LOW;

	banPasoSec=LOW;

	resetActSec=LOW;

	conSecLED1=0;
	conSecLED2=0;
	conSecLED3=0;

	nameLed=letraLed;
	pasoEstado=inicioPasoMef;

	uartWriteString(UART_USB,"Secuencia Invalida \r\n");
	uartWriteString(UART_USB,"Se ha detectado un error en la secuencia\r\n");

	if(recuLed1.modifSecLed==HIGH && recuLed1.enableLed==HIGH){
		resetActSec=HIGH;
		EnableSec=HIGH;

		secBorrar(1);//borra la secuencia del led1
		recuLed1.modifSecLed=0;
		recuLed1.enableLed=0;
		recSec(1);//recupera la secuencia del led1
		//verSecLed1();
		//verSecLed2();
		//verSecLed3();

	}else{
		if(recuLed2.modifSecLed==HIGH && recuLed2.enableLed==HIGH){
			resetActSec=HIGH;
			EnableSec=HIGH;

			secBorrar(2);
			recuLed2.modifSecLed=0;
			recuLed2.enableLed=0;
			recSec(2);
			//verSecLed1();
			//verSecLed2();
			//verSecLed3();

		}else{
			if(recuLed3.modifSecLed==HIGH && recuLed3.enableLed==HIGH){
				resetActSec=HIGH;

				EnableSec=HIGH;
				secBorrar(3);
				recuLed3.modifSecLed=0;
				recuLed3.enableLed=0;
				recSec(3);
				//verSecLed1();
				//verSecLed2();
				//verSecLed3();

			}
		}
	}
}
void fncConfSec(){
	//prepara las variables necesarias para recibir una nueva secuencia
	borraAuxPaso();
	auxAnt=0;
	banLedSec=LOW;
	datoRecSec=0;
	inicioPaso=HIGH;
	ErrorFlagPaso=LOW;
	banPasoSec=LOW;

	nameLed=letraLed;
	pasoEstado=inicioPasoMef;

}
void fncFinSec(){
	//ingresa cuando se recibe correctamente una secuencia.
	//Muestra la secuencia via UART.
	//prepara las variables necesarias para recibir una nueva secuencia
	escEstSec();
	borraAuxPaso();
	verSecLed1();
	auxAnt=0;
	datoRecSec=0;
	borraAuxPaso();
	nameLed=letraLed;
	pasoEstado=inicioPasoMef;
	banLedSec=LOW;
	datoRecSec= 0;
	inicioPaso=HIGH;
	ErrorFlagPaso=LOW;
	datoRecSec=0;

	banPasoSec=LOW;

	EnableSec=HIGH;
	resetActSec=HIGH;

	conSecLED1=0;
	conSecLED2=0;
	conSecLED3=0;

	uartWriteString(UART_USB,"Secuencia Valida \r\n");
	uartWriteString(UART_USB,"La sec es: \r\n");

	if(recuLed1.modifSecLed){
		recuLed1.modifSecLed=0;//Indica que ya no se modifica la secuencia Led1
		recuLed1.enableLed=1;//indica que se ingreso con exito una nueva secuencia para el Led1
		verSecLed1();
	}else{
		if(recuLed2.modifSecLed){
			recuLed2.modifSecLed=0;//Indica que ya no se modifica la secuencia Led2
			recuLed2.enableLed=1;//indica que se ingreso con exito una nueva secuencia para el Led2
			verSecLed2();
		}else{
			if(recuLed3.modifSecLed){
				recuLed3.modifSecLed=0;//Indica que ya no se modifica la secuencia Led3
				recuLed3.enableLed=1;//indica que se ingreso con exito una nueva secuencia para el Led3
				verSecLed3();
			}
		}
	}
}




void dispatcherSec(estadoMefSec estActual,evento3Mef eventoAct){
	void(*funcion)(void);
	funcion=tablaEventoSec[estActual][eventoAct].fncSec;
	funcion();//en base a la tabla de transiciones ejecuta la acción correspondiente
	estActSec=tablaEventoSec[estActual][eventoAct].estProxSec;//actualiza el estado dela MEF general con el siguiente estado dado en la tabla de transiciones
}

void detectEventSec(char *ptrDatoSec,uint8_t errorSecu){

	if(errorSecu){
		evenSecAct=errorCad; //prioriza el evento error durante la carga de la estructura
	}else{
		switch(*ptrDatoSec){

		case ',':
			if(inicioPaso){//diferencia entre la primera coma despues del l1 o l2 o l3, y la coma separador de pasos
				inicioPaso=0;
				evenSecAct=primerSep;
			}else{
				evenSecAct=ContSep;
			}
			break;

		case '.': //caracter usado para indicar el final de la secuencia
			evenSecAct=finSep;
			break;

		default:
			if(inicioPaso){
				evenSecAct=ledSep;
			}else{
				evenSecAct=nuevoDatoSep;
			}
			break;
		}
	}

}

void recSec(uint8_t nLed){

	for(int i=0;i<secAuxTam;i++){
		switch(nLed){
		case 1:
			secLED1[i].duracion=secAuxUlt[i].duracion;
			secLED1[i].intensidad=secAuxUlt[i].intensidad;
			secLED1[i].modo=secAuxUlt[i].modo;
			numPasosLED1=secAuxTam;
			break;
		case 2:
			secLED2[i].duracion=secAuxUlt[i].duracion;
			secLED2[i].intensidad=secAuxUlt[i].intensidad;
			secLED2[i].modo=secAuxUlt[i].modo;
			numPasosLED2=secAuxTam;
			break;
		case 3:
			secLED3[i].duracion=secAuxUlt[i].duracion;
			secLED3[i].intensidad=secAuxUlt[i].intensidad;
			secLED3[i].modo=secAuxUlt[i].modo;
			numPasosLED3=secAuxTam;
			break;
		}
	}
	secAuxID=0;


	secAuxTam=1;

}

void guardarUltSec(uint8_t nLed,uint8_t maxPasos){
	for(int j=0;j<10;j++){
		secAuxUlt[j].duracion=1000;
		secAuxUlt[j].intensidad=0;
		secAuxUlt[j].modo=1;
	}
	secAuxID=nLed;
	secAuxTam=maxPasos;
	for(int i=0;i<maxPasos;i++){
		switch(nLed){
		case 1:
			secAuxUlt[i].duracion=secLED1[i].duracion;
			secAuxUlt[i].intensidad=secLED1[i].intensidad;
			secAuxUlt[i].modo=secLED1[i].modo;
			break;
		case 2:
			secAuxUlt[i].duracion=secLED2[i].duracion;
			secAuxUlt[i].intensidad=secLED2[i].intensidad;
			secAuxUlt[i].modo=secLED2[i].modo;
			break;
		case 3:
			secAuxUlt[i].duracion=secLED3[i].duracion;
			secAuxUlt[i].intensidad=secLED3[i].intensidad;
			secAuxUlt[i].modo=secLED3[i].modo;
			break;
		}
	}

}

void cargarSecuencia(char *ptrDatoSec, bool_t *ptrEnableSec,bool_t *ptrResetActSec){
	datoRecSec=*ptrDatoSec; //Recoge el dato enviado por el usuario desde la interfaz serial y lo almacena en la variable datoRecSec
	detectEventSec(&datoRecSec,ErrorFlagPaso); //invoca a la función para determinar si debe lanzar un evento en función del carácter recibido como así también del valor de la variable ErrorFlagPaso
	dispatcherSec(estActSec,evenSecAct); //invoca a esta función con el objetivo de establecer la acción a realizar, como así también del estado próximo es función del estado actual y de evento producido.
	*ptrEnableSec=EnableSec; //carga el valor de EnableSec de esta capa y la envía a la capa main, actualizando el valor del puntero ptrEnableSec. Este valor habilita o deshabilita la actualización de la secuencia a través de los leds.
	*ptrResetActSec=resetActSec; //carga el valor de EnableSec de esta capa y la envía a la capa main, actualizando el valor del puntero ptrResetActSec. Este valor determina si se reinicia la actualización de la secuencia a través de los leds.
}

struct secLed recibirPasoLed1(uint8_t *contaLed1,bool_t resetSecled1){
	struct secLed auxPasoLed1; //declara una estructura de tipo secLed para cargarla y retornarla al final de la función
	if(resetSecled1){
		//entrega el primer elemento de la secuencia
		auxPasoLed1.duracion=secLED1[0].duracion;
		auxPasoLed1.intensidad=secLED1[0].intensidad;
		auxPasoLed1.modo=secLED1[0].modo;
		*contaLed1=0;
	}else{
		//entrega el elemento *contaLed1 + 1 de la secuencia
		auxPasoLed1.duracion=secLED1[*contaLed1].duracion;
		auxPasoLed1.intensidad=secLED1[*contaLed1].intensidad;
		auxPasoLed1.modo=secLED1[*contaLed1].modo;
		*contaLed1=*contaLed1+1;//actualiza el indice de la secuencia.
		if(*contaLed1==numPasosLED1)*contaLed1=0;//reinicia la presentación de la secuencia de ledq
	}
	return auxPasoLed1; //retorna la ewstructura cargada
}

struct secLed recibirPasoLed2(uint8_t *contaLed2,bool_t resetSecled2){
	struct secLed auxPasoLed2;
	if(resetSecled2){
		auxPasoLed2.duracion=secLED2[0].duracion;
		auxPasoLed2.intensidad=secLED2[0].intensidad;
		auxPasoLed2.modo=secLED2[0].modo;
		*contaLed2=0;
	}else{

		auxPasoLed2.duracion=secLED2[*contaLed2].duracion;
		auxPasoLed2.intensidad=secLED2[*contaLed2].intensidad;
		auxPasoLed2.modo=secLED2[*contaLed2].modo;
		*contaLed2=*contaLed2+1;
		if(*contaLed2==numPasosLED2)*contaLed2=0;
	}
	return auxPasoLed2;
}

struct secLed recibirPasoLed3(uint8_t *contaLed3,bool_t resetSecled3){
	struct secLed auxPasoLed3;
	if(resetSecled3){
		auxPasoLed3.duracion=secLED3[0].duracion;
		auxPasoLed3.intensidad=secLED3[0].intensidad;
		auxPasoLed3.modo=secLED3[0].modo;
		*contaLed3=0;
	}else{
		auxPasoLed3.duracion=secLED3[*contaLed3].duracion;
		auxPasoLed3.intensidad=secLED3[*contaLed3].intensidad;
		auxPasoLed3.modo=secLED3[*contaLed3].modo;
		*contaLed3=*contaLed3+1;
		if(*contaLed3==numPasosLED3)*contaLed3=0;
	}
	return auxPasoLed3;
}

void secBorrar(uint8_t nLed){

	for(int i=0;i<10;i++){ //Borra 10 elementos de una secuencia determinada
		switch(nLed){
		case 0: //borra todas las secuencias
			numPasosLED1=1;
			numPasosLED2=1;
			numPasosLED3=1;
			recuLed1.enableLed=0;
			recuLed2.enableLed=0;
			recuLed3.enableLed=0;
			recuLed1.modifSecLed=0;
			recuLed2.modifSecLed=0;
			recuLed3.modifSecLed=0;
			secLED1[i].duracion=1000;
			secLED1[i].intensidad=0;
			secLED1[i].modo=1;
			secLED2[i].duracion=1000;
			secLED2[i].intensidad=0;
			secLED2[i].modo=1;
			secLED3[i].duracion=1000;
			secLED3[i].intensidad=0;
			secLED3[i].modo=1;
			break;
		case 1: //borra la secuencia del Led1
			numPasosLED1=1;
			secLED1[i].duracion=1000;
			secLED1[i].intensidad=0;
			secLED1[i].modo=1;//modo apagar
			break;
		case 2://borra la secuencia del Led2
			numPasosLED2=1;
			secLED2[i].duracion=1000;
			secLED2[i].intensidad=0;
			secLED2[i].modo=1;//modo apagar
			break;
		case 3://borra la secuencia del Led3
			numPasosLED3=1;
			secLED3[i].duracion=1000;
			secLED3[i].intensidad=0;
			secLED3[i].modo=1;//modo apagar
			break;
		}
	}
}

void verSecLed1(){
	char auxCadena[20];
	for(int i=0;i<10;i++){ //mostrará 10 elementos de la secuencia almacenada para el led1 mediante la interfaz serial
		uartWriteString(UART_USB,"LED1\r\n");
		uartWriteString(UART_USB,"Pasos N:");

		char auxMostrar=i+48;

		uartWriteByte(UART_USB,auxMostrar);
		uartWriteString(UART_USB,"\r\n");
		uartWriteString(UART_USB,"tiempo= ");

		itoa(secLED1[i].duracion,auxCadena,10);//convierte el valor entero secLED1[i].duración en caracteres en donde lo almacena en la cadena auxCadena

		uartWriteString(UART_USB,auxCadena);
		uartWriteString(UART_USB,"\r\n");
		uartWriteString(UART_USB,"intensidad= ");

		itoa(secLED1[i].intensidad,auxCadena,10);//convierte el valor entero secLED1[i].intensidad en caracteres en donde lo almacena en la cadena auxCadena

		uartWriteString(UART_USB,auxCadena);
		uartWriteString(UART_USB,"\r\n");
		uartWriteString(UART_USB,"modo= ");
		char aux2=secLED1[i].modo+48;
		uartTxWrite(UART_USB,aux2);
		uartWriteString(UART_USB,"\r\n");
	}
}

void verSecLed2(){
	char auxCadena[20];
	for(int i=0;i<10;i++){
		uartWriteString(UART_USB,"LED2\r\n");
		uartWriteString(UART_USB,"Pasos N:");
		char auxMostrar=i+48;
		uartWriteByte(UART_USB,auxMostrar);
		uartWriteString(UART_USB,"\r\n");
		uartWriteString(UART_USB,"tiempo= ");
		itoa(secLED2[i].duracion,auxCadena,10);
		uartWriteString(UART_USB,auxCadena);
		uartWriteString(UART_USB,"\r\n");
		uartWriteString(UART_USB,"intensidad= ");
		itoa(secLED2[i].intensidad,auxCadena,10);
		uartWriteString(UART_USB,auxCadena);
		uartWriteString(UART_USB,"\r\n");
		uartWriteString(UART_USB,"modo= ");
		char aux2=secLED2[i].modo+48;
		uartTxWrite(UART_USB,aux2);
		uartWriteString(UART_USB,"\r\n");
	}
}

void verSecLed3(){
	char auxCadena[20];
	for(int i=0;i<10;i++){
		uartWriteString(UART_USB,"LED3\r\n");
		uartWriteString(UART_USB,"Pasos N:");
		char auxMostrar=i+48;
		uartWriteByte(UART_USB,auxMostrar);
		uartWriteString(UART_USB,"\r\n");
		uartWriteString(UART_USB,"tiempo= ");
		itoa(secLED3[i].duracion,auxCadena,10);
		uartWriteString(UART_USB,auxCadena);
		uartWriteString(UART_USB,"\r\n");
		uartWriteString(UART_USB,"intensidad= ");
		itoa(secLED3[i].intensidad,auxCadena,10);
		uartWriteString(UART_USB,auxCadena);
		uartWriteString(UART_USB,"\r\n");
		uartWriteString(UART_USB,"modo= ");
		char aux2=secLED3[i].modo+48;
		uartTxWrite(UART_USB,aux2);
		uartWriteString(UART_USB,"\r\n");
	}
}
