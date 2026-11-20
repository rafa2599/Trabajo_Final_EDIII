/***
 * @file      eint.h
 * @brief     Declaraciones y parámetros Maneja el pulsador de usuario en P2.10 (EINT0) 
 *            para alternar el flag modo_activo entre STANDBY (Reposo centrado) y ACTIVO 
 *            (Seguimiento lumínico).
 * @note      Capa de Dispositivos - TPI Electrónica Digital III (Grupo N° 4)
 ******************************************************************************/

#ifndef EINT_H_
#define EINT_H_

/*------------------------------------------------------------------------------
 * INCLUDES
 *----------------------------------------------------------------------------*/
#include "LPC17xx.h"
#include <lpc17xx_exti.h>
#include <stdbool.h>

/*------------------------------------------------------------------------------
 * PROTOTIPOS DE FUNCIONES
 *----------------------------------------------------------------------------*/

void configEint0(void);
#endif /*EINT_H_*/
