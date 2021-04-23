/*
 * secTpfPdm.h
 *
 *  Created on: Apr 21, 2021
 *      Author: franco-unt
 */

#ifndef PROJETS_SECPDM_INC_SECTPFPDM_H_
#define PROJETS_SECPDM_INC_SECTPFPDM_H_

#include "sapi.h"
#include "string.h"
//! Enumeración que contiene los estados de la máquina encargada de llevar la gestión de la estructura recibida por la interfaz serial del microcontrolador.
typedef enum{
	InicioSec,//!<Estado que indica el inicio del programa.
	esperaSec,//!<Estado que indica que el sistema está esperando datos.
	guardarSec,//!<Estado necesrio para indicar que se guardó un paso de la estructura.
	errorSec//!<Estado que se emplea para señalar una falla en el tratamiento de la estructura
}estadoMefSec;
//! Enumeración que contiene todos los posibles eventos que la MEF reconoce
typedef enum{
	ledSep,
	primerSep,
	nuevoDatoSep,
	ContSep,
	finSep,
	errorCad
}evento3Mef;
//! Enumeración que contiene estados para identificar un nuevo paso de una estructura
typedef enum{
	inicioPasoMef,
	tiempoPaso,
	modoPaso,
	intensPaso,
	errorPaso,
	validoPaso
}pasoEstMef;
//! Enumeración que contiene los estados de la maquina encargada de determinar si el usuario inicio con el formato solicitado para cargar una estructura
typedef enum{
	iniNameLed,
	letraLed,
	numeroLed,
	errorLed
}estNameSec;
//! Estructura que contiene los parametros auxiliares que la MEF utilizará para procesar y cargar la estructura
struct auxPaso{
	char auxTiempo[10];
	uint8_t contDigTiempo;
	char auxIntensidad[4];
	uint8_t contDigInt;
	uint8_t modo;
};
//! Estructura que contiene parametros que se usarán para indicar que función debe realizarse y a que estado debe llegar cuando aparece un evento transiciona la MEF
struct tranSec{
	void(*fncSec)(void);
	volatile estadoMefSec estProxSec;
};
//! Estructura que contiene los parametros necesarios para definir un paso de la secuencia
struct secLed{
	uint32_t duracion;
	uint8_t intensidad;
	uint8_t modo;
};

/**
 * @brief Es una función que devuelve una estructura del tipo secLed y que sirve para transferir un paso o estado de la secuencia
 * a la capa de led del presente proyecto, para eso recibe como parámetro de entrada una variable contaLED1, que indica que elemento
 * del vector de la secuencia del led1 ya se presento a fin de solicitar el siguiente paso, además también recibe la variable resetSecled1
 * que mediante un valor lógico uno, indica que la visualización se reinicia y por ende requiere el primer elemento.
 * 
 * @param contaLed1 puntero que apunta a la variable booleana que contiene el valor entero que se usará para indicar que elemento ya presento y por ende solicitar el siguiente
 * @param resetSecled1 variable booleana que mediante un TRUE indica que requiere el primer elemento de la secuencia y que además se reinicia la misma.
 * @return struct secLed 
 */
struct secLed recibirPasoLed1(uint8_t *contaLed1,bool_t resetSecled1);
/**
 * @brief Es una función que devuelve una estructura del tipo secLed y que sirve para transferir un paso o estado de la secuencia
 * a la capa de led del presente proyecto, para eso recibe como parámetro de entrada una variable contaLED2, que indica que elemento
 * del vector de la secuencia del led2 ya se presento a fin de solicitar el siguiente paso, además también recibe la variable resetSecled1 que mediante un valor lógico uno
 * indica que la visualización se reinicia y por ende requiere el primer elemento.
 *
 * @param contaLed2 puntero que apunta a la variable booleana que contiene el valor entero que se usará para indicar que elemento ya presento y por ende solicitar el siguiente
 * @param resetSecled2 variable booleana que mediante un TRUE indica que requiere el primer elemento de la secuencia y que además se reinicia la misma.
 * @return struct secLed 
 */
struct secLed recibirPasoLed2(uint8_t *contaLed2,bool_t resetSecled2);
/**
 * @brief Es una función que devuelve una estructura del tipo secLed y que sirve para transferir un paso o estado de la secuencia
 * a la capa de led del presente proyecto, para eso recibe como parámetro de entrada una variable contaLED3, que indica que elemento
 * del vector de la secuencia del led3 ya se presento a fin de solicitar el siguiente paso, además también recibe la variable resetSecled1 que mediante un valor lógico uno
 * indica que la visualización se reinicia y por ende requiere el primer elemento.
 * 
 * @param contaLed3 puntero que apunta a la variable booleana que contiene el valor entero que se usará para indicar que elemento ya presento y por ende solicitar el siguiente.
 * @param resetSecled3 variable booleana que mediante un TRUE indica que requiere el primer elemento de la secuencia y que además se reinicia la misma.
 * @return struct secLed 
 */
struct secLed recibirPasoLed3(uint8_t *contaLed3,bool_t resetSecled3);

/**
 * @brief Función que permite recuperar la copia que almacena la función guardarUltSec
 * 
 * @param nLed variable que indica que secuencia debe recuperar
 */
void recSec(uint8_t nLed);
/**
 * @brief Función que permite hacer una copia de la secuencia que se está modificando.
 * 
 * @param nLed variable que indica que secuencia debe guardar
 * @param maxPasos variable que indica el número de pasos totales que tiene la secuencia
 */
void guardarUltSec(uint8_t nLed,uint8_t maxPasos);
/**
 * @brief Función que permite visualizar la secuencia cargada del led1
 */
void verSecLed1();
/**
 * @brief Función que permite visualizar la secuencia cargada del led2
 */
void verSecLed2();
/**
 * @brief Función que permite visualizar la secuencia cargada del led3
 */
void verSecLed3();
/**
 * @brief Función que permite borrar completamente una estructura de un LED específico (dentro del conjunto LED1,LED2,LED3) en función del valor que reciba
 * con la variable nLed.
 * 
 * @param nLed variable entera de 8 bits que indica que secuencia debe borrar, con un 1 borra la del LED1, 2 para el LED2, 3 para el LED3, por último si
 * recibe un 0 borra todas las secuencias.
 */
void secBorrar(uint8_t nLed);
/**
 * @brief Función que permite guardar un paso válido de la secuencia.
 */
void escEstSec();

void fncGuardarSec();
/**
 * @brief Función que permite re-inicializar los datos necesarios para ingresar una nueva estructura. además de determinar si es necesario rescatar la secuencia guardada en la secuencia auxiliar
 */
void fncErrorSec();
/**
 * @brief Función que detecta si se ingreso correctamente el nombre del led para la cual se carga la secuencia
 */
void fncLedSec();
/**
 * @brief Función que detecta un paso válido
 */
void fncPasoSec();

void fncFinSec();
/**
 * @brief Función que permite reinicializar las variables necesarias para guardar un nuevo paso de la secuencia
 */
void fncConfSec();
/**
 * @brief Función que permite cargar una secuencia default para el led1
 */
void SecDefault();
/**
 * @brief Función que permite borrar la estructura auxiliar en donde se guarda temporalmente un paso de la secuencia.
 */
void borraAuxPaso();
/**
 * @brief Esta función se encarga de transmitir los caracteres que recibe el micro desde la interfaz serial hacia la máquina de estado encargada
 * de realizar el procesamiento de la estructura. También es la encarga de habilitar o deshabilitar la visualizacioón de la secuencia mediante
 * la manipulación de estados de los LED1, LED2, LED3 de acuerdo a la variable de salida de la MEF. Por último, también sirve para reiniciar la
 * visualización de la secuencia mediante una variable booleana.
 * @param ptrDatoSec variable que contiene el estado actual de la máquina dedicada a procesar los datos que llegan desde la interfaz serial.
 * @param ptrEnableSec puntero booleano que mediante TRUE habilita la presentación de la secuencia mediante los LED1,LED2,LED3.
 * @param ptrResetActSec puntero booleano que mediante TRUE indica a la MEF que debe iniciar el reinicio de la visualización de la
 * secuencia mediante los LED1,LED2,LED3.
 */
void cargarSecuencia(char *ptrDatoSec, bool_t *ptrEnableSec,bool_t *ptrResetActSec);
/**
 * @brief Esta función se encarga de recibir los datos que el usuario ingresa via interfaz serial para pre-procesarlos a fin de crear eventos
 * que deberán ser reconocidos por la MEF. Además establece la prioridad con la que deben atenderse cada uno.
 * @param ptrDatoSec es un puntero que contiene el caracter enviado desde la interfaz serial del microcontrolador.
 * @param errorSecu es un parámetro que mediante un valor TRUE indica que ocurrió un error en la carga de la estructura y por lo tanto debe
 * crear un evento de error necesario para indicar la correspondiente transición.
 */
void detectEventSec(char *ptrDatoSec,uint8_t errorSecu);
/**
 * @brief Esta función se encarga de establecer el siguiente estado y la acción a realizar cuando aparece una transición en la máquina de estado
 * que se encarga de procesar los datos que provienen de la interfaz UART, a fin de completar una secuencia válida. Para tal fin recibe como parámetros
 * el estado actual y el evento actual, devolviendo la función a realizar mediante un puntero de funciones, y establecer el nuevo estado, todo esto
 * gracias a una tabla de transiciones.
 * de la máquina.
 * @param estActual variable que contiene el estado actual de la máquina dedicada a procesar los datos que llegan desde la interfaz serial.
 * @param eventoAct variable que contiene el evento actual que se dispara y por el cual se debe actualizar el estado actual de de la máquina
 * dedicada a procesar los datos que llegan desde la interfaz serial.
 */
void dispatcherSec(estadoMefSec estActual,evento3Mef eventoAct);

#endif /* PROJETS_SECPDM_INC_SECTPFPDM_H_ */
