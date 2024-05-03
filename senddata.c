/*
 * File:   interrupt.c
 * Author: •XMEGANE
 *
 * Created on 2024/04/14, 23:38
 */

#include <xc.h>

#include "grobal_macro.h"
#include "interrupt.h"
#include "copydata.h"
#include "register_setup.h"
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
    
    gf_disable_interrupt();

    timer1_compare_mode_setup();
    gf_enable_ccp1_interrupt();
    gf_timer1_start();
    gf_timer2_start();
    
    led_data_ary_index = 0;
    led_data_index = 0;
    senddata_high_low_flag = SET;     /*data high/low flag*/

    gf_enable_interrupt();
    
    PORTAbits.RA3 = CLEAR;
    while(1)
    {                
        if(senddata_send_end_req == SET)
        {   /*copy end*/            
            break;
        }
    }
    PORTAbits.RA3 = CLEAR;

    gf_disable_interrupt();

    gf_disable_ccp1_interrupt();
    gf_timer1_stop();
    gf_timer2_stop();
    
    
    
    TMR1 = 0x0000;
    CCPR1 = 0x0000;

    senddata_high_low_flag = CLEAR;  
    senddata_send_end_req = CLEAR;
    senddata_send_end_flag = SET;       /*for sequence change*/

    gf_enable_interrupt();
}

void senddata_debug_test(void)
{
    unsigned char data_shift_buffer;
    
    if(senddata_high_low_flag == CLEAR)
    {
        senddata_high_low_flag = SET;
        PORTAbits.RA3 = SET;
    }
    else
    {
        senddata_high_low_flag = CLEAR;
        PORTAbits.RA3 = CLEAR;
    }
    
    CCPR1H = 0x00;  /*Initialize*/
    CCPR1L = 0x00;  /*Initialize*/
    
    
    /*CCPR1H‚Ö‚Ì’l‚ÌŠi”[*/
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
     
    
    TMR1 = 0x0000;
    
    
    /*CCP2 module duty setup*/
    if(senddata_high_low_flag == SET)
    {
        gf_ccp2_set_pwmduty_8bit(0x46);
    }
    else    /*if(senddata_high_low_flag == CLEAR)*/
    {
        gf_ccp2_set_pwmduty_8bit(0x00);
    }
}