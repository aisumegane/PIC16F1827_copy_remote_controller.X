/*
 * File:   copydata.c
 * Author: 氷MEGANE
 *
 * Created on 2024/04/14, 23:38
 */

#include <xc.h>

#include "register_setup.h"
#include "grobal_macro.h"
#include "interrupt.h"
#include "senddata.h"

#include "copydata.h"

#define COPY_DEBUG_TEST  CLEAR

/* 40byteしか連続領域確保できないので分ける */  /*40byte*6=240byte*/


unsigned char copydata_led_data_ary0[COPYDATA_DATA_INDEX] = {0}; /* LED信号保存用配列 */
unsigned char copydata_led_data_ary1[COPYDATA_DATA_INDEX] = {0}; /* LED信号保存用配列 */
unsigned char copydata_led_data_ary2[COPYDATA_DATA_INDEX] = {0}; /* LED信号保存用配列 */
unsigned char copydata_led_data_ary3[COPYDATA_DATA_INDEX] = {0}; /* LED信号保存用配列 */
unsigned char copydata_led_data_ary4[COPYDATA_DATA_INDEX] = {0}; /* LED信号保存用配列 */
unsigned char copydata_led_data_ary5[COPYDATA_DATA_INDEX] = {0}; /* LED信号保存用配列 */



volatile unsigned char copydata_copy_end_flag = CLEAR;   /* コピー処理の要求的なもの */
volatile unsigned char copydata_copy_fail_flag = CLEAR;

volatile unsigned char copydata_ex_interrupt_go_flag = CLEAR;
volatile unsigned char copydata_copy_start_flag = CLEAR;
volatile unsigned char copydata_copy_start = CLEAR;
volatile unsigned char copydata_copy_interrupt_wait = CLEAR;
volatile unsigned char copydata_copy_end_req = CLEAR;


unsigned char led_data_ary_index = 0;    /* 0?4 */
static unsigned char led_data_copy_fin = CLEAR;

unsigned char led_data_index = 0;    /* 最大200 */

void copydata_main(void);
void copydata_main_pre(void);
void copydata_1byte_copy_interrupt(void);

void copydata_set_copy_end_req(void);
void copydata_clear_copy_end_flag(void);

void copydata_fill_brank_data(void);

void copydata_debug_test(void);

void copydata_main(void)
{
    unsigned char loop_index;
    
    PORTAbits.RA2 = SET;    /*Processing time measurement*/
    gf_disable_interrupt();
    
    for(loop_index = 0;loop_index < COPYDATA_DATA_INDEX;loop_index++)
    {
        copydata_led_data_ary0[loop_index] = 0;
        copydata_led_data_ary1[loop_index] = 0;
        copydata_led_data_ary2[loop_index] = 0;
        copydata_led_data_ary3[loop_index] = 0;
        copydata_led_data_ary4[loop_index] = 0;
        copydata_led_data_ary5[loop_index] = 0;
    }
    


    timer1_capture_mode_setup();
    timer1_capture_mode_edge_select(CAPTURE_RISING_EDGE);   /*just to be sure...*/
    gf_disable_timer1_interrupt();  /*if no first CCP1(capture) interrupt, do not detect overflow(end or fail)*/
    
    gf_enable_ccp1_interrupt();
    gf_disable_timer0_interrupt();

    gf_timer1_start();

    interrupt_ccp1if_defend_cmp = CLEAR;
    timer0_event_divide_cnt = 0;
    
    gf_enable_interrupt();
    
    while(1)
    {
#if 1
        if((copydata_copy_start_flag == SET) && (copydata_copy_start == CLEAR))
        {
            gf_disable_interrupt();
            TMR0 = 0x00;
            gf_enable_timer0_interrupt();
            gf_enable_interrupt();
            
            copydata_copy_start = SET;
        }
#endif
        
        if(copydata_copy_end_req == SET)
        {   /*copy end*/            
            break;
        }
    }


    
    gf_disable_interrupt();

    gf_disable_ccp1_interrupt();
    
    gf_disable_timer0_interrupt();
    gf_disable_timer1_interrupt();
    timer1_capture_mode_edge_select(CAPTURE_RISING_EDGE);
    gf_timer1_stop();

    
    //copydata_fill_brank_data(); /*reset brank data*/
    
    /*flag reset*/  /*for next sampling preparing*/
    senddata_endary_index = led_data_ary_index;
    senddata_enddata_index = led_data_index;
    
    led_data_ary_index = 0;
    led_data_index = 0;
    
    timer0_event_divide_cnt = 0;
    interrupt_ccp1if_defend_cmp = CLEAR;
    
    /*hardware status reset*/
    TMR0 = 0x00;
    TMR1 = 0x0000;
    CCPR1 = 0x0000;

    copydata_copy_start = CLEAR;
    copydata_copy_start_flag = CLEAR;

    copydata_copy_end_req = CLEAR;
    copydata_copy_end_flag = SET;       /*for sequence change*/

    gf_enable_interrupt();
    PORTAbits.RA2 = CLEAR;    /*Processing time measurement*/
}

void copydata_fill_brank_data(void)
{
    unsigned char ary_index = 0;
    unsigned char data_index = 0;
    
    unsigned char loop_data_index;
    unsigned char loop_ary_index;
    
    unsigned char* p_data_ary;
    
    ary_index = led_data_ary_index;
    data_index = led_data_index;
    
    for(loop_ary_index = ary_index;loop_ary_index < COPYDATA_ARY_INDEX;loop_ary_index++)
    {
        if(loop_ary_index == 0)
        {
            p_data_ary =  copydata_led_data_ary0;
        }
        else if(loop_ary_index ==1)
        {
            p_data_ary =  copydata_led_data_ary1;
        }
        else if(loop_ary_index ==2)
        {
            p_data_ary =  copydata_led_data_ary2;
        }
        else if(loop_ary_index ==3)
        {
            p_data_ary =  copydata_led_data_ary3;
        }
        else if(loop_ary_index == 4)
        {
            p_data_ary =  copydata_led_data_ary4;
        }
        else if(loop_ary_index == 5)
        {
            p_data_ary =  copydata_led_data_ary5;
        }

        for(loop_data_index = data_index;loop_data_index < COPYDATA_DATA_INDEX;loop_data_index++)
        {
            p_data_ary += loop_data_index;  /*set address*/
            
            *p_data_ary = 0;    /*reset data*/  /*ポインタ配列に直したほうが短い*/
        }
    }
}

void copydata_copy_pre(void)
{
    //    if((copydata_copy_end_flag == SET) || (copydata_copy_fail_flag == SET))
    if(copydata_copy_end_req == SET)
    {   /*copy end*/
        gf_disable_interrupt();
        
        gf_disable_ccp1_interrupt();
        gf_disable_timer1_interrupt();
        timer1_capture_mode_edge_select(CAPTURE_RISING_EDGE);
        gf_timer1_stop();
        
        TMR1 = 0x0000;
        CCPR1 = 0x0000;
        
        copydata_copy_start_flag = CLEAR;
        
        copydata_copy_end_req = CLEAR;
        copydata_copy_end_flag = SET;       /*for sequence change*/
        
        gf_enable_interrupt();
    }
    else if((copydata_copy_start_flag == CLEAR) &&(copydata_copy_interrupt_wait == CLEAR))
    {   /*copy start*/
        gf_disable_interrupt();
        
        timer1_capture_mode_setup();
        timer1_capture_mode_edge_select(CAPTURE_RISING_EDGE);   /*just to be sure...*/
        gf_disable_timer1_interrupt();  /*if no first CCP1(capture) interrupt, do not detect overflow(end or fail)*/
        gf_enable_ccp1_interrupt();
        
        gf_timer1_start();
        copydata_copy_interrupt_wait = SET;
        
        gf_enable_interrupt();
    }
    else if((copydata_copy_start_flag == CLEAR) && (copydata_copy_interrupt_wait == SET))
    {   /*wait first ccp1(capture) interrupt*/
        gf_disable_interrupt();
        TMR1 = 0x0000;
        gf_enable_interrupt();
    }
    else if(copydata_copy_start_flag == SET)
    {   /*now ccp1(capture) interrupt*/
        copydata_copy_interrupt_wait = CLEAR;
    }
    else
    {   /*none*/
        ;
    }
}


/*割込み処理内では割込み許可しない*/
void copydata_1byte_copy_interrupt(void)
{
#if 0
    unsigned char local_capture_edge;
    
    local_capture_edge = register_setup_timer1_capture_edge;
    
     //PORTAbits.RA2 = SET;
    
    if(copydata_copy_end_req == CLEAR)
    {
        if(copydata_copy_start_flag == CLEAR)
        {
            copydata_copy_start_flag = SET;

            led_data_ary_index = 0;
            led_data_index = 0;

            TMR1 = 0x0000;
            CCPR1= 0x0000;
        }
        else if(copydata_copy_start_flag == SET)
        {
            if(led_data_ary_index == 0)
            {
                if(led_data_index == COPYDATA_DATA_INDEX)
                {   /*set index for next save array*/
                    led_data_ary_index = 1;
                    led_data_index = 0;
                }
                else
                {
                    copydata_led_data_ary0[led_data_index] = CCPR1H;
                    led_data_index++;
                }
            }
            else if(led_data_ary_index == 1)
            {
                if(led_data_index == COPYDATA_DATA_INDEX)
                {   /*set index for next save array*/
                    led_data_ary_index = 2;
                    led_data_index = 0;
                }
                else
                {
                    copydata_led_data_ary1[led_data_index] = CCPR1H;
                    led_data_index++;
                }
            }
            else if(led_data_ary_index == 2)
            {
                if(led_data_index == COPYDATA_DATA_INDEX)
                {   /*set index for next save array*/
                    led_data_ary_index = 3;
                    led_data_index = 0;
                }
                else
                {
                    copydata_led_data_ary2[led_data_index] = CCPR1H;
                    led_data_index++;
                }
            }
            else if(led_data_ary_index == 3)
            {
                if(led_data_index == COPYDATA_DATA_INDEX)
                {   /*set index for next save array*/
                    led_data_ary_index = 4;
                    led_data_index = 0;
                }
                else
                {
                    copydata_led_data_ary3[led_data_index] = CCPR1H;
                    led_data_index++;
                }
            }
            else if(led_data_ary_index == 4)
            {
                if(led_data_index == COPYDATA_DATA_INDEX)
                {   /*set index for next save array*/
                    led_data_ary_index = 5;
                    led_data_index = 0;
                }
                else
                {
                    copydata_led_data_ary4[led_data_index] = CCPR1H;
                    led_data_index++;
                }
            }
            else if(led_data_ary_index == 5)
            {
                if(led_data_index == COPYDATA_DATA_INDEX)
                {   /*set index for next save array*/
                    led_data_ary_index = 0;
                    led_data_index = 0;
                    copydata_copy_end_req = SET;
                    gf_disable_ccp1_interrupt();
                }
                else
                {
                    copydata_led_data_ary5[led_data_index] = CCPR1H;
                    led_data_index++;
                }
            }
        }
        else
        {
            ;
        }
    
        /*for next sampling preparing*/
        if(local_capture_edge == CAPTURE_RISING_EDGE)
        {
            gf_disable_interrupt();
            timer1_capture_mode_edge_select(CAPTURE_FALLING_EDGE);
        }
        else /*if(local_capture_edge == CAPTURE_FALLING_EDGE)*/
        {
            gf_disable_interrupt();
            timer1_capture_mode_edge_select(CAPTURE_RISING_EDGE);
        }

        TMR1 = 0x0000;
        CCPR1= 0x0000;
    }
     
#endif
}

void copydata_debug_test(void)
{
    unsigned char local_capture_edge;
    
    local_capture_edge = register_setup_timer1_capture_edge;
    
    copydata_copy_start_flag = SET;
    
#if 0
            copydata_led_data_ary0[led_data_index] = CCPR1>>7;
#endif  
    
    
    /*CCPR1Hの値取得*/
    if(copydata_copy_end_req == CLEAR)
    {
        if(led_data_ary_index == 0)
        {
            //copydata_led_data_ary0[led_data_index] = CCPR1H;
            copydata_led_data_ary0[led_data_index] = (unsigned char)(CCPR1H << 4) + (unsigned char)(CCPR1L >> 4) ;
            /*copydata_led_data_ary0[led_data_index] = (unsigned char)(CCPR1 >> 4);*/   /*この記述はNG*/
        }
        else if(led_data_ary_index == 1)
        {
            //copydata_led_data_ary1[led_data_index] = CCPR1H;
            copydata_led_data_ary1[led_data_index] = (unsigned char)(CCPR1H << 4) + (unsigned char)(CCPR1L >> 4) ;
        }
        else if(led_data_ary_index == 2)
        {
            //copydata_led_data_ary2[led_data_index] = CCPR1H;
            copydata_led_data_ary2[led_data_index] = (unsigned char)(CCPR1H << 4) + (unsigned char)(CCPR1L >> 4) ;
        }
        else if(led_data_ary_index == 3)
        {
            //copydata_led_data_ary3[led_data_index] = CCPR1H;
            copydata_led_data_ary3[led_data_index] = (unsigned char)(CCPR1H << 4) + (unsigned char)(CCPR1L >> 4) ;
        }
        else if(led_data_ary_index == 4)
        {
            //copydata_led_data_ary4[led_data_index] = CCPR1H;
            copydata_led_data_ary4[led_data_index] = (unsigned char)(CCPR1H << 4) + (unsigned char)(CCPR1L >> 4) ;
        }
        else if(led_data_ary_index == 5)
        {
            //copydata_led_data_ary5[led_data_index] = CCPR1H;
            copydata_led_data_ary5[led_data_index] = (unsigned char)(CCPR1H << 4) + (unsigned char)(CCPR1L >> 4) ;
        }
    }
            
            
    /*CCPR1Hのデータ保存配列の選択*/
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
            copydata_copy_end_req = SET;    /*うまく動いてない？*/
            gf_disable_ccp1_interrupt();
        }
    }
    
    /*次のサンプリングのエッジ選択*/
    if(local_capture_edge == CAPTURE_RISING_EDGE)
    {
        gf_disable_interrupt();
        timer1_capture_mode_edge_select(CAPTURE_FALLING_EDGE);
    }
    else /*if(local_capture_edge == CAPTURE_FALLING_EDGE)*/
    {
        gf_disable_interrupt();
        timer1_capture_mode_edge_select(CAPTURE_RISING_EDGE);
    }
    
    TMR0 = 0x00;
    TMR1 = 0x0000;
    CCPR1= 0x0000;
}

void copydata_set_copy_end_req(void)
{
    copydata_copy_end_req = SET;
}

void copydata_clear_copy_end_flag(void)
{
    copydata_copy_end_flag = CLEAR;
}