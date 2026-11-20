/***
 * @file      pwm.c
 * @brief     Implementación de la configuración del PWM para LPC1769
 * @details   
 * @note      
 ******************************************************************************/

/*------------------------------------------------------------------------------
 * INCLUDES
 *----------------------------------------------------------------------------*/
 #include "LPC17xx.h"
 #include "pwm.h"
 #include "pinConfig.h"

// -------------------------------------------------------------------------
// 1. CONFIGURACIÓN DE MODULO PWM 
// -------------------------------------------------------------------------
void configPwm (void){
// -------------------------------------------------------------------------
    // 1. CONFIGURACIÓN DE LA BASE DE TIEMPO (1 tick = 1 microsegundo)
    // -------------------------------------------------------------------------
    PWM_TIMERCFG_T timerCfg;
    timerCfg.prescaleOpt   = PWM_US;
    timerCfg.prescaleValue = 1;      // Modulo ajustado a resolución de 1 us
    PWM_InitTimer(&timerCfg);

    // -------------------------------------------------------------------------
    // 2. CONFIGURACIÓN DE PINES (Pin Connect Block mediante Driver)
    // -------------------------------------------------------------------------
    // P2.0 -> Canal PWM1.1 (Servo X - Azimut)
    PWM_PinConfig(PWM1_P2_0);
    
    // P2.1 -> Canal PWM1.2 (Servo Y - Elevación)
    PWM_PinConfig(PWM2_P2_1);

    // -------------------------------------------------------------------------
    // 3. MODO DE SALIDA POR CANAL (Single Edge)
    // -------------------------------------------------------------------------
    PWM_ChannelConfig(PWM_CHANNEL_1, PWM_SINGLE_EDGE);
    PWM_ChannelConfig(PWM_CHANNEL_2, PWM_SINGLE_EDGE);

    // -------------------------------------------------------------------------
    // 4. CONFIGURACIÓN DE REGISTROS DE MATCH (MCR y Valores Iniciales)
    // -------------------------------------------------------------------------
    
    // MR0: Define el período total (20 ms = 20000 us) y resetea el contador al llegar a cero
    PWM_MATCHCFG_T matchCfg0;
    matchCfg0.channel    = PWM_MATCH_0;
    matchCfg0.intEn      = DISABLE;
    matchCfg0.resetEn    = ENABLE;   // Reinicia el ciclo PWM
    matchCfg0.stopEn     = DISABLE;
    matchCfg0.matchValue = PWM_PERIODO_US;
    PWM_ConfigMatch(&matchCfg0);

    // MR1: Canal 1 (Servo X) - Pulso inicial en posición neutra (1500 us)
    PWM_MATCHCFG_T matchCfg1;
    matchCfg1.channel    = PWM_MATCH_1;
    matchCfg1.intEn      = DISABLE;
    matchCfg1.resetEn    = DISABLE;
    matchCfg1.stopEn     = DISABLE;
    matchCfg1.matchValue = PWM_NEUTRO_US;
    PWM_ConfigMatch(&matchCfg1);

    // MR2: Canal 2 (Servo Y) - Pulso inicial en posición neutra (1500 us)
    PWM_MATCHCFG_T matchCfg2;
    matchCfg2.channel    = PWM_MATCH_2;
    matchCfg2.intEn      = DISABLE;
    matchCfg2.resetEn    = DISABLE;
    matchCfg2.stopEn     = DISABLE;
    matchCfg2.matchValue = PWM_NEUTRO_US;
    PWM_ConfigMatch(&matchCfg2);

    // -------------------------------------------------------------------------
    // 5. HABILITACIÓN DE SALIDAS EN LOS PINES DE SALIDA
    // -------------------------------------------------------------------------
    PWM_ChannelEnable(PWM_CHANNEL_1);
    PWM_ChannelEnable(PWM_CHANNEL_2);

    // -------------------------------------------------------------------------
    // 6. ARRANQUE DEL PERIFÉRICO Y CONTADOR
    // -------------------------------------------------------------------------
    PWM_ResetCounter();
    PWM_CounterEnable(); // Habilita el Timer Counter (TC)
    PWM_Enable();        // Activa la salida general PWM (Master PWM Enable)

}
void PWM_ActualizarServos(uint32_t anchoPulseX_us, uint32_t anchoPulseY_us)
{
    // Actualiza los Match Registers MR1 y MR2 y activa el Latch Enable Register (LER)
    PWM_MatchUpdateSingle(PWM_MATCH_1, anchoPulseX_us);
    PWM_MatchUpdateSingle(PWM_MATCH_2, anchoPulseY_us);
}