#ifndef USER_H
#define USER_H
/*==================[inclusiones]============================================*/
#include <stdint.h>     /* para las definiciones de uint8_t por ej.*/

/*==================[macros]=================================================*/
/* User Level #define Macros                                                 */
/* TODO: Los parámetros de user.c pueden ponerse aquí */
    
//Entradas

#define PIN_OPTO        PORTEbits.RE3 
#define TRIS_ZENNER     TRISCbits.TRISC3      
#define PIN_ZENNER      PORTCbits.RC3   
#define TRIS_QA         TRISCbits.TRISC4
#define PIN_QA          PORTCbits.RC4
#define TRIS_QB         TRISCbits.TRISC5
#define PIN_QB          PORTCbits.RC5
#define TRIS_QA_CI      TRISCbits.TRISC1
#define PIN_QA_CI       PORTCbits.RC1
#define TRIS_QB_CI      TRISCbits.TRISC2
#define PIN_QB_CI       PORTCbits.RC2
#define TRIS_FUSIBLE    TRISCbits.TRISC6
#define PIN_FUSIBLE     PORTCbits.RC6

//Leds
#define	TRIS_LED1       TRISBbits.TRISB0
#define	PIN_LED1        PORTBbits.RB0
#define	TRIS_LED2       TRISBbits.TRISB1
#define	PIN_LED2        PORTBbits.RB1
#define	TRIS_LED3       TRISBbits.TRISB2
#define	PIN_LED3        PORTBbits.RB2
#define	TRIS_LED4       TRISBbits.TRISB3
#define	PIN_LED4        PORTBbits.RB3
#define	TRIS_LED5       TRISBbits.TRISB4
#define	PIN_LED5        PORTBbits.RB4
#define	TRIS_LED6       TRISBbits.TRISB5
#define	PIN_LED6        PORTBbits.RB5

//Alias de los Leds
#define	TRIS_FALLA_CI           TRIS_LED1 
#define	PIN_LED_FALLA_CI        PIN_LED1  //LED que informa falla en el circuito integrado (Rojo)
#define	TRIS_NORMAL             TRIS_LED2
#define	PIN_LED_NORMAL          PIN_LED2  //LED que informa el funcionamiento correcto del dispositivo (Verde)
#define	TRIS_INICIAL            TRIS_LED3
#define PIN_LED_INICIAL         PIN_LED3  //LED que informa inicializacion (Amarillo)
#define	TRIS_FALLA_TRAFO        TRIS_LED4       
#define	PIN_LED_FALLA_TRAFO     PIN_LED4  //LED que informa falla en el transformador (Rojo)
#define	TRIS_FALLA_ZENER        TRIS_LED5       
#define	PIN_LED_FALLA_ZENER     PIN_LED5  //LED que informa falla en el regulador de tension (Rojo)
#define	TRIS_FALLA_GENERAL      TRIS_LED6       
#define	PIN_LED_FALLA_GENERAL   PIN_LED6  //LED que informa falla general (Rojo)

//Buzzer
#define	TRIS_BUZZER    TRISCbits.TRISC0
#define	PIN_BUZZER     PORTCbits.RC0

/*==================[declaraciones de funciones externas]====================*/
void appInit(void);     /* Inicializa las I/O y los periféricos */
/*==================[fin del archivo]========================================*/
#endif// USER_H
