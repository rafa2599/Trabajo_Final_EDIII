/***
 * @file      eint.c
 * @brief     Implementación de la configuración de las interrupcion externas
 *            EINT0 y su respectiva ISR
 * @details  
 * @note      
 ******************************************************************************/

/*------------------------------------------------------------------------------
 * INCLUDES
 *----------------------------------------------------------------------------*/

#include "eint.h"
#include "pinConfig.h"
#include "control.h"

void configEint0(void) {
    // Configura la función del pin P2.10 como EINT0
    configPinEint0();

    EXTI_CFG_T extiCfg;
    extiCfg.line = EXTI_EINT0;
    extiCfg.mode = EXTI_EDGE_SENSITIVE;
    extiCfg.polarity = EXTI_FALLING_EDGE; // Disparo por flanco descendente (pulsador a GND)

    EXTI_ConfigEnable(&extiCfg);

    NVIC_SetPriority(EINT0_IRQn, 2);
}

void EINT0_IRQHandler(void) {
    EXTI_ClearFlag(EXTI_EINT0);

    // Alternar entre modo Activo y modo Standby
    modo_activo = !modo_activo;
}