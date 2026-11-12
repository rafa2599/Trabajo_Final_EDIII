/***
 * @file      temperatura.h
 * @brief     pasamos el valor sensado a temperatura . 
 * @details   Tomamos el valor del registros del canal 5 del ADC y pasamos dicho valor a volts.
 *            Luego pasamos ese valor a grados celcuis. En caso de subir la temeperatura por arriba
 *            del umbral maximo permitido, encendemos un alerta (buzzer).
 * @note      ESW
 ******************************************************************************/
#ifndef TEMPERATURA_H_
#define TEMPERATURA_H_
/*------------------------------------------------------------------------------
 * INCLUDES
 *----------------------------------------------------------------------------*/
#include "LPC17xx.h"
#include <lpc17xx_pinsel.h>
#include <lpc17xx_gpio.h>

/*------------------------------------------------------------------------------
 * DEFINICIONES 
 *----------------------------------------------------------------------------*/
//MAXIMA TEMPERATURA PERMITIDA

#define MAX_TEMPERATURE 30

//NUMERO DE CUENTAS MAXIMO 2^32 = 4095 QUE ADMITE LOS CANALES DEL ADC 

#define MAX_COUNTS_ADC_REGISTER 4095

//VOLTAJE MAXIMO DE TRABAJO DE LA PLACA 3V3 EN mV 

#define MAX_VOLTAGE 3300
/*------------------------------------------------------------------------------
 * PROTOTIPOS DE FUNCIONES
 *----------------------------------------------------------------------------*/
/**
 * @brief     Funciones que pasan de cuentas a mili voltios y luego de mili voltios
 *            a grados celcius
 * @details   Una vez convertido el valor captado por el sensor se chequea si se pasa la 
 *            temperatura maxima permitida. En el caso que si sea mayor , se dispara una alarma
 *            (buzzer) caso contrario se prende un led verde. 
 * @note      USW.
 ******************************************************************************/
uint32_t counts_to_miliVolts (uint32_t);
void miliVolts_to_celcius (uint32_t);
#endif /*TEMPERATURA_H_*/