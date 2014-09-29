#ifndef BLDC_H_
#define BLDC_H_

/**
 * @addtogroup MOTOR_MACROS
 * @{
 */

/* Choose between HALL or QEI */
#define USE_HALL 0

#if (USE_HALL == 0)
	#define USE_QEI 0
#endif

/* Choose between MCPWM or GPIO interrupt feedback */
#define USE_MCPWM_FEEDBACK 0

#define CW  0   // clockwise
#define CCW 1   // counter clockwise

#define QEI_steps	2000
#define QEI_offset  24*4

#include "LPC17xx.h"
//#define MCPWM LPC_MCPWM;

/**
 * @}
 */

/**
 * @addtogroup MOTOR_VARIABLES
 * @{
 */

typedef volatile struct _PIDstr {
  double    p;          /**< The PID Product-value */
  double    i;          /**< The PID Integrating-value */
  double    d;          /**< The PID Diff-value, not used but for future purpose */
  DWORD     sp;         /**< Setpoint in mechanical RPM */
  DWORD     pv;         /**< Process value, dt in sys counts, used for RPM calculations */
  double    err[3];     /**< Error in PI(D) calculations */
  DWORD     mv;         /**< Manipulated value, output of the PI(D) controller */
  BYTE      HALstate;   /**< Current HAL state              */
  DWORD     CMT_CNT;    /**< Commutation counter, check whether motor is commutated */
  DWORD     CMT_step;   /**< Current commutation step */
  DWORD     RPM;        /**< Current motor mechanical RPM */
  BYTE      Enable;     /**< Motor enable, 0 = DISABLE, 1 = ENABLE  */
  BYTE      Direction;  /**< Motor rotation direction, Clockwise = CW or Counter Clockwise = CCW */
  BYTE      Brake;      /**< Motor electrical break enable, 0 = DISABLE, 1 = ENABLE */
  DWORD     Period;     /**< Switching frequency of the phase drives */
  BYTE      Poles;      /**< Number of Pole-pairs in the motor */
  DWORD		Tick_cur;	/**< Current tick value, for RPM calculations */
  DWORD		Tick_old;	/**< Current tick value, for RPM calculations */
  DWORD		Tick_new;	/**< Current tick value, for RPM calculations */
} PIDstr;


/**
 * @}
 */



/**
 * @brief Initialize the system to control the BLDC motor
 */
void BLDC_Init (void);

/**
 * @brief Enable the the BLDC motor control
 */
void BLDC_Enable (void);

/**
 * @brief Calculate the ...
 */
void BLDC_calcRPM (PIDstr *pid_s);

/**
 * @brief give the motor the commutation step
 */
void BLDC_commutate (unsigned char step);

void BLDC_setDuty (S32);

#endif /* BLDC_H_ */

