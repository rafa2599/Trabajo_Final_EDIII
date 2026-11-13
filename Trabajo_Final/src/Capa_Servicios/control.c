/***
 * @file      control.c
 * @brief     Implementación del Controlador Proporcional Incremental (Tipo 1)
 * @details   Convierte el vector de error espacial (err_x, err_y) en anchos de
 *            pulso PWM (600 us a 2400 us) mediante integración discreta:
 *            u[k] = u[k-1] + Kp * e'[k]
 * @note      Se ejecuta cada T = 100 ms tras completar la ráfaga del ADC.
 ******************************************************************************/

/*------------------------------------------------------------------------------
 * INCLUDES
 *----------------------------------------------------------------------------*/
#include "control.h"
#include "pwm.h"

/*------------------------------------------------------------------------------
 * VARIABLES GLOBALES DEL LAZO DE CONTROL
 *----------------------------------------------------------------------------*/
// Arranca en modo STANDBY por seguridad hasta que el usuario presione EINT0
volatile bool modo_activo = false;

// Memoria de estado del integrador discreto: almacenan u[k-1] en microsegundos
volatile uint32_t pos_servo_x = PWM_NEUTRO_US;
volatile uint32_t pos_servo_y = PWM_NEUTRO_US;

/*------------------------------------------------------------------------------
 * IMPLEMENTACIÓN DE FUNCIONES
 *----------------------------------------------------------------------------*/

void configControl(void) {
    // Establece la condición inicial del acumulador en el centro mecánico (1500 us = 90°)
    pos_servo_x = PWM_NEUTRO_US;
    pos_servo_y = PWM_NEUTRO_US;

    // Actualiza inmediatamente los registros MR1 y MR2 del PWM1
    PWM_ActualizarServos(pos_servo_x, pos_servo_y);
}

void ejecutarControlProporcional(int32_t err_x, int32_t err_y) {
    // =========================================================================
    // PASO 1: VERIFICACIÓN DEL MODO OPERACIONAL (STANDBY vs. ACTIVO)
    // =========================================================================
    // Si el usuario no activó el seguimiento (o volvió a presionar el pulsador
    // para detenerlo), se fuerza el brazo a su posición neutra de reposo.
    if (!modo_activo) {
        pos_servo_x = PWM_NEUTRO_US;
        pos_servo_y = PWM_NEUTRO_US;
        PWM_ActualizarServos(pos_servo_x, pos_servo_y);
        return;
    }

    // =========================================================================
    // PASO 2: APLICACIÓN DE ZONA MUERTA (DEADBAND)
    // =========================================================================
    // Las LDR y el amplificador LM324 tienen ruido térmico y óptico residual.
    // Si el brazo ya apunta a la luz (error dentro de [-15, +15] u.e.), se
    // anula el error para que Δu = 0 y los servos queden quietos sin temblar.
    if (err_x > -DB_THRESHOLD && err_x < DB_THRESHOLD) {
        err_x = 0;
    }
    if (err_y > -DB_THRESHOLD && err_y < DB_THRESHOLD) {
        err_y = 0;
    }

    // =========================================================================
    // PASO 3: CÁLCULO DEL INCREMENTO PROPORCIONAL (Δu = Kp * e')
    // =========================================================================
    // Transforma unidades de error [u.e.] en variación de pulso [us].
    // Se usa punto fijo Q8 para evitar operaciones 'float' en el Cortex-M3:
    //   1. (err * KP_NUM) multiplica por el numerador escalado por 256.
    //   2. (+ 128) suma 0.5 + 256 para redondear al entero más cercano.
    //   3. (>> 8) divide rápidamente entre 256 mediante desplazamiento de bits.
    int32_t delta_u_x = (err_x * KP_NUM_X + 128) >> 8;
    int32_t delta_u_y = (err_y * KP_NUM_Y + 128) >> 8;

    // =========================================================================
    // PASO 4: LIMITACIÓN DE VELOCIDAD DE PASO (SLEW RATE LIMIT)
    // =========================================================================
    // Si la luz aparece de golpe en un extremo (error ≈ 1000), Δu pediría un
    // salto brusco mayor a la velocidad física del servo, causando sobrepaso
    // y oscilaciones. Acotar Δu a ±50 us garantiza una trayectoria suave y lineal.
    if (delta_u_x > DELTA_U_MAX)  delta_u_x = DELTA_U_MAX;
    if (delta_u_x < -DELTA_U_MAX) delta_u_x = -DELTA_U_MAX;

    if (delta_u_y > DELTA_U_MAX)  delta_u_y = DELTA_U_MAX;
    if (delta_u_y < -DELTA_U_MAX) delta_u_y = -DELTA_U_MAX;

    // =========================================================================
    // PASO 5: INTEGRACIÓN DISCRETA / ACUMULACIÓN (u[k] = u[k-1] + Δu[k])
    // =========================================================================
    // Clave del control incremental (Sistema Tipo 1): suma el incremento al
    // pulso que tenía el servo en la muestra anterior. Así, cuando el brazo
    // llega a la luz (e = 0 -> Δu = 0), u[k] conserva el valor acumulado y el
    // servo se queda apuntando en ese ángulo con error estacionario nulo.
    int32_t next_x = (int32_t)pos_servo_x + delta_u_x;
    int32_t next_y = (int32_t)pos_servo_y + delta_u_y;

    // =========================================================================
    // PASO 6: SATURACIÓN FÍSICA Y PROTECCIÓN ANTI-WINDUP (600 us a 2400 us)
    // =========================================================================
    // 1. Protección mecánica/eléctrica: evita enviar pulsos fuera del rango
    //    0°–180° que trabarían el motor contra sus topes (picos de >2A).
    // 2. Anti-windup: al recortar directamente el acumulador, se impide que
    //    la variable siga creciendo si la luz está fuera del alcance del brazo,
    //    permitiendo una respuesta inmediata cuando la luz regrese.
    if (next_x > (int32_t)PWM_MAX_US) next_x = (int32_t)PWM_MAX_US;
    if (next_x < (int32_t)PWM_MIN_US) next_x = (int32_t)PWM_MIN_US;

    if (next_y > (int32_t)PWM_MAX_US) next_y = (int32_t)PWM_MAX_US;
    if (next_y < (int32_t)PWM_MIN_US) next_y = (int32_t)PWM_MIN_US;

    // Actualiza la memoria de estado para el próximo ciclo de 100 ms
    pos_servo_x = (uint32_t)next_x;
    pos_servo_y = (uint32_t)next_y;

    // =========================================================================
    // PASO 7: ACTUALIZACIÓN DE REGISTROS DE HARDWARE (PWM1 -> MR1 y MR2)
    // =========================================================================
    // Carga los nuevos tiempos en alto (en us) en MR1 (P2.0) y MR2 (P2.1)
    // y activa el Latch Enable Register (LER) para aplicarlos en el próximo ciclo.
    PWM_ActualizarServos(pos_servo_x, pos_servo_y);
}