/***
 * @file      pinconfig.h
 * @brief     Declaraciones y mapeo de hardware para la inicializacion de pines
 * @details   Abstraccion de pines de I/O, ADC, PWM, UART y EINT del LPC1769
 * @note      ESW.2.1.16
 ******************************************************************************/

#ifndef PINCONFIG_H_
#define PINCONFIG_H_

/*------------------------------------------------------------------------------
 * INCLUDES
 *----------------------------------------------------------------------------*/
#include "LPC17xx.h"
#include <lpc17xx_pinsel.h>
#include <lpc17xx_gpio.h>

/*------------------------------------------------------------------------------
 * DEFINICIONES DE HARDWARE (MAPEO DE PINES)
 *----------------------------------------------------------------------------*/
// ADC - Sensores LDR (Entradas Analógicas)
#define PORT_ADC_LDR        PORT_0

#define PIN_ADC_LDR_SUP_IZQ     PIN_23    // AD0.0

#define PIN_ADC_LDR_SUP_DER     PIN_24    // AD0.1

#define PIN_ADC_LDR_INF_IZQ     PIN_25    // AD0.2

#define PIN_ADC_LDR_INF_DER     PIN_26    // AD0.3

// ADC - Sensor de Temperatura LM35
#define PORT_ADC_LM35          PORT_0
#define PIN_ADC_LM35_PIN       PIN_27    // AD0.4

// PWM1 - Servomotores (Azimut y Elevación)
#define PORT_PWM_SERVO_X       PORT_2
#define PIN_PWM_SERVO_X    PIN_0     // PWM1.1

#define PORT_PWM_SERVO_Y       PORT_2
#define PIN_PWM_SERVO_Y    PIN_1     // PWM1.2

// UART0 - Telemetría USB
#define PORT_UART_TX       PORT_0
#define PIN_UART_TX_PIN    PIN_2     // TXD0

#define PORT_UART_RX       PORT_0
#define PIN_UART_RX_PIN    PIN_3     // RXD0

// Interrupción Externa y GPIO
#define PORT_EINT_BTN          PORT_2
#define PIN_EINT_BTN_PIN       PIN_10    // EINT0 (Boton Modo)

/*------------------------------------------------------------------------------
 * PROTOTIPOS DE FUNCIONES
 *----------------------------------------------------------------------------*/
/**
 * @brief     Inicializa la matriz de pines del microcontrolador
 * @details   Configura las multiplexaciones PINSEL, modulos de pull-up/down y 
 *            modos open-drain para todos los perifericos del SEP.
 * @note      USW.2.1.16.1
 ******************************************************************************/
void configPinsAdc(void);
void configPinEint0(void);
void configPinServos(void);
void configPinUart (void);

#endif /* PINCONFIG_H_ */