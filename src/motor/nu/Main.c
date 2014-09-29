/**
 * @file	Main.c
 * @purpose	Application code for running on an BLDC motor on the LPC1700
 * @brief	Main file for the BLDC motor control application
 * @version	1.0
 * @date	15 July 2009
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

#include "Application.h"

/************************** APPLICATION VARIABLES *********************/

/**
 * @addtogroup APPLICATION_VARIABLES
 * @{
 */

/** SysTick Counter                  */
volatile uint32_t SysTickCnt=0;
volatile uint8_t cntr=0;


/** UART ring buffer */
UART_RING_BUFFER_T rb;

/* */
volatile uint32_t BLDC_Tick = 0;


/* */
extern volatile PIDstr PID_Motor;

/**
 * @}
 */

/************************** PRIVATE FUNCTIONS *************************/

void UART0_IRQHandler(void);
void UART0_IntReceive(void);

/**
 * 	@brief		The Systick interrupt handler
 * 	@param[in]	None
 * 	@return		None
 */
void SysTick_Handler (void) {           /* SysTick Interrupt Handler (1ms)    */
  SysTickCnt++;

  /* Increase the BLDC counter tick, used for RPM calculations */
  PID_Motor.Tick_cur++;
}

/**
 * 	@brief		
 * 	@param[in]	
 * 	@return		
 */
int sendchar (int c) 
{
	uint8_t *data,
			len = 0;
	
	rb.tx[rb.tx_head] = c;

	if (rb.tx[rb.tx_head] == '\n')
	{
		/* Point to the start of the data in the rb.tx buffer*/
		data = (uint8_t *)&rb.tx[rb.tx_tail];
		
		len = rb.tx_head - rb.tx_tail;
		
		UART_Send(LPC_UART0, data, len+1, BLOCKING);
		
		/* Reset the head and tail */ 
		__BUF_RESET (rb.tx_tail);
		__BUF_RESET (rb.tx_head);
	}
	else
	{
		__BUF_INCR(rb.tx_head);
	}
		
  	return (c);
}


#ifdef  DEBUG
/*******************************************************************************
* @brief		Reports the name of the source file and the source line number
* 				where the CHECK_PARAM error has occurred.
* @param[in]	file Pointer to the source file name
* @param[in]    line assert_param error line source number
* @return		None
*******************************************************************************/
void check_failed(uint8_t *file, uint32_t line)
{
	/* User can add his own implementation to report the file name and line number,
	 ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

	/* Infinite loop */
	while(1);
}
#endif


/**
 *  @brief		The UART0 interrupt handler
 *  @param[in] 	None
 * 	@return		None
 */
void UART0_IRQHandler (void)
{
	/* Call Standard UART 0 interrupt handler */
	UART0_StdIntHandler();

}

/**
 * @brief 		UART receive function, uses the interrupt
 * @param[in] 	None
 * @return		None
 */
void UART0_IntReceive(void)
{
	uint8_t tmpc;
	uint32_t rLen;

	while(1)
	{
		/* Call UART read function in UART driver */
		rLen = UART_Receive(LPC_UART0, &tmpc, 1, NONE_BLOCKING);
		/* If data received */
		if (rLen)
		{
			/* Check if buffer is more space
			 * If no more space, remaining character will be trimmed out
			 */
			if (!__BUF_IS_FULL(rb.rx_head,rb.rx_tail)){
				rb.rx[rb.rx_head] = tmpc;
				__BUF_INCR(rb.rx_head);
			}
		}
		/* no more data */
		else
		{
			break;
		}
	}
}


/**
 * @brief       The 1 ms periodic system function
 */
void Call_1ms(void)
{

	/* If motor is enabled, do PID calculation */
	if (PID_Motor.Enable)
	{
		if (PID_Motor.RPM == 0)
		{
			BLDC_commutate(2);
			BLDC_commutate(3);
			BLDC_commutate(1);
			BLDC_commutate(5);
			BLDC_commutate(4);
			BLDC_commutate(6);
		}

		PID_calc_cntr(&PID_Motor);
	}
	else
	{
		PID_Motor.RPM = 0;
	}

}

/**
 * @brief       The 10 ms periodic system function
 */
void Call_10ms(void)
{
	uint8_t idx;

	if (!(__BUF_IS_EMPTY(rb.rx_head, rb.rx_tail)))
	{
		/* Search for a '\n' character */
		for (idx = rb.rx_tail; idx != rb.rx_head; idx++)
		{
			/* If '\n' is found process data */
			if (rb.rx[idx] == '\n')
			{
				UART_processMessage ((UART_RING_BUFFER_T *)&rb, &PID_Motor, idx);
			}
		}

	}

}

/**
 * @brief       The 100 ms periodic system function
 */
void Call_100ms(void)
{
//   	uint32_t tmp;
	
	if (cntr == 3)
	{
		printf("RPM = %d\n", PID_Motor.RPM);
		cntr=0;
	}
	cntr++;

}

/**
 * @brief       The 1 s periodic system function
 */
void Call_1s(void)
{

}

/**
 * @brief	Initialize the application
 */
void initApplication (void)
{
    /* UART Configuration structure variable */
    UART_CFG_Type UARTConfigStruct;
    /* UART FIFO configuration Struct variable */
    UART_FIFO_CFG_Type UARTFIFOConfigStruct;

    /* Pin configuration for UART0 */
    PINSEL_CFG_Type PinCfg;

    /* SYSTICK CONFIGURATION ******************************************/
    /* Generate interrupt each 1 ms */
    SysTick_Config(SysTick_VALUE);

    /* LED CONFIGURATION **********************************************/
    /* LEDs defined as Output		*/
    FIO_SetDir(0, 1<<20, 1);

    /* UART CONFIGURATION *********************************************/
    /* Initialize UART0 pin connect */
    PinCfg.Funcnum = 1;
    PinCfg.OpenDrain = 0;
    PinCfg.Pinmode = 0;
    PinCfg.Pinnum = 2;
    PinCfg.Portnum = 0;
    PINSEL_ConfigPin(&PinCfg);
    PinCfg.Pinnum = 3;
    PINSEL_ConfigPin(&PinCfg);

    /* Initialize UART Configuration parameter structure to default state:
    * Baudrate = 115200bps
    * 8 data bit
    * 1 Stop bit
    * None parity
    */
	UARTConfigStruct.Baud_rate = 115200;
	UARTConfigStruct.Databits = UART_DATABIT_8;
	UARTConfigStruct.Parity = UART_PARITY_NONE;
	UARTConfigStruct.Stopbits = UART_STOPBIT_1;

    /* Initialize UART0 peripheral with given to corresponding parameter */
    UART_Init(LPC_UART0, &UARTConfigStruct);

    /* Initialize FIFOConfigStruct to default state:
    *        - FIFO_DMAMode = DISABLE
    *        - FIFO_Level = UART_FIFO_TRGLEV0
    *        - FIFO_ResetRxBuf = ENABLE
    *        - FIFO_ResetTxBuf = ENABLE
    *        - FIFO_State = ENABLE
    */
    UART_FIFOConfigStructInit(&UARTFIFOConfigStruct);

    /* Initialize FIFO for UART0 peripheral */
    UART_FIFOConfig(LPC_UART0, &UARTFIFOConfigStruct);

    /* Configure the UART0 interrupt, enable the RBR interrupt */
    UART_IntConfig(LPC_UART0, UART_INTCFG_RBR, ENABLE);

    /* Set the Receive callback */
	UART_SetupCbs(LPC_UART0, 0, (void *)UART0_IntReceive);

	/* Reset ring buf head and tail idx */
	__BUF_RESET(rb.rx_head);
	__BUF_RESET(rb.rx_tail);
	__BUF_RESET(rb.tx_head);
	__BUF_RESET(rb.tx_tail);

    /* preemption = 1, sub-priority = 1 */
    NVIC_SetPriority(UART0_IRQn, ((0x01<<3)|0x01));
	/* Enable Interrupt for UART0 channel */
    NVIC_EnableIRQ(UART0_IRQn);

    /* Enable UART Transmit */
    UART_TxCmd(LPC_UART0, ENABLE);

	/* Print the welcome message */
    printf("Welcome to the LPC1700 BLDC Motor Control Demo\n");
	  
    /* BLDC MOTOR CONTROL CONFIGURATION *******************************/
    BLDC_Init ();

    return;
}

/***********************************************************************//**
* @brief	Main BLDC motor control function
***********************************************************************/
int main (void) {

  DWORD cnt_1s=0, cnt_1ms=0, cnt_10ms=0, cnt_100ms=0;

  /** Initialize the clocks */
  SystemInit ();

  /* Expansion statement ----------------------------------------------------- */
  /* DeInit NVIC and SCBNVIC */
  NVIC_DeInit();
  NVIC_SCBDeInit();

  /* Configure the NVIC Preemption Priority Bits:
   * two (2) bits of preemption priority, six (6) bits of sub-priority.
   * Since the Number of Bits used for Priority Levels is five (5), so the
   * actual bit number of sub-priority is three (3)
   */
  NVIC_SetPriorityGrouping(0x05);

  //  Set Vector table offset value
  #if (__RAM_MODE__==1)
  NVIC_SetVTOR(0x10000000);
  #else
  NVIC_SetVTOR(0x00000000);
  #endif
  /* End of Expansion statement ------------------------------------------------ */

  /* Initialize the current application */
  initApplication();

  /* Enable the BLDC motor */
  BLDC_Enable();

  while (1)
  {
      if ( SysTickCnt >= cnt_1ms )      {cnt_1ms = SysTickCnt+1;      Call_1ms();}
      if ( SysTickCnt >= cnt_10ms )     {cnt_10ms = SysTickCnt+10;    Call_10ms();}
      if ( SysTickCnt >= cnt_100ms )    {cnt_100ms = SysTickCnt+100;  Call_100ms();}
      if ( SysTickCnt >= cnt_1s )       {cnt_1s = SysTickCnt+1000;    Call_1s();}
  }
}

/*********************************************************************//**
* @brief  Enters main if called
***********************************************************************/
int c_entry(void)
{
	return main();
}


