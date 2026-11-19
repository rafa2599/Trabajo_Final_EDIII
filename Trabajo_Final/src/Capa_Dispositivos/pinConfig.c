/***
 * @file      pinconfig.c
 * @brief     Implementación de la configuración física de pines para LPC1769
 * @details   Carga los registros PINSEL0, PINSEL1 y PINSEL4 del procesador
 * @note      ESW.2.1.16
 ******************************************************************************/

/*------------------------------------------------------------------------------
 * INCLUDES
 *----------------------------------------------------------------------------*/
#include "pinConfig.h"
// -------------------------------------------------------------------------
// 1. CONFIGURACIÓN DE PINES ANALÓGICOS (ADC: LDRs + LM35)
// -------------------------------------------------------------------------

#include "pinConfig.h"

void configPinsAdc(void) {
    // Channel 0 (P0.23) -> LDR Superior IZQ (FUNC_01 = AD0.0)
    PINSEL_CFG_T cfgAdc0 = {
        .port = PORT_ADC_LDR,
        .pin = PIN_ADC_LDR_SUP_IZQ,
        .func = PINSEL_FUNC_01,
        .mode = PINSEL_TRISTATE,
        .openDrain = DISABLE
    };
    PINSEL_ConfigPin(&cfgAdc0);

    // Channel 1 (P0.24) -> LDR Superior DER (FUNC_01 = AD0.1)
    PINSEL_CFG_T cfgAdc1 = {
        .port = PORT_ADC_LDR,
        .pin = PIN_ADC_LDR_SUP_DER,
        .func = PINSEL_FUNC_01,
        .mode = PINSEL_TRISTATE,
        .openDrain = DISABLE
    };
    PINSEL_ConfigPin(&cfgAdc1);

    // Channel 2 (P0.25) -> LDR Inferior IZQ (FUNC_01 = AD0.2)
    PINSEL_CFG_T cfgAdc2 = {
        .port = PORT_ADC_LDR,
        .pin = PIN_ADC_LDR_INF_IZQ,
        .func = PINSEL_FUNC_01,
        .mode = PINSEL_TRISTATE,
        .openDrain = DISABLE
    };
    PINSEL_ConfigPin(&cfgAdc2);

    // Channel 3 (P0.26) -> LDR Inferior DER (FUNC_01 = AD0.3)
    PINSEL_CFG_T cfgAdc3 = {
        .port = PORT_ADC_LDR,
        .pin = PIN_ADC_LDR_INF_DER,
        .func = PINSEL_FUNC_01,
        .mode = PINSEL_TRISTATE,
        .openDrain = DISABLE
    };
    PINSEL_ConfigPin(&cfgAdc3);

    // Channel 4 (P1.30) -> Sensor LM35 (FUNC_11 = AD0.4)
    PINSEL_CFG_T cfgAdc4 = {
        .port = PORT_ADC_LM35,
        .pin = PIN_ADC_LM35_PIN,
        .func = PINSEL_FUNC_11,
        .mode = PINSEL_TRISTATE,
        .openDrain = DISABLE
    };
    PINSEL_ConfigPin(&cfgAdc4);
}

void configPinEint0(void) {
    // Pull-up interno activo para evitar estado flotante en P2.10 (ISP)
    PINSEL_CFG_T cfgEint0 = {
        .port = PORT_EINT_BTN,
        .pin = PIN_EINT_BTN_PIN,
        .func = PINSEL_FUNC_01,
        .mode = PINSEL_PULLUP,
        .openDrain = DISABLE
    };
    PINSEL_ConfigPin(&cfgEint0);
}
void configPinServos(void){
	PINSEL_CFG_T cfgPwmX = {
		.port = PORT_PWM_SERVO_X ,
		.pin = PIN_PWM_SERVO_X,
		.func = PINSEL_FUNC_01,
		.mode =PINSEL_TRISTATE,
		.openDrain = DISABLE
	};
	PINSEL_ConfigPin(&cfgPwmX);

		PINSEL_CFG_T cfgPwmY = {
		.port = PORT_PWM_SERVO_Y ,
		.pin = PIN_PWM_SERVO_Y,
		.func = PINSEL_FUNC_01,
		.mode =PINSEL_TRISTATE,
		.openDrain = DISABLE
	};
	PINSEL_ConfigPin(&cfgPwmY);
}
