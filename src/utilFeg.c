/*
 * utilFeg.c
 *
 *  Created on: Apr 21, 2021
 *      Author: franco-unt
 */
#include "sapi.h"
#include "string.h"

char* itoa(int value, char* result, int base) {
   // check that the base if valid
   if (base < 2 || base > 36) { *result = '\0'; return result; }

   char* ptr = result, *ptr1 = result, tmp_char;
   int tmp_value;

   do {
      tmp_value = value;
      value /= base;
      *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * base)];
   } while ( value );

   // Apply negative sign
   if (tmp_value < 0) *ptr++ = '-';
   *ptr-- = '\0';
   while(ptr1 < ptr) {
      tmp_char = *ptr;
      *ptr--= *ptr1;
      *ptr1++ = tmp_char;
   }
   return result;
}

uint32_t myAtoi(char* ptrDato){
	uint32_t resultado=0;
	uint32_t res=0;
	do{
		res = res*10 + *ptrDato;
		res = res - 48;
		ptrDato++;
	}while(!(*ptrDato==0));
	resultado=(uint32_t)res;
return resultado;
}
