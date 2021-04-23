/*
 * utilFeg.h
 *
 *  Created on: Apr 21, 2021
 *      Author: franco-unt
 */

#ifndef PROJETS_SECPDM_INC_UTILFEG_H_
#define PROJETS_SECPDM_INC_UTILFEG_H_

#include "sapi.h"
#include "string.h"
/**
 * @brief Función que permite convertir el valor entero almacenado en la variable "value", en caracteres los cuales se guardan en la cadena de
 * caracteres a la que apunta "result", todo esto desarrollado en la base del sistema de numeración indicada por el valor almacenado en "base".
 *
 * @param value variable entera que contiene el valor que queremos convertir.
 * @param base variable entera que contiene el valor de la base del sistema de numeración que vamos a utilizar. Por ejemplo, 10 para base decimal.
 */
char* itoa(int value, char* result, int base);
/**
 * @brief Función que permite convertir una cadena de caracteres en un numero entero en sistema decimal.
 *
 * @param ptrDato puntero que apunta a la cadena de caracteres que se quiere convertir.
 * return devuelve el valor entero convertido.
 */
uint32_t myAtoi(char* ptrDato);

#endif /* PROJETS_SECPDM_INC_UTILFEG_H_ */
