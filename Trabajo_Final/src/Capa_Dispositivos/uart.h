/***
 * @file      uart.h
 * @brief     Declaraciones y parámetros para la comunicación serie UART0 y
 *            telemetría asistida por GPDMA en el LPC1769.
 * @details   Configura UART0 en P0.2 (TXD0) y P0.3 (RXD0) a 115200 bps (8N1),
 *            gestiona el formateo y transmisión no bloqueante de variables de
 *            estado mediante GPDMA y procesa comandos remotos por interrupción.
 * @note      Capa de Dispositivos - TPI Electrónica Digital III (Grupo N° 4)
 ******************************************************************************/

#ifndef UART_H_
#define UART_H_

/*------------------------------------------------------------------------------
 * INCLUDES
 *----------------------------------------------------------------------------*/
#include "LPC17xx.h"
#include <lpc17xx_uart.h>
#include <stdint.h>
#include <stdbool.h>

/*------------------------------------------------------------------------------
 * DEFINICIONES Y MACROS
 *----------------------------------------------------------------------------*/
/** Velocidad de transmisión en baudios para enlace USB-TTL (CP2102) */
#define UART0_BAUDRATE          115200U

/** Tamaño máximo del buffer de telemetría alojado en AHB SRAM0 */
#define UART0_TX_BUFFER_SIZE    128U

/** Prioridad NVIC de UART0 (menor prioridad que ADC=1 y EINT0=2) */
#define UART0_NVIC_PRIORITY     3U

/*------------------------------------------------------------------------------
 * PROTOTIPOS DE FUNCIONES
 *----------------------------------------------------------------------------*/
/**
 * @brief   Inicializa el periférico UART0 y habilita su soporte DMA e interrupción RX.
 * @details Asigna los pines P0.2 (TXD0) y P0.3 (RXD0), configura 115200 bps 8N1,
 *          activa las FIFOs de 16 bytes con soporte DMA (FCR.DMAMODE) y habilita
 *          la interrupción por recepción de datos (RBR) en el NVIC.
 */
void configUart0(void);

/**
 * @brief   Formatea y transmite por DMA una trama con el estado actual del sistema.
 * @details Toma las lecturas promediadas de los 4 LDR, errores X/Y, anchos de
 *          pulso PWM, temperatura LM35 y modo operativo, las escribe en el buffer
 *          ubicado en AHB SRAM0 y dispara una transferencia M2P por GPDMA.
 */
void UART0_EnviarTelemetriaDMA(void);

#endif /* UART_H_ */