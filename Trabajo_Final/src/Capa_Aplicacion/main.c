#include "LPC17xx.h"
#include "adc.h"
#include "pwm.h"
#include "timer.h"
#include "control.h"
#include "eint.h"
#include "temperatura.h"
#include "gpdma.h"                 
#include "uart.h"                  

volatile uint32_t ticks_100ms = 0;

int main(void) {
    SystemInit();

    configPinLedLM35();
    configPwm();
    configTimer0();
    configAdc();
    configEint0();
    configControl();
    configGpdma();                 
    configUart0();                 


    // Calibra ganancias Q10 durante ~1.6s en STANDBY con luz uniforme
    calibrarGananciaLDR();


    while (1) {
        if (adcMuestreoCompleto > 0) {
            adcMuestreoCompleto = 0;
            ticks_100ms++;

            uint32_t mV = counts_to_miliVolts(adcValueLM35);
            miliVolts_to_celcius(mV);


            ejecutarControlProporcional(error_x, error_y);

            if ((ticks_100ms % 10U) == 0U) {
                UART0_EnviarTelemetriaDMA();
            }
        }
    }
}