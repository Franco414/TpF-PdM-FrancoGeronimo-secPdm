/*
 * ledTpfPdm.h
 *
 *  Created on: Apr 21, 2021
 *      Author: franco-unt
 */

#ifndef PROJETS_SECPDM_INC_LEDTPFPDM_H_
#define PROJETS_SECPDM_INC_LEDTPFPDM_H_

#include "sapi.h"
/**
 * @brief función que permite configurar los delay para los LED1, LED2, LED3.
 * 
 */
void configDelay();
/**
 * @brief función que permite actualizar el estado del LED1, en función de un determinado paso de la secuencia
 * 
 */
void actLed1();

/**
 * @brief función que permite actualizar el estado del LED2, en función de un determinado paso de la secuencia
 * 
 */
void actLed2();
/**
 * @brief función que permite actualizar el estado del LED3, en función de un determinado paso de la secuencia
 * 
 */
void actLed3();
/**
 * @brief Función que permite actualizar los estados de los LED1, LED2, LED3 de acuerdo a un paso solicitado a la capa secTpfPdm
 * 
 * @param resetActSec variable booleana, que mediante un valor igual a TRUE indica a la funcion actSecLeds que debe reiniciar la visualización
 * de la secuencia.
 */
void actSecLeds(bool_t *resetActSec);
/**
 * @brief función que permite re-inicializar las variables necesarias para reiniciar la secuencia de Leds.
 * 
 */
void resetSecLed();
/**
 * @brief función que permite apagar los LED1,LED2,LED3 y por ende su correspondiente secuencia.
 * 
 */
void apagarSec();

#endif /* PROJETS_SECPDM_INC_LEDTPFPDM_H_ */
