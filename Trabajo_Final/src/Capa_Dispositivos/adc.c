/***
 * @file      adc.c
 * @brief     Implementación de la configuración del ADC para LPC1769
 * @details   Configura los registros del ADC para la lectura de los sensores LDR y LM35
 * @note      
 ******************************************************************************/

/*------------------------------------------------------------------------------
 * INCLUDES
 *----------------------------------------------------------------------------*/
 #include "LPC17xx.h"
 #include "adc.h"
 #include "pinConfig.h"

// -------------------------------------------------------------------------
// 1. CONFIGURACIÓN DE MODULOS DE ADC (LDRs + LM35)
// -------------------------------------------------------------------------
volatile uint32_t adcValue[4]; // Variable global para almacenar los valores del LDR
volatile uint32_t adcValueLM35; // Variable global para almacenar el valor del LM35
volatile int32_t adcTotal;
volatile int32_t error_x, error_y;
volatile uint8_t  adcMuestreoCompleto = 0; //
static uint8_t canalActual = 0; // Variable global para almacenar el canal actual del ADC
void configAdc(void){

    //Energizamos el periferico ADC
	ADC_PowerUp();
    
    //Configuramos el pin del canal 0 del ADC (P0.23) para el LDR
	ADC_PinConfig(ADC_CHANNEL_0);
  
    //Configuramos el disparo inicial: MAT0.1 en flanco ascendente
	ADC_EdgeStartConfig(ADC_START_ON_RISING);
    ADC_StartCmd(ADC_START_ON_MAT01);

    //Habilitamos el canal 0 del ADC para el LDR
    ADC_ChannelEnable(ADC_CHANNEL_0);
  
    //Hablitamos las interrupciones de los canales 0, 1, 2 y 3 del ADC
    for (volatile uint8_t i = 0; i < 4; i++) {
        ADC_IntEnable(ADC_INT_CH0 + i);
    }
	// Inicializamos el ADC con una frecuencia de muestreo
	ADC_Init(RATES_SAMPLES_FREC);

    //Habilitamos la interrupción del ADC en el NVIC y configuramos su prioridad
	NVIC_EnableIRQ(ADC_IRQn);
    NIVIC_SetPriority(ADC_IRQn, 1); // Configura la prioridad de la interrupción del ADC
}
void calc_error(void)
{
    adcTotal = 0;
    for (uint8_t i = 0; i < 4; i++) {
        adcTotal += adcValue[i];
    }

    // Protección contra división por cero o penumbra extrema
    if (adcTotal < S_MINIMO) {
        error_x = 0;
        error_y = 0;
        return;
    }

    // Formulación normalizada según informe técnico (Rango -1000 a +1000)
    // adcValue[0] = STL, adcValue[1] = STR, adcValue[2] = SBL, adcValue[3] = SBR
    error_x = (1000 * ((int32_t)(adcValue[1] + adcValue[3]) - (int32_t)(adcValue[0] + adcValue[2]))) / adcTotal;
    error_y = (1000 * ((int32_t)(adcValue[0] + adcValue[1]) - (int32_t)(adcValue[2] + adcValue[3]))) / adcTotal;
}
void ADC_IRQHandler(void){

    if (ADC_ChannelGetStatus(canalActual, ADC_DATA_DONE)) {
        if (canalActual < 4){
            adcValue[canalActual] = ADC_ChannelGetData (canalActual);
             

        }
        else {
            adcValueLM35 = ADC_ChannelGetData (canalActual); 
        }
        // Deshabilitar el canal actual
        ADC_ChannelDisable(canalActual);

        if (canalActual < 4) 
        {
            // Pasar al siguiente canal secuencialmente por software
            canalActual++;
            ADC_ChannelEnable(canalActual);
            ADC_StartCmd(ADC_START_CONTINUOUS); // Iniciar conversión inmediata
        } 
        else 
        {   calc_error();
            // Fin de la ráfaga de 5 canales: Reconfigurar para el próximo pulso de MAT0.1
            canalActual = 0;
            adcMuestreoCompleto += 1;

            ADC_ChannelEnable(ADC_CHANNEL_0);
            ADC_StartCmd(ADC_START_ON_MAT01); // Aguardar próximo evento del Timer 0
    }
}
