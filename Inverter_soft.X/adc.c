#include "adc.h"
#include "system.h"
#include <stdio.h>
#include <xc.h>
void adcInit(void) {
    CHS0 = 0;
    CHS1 = 1;
    CHS2 = 1;     //Selecciono el canal 15 para hacer la conversion
    CHS3 = 1;
    CHS4 = 1;
 
    PVCFG0 = 1;
    PVCFG1 = 0;  //Habilito las referencias positivas internas (Vref+)
   
    NVCFG0 = 0;
    NVCFG1 = 0;  //Habilito las referencias negativas internas (Vss/GND)
    
    FVREN = 1;
    
    FVRS0 = 1;  //Selecciono el multiplicador de VREF
    FVRS1 = 1;  //1:4 (1 cuenta = 4mV)
    
    ADFM = 1;    //Justifica a la derecha
  
    ACQT0 = 1;
    ACQT1 = 1;    //Selecciono tiempo de adquisicion (En este PIC se hace por hardware)
    ACQT2 = 1;
 
    ADCS0 = 1;
    ADCS1 = 1;    //Selecciono el clock source para la conversion 
    
    ADON = 1; //Habilito el conversor AD
    
    /*Nota:
     *El WDT tiene 16bit e incrementa en uno cada 4ms 
     * (2^16)*4/1000 = 262.144s antes de que se reinicie el uC 
     */
    
    
}

/**
 * @brief	Realiza una conversión AD
 * @param[in]   analogInput Selecciona la entrada analógica deseada
 * @return  resultado de la conversión
 */
uint16_t adcRead(adcMap_t analogInput){
    ADCON0bits.CHS = analogInput;// me paro en un pin analogico 
    //__delay_us(50); El tiempo de adquisicion deberia hacerse por hardware
    GO = 1;                      //Inicio de la medicion
    while (GO){ // Espero a que termine la conversion 
        ;
    }
    uint16_t adcRes;
    adcRes = ADRESL;
    adcRes = adcRes | (ADRESH<<8);  //guardo la conversion en una variable 
    return (adcRes);
}

/**
 * @brief	Realiza una conversión AD en mV
 * @param[in]   analogInput Selecciona la entrada analógica deseada
 * @note    Si recibe como parámetro 255 ( o la etiqueta `VDD_CALC` ) determina la tensión de alimentación VDD o tensión de referencia actual 
 * @return  resultado de la conversión en mV
 * @note    Antes de poder medir cualquier canal se debe medir por lo menos una vez la tensión de alimentación o referencia
 */
uint16_t adcRead_mV(adcMap_t analogInput) {
        return ((4*adcRead(analogInput)));    
}