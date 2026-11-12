/***
 * @file      pwm.c
 * @brief     Implementación de la configuración del PWM para LPC1769
 * @details   
 * @note      
 ******************************************************************************/

/*------------------------------------------------------------------------------
 * INCLUDES
 *----------------------------------------------------------------------------*/
#include "temperatura.h"

volatile uint32_t temperatura_celsius = 0;
volatile bool alerta_sobretemperatura = false;

uint32_t counts_to_miliVolts(uint32_t counts) {
    // 3300 mV / 4095 cuentas
    return (counts * MAX_VOLTAGE) / MAX_COUNTS_ADC_REGISTER;
}

void miliVolts_to_celcius(uint32_t miliVolts) {
    // El LM35 entrega 10 mV / °C
    temperatura_celsius = miliVolts / 10;

    if (temperatura_celsius > MAX_TEMPERATURE) {
        alerta_sobretemperatura = true;
        // Accionar salida de alerta (Buzzer / LED)
        LPC_GPIO0->FIOSET = (1 << 22); // Ejemplo en P0.22
    } else {
        alerta_sobretemperatura = false;
        LPC_GPIO0->FIOCLR = (1 << 22);
    }
}