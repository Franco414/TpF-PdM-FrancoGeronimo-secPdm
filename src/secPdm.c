/*=============================================================================
 * Author: Franco Ezequiel Geronimo <franco.alfa76@gmail.com>
 * Date: 2021/04/12
 * Version: 0.1
 *===========================================================================*/

/*=====[Inclusions of function dependencies]=================================*/

#include "secPdm.h"
#include "ledTpfPdm.h"
#include "secTpfPdm.h"
#include "utilFeg.h"
#include "sapi.h"
#include "string.h"
/*=====[Definition macros of private constants]==============================*/

/*=====[Definitions of extern global variables]==============================*/
/*=====[Definitions of public global variables]==============================*/

/*--------------------Variables necesarias para implementar la secuencia---------------------------*/


volatile bool_t EnableSec=LOW;
/*-------------------------------------------------------------------------------------------------*/
volatile bool_t resetActSec=LOW;
volatile uint8_t dato  = 0;
/*=====[Definitions of private global variables]=============================*/

/*=====[Main function, program entry point after power on or reset]==========*/

int main( void )
{
   // ----- Setup -----------------------------------
   boardInit();
   configDelay();
   uartConfig( UART_USB, 115200 );


   bool_t valor = 0;

   /* Configurar PWM */
   valor = pwmConfig( 0, PWM_ENABLE );
   valor = pwmConfig( PWM7, PWM_ENABLE_OUTPUT );
   valor = pwmConfig( PWM8, PWM_ENABLE_OUTPUT );
   valor = pwmConfig( PWM9, PWM_ENABLE_OUTPUT );

   /* Usar PWM */
   pwmWrite( PWM7, 0 );
   pwmWrite( PWM8, 0 );
   pwmWrite( PWM9, 0 );
   secBorrar(0);
   // ----- Repeat for ever -------------------------
   while( true ) {
	   if(uartReadByte( UART_USB , &dato)){
		   cargarSecuencia(&dato,&EnableSec,&resetActSec);
	   	   }
	   if(EnableSec)actSecLeds(&resetActSec);
	   else apagarSec();
   }

   // YOU NEVER REACH HERE, because this program runs directly or on a
   // microcontroller and is not called by any Operating System, as in the 
   // case of a PC program.
   return 0;
}
