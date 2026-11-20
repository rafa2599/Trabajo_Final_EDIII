/***
 * @file      uart.c
 * @brief     Implementación de comunicación UART0 con transmisión de telemetría
 *            vía GPDMA y recepción de comandos por interrupción.
 * @details   Permite supervisar en tiempo real todas las variables del seguidor
 *            solar y conmutar su estado operativo desde una terminal en PC sin
 *            afectar los tiempos del lazo de control de 100 ms.
 * @note      Capa de Dispositivos - TPI Electrónica Digital III (Grupo N° 4)
 ******************************************************************************/

/*------------------------------------------------------------------------------
 * INCLUDES
 *----------------------------------------------------------------------------*/
#include "uart.h"
#include "gpdma.h"
#include "adc.h"
#include "control.h"
#include <stdio.h>

/*------------------------------------------------------------------------------
 * VARIABLES EXTERNAS DE OTROS MÓDULOS (SOLO LECTURA DE TELEMETRÍA)
 *----------------------------------------------------------------------------*/
extern volatile uint32_t temperatura_celsius;
extern volatile bool     alerta_sobretemperatura;

/*------------------------------------------------------------------------------
 * VARIABLES PRIVADAS DEL MÓDULO
 *----------------------------------------------------------------------------*/
/** Puntero al buffer de transmisión ubicado al inicio de AHB SRAM Bank 0
 *  (0x2007C000) para que el controlador GPDMA acceda por su bus dedicado. */
static char * const uart0_tx_buffer = (char *)GPDMA_AHB_SRAM0_BASE;

// -------------------------------------------------------------------------
// 1. CONFIGURACIÓN DEL PERIFÉRICO UART0
// -------------------------------------------------------------------------
void configUart0(void) {
    // 1. Configurar pines físicos P0.2 (TXD0) y P0.3 (RXD0) en modo Tristate
    UART_PinConfig(UART_TX0_P0_2);
    UART_PinConfig(UART_RX0_P0_3);

    // 2. Configurar formato de trama: 115200 bps, 8 bits de datos, sin paridad, 1 stop
    UART_CFG_T uartCfg = {
        .baudRate = UART0_BAUDRATE,
        .parity   = UART_PARITY_NONE,
        .dataBits = UART_DBITS_8,
        .stopBits = UART_STOPBIT_1
    };
    UART_Init(UART0, &uartCfg);

    // 3. Configurar FIFOs de 16 bytes:
    //    - resetRxBuf / resetTxBuf: limpia residuos iniciales
    //    - dmaMode = ENABLE: habilita las peticiones hardware hacia el GPDMA
    //    - level = TRGLEV0: dispara interrupción RX al recibir 1 carácter
    UART_FIFO_CFG_T fifoCfg = {
        .resetRxBuf = ENABLE,
        .resetTxBuf = ENABLE,
        .dmaMode    = ENABLE,
        .level      = UART_FIFO_TRGLEV0
    };
    UART_FIFOConfig(UART0, &fifoCfg);

    // 4. Habilitar el transmisor serie (TER.TXEN)
    UART_TxEnable(UART0);

    // 5. Habilitar interrupción por recepción de datos (RBR) con prioridad 3
    UART_IntConfig(UART0, UART_INT_RBR, ENABLE);
    NVIC_SetPriority(UART0_IRQn, UART0_NVIC_PRIORITY);
    NVIC_EnableIRQ(UART0_IRQn);
}

// -------------------------------------------------------------------------
// 2. ENVÍO DE TELEMETRÍA MEDIANTE GPDMA (MEMORIA -> UART0 TX)
// -------------------------------------------------------------------------
void UART0_EnviarTelemetriaDMA(void) {
    // Si el canal DMA aún está vaciando la trama anterior, salir sin bloquear
    // ni sobreescribir el buffer en AHB SRAM0.
    if (GPDMA_CanalOcupado(GPDMA_CANAL_UART0_TX)) {
        return;
    }

    // Formatear trama legible en el buffer de AHB SRAM0 con el estado del brazo
    int longitud = snprintf(
        uart0_tx_buffer,
        UART0_TX_BUFFER_SIZE,
        "MODO:%u | ERR:(%ld,%ld) | PWM:(%lu,%lu) | T:%luC(%u) | LDR:[%lu,%lu,%lu,%lu]\r\n",
        (unsigned int)(modo_activo ? 1U : 0U),
        (long)error_x,
        (long)error_y,
        (unsigned long)pos_servo_x,
        (unsigned long)pos_servo_y,
        (unsigned long)temperatura_celsius,
        (unsigned int)(alerta_sobretemperatura ? 1U : 0U),
        (unsigned long)adcValue[0],
        (unsigned long)adcValue[1],
        (unsigned long)adcValue[2],
        (unsigned long)adcValue[3]
    );

    if (longitud <= 0) {
        return;
    }
    if (longitud >= (int)UART0_TX_BUFFER_SIZE) {
        longitud = (int)(UART0_TX_BUFFER_SIZE - 1U);
    }

    // Lanzar transferencia Memoria -> Periférico por el canal DMA asignado
    GPDMA_IniciarTransferenciaM2P(GPDMA_CANAL_UART0_TX,
                                  (uint32_t)uart0_tx_buffer,
                                  GPDMA_UART0_Tx,
                                  (uint32_t)longitud);
}

/* -------------------------------------------------------------------------
// 3. RUTINA DE INTERRUPCIÓN DE RECEPCIÓN (COMANDOS DESDE PC)
// -------------------------------------------------------------------------
void UART0_IRQHandler(void) {
    uint32_t iir   = UART_GetIntId(UART0);
    uint32_t intId = iir & UART_IIR_INTID_MASK;

    // Atender tanto dato disponible (RDA) como timeout de carácter en FIFO (CTI)
    if ((intId == UART_IIR_INTID_RDA) || (intId == UART_IIR_INTID_CTI)) {
        while (UART_GetLineStatus(UART0) & UART_LSR_RDR) {
            uint8_t comando = UART_ReceiveByte(UART0);

            switch (comando) {
                case 'm':
                case 'M':
                    // Conmuta entre STANDBY y ACTIVO (misma acción que pulsador EINT0)
                    modo_activo = !modo_activo;
                    break;

                case 'a':
                case 'A':
                    // Fuerza modo ACTIVO (seguimiento lumínico habilitado)
                    modo_activo = true;
                    break;

                case 's':
                case 'S':
                    // Fuerza modo STANDBY (servos centrados en 1500 us)
                    modo_activo = false;
                    break;

                default:
                    break;
            }
        }
    }
}*/