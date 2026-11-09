/***
 * @file      timer.c
 * @brief     Implementación de la configuración del Timer para LPC1769
 * @details   Configura los registros del Timer para la generación de interrupciones
 *            cada 100 ms, utilizando el valor de comparación definido en MATCH_VALUE.
 * @note      
 ******************************************************************************/

/*------------------------------------------------------------------------------
 * INCLUDES
 *----------------------------------------------------------------------------*/
 #include "LPC17xx.h"
 #include "timer.h"
 #include "pinConfig.h"

// -------------------------------------------------------------------------
// 1. CONFIGURACIÓN DE MODULOS DE TIMER 
// -------------------------------------------------------------------------
void configTimer0(void){
     // Configuración del Timer0 para contar en microsegundos con un prescaler de 1
    TIM_TIMERCFG_T timerConfig = {TIM_US,1};

    TIM_MATCHCFG_T matchConfig={
			.channel = TIM_MATCH_1,
			.intEn=DISABLE,
			.resetEn = ENABLE,
			.stopEn = DISABLE,
			.extOpt=TIM_TOGGLE,
			.matchValue=MATCH_VALUE
	};
    	
	TIM_InitTimer(LPC_TIM0,&timerConfig);
	TIM_ConfigMatch(LPC_TIM0,&matchConfig);
    TIM_Enable(LPC_TIM0);
   
}

