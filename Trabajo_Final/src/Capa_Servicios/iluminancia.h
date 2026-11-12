/***
 * @file      adc.h
 * @brief     Configuración del ADC del microcontrolador LPC1769 y su respectiva ISR
 * @details   Configuración de los registros del ADC para la lectura de los sensores LDR y LM35, 
 *            implementando el modo BRUST y la interrupción por finalización de conversión.
 * @note      ESW
 ******************************************************************************/
#ifndef ILUMINANCIA_H_
#define ILUMINANCIA_H_
/*------------------------------------------------------------------------------
 * INCLUDES
 *----------------------------------------------------------------------------*/
#include "LPC17xx.h"
#include <lpc17xx_adc.h>
#include <lpc17xx_gpio.h>
/*------------------------------------------------------------------------------
 * DEFINICIONES 
 *----------------------------------------------------------------------------*/
#define RATES_SAMPLES_FREC 20 //
/*------------------------------------------------------------------------------
 * PROTOTIPOS DE FUNCIONES
 *----------------------------------------------------------------------------*/
/**
 * @brief     Inicializa y configura el periférico ADC del microcontrolador
 * @details   Configura el ADC para la lectura de los sensores LDR y LM35 mediante 
 * el disparo del Timer 0 cada 100 ms.
 * @note      USW.
 ******************************************************************************/






#endif /* ILUMINANCIA_H_ */