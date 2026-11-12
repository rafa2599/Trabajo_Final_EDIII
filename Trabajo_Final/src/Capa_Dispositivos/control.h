/***
 * @file      control.h
 * @brief     Declaraciones y parámetros del Controlador Proporcional Incremental
 * @details   Define las constantes de sintonía (ganancia Kp en punto fijo Q8,
 *            zona muerta y límite de velocidad de paso) y las variables de estado
 *            del lazo cerrado para los ejes Azimut (X) y Elevación (Y).
 * @note      Capa de Aplicación - TPI Electrónica Digital III (Grupo N° 4)
 ******************************************************************************/

#ifndef CONTROL_H_
#define CONTROL_H_

/*------------------------------------------------------------------------------
 * INCLUDES
 *----------------------------------------------------------------------------*/
#include "LPC17xx.h"
#include <stdint.h>
#include <stdbool.h>

/*------------------------------------------------------------------------------
 * PARÁMETROS DE DISEÑO DEL CONTROLADOR PROPORCIONAL INCREMENTAL
 *----------------------------------------------------------------------------*/

/**
 * @brief Zona muerta (Deadband) en unidades de error [u.e.] (rango error: ±1000).
 * @details Si |error| < DB_THRESHOLD, el error se fuerza a 0. Evita que el ruido
 *          eléctrico de las LDR o variaciones mínimas de luz hagan vibrar
 *          ("jitter") a los servomotores cuando el brazo ya está centrado.
 *          Experimentalmente se elige DB ≈ 3 * sigma_e.
 */
#define DB_THRESHOLD      15

/**
 * @brief Límite máximo de incremento de pulso por ciclo de muestreo [us].
 * @details Con un muestreo T = 100 ms (10 Hz) y Km ≈ 0.1 °/us, limitar Δu a
 *          ±50 us equivale a un paso máximo de ~5° por ciclo (~50 °/s).
 *          Mantiene al servomotor dentro de su zona lineal de velocidad y
 *          evita golpes mecánicos ante cambios bruscos de luz.
 */
#define DELTA_U_MAX       50

/**
 * @brief Ganancias proporcionales Kp escaladas en aritmética de punto fijo Q8.
 * @details Fórmula de diseño: Kp = (10 * K) / (G * Km) [us / u.e.]
 *          Para K = 0.4, Km = 0.1 °/us y G = 20 u.e./° -> Kp = 0.2 us/u.e.
 *          Representación Q8 (multiplicar por 2^8 = 256):
 *          KP_NUM = round(0.2 * 256) = 51  --->  51 / 256 = 0.1992 us/u.e.
 *          NOTA: Si al probar en el brazo real un eje gira en sentido opuesto
 *          a la fuente de luz, invertir el signo de la constante (-51).
 */
#define KP_NUM_X          51
#define KP_NUM_Y          51

/*------------------------------------------------------------------------------
 * VARIABLES GLOBALES DE ESTADO DEL SISTEMA
 *----------------------------------------------------------------------------*/
extern volatile bool     modo_activo;   // false: STANDBY (reposo centrado) | true: ACTIVO (seguimiento)
extern volatile uint32_t pos_servo_x;   // Ancho de pulso actual acumulado u_X[k] [us]
extern volatile uint32_t pos_servo_y;   // Ancho de pulso actual acumulado u_Y[k] [us]

/*------------------------------------------------------------------------------
 * PROTOTIPOS DE FUNCIONES
 *----------------------------------------------------------------------------*/
/**
 * @brief Inicializa el estado del controlador y lleva los servos a 90° (1500 us).
 */
void configControl(void);

/**
 * @brief Ejecuta un ciclo del algoritmo de control proporcional incremental.
 * @param err_x Error espacial normalizado en Azimut [-1000 a +1000 u.e.]
 * @param err_y Error espacial normalizado en Elevación [-1000 a +1000 u.e.]
 */
void ejecutarControlProporcional(int32_t err_x, int32_t err_y);

#endif /* CONTROL_H_ */