/*
 * 1754adc.h
 *
 *  Created on: 08.06.2012
 *      Author: Graph
 */

#ifndef L1754ADC_H_
#define L1754ADC_H_

#include "lpc_types.h"
#include "lpc17xx_nvic.h"
#include "lpc17xx_adc.h"
#include "lpc17xx_gpdma.h"



void my_ADC_Init(void);

U32 Bright_filter(U16);

U16 Input_adc_scale(U16);

U32 U_filter(U16);




#endif /* L1754ADC_H_ */
