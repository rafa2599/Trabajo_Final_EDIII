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



    PWM_ActualizarServos(pos_servo_x, pos_servo_y);
}
