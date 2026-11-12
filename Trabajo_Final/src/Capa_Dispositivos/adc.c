/***
 * @file      adc.c
 * @brief     Implementación corregida de la configuración del ADC e ISR
 ******************************************************************************/
#include "LPC17xx.h"
#include "adc.h"
#include "pinConfig.h"

volatile uint32_t adcValue[4];     // STL, STR, SBL, SBR
volatile uint32_t adcValueLM35;  // Lectura LM35
volatile int32_t adcTotal;
volatile int32_t error_x, error_y;
volatile uint8_t adcMuestreoCompleto = 0;
static uint8_t canalActual = 0;

void configAdc(void){
    // Configurar pines analógicos P0.23 a P0.27
    configPinsAdc();

    ADC_PowerUp();
    
    // Configurar disparo inicial por MAT0.1 en flanco ascendente
    ADC_EdgeStartConfig(ADC_START_ON_RISING);
    ADC_StartCmd(ADC_START_ON_MAT01);

    // Habilitar canal inicial
    ADC_ChannelEnable(ADC_CHANNEL_0);
  
    // Habilitar interrupciones para los 5 canales (0 a 4)
    for (uint8_t i = 0; i < 5; i++) {
        ADC_IntEnable(ADC_INT_CH0 + i);
    }

    ADC_Init(RATES_SAMPLES_FREC);

    // Corregido: NVIC_SetPriority
    NVIC_SetPriority(ADC_IRQn, 1);
    NVIC_EnableIRQ(ADC_IRQn);
}

void calc_error(void)
{
    adcTotal = 0;
    for (uint8_t i = 0; i < 4; i++) {
        adcTotal += adcValue[i];
    }

    if (adcTotal < S_MINIMO) {
        error_x = 0;
        error_y = 0;
        return;
    }

    // Normalización del vector de error espacial (-1000 a +1000)
    // adcValue[0]=STL, [1]=STR, [2]=SBL, [3]=SBR
    error_x = (1000 * ((int32_t)(adcValue[1] + adcValue[3]) - (int32_t)(adcValue[0] + adcValue[2]))) / adcTotal;
    error_y = (1000 * ((int32_t)(adcValue[0] + adcValue[1]) - (int32_t)(adcValue[2] + adcValue[3]))) / adcTotal;
}

void ADC_IRQHandler(void){
    if (ADC_ChannelGetStatus(canalActual, ADC_DATA_DONE)) {
        if (canalActual < 4) {
            adcValue[canalActual] = ADC_ChannelGetData(canalActual);
        } else {
            adcValueLM35 = ADC_ChannelGetData(canalActual); 
        }

        ADC_ChannelDisable(canalActual);

        if (canalActual < 4) {
            // Pasar al siguiente canal en la ráfaga
            canalActual++;
            ADC_ChannelEnable(canalActual);
            ADC_StartCmd(ADC_START_CONTINUOUS);
        } else {
            // Fin de la ráfaga de 5 canales (0..4)
            calc_error();
            canalActual = 0;
            adcMuestreoCompleto++;

            // Reconfigurar para aguardar el próximo disparo de Timer 0
            ADC_ChannelEnable(ADC_CHANNEL_0);
            ADC_StartCmd(ADC_START_ON_MAT01);
        }
    }
}