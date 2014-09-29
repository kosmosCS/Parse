/*
 * inter_volatile.c
 *
 *  Created on: 16.07.2012
 *      Author: Graph
 */

#include "inter_volatile.h"

volatile U32 Bar_zero_pozition=1;// 1 - center, 0 - left, 2 - right

volatile U16 ADC_Res1 = 0;
volatile U16 ADC_Input = 0;
volatile U16 ADC_Res2 = 0;

volatile U32 ADC_Res1_16=0;
volatile U32 ADC_Res2_16=0;

volatile U16 ADC_counter1=0;
volatile U16 ADC_counter2=0;


volatile U32 ADC_Res1_r=0;
volatile U32 ADC_Res2_r=0;

volatile U8 G_sensor_send_buf[6]={0,0,0,0,0,0};
volatile U8 G_sensor_sends=0;
volatile U8 G_sensor_work_buf[6]={0,0,0,0,0,0};


//яркость//переменные для работы фильтра с гистерезисом
volatile U16 Bright_level_past=0;
volatile U16 Bright_level_run=0;
volatile U16 Bright_adc_value_past=0;
volatile U16 Bright_adc_value_run=0;
volatile U16 Histerezis_Bright=4;
volatile U16 Bright_adc_value_MAX=3095;
volatile U16 Bright_delta=0;

//Uвходное//переменные для работы фильтра с гистерезисом
volatile U16 Input_level_past=0;
volatile U16 Input_level_run=0;
volatile U16 Input_adc_value_past=0;
volatile U16 Input_adc_value_run=0;
volatile U16 Histerezis_Input=20;
volatile U16 Input_adc_value_MAX=3097;

volatile U32 Input_adc_scaler=1647;// 5'000'000/3097

volatile U16 Input_delta=50;// 5000/100 50мВ на 1 диод при 5В на входе

volatile U8  Adc_calibration_flag=0;//флаг старта калибровки
volatile U32 Adc_symm_16=0;  //сумма значений АЦП за 16 отсчетов
volatile U8  Adc_symm_count=0;
volatile U8  Adc_symm_ready_flag=0;
volatile U16 Adc_calibration_arg=0;

volatile U8  Bright_switch_flag=0; //перекл. упр. 0-интерфейс, 1-резистор
volatile U16 temp_bright=0;


U32 DIV_round_U32(U32 chi, U32 zna)
{
	U32 rez=0;
	rez=chi/zna;
	if((chi - rez*zna) >  (zna/2)) rez++;
	return rez;
}





volatile U8  Flash_flag=1;



volatile U8  Dot_flag=0;
volatile U8  n7[2]={0,0};

volatile U8  Save_work_struct_flag=0;
volatile U8  write_repeat=0;

volatile U8	 RE_write_structs_flag=0;
volatile U8	 re_write_repeat=0;

volatile U8  RE_write_cur_struct_flag=0;
volatile U8	 re_write_cur_repeat=0;

volatile U32 wdt_n0=0, wdt_n1=0;


