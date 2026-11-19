/***
 * @file      gpdma.h
 * @brief     Declaraciones y parámetros para la configuración y control del
 *            periférico GPDMA (General Purpose DMA) en el LPC1769.
 * @details   Provee funciones de inicialización del controlador DMA, verificación
 *            de estado de canal y disparo de transferencias Memoria a Periférico
 *            (M2P) sin bloquear la CPU.
 * @note      Capa de Dispositivos - TPI Electrónica Digital III (Grupo N° 4)
 ******************************************************************************/

#ifndef GPDMA_H_
#define GPDMA_H_

/*------------------------------------------------------------------------------
 * INCLUDES
 *----------------------------------------------------------------------------*/
#include "LPC17xx.h"
#include <lpc17xx_gpdma.h>
#include <stdint.h>
#include <stdbool.h>

/*------------------------------------------------------------------------------
 * DEFINICIONES Y MACROS
 *----------------------------------------------------------------------------*/
/** Dirección base del Banco 0 de la AHB SRAM (16 KB), accesible por el GPDMA
 *  sin contención de bus con la SRAM principal utilizada por la CPU. */
#define GPDMA_AHB_SRAM0_BASE    0x2007C000UL

/** Canal DMA asignado a la transmisión de telemetría por UART0 */
#define GPDMA_CANAL_UART0_TX    GPDMA_CH_0

/** Cantidad máxima de elementos permitidos por el hardware en una transferencia DMA */
#define GPDMA_MAX_TRANSFER_SIZE 4095U

/*------------------------------------------------------------------------------
 * PROTOTIPOS DE FUNCIONES
 *----------------------------------------------------------------------------*/
/**
 * @brief   Inicializa el controlador GPDMA y limpia todos sus canales.
 * @details Habilita el clock del periférico en PCONP, resetea los 8 canales DMA,
 *          limpia banderas de interrupción pendientes y mantiene deshabilitada
 *          DMA_IRQn en el NVIC para no interferir con el lazo de control.
 */
void configGpdma(void);

/**
 * @brief   Verifica si un canal DMA se encuentra actualmente ejecutando una transferencia.
 * @param   canal  Canal DMA a consultar (GPDMA_CH_0 ... GPDMA_CH_7).
 * @return  true si el canal está activo/ocupado, false si está libre.
 */
bool GPDMA_CanalOcupado(GPDMA_CH canal);

/**
 * @brief   Configura e inicia una transferencia Memoria a Periférico (M2P) de 8 bits.
 * @details Transfiere un bloque de bytes consecutivos desde RAM hacia el registro
 *          FIFO de un periférico de salida controlado por flujo hardware.
 * @param   canal              Canal GPDMA a utilizar (GPDMA_CH_0 ... GPDMA_CH_7).
 * @param   dirOrigenMem       Dirección inicial en memoria RAM (preferentemente AHB SRAM).
 * @param   perifericoDestino  Línea de petición DMA del periférico destino (ej. GPDMA_UART0_Tx).
 * @param   cantidadBytes      Cantidad de bytes a transferir (1 a 4095).
 * @return  SUCCESS si la transferencia se inició correctamente, ERROR en caso contrario.
 */
Status GPDMA_IniciarTransferenciaM2P(GPDMA_CH canal,
                                     uint32_t dirOrigenMem,
                                     GPDMA_CONNECTION perifericoDestino,
                                     uint32_t cantidadBytes);

#endif /* GPDMA_H_ */