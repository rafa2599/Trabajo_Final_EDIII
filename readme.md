## Arquitectura y Diagrama de Secuencia

El firmware está organizado en una arquitectura modular por periférico sobre el **LPC1769 (ARM Cortex-M3)**, separando la adquisición analógica, el lazo de control en punto fijo (Q8/Q10), la protección térmica y la telemetría no bloqueante asistida por **GPDMA**.

### Jerarquía de Interrupciones (NVIC)

Para garantizar el determinismo del muestreo y de los actuadores frente a la comunicación con la PC, se establece la siguiente prioridad de interrupciones:

| Prioridad NVIC | Interrupción | Módulo | Función |
| :---: | :--- | :--- | :--- |
| **1 (Máxima)** | `ADC_IRQn` | `adc.c` | Lectura encadenada de los 5 canales analógicos (`AD0.0`–`AD0.4`). |
| **2 (Media)** | `EINT0_IRQn` | `eint.c` | Atención asíncrona del pulsador físico (`P2.10`) con antirrebote de `200 ms`. |
| **3 (Baja)** | `UART0_IRQn` | `uart.c` | Recepción de comandos desde la PC (`RXD0` en `P0.3`) sin bloquear el control. |
| **Deshabilitada** | `DMA_IRQn` | `gpdma.c` | El estado del Canal 0 se consulta por bandera hardware (`DMACEnbldChns`). |

---

### Diagrama de Secuencia del Sistema

```mermaid
sequenceDiagram
    autonumber
    actor USR as Usuario / PC (CP2102)
    participant HW as Hardware / NVIC<br/>(Timer0, ADC, EINT0, UART0)
    participant MAIN as main.c
    participant DRV as Drivers<br/>(pwm, timer, eint)
    participant ADC as adc.c
    participant TEMP as temperatura.c
    participant CTRL as control.c
    participant UART as uart.c
    participant DMA as gpdma.c

    Note over MAIN,DMA: FASE 1: INICIALIZACIÓN Y CALIBRACIÓN (Arranque en STANDBY)
    MAIN->>HW: SystemInit() y GPIO P0.22 (LED alarma)
    MAIN->>DRV: configPwm()
    Note right of DRV: PWM1.1 (P2.0) y PWM1.2 (P2.1)<br/>Período 20 ms, pulso neutro 1500 us
    MAIN->>DRV: configTimer0()
    Note right of DRV: MAT0.1 conmuta cada 5 ms<br/>(flanco ascendente cada 10 ms)
    MAIN->>ADC: configAdc()
    ADC->>HW: configPinsAdc() + NVIC_EnableIRQ(ADC_IRQn) [Prio 1]
    MAIN->>DRV: configEint0()
    DRV->>HW: configPinEint0() + NVIC_EnableIRQ(EINT0_IRQn) [Prio 2]
    MAIN->>CTRL: configControl()
    MAIN->>DMA: configGpdma()
    Note right of DMA: Habilita PCGPDMA, limpia 8 canales<br/>y deshabilita DMA_IRQn
    MAIN->>UART: configUart0()
    UART->>HW: UART0 (115200 8N1) + FIFO DMA + NVIC_EnableIRQ(UART0_IRQn) [Prio 3]

    alt MODO_TEST_SERVOS es 0
        MAIN->>ADC: calibrarGananciaLDR()
        Note right of ADC: Descarta 2 ventanas y promedia<br/>16 ventanas para gain_ldr[0..3] (Q10)
        ADC-->>MAIN: Ganancias Q10 calibradas
    end

    Note over HW,UART: FASE 2: INTERRUPCIONES ASÍNCRONAS DE HARDWARE
    loop Cada flanco ascendente de MAT0.1
        HW->>ADC: ADC_IRQHandler() [Ch0, Ch1, Ch2, Ch3, Ch4]
        Note right of ADC: Encadena conversiones con ADC_START_NOW<br/>y acumula muestras
        alt muestrasCount llega a 100 (MUESTRAS_PROMEDIO)
            ADC->>ADC: calc_error()
            Note right of ADC: Aplica gain_ldr[] (Q10)<br/>y calcula error_x, error_y
            ADC-->>MAIN: adcMuestreoCompleto + 1
        end
    end

    opt Pulsación de botón físico en P2.10
        USR->>HW: Flanco descendente EINT0
        HW->>DRV: EINT0_IRQHandler()
        Note right of DRV: Limpia EXTI_EINT0, antirrebote 2 ticks<br/>e invierte modo_activo
    end

    opt Comando recibido desde PC por RXD0 (P0.3)
        USR->>HW: Envía carácter ('m', 'a' o 's')
        HW->>UART: UART0_IRQHandler()
        Note right of UART: m = alterna modo_activo<br/>a = ACTIVO / s = STANDBY
    end

    Note over MAIN,DMA: FASE 3: BUCLE PRINCIPAL (while(1)) Y TELEMETRÍA POR DMA
    loop Continuo
        opt adcMuestreoCompleto mayor a 0
            MAIN->>MAIN: adcMuestreoCompleto = 0, ticks_100ms + 1
            MAIN->>TEMP: counts_to_miliVolts(adcValueLM35)
            TEMP-->>MAIN: mV
            MAIN->>TEMP: miliVolts_to_celcius(mV)
            Note right of TEMP: Actualiza temperatura_celsius<br/>Histéresis alarma P0.22 (encendida sobre 40°C / apagada desde 37°C)

            alt MODO_TEST_SERVOS es 1
                MAIN->>CTRL: probarServosAislados(ticks_100ms)
                CTRL->>DRV: PWM_ActualizarServos(pos_servo_x, pos_servo_y)
            else MODO_TEST_SERVOS es 0
                MAIN->>CTRL: ejecutarControlProporcional(error_x, error_y)
                Note right of CTRL: STANDBY (1500 us) o ACTIVO<br/>(Deadband + KP Q8 + Slew + Sat)
                CTRL->>DRV: PWM_ActualizarServos(pos_servo_x, pos_servo_y)
            end

            MAIN->>UART: UART0_EnviarTelemetriaDMA()
            UART->>DMA: GPDMA_CanalOcupado(GPDMA_CH_0)
            alt Canal 0 libre
                Note right of UART: Formatea trama ASCII en<br/>AHB SRAM0 (0x2007C000)
                UART->>DMA: GPDMA_IniciarTransferenciaM2P(CH_0, buf, GPDMA_UART0_Tx, len)
                DMA-->>HW: Transfiere bytes de AHB SRAM0 a LPC_UART0.THR sin CPU
                HW-->>USR: Envía trama por TXD0 (P0.2) a 115200 bps
            end
        end
    end
```