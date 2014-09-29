/**
 * @file	Application.h
 * @purpose
 * @brief
 * @version
 * @date	30-sep-2009
 * @author	nlv15840
*/
/*----------------------------------------------------------------------------
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * products. This software is supplied "AS IS" without any warranties.
 * NXP Semiconductors assumes no responsibility or liability for the
 * use of the software, conveys no license or title under any patent,
 * copyright, or mask work right to the product. NXP Semiconductors
 * reserves the right to make changes in the software without
 * notification. NXP Semiconductors also make no representation or
 * warranty that such application will be suitable for the specified
 * use without further testing or modification.
 **********************************************************************/

#ifndef APPLICATION_H_
#define APPLICATION_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "LPC17xx.h"
#include "lpc17xx_nvic.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_qei.h"
#include "lpc17xx_mcpwm.h"
#include "lpc17xx_pinsel.h"

#include "lpc17xx_nvic.h"
#include "lpc17xx_timer.h"
#include "lpc17xx_clkpwr.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_uart.h"

#include "type.h"
#include "BLDC.h"
#include "UART_ProcessData.h"
#include "PID.h"

#define SysTick_VALUE (SystemFrequency/1000 - 1)

#endif /* APPLICATION_H_ */
