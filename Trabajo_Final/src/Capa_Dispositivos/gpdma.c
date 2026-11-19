/***
 * @file      gpdma.c
 * @brief     Implementación del controlador GPDMA para transferencias en segundo
 *            plano en el microcontrolador LPC1769.
 * @details   Gestiona la inicialización del bloque GPDMA y el lanzamiento de
 *            transferencias Memoria a Periférico (M2P) de ancho byte.
 * @note      Capa de Dispositivos - TPI Electrónica Digital III (Grupo N° 4)
 ******************************************************************************/

/*------------------------------------------------------------------------------
 * INCLUDES
 *----------------------------------------------------------------------------*/
#include "gpdma.h"

// -------------------------------------------------------------------------
// 1. INICIALIZACIÓN DEL CONTROLADOR GPDMA
// -------------------------------------------------------------------------
void configGpdma(void) {
    // Deshabilitar interrupción global del GPDMA en el NVIC: el estado de fin
    // de transferencia se consulta por bandera de canal activo sin interrumpir
    // a la CPU ni alterar las prioridades de ADC_IRQn (1) y EINT0_IRQn (2).
    NVIC_DisableIRQ(DMA_IRQn);

    // Enciende el periférico en PCONP (bit PCGPDMA), resetea los 8 canales,
    // limpia flags de Terminal Count / Error y habilita el controlador.
    GPDMA_Init();
}

// -------------------------------------------------------------------------
// 2. CONSULTA DE ESTADO DE CANAL
// -------------------------------------------------------------------------
bool GPDMA_CanalOcupado(GPDMA_CH canal) {
    if (GPDMA_IntGetStatus(GPDMA_ENABLED_CH, canal) == SET) {
        return true;
    }
    return false;
}

// -------------------------------------------------------------------------
// 3. CONFIGURACIÓN Y DISPARO DE TRANSFERENCIA MEMORIA -> PERIFÉRICO (M2P)
// -------------------------------------------------------------------------
Status GPDMA_IniciarTransferenciaM2P(GPDMA_CH canal,
                                     uint32_t dirOrigenMem,
                                     GPDMA_CONNECTION perifericoDestino,
                                     uint32_t cantidadBytes) {
    // Validación de parámetros básicos antes de tocar registros
    if ((cantidadBytes == 0U) || (cantidadBytes > GPDMA_MAX_TRANSFER_SIZE)) {
        return ERROR;
    }

    // Evitar pisar una transferencia que aún está en progreso en este canal
    if (GPDMA_CanalOcupado(canal)) {
        return ERROR;
    }

    // Limpiar banderas residuales de Terminal Count y Error del canal
    GPDMA_ClearIntPending(GPDMA_CLR_INTTC, canal);
    GPDMA_ClearIntPending(GPDMA_CLR_INTERR, canal);

    // Estructura de configuración del canal DMA:
    // - Se definen explícitamente ancho (BYTE) y ráfaga (BSIZE_1) en ambos
    //   extremos para evitar ambigüedades de las tablas _AUTO del driver.
    // - El puntero de origen incrementa en memoria; el destino permanece fijo
    //   en el registro de datos del periférico (ej. LPC_UART0->THR).
    GPDMA_Channel_CFG_T dmaCfg = {
        .channelNum   = canal,
        .transferSize = cantidadBytes,
        .type         = GPDMA_M2P,
        .srcMemAddr   = dirOrigenMem,
        .dstMemAddr   = 0U,
        .srcConn      = perifericoDestino,
        .dstConn      = perifericoDestino,
        .src = {
            .width     = GPDMA_BYTE,
            .burst     = GPDMA_BSIZE_1,
            .increment = ENABLE
        },
        .dst = {
            .width     = GPDMA_BYTE,
            .burst     = GPDMA_BSIZE_1,
            .increment = DISABLE
        },
        .intTC      = DISABLE,
        .intErr     = DISABLE,
        .linkedList = 0U
    };

    if (GPDMA_SetupChannel(&dmaCfg) != SUCCESS) {
        return ERROR;
    }

    // Habilita el canal: el hardware transferirá cada byte a medida que el
    // periférico destino active su señal de solicitud DMA.
    GPDMA_ChannelStart(canal);

    return SUCCESS;
}