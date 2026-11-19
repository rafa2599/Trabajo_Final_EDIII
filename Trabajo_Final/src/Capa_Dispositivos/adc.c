#include "LPC17xx.h"
#include "adc.h"
#include "pinConfig.h"

volatile uint32_t adcValue[4];      // Promedio STL, STR, SBL, SBR
volatile uint32_t adcValueLM35;     // Promedio LM35
volatile int32_t  adcTotal;
volatile int32_t  error_x, error_y;
volatile uint8_t  adcMuestreoCompleto = 0;

static uint8_t  canalActual   = 0;
static uint32_t adcAcc[4]     = {0, 0, 0, 0};
static uint32_t adcAccLM35    = 0;
static uint16_t muestrasCount = 0;
static uint32_t gain_ldr[4]   = {1024U, 1024U, 1024U, 1024U}; // 1.0 en Q10 por defecto

void configAdc(void) {
    configPinsAdc();
    ADC_Init(RATES_SAMPLES_FREC);

    ADC_StartCmd(ADC_START_ON_MAT01);
    ADC_EdgeStartConfig(ADC_START_ON_RISING);

    canalActual = 0;
    ADC_ChannelEnable(ADC_CHANNEL_0);

    ADC_IntDisable(ADC_INT_GLOBAL);
    for (uint8_t i = 0; i < 5; i++) {
        ADC_IntEnable((ADC_INT_SOURCE)(ADC_INT_CH0 + i));
    }

    NVIC_SetPriority(ADC_IRQn, 1);
    NVIC_EnableIRQ(ADC_IRQn);
}

void calibrarGananciaLDR(void) {
    uint32_t sum_ch[4] = {0, 0, 0, 0};

    // Descartar 2 ventanas iniciales (200 ms) para estabilizar acumuladores
    for (uint8_t d = 0; d < 2; d++) {
        adcMuestreoCompleto = 0;
        while (adcMuestreoCompleto == 0);
    }

    // Promediar 16 ventanas de 100 ms (1.6 s) bajo luz uniforme 
    for (uint8_t k = 0; k < 16; k++) {
        adcMuestreoCompleto = 0;
        while (adcMuestreoCompleto == 0);
        for (uint8_t i = 0; i < 4; i++) {
            sum_ch[i] += adcValue[i];
        }
    }

    uint32_t prom_ch[4];
    uint32_t prom_total = 0;
    for (uint8_t i = 0; i < 4; i++) {
        prom_ch[i] = sum_ch[i] / 16U;
        prom_total += prom_ch[i];
    }
    prom_total /= 4U;

    // Si hay iluminación válida, igualar pendientes en punto fijo Q10
    if (prom_total >= (S_MINIMO / 4U)) {
        for (uint8_t i = 0; i < 4; i++) {
            if (prom_ch[i] > 0U) {
                gain_ldr[i] = (prom_total << 10) / prom_ch[i]; // <<<10 es lo mismo que x1024
            } else {
                gain_ldr[i] = 1024U;
            }
        }
    } else {
        // Fallback seguro si se encendió a oscuras
        for (uint8_t i = 0; i < 4; i++) {
            gain_ldr[i] = 1024U;
        }
    }
}

void calc_error(void) {
    uint32_t adcCorr[4];
    adcTotal = 0;

    for (uint8_t i = 0; i < 4; i++) {
        adcCorr[i] = (adcValue[i] * gain_ldr[i]) >> 10;
        adcTotal  += (int32_t)adcCorr[i];
    }

    if (adcTotal < S_MINIMO) {
        error_x = 0;
        error_y = 0;
        return;
    }

    // adcCorr[0]=STL, [1]=STR, [2]=SBL, [3]=SBR
    error_x = (1000 * ((int32_t)(adcCorr[1] + adcCorr[3]) - (int32_t)(adcCorr[0] + adcCorr[2]))) / adcTotal;
    error_y = (1000 * ((int32_t)(adcCorr[0] + adcCorr[1]) - (int32_t)(adcCorr[2] + adcCorr[3]))) / adcTotal;
}

void ADC_IRQHandler(void) {
    if (ADC_ChannelGetStatus((ADC_CHANNEL)canalActual, ADC_DATA_DONE)) {
        uint16_t dato = ADC_ChannelGetData((ADC_CHANNEL)canalActual);

        if (canalActual < 4) {
            adcAcc[canalActual] += dato;
        } else {
            adcAccLM35 += dato;
        }

        ADC_ChannelDisable((ADC_CHANNEL)canalActual);

        if (canalActual < 4) {
            canalActual++;
            ADC_ChannelEnable((ADC_CHANNEL)canalActual);
            ADC_StartCmd(ADC_START_NOW);
        } else {
            canalActual = 0;
            muestrasCount++;

            if (muestrasCount >= MUESTRAS_PROMEDIO) {
                for (uint8_t i = 0; i < 4; i++) {
                    adcValue[i] = adcAcc[i] / MUESTRAS_PROMEDIO;
                    adcAcc[i]   = 0;
                }
                adcValueLM35  = adcAccLM35 / MUESTRAS_PROMEDIO;
                adcAccLM35    = 0;
                muestrasCount = 0;

                calc_error();
                adcMuestreoCompleto++;
            }

            // Rearmar disparo por hardware para el próximo flanco ascendente de MAT0.1
            ADC_ChannelEnable(ADC_CHANNEL_0);
            ADC_StartCmd(ADC_START_ON_MAT01);
        }
    }
}
