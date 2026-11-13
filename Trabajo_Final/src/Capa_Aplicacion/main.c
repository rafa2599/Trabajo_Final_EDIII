/***
 * @file      main.c
 * @brief     Programa principal - Brazo Robótico Seguidor de Luz (TPI EDIII)
 ******************************************************************************/

#include "LPC17xx.h"
#include "adc.h"
#include "pwm.h"
#include "timer.h"
#include "control.h"
#include "eint.h"
#include "temperatura.h"
//#include "uart_telemetria.h"

extern volatile uint8_t adcMuestreoCompleto;
extern volatile int32_t error_x, error_y;
extern volatile uint32_t adcValueLM35;

int main(void) {
    // 1. Inicialización de la arquitectura y reloj del sistema
    SystemInit();

    // 2. Configuración de periféricos
    configPwm();       // Configura PWM1 (50Hz) en P2.0 y P2.1
    configTimer0();    // Configura Timer 0 (100 ms) para disparo de MAT0.1
    configAdc();       // Configura ADC (P0.23-P0.27) y disparo por MAT0.1
    configEint0();     // Configura Botón de cambio de modo en P2.10
    configControl();   // Inicializa posición neutra de servos
    //configUart0();     // Inicializa UART0 a 115200 bps

    // Configuración del LED/Buzzer de Alerta en P0.22 como salida
    LPC_GPIO0->FIODIR |= (1 << 22);

    while (1) {
        // Ejecutar el procesamiento cuando el ADC completa la ráfaga de 5 canales (cada 100 ms)
        if (adcMuestreoCompleto > 0) {
            adcMuestreoCompleto = 0;

            // 1. Procesar lectura del sensor de temperatura LM35
            uint32_t mV = counts_to_miliVolts(adcValueLM35);
            miliVolts_to_celcius(mV);

            // 2. Ejecutar algoritmo del Controlador Proporcional Incremental
            ejecutarControlProporcional(error_x, error_y);

            // 3. Transmitir telemetría de monitoreo vía UART0
            transmitirTelemetria();
        }
    }
}
