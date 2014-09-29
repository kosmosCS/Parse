/*
 * inter_volatile.h
 *
 *  Created on: 16.07.2012
 *      Author: Graph
 */

#ifndef INTER_VOLATILE_H_
#define INTER_VOLATILE_H_

#include "vkv_compiler.h"

extern volatile U32 Bar_zero_pozition;// 1 - center, 0 - left, 2 - right

extern volatile U16 ADC_Res1;
extern volatile U16 ADC_Input;
extern volatile U16 ADC_Res2;

extern volatile U16 ADC_counter1;
extern volatile U16 ADC_counter2;

extern volatile U32 ADC_Res1_16;
extern volatile U32 ADC_Res2_16;

extern volatile U32 ADC_Res1_r;
extern volatile U32 ADC_Res2_r;

extern volatile U8 G_sensor_send_buf[6];
extern volatile U8 G_sensor_sends;
extern volatile U8 G_sensor_work_buf[6];

extern volatile U16 Bright_level_past;
extern volatile U16 Bright_level_run;
extern volatile U16 Bright_adc_value_past;
extern volatile U16 Bright_adc_value_run;
extern volatile U16 Histerezis_Bright;
extern volatile U16 Bright_adc_value_MAX;
extern volatile U16 Bright_delta;

extern volatile U16 Input_level_past;
extern volatile U16 Input_level_run;
extern volatile U16 Input_adc_value_past;
extern volatile U16 Input_adc_value_run;
extern volatile U16 Histerezis_Input;
extern volatile U16 Input_adc_value_MAX;
extern volatile U32 Input_adc_scaler;
extern volatile U16 Input_delta;

extern volatile U8  Adc_calibration_flag;
extern volatile U32 Adc_symm_16;
extern volatile U8  Adc_symm_count;
extern volatile U8  Adc_symm_ready_flag;
extern volatile U16 Adc_calibration_arg;

extern volatile U8  Bright_switch_flag;
extern volatile U16 temp_bright;



U32 DIV_round_U32(U32, U32);




extern volatile U8  Flash_flag;

extern volatile U8  Dot_flag;
extern volatile U8  n7[2];

extern volatile U8  Save_work_struct_flag;
extern volatile U8  write_repeat;

extern volatile U8	RE_write_structs_flag;
extern volatile U8	re_write_repeat;

extern volatile U8	RE_write_cur_struct_flag;
extern volatile U8	re_write_cur_repeat;


extern volatile U32 wdt_n0, wdt_n1;

#endif /* INTER_VOLATILE_H_ */
