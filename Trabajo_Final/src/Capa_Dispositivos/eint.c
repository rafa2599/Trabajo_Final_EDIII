#include "eint.h"
#include "pinConfig.h"
#include "control.h"

extern volatile uint32_t ticks_100ms;
static uint32_t ultimo_tick_boton = 0;
static bool     primera_pulsacion = true;

void configEint0(void) {
    configPinEint0();

    EXTI_CFG_T extiCfg;
    extiCfg.line     = EXTI_EINT0;
    extiCfg.mode     = EXTI_EDGE_SENSITIVE;
    extiCfg.polarity = EXTI_FALLING_EDGE;

    EXTI_ConfigEnable(&extiCfg);
    NVIC_SetPriority(EINT0_IRQn, 2);
}

void EINT0_IRQHandler(void) {
    EXTI_ClearFlag(EXTI_EINT0);

    // Antirrebote temporal de 200 ms (2 ticks de 100 ms) sin bloqueo en el primer toque
    if (primera_pulsacion || ((ticks_100ms - ultimo_tick_boton) >= 2U)) {
        modo_activo       = !modo_activo;
        ultimo_tick_boton = ticks_100ms;
        primera_pulsacion = false;
    }
}
