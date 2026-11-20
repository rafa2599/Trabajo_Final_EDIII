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

    NVIC_DisableIRQ(DMA_IRQn);


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
   
    if ((cantidadBytes == 0U) || (cantidadBytes > GPDMA_MAX_TRANSFER_SIZE)) {
        return ERROR;
    }

    
    if (GPDMA_CanalOcupado(canal)) {
        return ERROR;
    }

   
    GPDMA_ClearIntPending(GPDMA_CLR_INTTC, canal);
    GPDMA_ClearIntPending(GPDMA_CLR_INTERR, canal);

 
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

    GPDMA_ChannelStart(canal);

    return SUCCESS;
}