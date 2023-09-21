#include "system.h"
#include <xc.h>
#include <stdio.h>
//===============================Defines========================================
#define TX2     PORTBbits.RB6 
#define RX2     PORTBbits.RB7
#define TX1     PORTCbits.RC6 
#define RX1     PORTCbits.RC7

#define TRIS_TX2    TRISBbits.TRISB6
#define TRIS_RX2    TRISBbits.TRISB7
#define TRIS_TX1    TRISCbits.TRISC6
#define TRIS_RX1    TRISCbits.TRISC7

#define UART_BAUDRATE   9600


//===============================Datos Externos=================================

void uartInit(void);    /* Inicializa la UART */
void putch(char data);  /* Pone un byte en stdout */
char getch(void);       /* Toma un byte en stdin  */
char getche(void);      /* Toma un byte en stdin con eco */
void uart2WriteByte( uint8_t value );      /* Envia un byte en forma boqueante */
__bit uart2ReadByte( uint8_t* receivedByte );/* Toma un byte de la UART */





