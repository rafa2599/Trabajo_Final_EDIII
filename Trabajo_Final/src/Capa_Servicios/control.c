#include "control.h"
#include "pwm.h"

volatile bool     modo_activo = false;
volatile uint32_t pos_servo_x = PWM_NEUTRO_US;
volatile uint32_t pos_servo_y = PWM_NEUTRO_US;

void configControl(void) {
    pos_servo_x = PWM_NEUTRO_US;
    pos_servo_y = PWM_NEUTRO_US;
    PWM_ActualizarServos(pos_servo_x, pos_servo_y);
}

void ejecutarControlProporcional(int32_t err_x, int32_t err_y) {
    if (!modo_activo) {
        pos_servo_x = PWM_NEUTRO_US;
        pos_servo_y = PWM_NEUTRO_US;
        PWM_ActualizarServos(pos_servo_x, pos_servo_y);
        return;
    }

    // Zona muerta
    if (err_x > -DB_THRESHOLD && err_x < DB_THRESHOLD) err_x = 0;
    if (err_y > -DB_THRESHOLD && err_y < DB_THRESHOLD) err_y = 0;

    // Incremento proporcional en punto fijo Q8
    int32_t delta_u_x = (err_x * KP_NUM_X + 128) >> 8;
    int32_t delta_u_y = (err_y * KP_NUM_Y + 128) >> 8;

    // Slew rate limit
    if (delta_u_x >  DELTA_U_MAX) delta_u_x =  DELTA_U_MAX;
    if (delta_u_x < -DELTA_U_MAX) delta_u_x = -DELTA_U_MAX;
    if (delta_u_y >  DELTA_U_MAX) delta_u_y =  DELTA_U_MAX;
    if (delta_u_y < -DELTA_U_MAX) delta_u_y = -DELTA_U_MAX;

    // Integración discreta y saturación anti-windup
    int32_t next_x = (int32_t)pos_servo_x + delta_u_x;
    int32_t next_y = (int32_t)pos_servo_y + delta_u_y;

    if (next_x > (int32_t)PWM_MAX_US) next_x = (int32_t)PWM_MAX_US;
    if (next_x < (int32_t)PWM_MIN_US) next_x = (int32_t)PWM_MIN_US;
    if (next_y > (int32_t)PWM_MAX_US) next_y = (int32_t)PWM_MAX_US;
    if (next_y < (int32_t)PWM_MIN_US) next_y = (int32_t)PWM_MIN_US;

#if HABILITAR_EJE_X
    pos_servo_x = (uint32_t)next_x;
#else
    pos_servo_x = PWM_NEUTRO_US;
#endif

#if HABILITAR_EJE_Y
    pos_servo_y = (uint32_t)next_y;
#else
    pos_servo_y = PWM_NEUTRO_US;
#endif

    PWM_ActualizarServos(pos_servo_x, pos_servo_y);
}

// Secuencia cíclica de 8 segundos (80 ticks de 100 ms) para probar cada servo solo
void probarServosAislados(uint32_t tick_100ms) {
    uint32_t fase = (tick_100ms / 10U) % 8U; // Cada paso dura 1 segundo (10 ticks)

    switch (fase) {
        case 0: pos_servo_x = 1500U; pos_servo_y = 1500U; break; // Centro
        case 1: pos_servo_x = 1100U; pos_servo_y = 1500U; break; // Solo X a ~50°
        case 2: pos_servo_x = 1900U; pos_servo_y = 1500U; break; // Solo X a ~130°
        case 3: pos_servo_x = 1500U; pos_servo_y = 1500U; break; // X vuelve a centro
        case 4: pos_servo_x = 1500U; pos_servo_y = 1100U; break; // Solo Y a ~50°
        case 5: pos_servo_x = 1500U; pos_servo_y = 1900U; break; // Solo Y a ~130°
        default: pos_servo_x = 1500U; pos_servo_y = 1500U; break; // Reposo en centro
    }

    PWM_ActualizarServos(pos_servo_x, pos_servo_y);
}
