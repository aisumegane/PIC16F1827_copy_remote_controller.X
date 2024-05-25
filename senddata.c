/*
 * File:   interrupt.c
 * Author: 氷MEGANE
 *
 * Created on 2024/04/14, 23:38
 */

#include <xc.h>

#include "grobal_macro.h"
#include "interrupt.h"
#include "copydata.h"
#include "register_setup.h"
#include "peripheral_out.h"

#include "senddata.h"

#define DEBUG_TEST  CLEAR

volatile unsigned char senddata_send_end_req = CLEAR;
unsigned char senddata_send_end_flag = CLEAR;

volatile unsigned char senddata_high_low_flag = CLEAR;

volatile unsigned char senddata_enddata_index = CLEAR;
volatile unsigned char senddata_endary_index = CLEAR;

void senddata_main(void);
void senddata_debug_test(void);


void senddata_main(void)
{
    rs_disable_interrupt();

    timer1_compare_mode_setup();
    gf_enable_ccp1_interrupt();
    gf_timer1_start();
    gf_timer2_start();

    led_data_ary_index = 0;
    led_data_index = 0;

    senddata_high_low_flag = SET;     /*data high/low flag*/
    peripheral_out_infrared_led_off();     /*pwm止めてもポートは0に落ちないため*/

    rs_enable_interrupt();

    while(1)
    {                
        if(senddata_send_end_req == SET)
        {   /*copy end*/            
            break;
        }
    }

    rs_disable_interrupt();

    gf_disable_ccp1_interrupt();
    gf_timer1_stop();
    gf_timer2_stop();


    TMR1 = 0x0000;
    CCPR1 = 0x0000;

    senddata_send_end_req = CLEAR;
    senddata_send_end_flag = SET;       /*for sequence change*/

    senddata_high_low_flag = SET;  
    peripheral_out_infrared_led_off();     /*pwm止めてもポートは0に落ちないため*/

    rs_enable_interrupt();
}

void senddata_debug_test(void)
{
    unsigned char data_shift_buffer;
    
    if(senddata_high_low_flag == CLEAR)
    {
        senddata_high_low_flag = SET;
    }
    else
    {
        senddata_high_low_flag = CLEAR;
    }
    
    CCPR1H = 0x00;  /*Initialize*/
    CCPR1L = 0x00;  /*Initialize*/
    
    
    /*CCPR1Hへの値の格納*/
    if(senddata_send_end_req == CLEAR)
    {
        if(led_data_ary_index == 0)
        {
#if DEBUG_TEST == SET
            CCPR1H = copydata_led_data_ary0[led_data_index];
#else
            data_shift_buffer = copydata_led_data_ary0[led_data_index];
            //CCPR1L = copydata_led_data_ary0[led_data_index];
            //CCPR1H = copydata_led_data_ary0[led_data_index] >> 4;
            CCPR1L = (unsigned char)(data_shift_buffer << 4) + (0x00);
            CCPR1H = (0x00) + (unsigned char)(data_shift_buffer >> 4);
#endif
        }
        else if(led_data_ary_index == 1)
        {
#if DEBUG_TEST == SET
            CCPR1H = copydata_led_data_ary1[led_data_index];
#else
            data_shift_buffer = copydata_led_data_ary1[led_data_index];
            //CPR1L = copydata_led_data_ary1[led_data_index];
            //CCPR1H = copydata_led_data_ary1[led_data_index] >> 4;
            CCPR1L = (unsigned char)(data_shift_buffer << 4) + (0x00);
            CCPR1H = (0x00) + (unsigned char)(data_shift_buffer >> 4);
#endif
        }
        else if(led_data_ary_index == 2)
        {
#if DEBUG_TEST == SET
            CCPR1H = copydata_led_data_ary2[led_data_index];
#else
            data_shift_buffer = copydata_led_data_ary2[led_data_index];
            //CCPR1L = copydata_led_data_ary2[led_data_index];
            //CCPR1H = copydata_led_data_ary2[led_data_index] >> 4;
            CCPR1L = (unsigned char)(data_shift_buffer << 4) + (0x00);
            CCPR1H = (0x00) + (unsigned char)(data_shift_buffer >> 4);
#endif
        }
        else if(led_data_ary_index == 3)
        {
#if DEBUG_TEST == SET
            CCPR1H = copydata_led_data_ary3[led_data_index];
#else
            data_shift_buffer = copydata_led_data_ary3[led_data_index];
            //CCPR1L = copydata_led_data_ary3[led_data_index];
            //CCPR1H = copydata_led_data_ary3[led_data_index] >> 4;
            CCPR1L = (unsigned char)(data_shift_buffer << 4) + (0x00);
            CCPR1H = (0x00) + (unsigned char)(data_shift_buffer >> 4);
#endif
        }
        else if(led_data_ary_index == 4)
        {
#if DEBUG_TEST == SET
            CCPR1H = copydata_led_data_ary4[led_data_index];
#else
            data_shift_buffer = copydata_led_data_ary4[led_data_index];
            //CCPR1L = copydata_led_data_ary4[led_data_index];
            //CCPR1H = copydata_led_data_ary4[led_data_index] >> 4;
            CCPR1L = (unsigned char)(data_shift_buffer << 4) + (0x00);
            CCPR1H = (0x00) + (unsigned char)(data_shift_buffer >> 4);
#endif
        }
        else if(led_data_ary_index == 5)
        {
#if DEBUG_TEST == SET
            CCPR1H = copydata_led_data_ary5[led_data_index];
#else
            data_shift_buffer = copydata_led_data_ary5[led_data_index];
            //CCPR1L = copydata_led_data_ary5[led_data_index];
            //CCPR1H = copydata_led_data_ary5[led_data_index] >> 4;
            CCPR1L = (unsigned char)(data_shift_buffer << 4) + (0x00);
            CCPR1H = (0x00) + (unsigned char)(data_shift_buffer >> 4);
#endif
        }
    }
    

    /*data index shift*/
    if(led_data_ary_index == 0)
    {
        led_data_index++;
        
        if(led_data_index == COPYDATA_DATA_INDEX)
        {   /*set index for next save array*/
            led_data_ary_index = 1;
            led_data_index = 0;
        }
    }
    else if(led_data_ary_index == 1)
    {    
        led_data_index++;
        
        if(led_data_index == COPYDATA_DATA_INDEX)
        {   /*set index for next save array*/
            led_data_ary_index = 2;
            led_data_index = 0;
        }
    }
    else if(led_data_ary_index == 2)
    {    
        led_data_index++;
        
        if(led_data_index == COPYDATA_DATA_INDEX)
        {   /*set index for next save array*/
            led_data_ary_index = 3;
            led_data_index = 0;
        }
    }
    else if(led_data_ary_index == 3)
    {
        led_data_index++;
        
        if(led_data_index == COPYDATA_DATA_INDEX)
        {   /*set index for next save array*/
            led_data_ary_index = 4;
            led_data_index = 0;
        }
    }
    else if(led_data_ary_index == 4)
    {
        led_data_index++;
        
        if(led_data_index == COPYDATA_DATA_INDEX)
        {   /*set index for next save array*/
            led_data_ary_index = 5;
            led_data_index = 0;
        }
    }
    else if(led_data_ary_index == 5)
    {
        led_data_index++;
        
        if(led_data_index == COPYDATA_DATA_INDEX)
        {   /*set index for next save array*/
            led_data_ary_index = 0;
            led_data_index = 0;
            senddata_send_end_req = SET;
            gf_disable_ccp1_interrupt();
        }
    }
    
#if 0
    /*これも動作微妙...*/
    if(data_shift_buffer >= 0xFF)       /*次出力するデータが0xFF(最後)なら、終わる*/
    {
        senddata_send_end_req = SET;
    }
#endif
    
#if 0
    /*動作不安定、たまに途中で送信とまる*/
    if((senddata_endary_index == led_data_ary_index) && (senddata_enddata_index == led_data_index))
    {
        senddata_send_end_req = SET;
    }
#endif
    
    TMR1 = 0x0000;
    
    
    /*CCP2 module duty setup*/
    if(senddata_high_low_flag == SET)
    {
        rs_ccp2_set_pwmduty_8bit(0x46);
    }
    else    /*if(senddata_high_low_flag == CLEAR)*/
    {
        rs_ccp2_set_pwmduty_8bit(0x00);
    }
}