/**
 * @file	BLDC.c
 * @purpose	Initializing and controlling BLDC motor
 * @brief	BLDC motor control initialization and controlling file
 * @version	1.0
 * @date	29 July 2009
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

volatile uint8_t test=0;

/************************** PRIVATE VARIABLES *************************/
/**
 * @addtogroup PRIVATE_VARIABLES
 * @{
 */

/* Pin configuration data */
/** Motor Control Channel 0 Output A */
const PINSEL_CFG_Type mcpwm_mco0a_pin[1] = {{1, 19, 1, 0, 0}};
/** Motor Control Channel 0 Output B */
const PINSEL_CFG_Type mcpwm_mco0b_pin[1] = {{1, 22, 1, 0, 0}};

/** Motor Control Channel 1 Output A */
const PINSEL_CFG_Type mcpwm_mco1a_pin[1] = {{1, 25, 1, 0, 0}};
/** Motor Control Channel 1 Output B */
const PINSEL_CFG_Type mcpwm_mco1b_pin[1] = {{1, 26, 1, 0, 0}};

/** Motor Control Channel 2 Output A */
const PINSEL_CFG_Type mcpwm_mco2a_pin[1] = {{1, 28, 1, 0, 0}};
/** Motor Control Channel 2 Output B */
const PINSEL_CFG_Type mcpwm_mco2b_pin[1] = {{1, 29, 1, 0, 0}};

#if (USE_HALL == 1)
/** Motor Control Feed Back Channel 0 - MCI0 */
const PINSEL_CFG_Type mcpwm_mcfb0_pin[1] = {{1, 20, 1, 0, 0}};
/** Motor Control Feed Back Channel 1 - MCI1 */
const PINSEL_CFG_Type mcpwm_mcfb1_pin[1] = {{1, 23, 1, 0, 0}};
/** Motor Control Feed Back Channel 2 - MCI2 */
const PINSEL_CFG_Type mcpwm_mcfb2_pin[1] = {{1, 24, 1, 0, 0}};
#endif

/** Motor Control Low-active abort feed back */
const PINSEL_CFG_Type mcpwm_mcabort_pin[1] = {{ 1, 21, 1, 0, 0}};

/** @brief MCPWM Channel configuration data */
MCPWM_CHANNEL_CFG_Type channelsetup[3];

/** @brief Capture configuration data */
MCPWM_CAPTURE_CFG_Type captureCfg;

/**
 * @}
 */

/**
 * @defgroup MOTOR_VARIABLES
 * @{
 */

/** Motor controlling structure */
volatile PIDstr PID_Motor
= {
/* PID_Motor.p           */ 4,
/* PID_Motor.i           */ 10,
/* PID_Motor.d           */ 0,
/* PID_Motor.sp          */ 1500,
/* PID_Motor.pv          */ 0,
/* PID_Motor.err[3]      */ {0,0,0},
/* PID_Motor.mv          */ 0,
/* PID_Motor.HALstate    */ 0,
/* PID_Motor.CMT_CNT     */ 0,
/* PID_Motor.CMT_step    */ 0,
/* PID_Motor.RPM         */ 0,
/* PID_Motor.Enable      */ DISABLE,
/* PID_Motor.Direction   */ CW,
/* PID_Motor.Brake       */ ENABLE,
/* PID_Motor.Period      */ 25000,
/* PID_Motor.Poles       */ 1,
/* PID_Motor.Tick_cur    */ 0,
/* PID_Motor.Tick_old    */ 0,
/* PID_Motor.Tick_new    */ 0
};

/** Commutation table, connected to hall sensor */
volatile uint8_t CMT_tbl[2][8] = {	{0xF, 2, 0, 1, 4, 3, 5, 0xF},
									{0xF, 5, 3, 4, 1, 0, 2, 0xF} };

/** Lookup table for QEI compare interrupts */
volatile uint32_t QEI_POS_tbl [2][6] = 	{
											/* Table for running CW */ 
											{
												
												
												1000+QEI_offset,
												1333+QEI_offset,
												1667+QEI_offset,
												0+QEI_offset,
												333+QEI_offset,
												667+QEI_offset,
												
														
											},
											/* Table for running CCW */ 
											{
												333+QEI_offset,
												0+QEI_offset,
												1667+QEI_offset,
												1333+QEI_offset,
												1000+QEI_offset,	
												667+QEI_offset,
											}
										};

/** Old systick value for RPM calculation */
volatile uint32_t bldc_SySTickold = 0;
/** New systick value for RPM calculation */
volatile uint32_t bldc_SySTicknew = 0;

/**
 * @}
 */

/**
 * @brief	Initialize the system to control the Brushless DC motor
 * @par		Pin settings:
 *			- P1.19 = MC0A
 *			- P1.22 = MC0B
 *			- P1.25 = MC1A
 *			- P1.26 = MC1B
 *			- P1.28	= MC2A
 *			- P1.29 = MC2B
 *			- P1.20 = MCFB0
 *			- P1.23 = MCFB1
 *			- P1.24 = MCFB2
 *			- P1.21 = MCABORT
 * @par	Channel settings:
 *			- Type = edge mode
 *			- Polarity = Low passive
 *			- Deadtime = disabled
 *			- Deadtime value = 0
 */
void BLDC_Init (void)
{
#if (USE_QEI == 1)
	/* QEI configuration structure variable */
	QEI_CFG_Type QEIConfigStruct;
#endif

    /* MCPWM INITIALIZATION *******************************************/
    /* Initializes pin corresponding to MCPWM function */
    PINSEL_ConfigPin((PINSEL_CFG_Type *)&mcpwm_mco0a_pin[0]);
    PINSEL_ConfigPin((PINSEL_CFG_Type *)&mcpwm_mco0b_pin[0]);
    PINSEL_ConfigPin((PINSEL_CFG_Type *)&mcpwm_mco1a_pin[0]);
    PINSEL_ConfigPin((PINSEL_CFG_Type *)&mcpwm_mco1b_pin[0]);
    PINSEL_ConfigPin((PINSEL_CFG_Type *)&mcpwm_mco2a_pin[0]);
    PINSEL_ConfigPin((PINSEL_CFG_Type *)&mcpwm_mco2b_pin[0]);
	PINSEL_ConfigPin((PINSEL_CFG_Type *)&mcpwm_mcabort_pin[0]);

#if (USE_HALL == 1)
#if (USE_MCPWM_FEEDBACK == 1)
	/* Configure the MCPWM Feedback pins to be CAP inputs */
    PINSEL_ConfigPin((PINSEL_CFG_Type *)&mcpwm_mcfb0_pin[0]);
    PINSEL_ConfigPin((PINSEL_CFG_Type *)&mcpwm_mcfb1_pin[0]);
    PINSEL_ConfigPin((PINSEL_CFG_Type *)&mcpwm_mcfb2_pin[0]);
    /* Disable interrupt for MCPWM */
	NVIC_SetPriority(MCPWM_IRQn, ((0x01<<3)|0x01));
	NVIC_DisableIRQ(MCPWM_IRQn);
#else
	/* Disable interrupt for MCPWM */
	/* preemption = 1, sub-priority = 1 */
	NVIC_DisableIRQ(EINT3_IRQn);
	NVIC_SetPriority(EINT3_IRQn, ((0x01<<3)|0x01));

	/*-- HALL input settings using GPIO interrupts --*/
    /* Make sure P0.16..0.18 are GPIO */
    PINCON->PINSEL1 &= ~(0x3F<<0);
    /* Set P0.16..0.18 as input */
    GPIO0->FIODIR &= ~(0x7<<16);

    /* Clear all pending interrupt */
    GPIOINT->IO0IntClr = (0xFFFFFFFF);
    /* Enable the rising edge interrupts on GPIO */
    GPIOINT->IO0IntEnR |= (0x7<<16);
    /* Enable the falling edge interrupts on GPIO */
    GPIOINT->IO0IntEnF |= (0x7<<16);
#endif
#endif

    /* Init MCPWM peripheral */

    MCPWM_Init(LPC_MCPWM);

    /* MCPWM Channel 0 setup */
    channelsetup[0].channelType = MCPWM_CHANNEL_EDGE_MODE;
    channelsetup[0].channelPolarity = MCPWM_CHANNEL_PASSIVE_HI;
    channelsetup[0].channelDeadtimeEnable = ENABLE;
    channelsetup[0].channelDeadtimeValue = 1250;
    channelsetup[0].channelUpdateEnable = ENABLE;
    channelsetup[0].channelTimercounterValue = 0;
    channelsetup[0].channelPeriodValue = PID_Motor.Period;
    channelsetup[0].channelPulsewidthValue = 0;

    /* MCPWM Channel 1 setup */
    channelsetup[1].channelType = MCPWM_CHANNEL_EDGE_MODE;
    channelsetup[1].channelPolarity = MCPWM_CHANNEL_PASSIVE_HI;
    channelsetup[1].channelDeadtimeEnable = ENABLE;
    channelsetup[1].channelDeadtimeValue = 1250;
    channelsetup[1].channelUpdateEnable = ENABLE;
    channelsetup[1].channelTimercounterValue = 0;
    channelsetup[1].channelPeriodValue = PID_Motor.Period;
    channelsetup[1].channelPulsewidthValue = 0;

    /* MCPWM Channel 2 setup */
    channelsetup[2].channelType = MCPWM_CHANNEL_CENTER_MODE;
    channelsetup[2].channelPolarity = MCPWM_CHANNEL_PASSIVE_HI;
    channelsetup[2].channelDeadtimeEnable = ENABLE;
    channelsetup[2].channelDeadtimeValue = 1250;
    channelsetup[2].channelUpdateEnable = ENABLE;
    channelsetup[2].channelTimercounterValue = 0;
    channelsetup[2].channelPeriodValue = PID_Motor.Period;
    channelsetup[2].channelPulsewidthValue = 0;

    /* Apply the setup to the channels */
    MCPWM_ConfigChannel(LPC_MCPWM, 0, &channelsetup[0]);
    MCPWM_ConfigChannel(LPC_MCPWM, 1, &channelsetup[1]);
    MCPWM_ConfigChannel(LPC_MCPWM, 2, &channelsetup[2]);

#if (USE_HALL == 1) && (USE_MCPWM_FEEDBACK == 1)
    captureCfg.captureChannel = 0;
    captureCfg.captureFalling = ENABLE;
    captureCfg.captureRising = ENABLE;
    captureCfg.hnfEnable = DISABLE;
    captureCfg.timerReset = DISABLE;
    MCPWM_ConfigCapture(MCPWM, 0, &captureCfg);

    captureCfg.captureChannel = 1;
    captureCfg.captureFalling = ENABLE;
    captureCfg.captureRising = ENABLE;
    captureCfg.hnfEnable = DISABLE;
    captureCfg.timerReset = DISABLE;
    MCPWM_ConfigCapture(MCPWM, 1, &captureCfg);

    captureCfg.captureChannel = 2;
    captureCfg.captureFalling = ENABLE;
    captureCfg.captureRising = ENABLE;
    captureCfg.hnfEnable = DISABLE;
    captureCfg.timerReset = DISABLE;
    MCPWM_ConfigCapture(MCPWM, 2, &captureCfg);
#endif


#if (USE_QEI == 1)
	/* QEI CONFIGURATION **********************************************/
	/* Configure the QEI */
	QEIConfigStruct.CaptureMode = QEI_CAPMODE_4X;
	QEIConfigStruct.DirectionInvert = QEI_DIRINV_NONE;
	QEIConfigStruct.InvertIndex = QEI_INVINX_NONE;
	QEIConfigStruct.SignalMode = QEI_SIGNALMODE_QUAD;
	/* Initialize the QEI */
	QEI_Init(LPC_QEI, &QEIConfigStruct);
	/* Set the QEI Filter clock counter*/
	QEI_SetDigiFilter(LPC_QEI, 20);
	/* Set the max QEI position */
	QEI_SetMaxPosition(LPC_QEI, 2000);
	/* Set the first position compare value */
	QEI_SetPositionComp(LPC_QEI,QEI_COMPPOS_CH_0, 333+QEI_offset);

	LPC_QEI->QEICON = (1<<1);

	/* Enable interrupt for velocity Timer overflow for capture velocity into Acc */
	QEI_IntCmd(LPC_QEI, QEI_INTFLAG_POS0_Int, ENABLE);
	/* preemption = 1, sub-priority = 1 */
	NVIC_SetPriority(QEI_IRQn, ((0x01<<3)|0x01));
	/* Enable interrupt for QEI  */
	NVIC_EnableIRQ(QEI_IRQn);

#endif // END OF USE_QEI


}

/**
 *	@brief
 * 			This function enables the BLDC motor control by setting the MCPWM in the
 *			right mode, enable the capturing and starting the MCPWM
 */
void BLDC_Enable (void)
{

    /* Enable the MCPWM DC mode and all output pins */
    MCPWM_ACMode(LPC_MCPWM, ENABLE);

    /* Set the PWM output value */
    //channelsetup[0].channelPulsewidthValue = 100;

    /* Write to the shadow register */
    MCPWM_WriteToShadow(LPC_MCPWM, 0, &channelsetup[0]);

    /* Disable LIM0 and MAT0 interrupt flag */
    MCPWM_IntConfig(LPC_MCPWM, MCPWM_INTFLAG_LIM0, DISABLE);
    MCPWM_IntConfig(LPC_MCPWM, MCPWM_INTFLAG_MAT0, DISABLE);

    /* Disable LIM1 and MAT1 interrupt flag */
    MCPWM_IntConfig(LPC_MCPWM, MCPWM_INTFLAG_LIM1, DISABLE);
    MCPWM_IntConfig(LPC_MCPWM, MCPWM_INTFLAG_MAT1, DISABLE);

    /* Disable LIM2 and MAT2 interrupt flag */
    MCPWM_IntConfig(LPC_MCPWM, MCPWM_INTFLAG_LIM2, DISABLE);
    MCPWM_IntConfig(LPC_MCPWM, MCPWM_INTFLAG_MAT2, DISABLE);

#if (USE_HALL == 1)
#if (USE_MCPWM_FEEDBACK == 1)
    /* Enable interrupt for capture event on MCI0 (MCFB0)  */
    MCPWM_IntConfig(LPC_MCPWM, MCPWM_INTFLAG_CAP0, ENABLE);

    /* Enable interrupt for capture event on MCI0 (MCFB1) */
    MCPWM_IntConfig(LPC_MCPWM, MCPWM_INTFLAG_CAP1, ENABLE);

    /* Enable interrupt for capture event on MCI0 (MCFB2) */
    MCPWM_IntConfig(LPC_MCPWM, MCPWM_INTFLAG_CAP2, ENABLE);
	
	/* Enable interrupt for MCPWM  */
    NVIC_EnableIRQ(MCPWM_IRQn);
#else
	NVIC_EnableIRQ(EINT3_IRQn);
#endif
#endif

    /* Start up the MCPWM */
    MCPWM_Start(LPC_MCPWM, ENABLE, ENABLE, ENABLE);

    /* Disable the brake */
    PID_Motor.Brake = DISABLE;

    /* Enable the motor */
    PID_Motor.Enable = ENABLE;
}

void BLDC_setDuty (S32 duty)
{

	U32 duty1=0;
	U32 duty2=0;

	if (duty>0)
	{
		duty1=duty+2000;
		if (duty1>=23750)  duty1=23750;
		duty2=0;
	}

	if (duty<0)
	{
		duty1=0;
		duty2=-duty;
		duty2=duty2+2000;
		if (duty2>=23750) duty2=23750;

	}

	if (duty==0)
	{
		duty1=0;
		duty2=0;
	}
		/* Set the PWM output value */
	channelsetup[0].channelPulsewidthValue = duty1;
    /* Write to the shadow register */
	MCPWM_WriteToShadow(LPC_MCPWM, 0, &channelsetup[0]);

	/* Set the PWM output value */
	channelsetup[1].channelPulsewidthValue = duty2;
    /* Write to the shadow register */
	MCPWM_WriteToShadow(LPC_MCPWM, 1, &channelsetup[1]);

	/* Set the PWM output value */
	channelsetup[2].channelPulsewidthValue = duty;
    /* Write to the shadow register */
	MCPWM_WriteToShadow(LPC_MCPWM, 2, &channelsetup[2]);
}

void BLDC_calcRPM (PIDstr *ptr)
{

//	ptr->Tick_new = SysTick->VAL;
//
//	/* RPM calculation */
//	if (ptr->Tick_cur)
//	{
//		ptr->pv = (((DWORD)ptr->Tick_cur-1)*(DWORD)SysTick_VALUE)+
//						((DWORD)SysTick_VALUE-(DWORD)ptr->Tick_new +
//						((DWORD)ptr->Tick_old));
//	}
//	else
//	{
//		if ((DWORD)bldc_SySTickold > (DWORD)ptr->Tick_new)
//			ptr->pv = (DWORD)ptr->Tick_old-(DWORD)ptr->Tick_new;
//
//	}
//	/* Clear the 1 ms counter */
//	ptr->Tick_cur = 0;
//	ptr->Tick_old = ptr->Tick_new;

}

void BLDC_break (void)
{

	/* Set the PWM output value */
	channelsetup[0].channelPulsewidthValue = 900;
    /* Write to the shadow register */
	MCPWM_WriteToShadow(LPC_MCPWM, 0, &channelsetup[0]);

	/* Set the PWM output value */
	channelsetup[1].channelPulsewidthValue = 900;
    /* Write to the shadow register */
	MCPWM_WriteToShadow(LPC_MCPWM, 0, &channelsetup[1]);

	/* Set the PWM output value */
	channelsetup[2].channelPulsewidthValue = 900;
    /* Write to the shadow register */
	MCPWM_WriteToShadow(LPC_MCPWM, 0, &channelsetup[2]);
}

void BLDC_commutate (unsigned char step)
{
    unsigned char pattern = 0;

    /* Check whether there is a setpoint and an enable */
    if((PID_Motor.sp == 0) || (PID_Motor.Enable == DISABLE))
    {
        /* If no setpoint and brake is enable BREAK! */
        if(PID_Motor.Brake == ENABLE)
        {
            step = 'B';
        }
        /* Else let it run free */
        else
        {
            step = 'C';
        }
    }

//	PID_Motor.mv = PID_Motor.Period/2;

    if(PID_Motor.mv > PID_Motor.Period)
    {
    	BLDC_setDuty(PID_Motor.Period * 0.01);
    }
    else
    {
    	BLDC_setDuty(PID_Motor.Period-PID_Motor.mv);
    }

    switch (step)
	{
		case 0:
			pattern = MCPWM_PATENT_A0 | MCPWM_PATENT_B1;
			break;
		case 1:
			pattern = MCPWM_PATENT_B1 | MCPWM_PATENT_A2;
			break;
		case 2:
			pattern = MCPWM_PATENT_B0 | MCPWM_PATENT_A2;
			break;
		case 3:
			pattern = MCPWM_PATENT_B0 | MCPWM_PATENT_A1;
			break;
		case 4:
			pattern = MCPWM_PATENT_A1 | MCPWM_PATENT_B2;
			break;
		case 5:
			pattern = MCPWM_PATENT_A0 | MCPWM_PATENT_B2;
			break;
		case 'B':
			BLDC_setDuty(0);
			pattern = MCPWM_PATENT_B0 | MCPWM_PATENT_B1 | MCPWM_PATENT_B2;
			break;
		case 'C':
			pattern = 0;
			break;
		default:
			pattern = 0;
			break;
	}


    LPC_MCPWM->MCCCP = pattern;

}

#if (USE_HALL == 1)
#if (USE_MCPWM_FEEDBACK == 1)
/**
 *  @brief  This function handles the MCPWM feedback interrupts.
 *          The HALL sensor is connected to the MCPWM feedback inputs
 *          and will cause a pattern which determines the next commutation pattern.
 */
void MCPWM_IRQHandler (void)
{
	unsigned char pinstat = 0;
	
	/* Clear pending interrupt */
	MCPWM_IntClear(MCPWM, MCPWM_INTFLAG_CAP0);
	MCPWM_IntClear(MCPWM, MCPWM_INTFLAG_CAP1);
	MCPWM_IntClear(MCPWM, MCPWM_INTFLAG_CAP2);
	
	/* Calculate the RPM */
	BLDC_calcRPM (&PID_Motor);
	
	/* Read the current HALL sensor input */
	pinstat = (GPIO_ReadValue(1) & 1<<20) >> 20;
	pinstat |= (GPIO_ReadValue(1) & 3<<23) >> 22;
	PID_Motor.CMT_step = pinstat;
	
	/* Commutate the motor */
	BLDC_commutate(pinstat);
	
	return;
}

#else
/**
 *  @brief
 *          This function handles the GPIO P0.16..18 interrupts.
 *          The HALL sensor is connected to the P0.16..18 inputs
 *          and will cause a pattern which determines the next commutation pattern.
 */
void EINT3_IRQHandler (void)
{
    /* Clear all pending interrupt */
    GPIOINT->IO0IntClr = (0x7<<16);

	/* Calculate the RPM */
	BLDC_calcRPM (&PID_Motor);
	
	/* Read the current hall value 1..6 */
    PID_Motor.HALstate = (GPIO_ReadValue(0) & 0x7<<16) >> 16;

    /* Fetch the commutation step from Commutation table */
    PID_Motor.CMT_step = CMT_tbl[PID_Motor.Direction][PID_Motor.HALstate];

    /* Commutate the motor */
    BLDC_commutate(PID_Motor.CMT_step);

	printf(	"HALL_state = %d\t"
			"CMT_step = %d\n", PID_Motor.HALstate, PID_Motor.CMT_step);

    return;
}
#endif
#elif (USE_QEI == 1)
/**
 *  @brief
 */
void QEI_IRQHandler (void)
{
	uint32_t tmp;
	/* Get the current position */
	uint32_t pos = LPC_QEI->QEIPOS;
	/* Clear the position interrupt */
	QEI_IntClear(LPC_QEI,QEI_INTFLAG_POS0_Int);
	
	/* Calculate the RPM using Systick */
    BLDC_calcRPM (&PID_Motor);

	
	/* */
	if (PID_Motor.Direction == CW)
	{
		QEI_SetPositionComp(LPC_QEI,QEI_COMPPOS_CH_0, QEI_POS_tbl[CCW][PID_Motor.CMT_step]);
		/* Commutate the motor */
    	BLDC_commutate((PID_Motor.CMT_step+4)%6);
		
		tmp = PID_Motor.CMT_step+1;
		tmp--;
		if (tmp==0)
			tmp=6;		

		PID_Motor.CMT_step = tmp-1;

	}
	else
	{
		QEI_SetPositionComp(LPC_QEI,QEI_COMPPOS_CH_0, QEI_POS_tbl[CCW][PID_Motor.CMT_step]);

		/* Commutate the motor */
    	BLDC_commutate(PID_Motor.CMT_step);
		PID_Motor.CMT_step++;
		PID_Motor.CMT_step = PID_Motor.CMT_step%6;

	}
	/* Re-enable QEI index interrupt */
	LPC_QEI->QEICON = (1<<1);
    return;
}
#endif

