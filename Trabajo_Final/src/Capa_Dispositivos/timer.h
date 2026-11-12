/***
 * @file      timer.h
 * @brief     Configuración del Timer del microcontrolador LPC1769 y su respectiva ISR
 * @details   Configuración de los registros del Timer para la generación de interrupciones
 * @note      ESW
 ******************************************************************************/
#ifndef TIMER_H_
#define TIMER_H_
/*------------------------------------------------------------------------------
 * INCLUDES
 *----------------------------------------------------------------------------*/
#include "LPC17xx.h"
#include <lpc17xx_timer.h>
#include <lpc17xx_gpio.h>

/*------------------------------------------------------------------------------
 * DEFINICIONES DE HARDWARE (MAPEO DE PINES)
 *----------------------------------------------------------------------------*/
//  99999 (100,000 ticks de 1 us = 100 ms)
#define MATCH_VALUE 99999U
/*------------------------------------------------------------------------------
 * PROTOTIPOS DE FUNCIONES
 *----------------------------------------------------------------------------*/
/**
 * @brief     Configuracion del Timer0 para generar interrupciones periódicas
 * @details   Configura el Timer0 para generar interrupciones periódicas cada 100 ms,
 *            utilizando el valor de comparación definido en MATCH_VALUE.
 * @note      USW.
 ******************************************************************************/
void configTimer0(void);
#endif /* TIMER_H_ */