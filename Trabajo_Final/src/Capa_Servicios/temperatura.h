#ifndef TEMPERATURA_H_
#define TEMPERATURA_H_

#include "LPC17xx.h"
#include <lpc17xx_pinsel.h>
#include <lpc17xx_gpio.h>

#define TEMP_UMBRAL_ALTO        40U   // Enciende alerta por encima de 40 °C
#define TEMP_UMBRAL_BAJO        37U   // Apaga alerta recién al bajar de 37 °C (histéresis de 3 °C)
#define MAX_TEMPERATURE         TEMP_UMBRAL_ALTO

#define MAX_COUNTS_ADC_REGISTER 4095U
#define MAX_VOLTAGE             3300U

uint32_t counts_to_miliVolts(uint32_t counts);
void miliVolts_to_celcius(uint32_t miliVolts);

#endif /* TEMPERATURA_H_ */
