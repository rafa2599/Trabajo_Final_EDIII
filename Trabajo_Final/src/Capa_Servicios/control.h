#ifndef CONTROL_H_
#define CONTROL_H_

#include "LPC17xx.h"
#include <stdint.h>
#include <stdbool.h>

// =============================================================================
// SWITCHES DE DIAGNÓSTICO Y AISLAMIENTO DE SERVOS
// =============================================================================
// 1: Ejecuta secuencia de prueba aislada (primero Servo X, luego Servo Y)
// 0: Operación normal de seguimiento lumínico
#define MODO_TEST_SERVOS  0

// Permiten probar un eje en lazo cerrado manteniendo el otro fijo en 1500 us
#define HABILITAR_EJE_X   1
#define HABILITAR_EJE_Y   1

// =============================================================================
// PARÁMETROS DEL CONTROLADOR PROPORCIONAL INCREMENTAL (Q8)
// =============================================================================
#define DB_THRESHOLD      45
#define DELTA_U_MAX       50

// Mitad de la ganancia nominal (102 / 256 ≈ 0.098 us/u.e.) para diagnóstico.

#define KP_NUM_X          -102
#define KP_NUM_Y          -102

extern volatile bool     modo_activo;
extern volatile uint32_t pos_servo_x;
extern volatile uint32_t pos_servo_y;

void configControl(void);
void ejecutarControlProporcional(int32_t err_x, int32_t err_y);
void probarServosAislados(uint32_t tick_100ms);

#endif /* CONTROL_H_ */
