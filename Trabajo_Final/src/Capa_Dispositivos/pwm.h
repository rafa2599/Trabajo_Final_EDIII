/***
 * @file      pwm.h
 * @brief     Librerias y definciones necesarias para la configuracion del periferico 
 *            PWM 
 * @details   
 * @note      ESW
 ******************************************************************************/
#ifndef PWM_H_
#define PWM_H_
/*------------------------------------------------------------------------------
 * INCLUDES
 *----------------------------------------------------------------------------*/
#include "LPC17xx.h"
#include <lpc17xx_pwm.h>
#include <lpc17xx_gpio.h>

/*------------------------------------------------------------------------------
 * DEFINICIONES 
 *----------------------------------------------------------------------------*/
// Definiciones de tiempos para Servomotores analógicos estándar
#define PWM_PERIODO_US    20000U  // 20 ms -> Frecuencia de 50 Hz
#define PWM_NEUTRO_US     1500U   // 1.5 ms -> Posición neutra (90°)
#define PWM_MIN_US        600U    // 0.6 ms -> Límite mínimo (0°)
#define PWM_MAX_US        2400U   // 2.4 ms -> Límite máximo (180°)
/*------------------------------------------------------------------------------
 * PROTOTIPOS DE FUNCIONES
 *----------------------------------------------------------------------------*/
/**
 * @brief     Funciones de configuracion
 * @details   Funcion que configura el modo de trabajo del periferico PWM donde 
 *            se determina la base de timepo y la escala . La configuracion de 
 *            los pines, el modo de trabajo (flanco simple) y el comportamiento del
 *            periferico cuando hay match 
 *            
 * @note      
 ******************************************************************************/
void configPwm(void);
/**
 * @brief  Actualiza la posición angular de los servomotores X e Y en microsegundos.
 * @param  anchoPulseX_us Ancho de pulso en µs para el Servo X (PWM1.1 en P2.0)
 * @param  anchoPulseY_us Ancho de pulso en µs para el Servo Y (PWM1.2 en P2.1)
 */
void PWM_ActualizarServos(uint32_t anchoPulseX_us, uint32_t anchoPulseY_us);

#endif /* PWM_H_ */
