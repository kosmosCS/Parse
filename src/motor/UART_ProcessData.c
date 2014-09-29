/**
 * @file	UART_ProcessData.c
 * @purpose
 * @brief
 * @version
 * @date	29-sep-2009
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
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "LPC17xx.h"
#include "lpc17xx_mcpwm.h"
#include "lpc17xx_uart.h"
#include "lpc_types.h"
#include "type.h"

#include "BLDC.h"
#include "UART_ProcessData.h"

void UART_processERROR (void)
{
	printf("Error: Invalid command\n\n");
	return;
}

void UART_processMessage (UART_RING_BUFFER_T *rb, PIDstr *pid, uint8_t loc)
{
	uint8_t 	procdata[20] = {0},
				cmd[20] = {0},
				value_s[20] = {0},
				idx=0,
				len=0,
				*pch;
	uint16_t 	value_u16 =0;
	double 		value_d	 = 0;

	/* copy data to local buffer for processing */
	while((rb->rx_tail != loc) || (rb->rx_tail == rb->rx_head))
	{
		procdata[idx] = rb->rx[rb->rx_tail];
		__BUF_INCR(rb->rx_tail);
		idx++;
	}
	
	/* set the tail equal to the head */
	__BUF_INCR(rb->rx_tail);
	
	/* Clear the '\n' in the data */
	procdata[idx-1] = 0;

	/* Handle the start command */
	if (!strcmp("start", (const char *)procdata))
	{
		pid->Brake = DISABLE;
		pid->Enable = ENABLE;
		return;
	}
	/* */
	else if (!strcmp("stop",(const char *)procdata))
	{
		pid->Brake = DISABLE;
		pid->Enable = DISABLE;
		pid->RPM = 0;
		pid->pv = 0;
		return;
	}
	/* */
	else if (!strcmp("break",(const char *)procdata))
	{
		pid->Brake = ENABLE;
		pid->Enable = DISABLE;
		pid->RPM = 0;
		pid->pv = 0;
		return;
	}

	/* look for the = in the string */
	pch = (uint8_t *)strchr((const char *)procdata, '=');

	/* Check for valid command */
	if(pch == NULL)
	{
		/* Invalid command */
		UART_processERROR();
		return;
	}

	/* Calculate the location of the searched char */
	loc = (uint8_t)(pch - procdata);

	/* Copy the command part to the command array */
	strncpy ((char *)cmd, (char *)procdata, (size_t)(loc));

	/* Get the remaining number of bytes */
	len = strlen((const char *) procdata) - loc;
	memmove ((void *)value_s,(const void *)(procdata+loc+1), (size_t)len );

	/* Check whether value is double or interger */
	pch =(uint8_t *)strchr((const char *)value_s, '.');
	if(pch == NULL)
	{
		value_u16 = atoi((const char *)value_s);
	}
	else
	{
		value_d = atof((const char *)value_s);
		//value_d = my_atof((const char *)value_s);
	}

	/* */
	if (!strcmp("speed", (const char *)cmd))
	{
		pid->sp = value_u16;
		return;
	}
	/* */
	else if (!strcmp("poles", (const char *)cmd))
	{
		pid->Poles = value_u16;
		return;
	}
	/* */
	else if (!strcmp("dir", (const char *)cmd))
	{
		if (value_u16 == CCW)
		{
			pid->Direction = CCW;
		}
		else if (value_u16 == CW)
		{
			pid->Direction = CW;
		}
		return;
	}
	else if (!strcmp("p", (const char *)cmd))
	{
		pid->p = value_d;
		return;
	}
	else if (!strcmp("i", (const char *)cmd))
	{
		pid->i = value_d;
		return;
	}
	else
	{
		UART_processERROR();
		return;
	}

}
