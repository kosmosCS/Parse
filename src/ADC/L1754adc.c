/*
 * 1754adc.c
 *
 *  Created on: 08.06.2012
 *      Author: Graph
 */

#include "L1754adc.h"
#include "lpc17xx_adc.h"
#include "lpc17xx_clkpwr.h"
#include "debug_frmwrk.h"
#include "inter_volatile.h"
#include "lpc17xx_pinsel.h"



volatile U16 ADC_DMA_BUFF[256];

	GPDMA_Channel_CFG_Type D_M_A_5 = {
			5, /* uint32_t ChannelNum; 	< DMA channel number, should be in
										range from 0 to 7.
										Note: DMA channel 0 has the highest priority
										and DMA channel 7 the lowest priority.
										*/
			128, 						/*uint32_t TransferSize;	< Length/Size of transfer */
			GPDMA_WIDTH_HALFWORD, /*uint32_t TransferWidth;	< Transfer width - used for TransferType is GPDMA_TRANSFERTYPE_M2M only */
			0, /*uint32_t SrcMemAddr;	< Physical Source Address, used in case TransferType is chosen as
										 GPDMA_TRANSFERTYPE_M2M or GPDMA_TRANSFERTYPE_M2P */
			(uint32_t)&ADC_DMA_BUFF, /* uint32_t DstMemAddr;	< Physical Destination Address, used in case TransferType is chosen as
										 GPDMA_TRANSFERTYPE_M2M or GPDMA_TRANSFERTYPE_P2M */
			GPDMA_TRANSFERTYPE_P2M, /* uint32_t TransferType;	< Transfer Type, should be one of the following:
									- GPDMA_TRANSFERTYPE_M2M: Memory to memory - DMA control
									- GPDMA_TRANSFERTYPE_M2P: Memory to peripheral - DMA control
									- GPDMA_TRANSFERTYPE_P2M: Peripheral to memory - DMA control
									- GPDMA_TRANSFERTYPE_P2P: Source peripheral to destination peripheral - DMA control
									*/
			GPDMA_CONN_ADC, /*uint32_t SrcConn;		< Peripheral Source Connection type, used in case TransferType is chosen as
									GPDMA_TRANSFERTYPE_P2M or GPDMA_TRANSFERTYPE_P2P, should be one of
									following:
									 - GPDMA_CONN_SSP0_Tx: SSP0, Tx
									 - GPDMA_CONN_SSP0_Rx: SSP0, Rx
									 - GPDMA_CONN_SSP1_Tx: SSP1, Tx
									 - GPDMA_CONN_SSP1_Rx: SSP1, Rx
									 - GPDMA_CONN_ADC: ADC
									 - GPDMA_CONN_I2S_Channel_0: I2S Channel 0
									 - GPDMA_CONN_I2S_Channel_1: I2S Channel 1
									 - GPDMA_CONN_DAC: DAC
									 - GPDMA_CONN_UART0_Tx_MAT0_0: UART0 Tx / MAT0.0
									 - GPDMA_CONN_UART0_Rx_MAT0_1: UART0 Rx / MAT0.1
									 - GPDMA_CONN_UART1_Tx_MAT1_0: UART1 Tx / MAT1.0
									 - GPDMA_CONN_UART1_Rx_MAT1_1: UART1 Rx / MAT1.1
									 - GPDMA_CONN_UART2_Tx_MAT2_0: UART2 Tx / MAT2.0
									 - GPDMA_CONN_UART2_Rx_MAT2_1: UART2 Rx / MAT2.1
									 - GPDMA_CONN_UART3_Tx_MAT3_0: UART3 Tx / MAT3.0
									 - GPDMA_CONN_UART3_Rx_MAT3_1: UART3 Rx / MAT3.1
									 */
			0, /*uint32_t DstConn;		< Peripheral Destination Connection type, used in case TransferType is chosen as
									GPDMA_TRANSFERTYPE_M2P or GPDMA_TRANSFERTYPE_P2P, should be one of
									following:
									 - GPDMA_CONN_SSP0_Tx: SSP0, Tx
									 - GPDMA_CONN_SSP0_Rx: SSP0, Rx
									 - GPDMA_CONN_SSP1_Tx: SSP1, Tx
									 - GPDMA_CONN_SSP1_Rx: SSP1, Rx
									 - GPDMA_CONN_ADC: ADC
									 - GPDMA_CONN_I2S_Channel_0: I2S Channel 0
									 - GPDMA_CONN_I2S_Channel_1: I2S Channel 1
									 - GPDMA_CONN_DAC: DAC
									 - GPDMA_CONN_UART0_Tx_MAT0_0: UART0 Tx / MAT0.0
									 - GPDMA_CONN_UART0_Rx_MAT0_1: UART0 Rx / MAT0.1
									 - GPDMA_CONN_UART1_Tx_MAT1_0: UART1 Tx / MAT1.0
									 - GPDMA_CONN_UART1_Rx_MAT1_1: UART1 Rx / MAT1.1
									 - GPDMA_CONN_UART2_Tx_MAT2_0: UART2 Tx / MAT2.0
									 - GPDMA_CONN_UART2_Rx_MAT2_1: UART2 Rx / MAT2.1
									 - GPDMA_CONN_UART3_Tx_MAT3_0: UART3 Tx / MAT3.0
									 - GPDMA_CONN_UART3_Rx_MAT3_1: UART3 Rx / MAT3.1
									 */
			0 /* uint32_t DMALLI;		< Linker List Item structure data address
									if there's no Linker List, set as '0'
									*/

	};

typedef enum
	{
	ADC_Res_1 	= 2,
	ADC_Res_2	= 5
	} ADC_e;

void my_ADC_Init(void)
{

	PINSEL_CFG_Type m_pin_sel[2]=	{
	{PINSEL_PORT_0, PINSEL_PIN_25, PINSEL_FUNC_1, PINSEL_PINMODE_TRISTATE, 	PINSEL_PINMODE_NORMAL },//2 канал
	{PINSEL_PORT_1, PINSEL_PIN_31, PINSEL_FUNC_3, PINSEL_PINMODE_TRISTATE, PINSEL_PINMODE_NORMAL }  //5 канал
	};

	for(U8 i = 0; i < 2; i++)
	{
		PINSEL_ConfigPin(&m_pin_sel[i]);
	}


	uint32_t tmp;
	NVIC_DisableIRQ(ADC_IRQn);
	// Turn on power and clock
	CLKPWR_ConfigPPWR (CLKPWR_PCONP_PCAD, ENABLE);

	LPC_ADC->ADCR = 0;
	//Enable PDN bit
	tmp = ADC_CR_PDN;
	// Set clock frequency
//	temp = 0;
	tmp |=  ADC_CR_CLKDIV(1);

	LPC_ADC->ADCR = tmp;

	LPC_ADC->ADINTEN = 0;

	ADC_IntConfig(LPC_ADC, ADC_ADINTEN5 ,ENABLE); // Резистор без усиления
	//ADC_IntConfig(LPC_ADC, ADC_ADINTEN5 ,ENABLE); // Вход
	ADC_IntConfig(LPC_ADC, ADC_ADINTEN2 ,ENABLE); // Резистор с усилением

	ADC_ChannelCmd(LPC_ADC, ADC_Res_1, ENABLE);
	ADC_ChannelCmd(LPC_ADC, ADC_Res_2, ENABLE);

	/* preemption = 1, sub-priority = 1 */
	//
	NVIC_SetPriority(ADC_IRQn, ((0x01<<3)|0x01));
	NVIC_EnableIRQ(ADC_IRQn);

	ADC_BurstCmd(LPC_ADC, ENABLE);
}


void ADC_IRQHandler(void)
{
	if (ADC_ChannelGetStatus(LPC_ADC, ADC_Res_1, ADC_DATA_DONE))
	{
		ADC_Res1 =  ADC_ChannelGetData(LPC_ADC,ADC_Res_1);
		ADC_counter1++;
		ADC_Res1_16=ADC_Res1_16+ADC_Res1;
		if (ADC_counter1>=16)
		{
			ADC_Res1_r = ADC_Res1_16/16; //усредненное за 16 отсчетов
			ADC_Res1_16=0;
			ADC_counter1=0;
		}

	}

	if (ADC_ChannelGetStatus(LPC_ADC, ADC_Res_2, ADC_DATA_DONE))
	{
		ADC_Res2 =  ADC_ChannelGetData(LPC_ADC, ADC_Res_2);
		ADC_counter2++;
		ADC_Res2_16=ADC_Res2_16+ADC_Res2;
		if (ADC_counter2>=16)
		{
			ADC_Res2_r = ADC_Res2_16/16; //усредненное за 16 отсчетов
			ADC_Res2_16=0;
			ADC_counter2=0;
		}


	}
}





