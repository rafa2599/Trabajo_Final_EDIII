#include "temperatura.h"
#include <stdbool.h>

volatile uint32_t temperatura_celsius = 0;
volatile bool alerta_sobretemperatura = false;

uint32_t counts_to_miliVolts(uint32_t counts) {
    return (counts * MAX_VOLTAGE) / MAX_COUNTS_ADC_REGISTER;
}

void miliVolts_to_celcius(uint32_t miliVolts) {
    temperatura_celsius = miliVolts / 10U;

    if (!alerta_sobretemperatura && (temperatura_celsius > TEMP_UMBRAL_ALTO)) {
        alerta_sobretemperatura = true;
        LPC_GPIO0->FIOSET = (1UL << 22);
    } else if (alerta_sobretemperatura && (temperatura_celsius <= TEMP_UMBRAL_BAJO)) {
        alerta_sobretemperatura = false;
        LPC_GPIO0->FIOCLR = (1UL << 22);
    }
}
