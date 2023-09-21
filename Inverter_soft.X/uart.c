#include <xc.h>
#include <stdint.h>     /* para las definiciones de uint8_t por ej.*/

#include "system.h"     /* Funciones/Parametros System, como conf de osc/perif */
#include "uart.h"       /* Funciones/Parametros Uart */

void uartInit(void){
#if((SYS_FREQ - UART_BAUDRATE*16L)/(UART_BAUDRATE*16L)) > 255 //Si necesita High Baud Rate
#if	(SYS_FREQ - UART_BAUDRATE*64L)/(UART_BAUDRATE*64L) > 255
#error Baud Rate demasiado Bajo
#endif
    SPBRG = (SYS_FREQ - UART_BAUDRATE * 64L) / (UART_BAUDRATE * 64L); //SPBRG para Low Baud Rate
    BRGH = 0;   //Selecciona Low Baud Rate
#else
    SPBRG = (SYS_FREQ - UART_BAUDRATE * 16L) / (UART_BAUDRATE * 16L); //SPBRG para High Baud Rate
    BRGH2 = 1;   //Selecciona High Baud Rate para el Puerto serie 2
#endif
    SYNC2 = 0;   //Selecciona Modo Asincronico
    SPEN2 = 1;   //Habilita Puerto Serie
    TRISB7 = 1;
    TRISB6 = 1;
    CREN2 = 1;   //Habilita Recepción continua
    TXEN2 = 1;   //Habilita Transmisión en puerto dos   
}

void putch(char data) {
    while (!TX2IF)   //Espera que haya espacio en la FIFO (bloquea poco tiempo)
        continue;
    TX2REG = data;   //Envía el byte
}

void uart2WriteByte( uint8_t value ){
    while (!TX2IF)   //Espera que haya espacio en la FIFO (bloquea poco tiempo)
        continue;
    TX2REG = value;  //Envía el byte
}
char getch(void) {
    while (!RC2IF)   //Espera hasta que haya un byte recibido (bloqueante)
        continue;
    return RC2REG;   //retorna lo recibido
}
__bit uart2ReadByte( uint8_t* receivedByte ){
   if ( RC2IF ){     //Si hay por lo menos un byte recibido
      *receivedByte = RC2REG;    //lo coloca en el lugar de destino
      return 1;     //Retorna 1 para indicar que ha recibido
   }
   return 0;        //Retorna 0 para indicar que no ha recibido
}



