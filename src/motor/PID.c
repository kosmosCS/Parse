/**
 * @file	PID.c
 * @purpose
 * @brief
 * @version
 * @date	28-sep-2009
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

void PID_calc_cntr( PIDstr *ptr )
{
   /* double temp;
    DWORDS measurementValue;

    if (ptr->Brake == ENABLE)
    {
    	ptr->pv = 0;
    	ptr->RPM = 0;
    	ptr->Tick_new = 0;
    	ptr->Tick_old = 0;
    }

    measurementValue = ptr->pv;
    if(measurementValue==0)
    {
        measurementValue = 0x7FFFFFFF;
    }
    measurementValue = (((DWORD)SystemFrequency*10)/(measurementValue))/ptr->Poles;    // convert to RPM
    ptr->err[0] = ((DWORDS)ptr->sp - measurementValue);

    ptr->RPM = measurementValue;

    temp = ((ptr->err[0]*ptr->p)/10) + ((ptr->err[1]*ptr->i)/1000); 
    if(temp<0)
    {
        ptr->mv=0;
    }
    else
    {
        ptr->mv=(DWORD)temp;
    }

	if((ptr->mv < ptr->Period) && (ptr->mv > 0))
	{
		ptr->err[1] += ptr->err[0];//(ptr->err[0]/(ptr->RPM/6));
	}*/

}

