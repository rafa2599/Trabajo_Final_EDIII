/***
 * @file      adc.h
 * @brief     Declaraciones y parámetros para la configuracion del ADC , su rutina ISR y
 *            el calculo de errores junto con su ganancia.  
 * @note      Capa de Dispositivos- TPI Electrónica Digital III (Grupo N° 4)
 ******************************************************************************/
/*------------------------------------------------------------------------------
 * INCLUDES
 *----------------------------------------------------------------------------*/
#ifndef ADC_H_
#define ADC_H_

#include "LPC17xx.h"
#include <lpc17xx_adc.h>
#include <lpc17xx_gpio.h>
/*------------------------------------------------------------------------------
 * DEFINICIONES 
 *----------------------------------------------------------------------------*/
#define RATES_SAMPLES_FREC 100000U
#define MUESTRAS_PROMEDIO  100U     // 100 muestras a 1 kHz = ventana de 100 ms
#define S_MINIMO           400      // Umbral mínimo de luz total (suma de 4 canales)
/*------------------------------------------------------------------------------
 * PROTOTIPOS DE FUNCIONES
 *----------------------------------------------------------------------------*/

extern volatile uint32_t adcValue[4];
extern volatile uint32_t adcValueLM35;
extern volatile int32_t  error_x, error_y;
extern volatile uint8_t  adcMuestreoCompleto;

void configAdc(void);
void calc_error(void);
void calibrarGananciaLDR(void);

#endif /* ADC_H_ */
