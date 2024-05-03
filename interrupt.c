/*
 * File:   interrupt.c
 * Author: �XMEGANE
 *
 * Created on 2024/03/27, 23:38
 */

#include <xc.h>

#include "register_setup.h"
#include "grobal_macro.h"
#include "interrupt.h"
#include "copydata.h"
#include "senddata.h"
#include "main.h"


/*first interrupt defender*/
#define FIRST_TMR1IF_DEFENDER   CLEAR
#define FIRST_CCP1IF_DEFENDER   SET
#define FIRST_CCP2IF_DEFENDER   SET


unsigned int cnt = 0;
unsigned char interrupt__maintask_go = CLEAR;

static void __interrupt() isr(void);
static void interrupt_timer0_tmr0if(void);
static void interrupt_timer1_tmr1if(void);
static void interrupt_timer1_ccp1if(void);
static void interrupt_timer2_tmr2if(void);
static void interrupt_inte(void);
static void first_interruptdefend(void);


static unsigned char tmr1if_defend_cmp = CLEAR;
unsigned char interrupt_ccp1if_defend_cmp = CLEAR;
unsigned char interrupt_ccp2if_defend_cmp = CLEAR;

static void first_interruptdefend(void)
{   /*if you want to defend first interrupt(ex.power on), please write this*/
    
    if(FIRST_TMR1IF_DEFENDER == SET)
    {
        if((PIR1bits.TMR1IF == SET)&&(tmr1if_defend_cmp == CLEAR))
        {
            {
                PIR1bits.TMR1IF = CLEAR;
                tmr1if_defend_cmp = SET;
            }
        }
    }
    
    if(FIRST_CCP1IF_DEFENDER == SET)
    {
        if((PIR1bits.CCP1IF == SET)&&(interrupt_ccp1if_defend_cmp == CLEAR))
        {
            {
                PIR1bits.CCP1IF = CLEAR;
                interrupt_ccp1if_defend_cmp = SET;
            }
        }
    }
    
    if(FIRST_CCP2IF_DEFENDER == SET)
    {
        if((PIR2bits.CCP2IF == SET)&&(interrupt_ccp2if_defend_cmp == CLEAR))
        {
            {
                PIR2bits.CCP2IF = CLEAR;
                interrupt_ccp2if_defend_cmp = SET;
            }
        }
    }
}


static void __interrupt() isr(void)
{    
    /*�����݂��ꂽ���_�ő��̊����݋֎~*/
    INTCONbits.GIE = CLEAR;         // �O���[�o�����荞�݋֎~
    INTCONbits.PEIE = CLEAR;        // �y���t�F���������݋֎~
    
    /*first interrupt defender*/
    first_interruptdefend();
    
    /*Even if only hardware(ex.CCP) functions are used, the xxIF flag must be cleared in software*/
    
    if((INTCONbits.TMR0IF == SET) && (INTCONbits.TMR0IE == SET))
    {
        interrupt_timer0_tmr0if();
        INTCONbits.TMR0IF = CLEAR;
    }
    if((PIR1bits.TMR1IF == SET) && ( PIE1bits.TMR1IE == SET))      /*timer1 overflow interrupt*/    //if((PIR1bits.TMR1IF == SET))      /*timer1 overflow interrupt*/
    {
        interrupt_timer1_tmr1if();
        PIR1bits.TMR1IF = CLEAR;
    }
    if((PIR1bits.CCP1IF == SET) && (PIE1bits.CCP1IE == SET))
    {
        interrupt_timer1_ccp1if();
        PIR1bits.CCP1IF = CLEAR;
    }
    if(INTCONbits.INTF == SET)
    {
        //interrupt_inte();
        INTCONbits.INTF = CLEAR;
    }
    if(PIR1bits.TMR2IF == SET)
    {
        //interrupt_timer2_tmr2if();
        PIR1bits.TMR2IF = CLEAR;
    }
    if((PIR2bits.CCP2IF == SET) && (PIE2bits.CCP2IE == SET))
    {       
       PIR2bits.CCP2IF = CLEAR;
    }
    
    INTCONbits.GIE = SET;         // �O���[�o�����荞�݋���
    INTCONbits.PEIE = SET;        // �y���t�F���������݋���
}

static void interrupt_timer0_tmr0if(void)
{   /*ovf�̂Ƃ�8ms�o��*/
    if (cnt == 10)
    {
        copydata_set_copy_end_req();
        cnt = 0;            // �I�[�o�[�t���[�J�E���^�����Z�b�g        
    }
    cnt++;                  // �I�[�o�[�t���[�J�E���^���C���N��
}

static void interrupt_timer1_tmr1if(void)
{
    copydata_set_copy_end_req();
}

static void interrupt_timer1_ccp1if(void)
{
#if 0
    TMR1L = 0x00;   /*CCPR1H,CCPR1L�ɕۑ������^�C�~���O���ザ��Ȃ��Ə�����*/
    TMR1H = 0x00;
   
    if(register_setup_timer1_capture_edge == CAPTURE_RISING_EDGE)
    {
        timer1_capture_mode_edge_select(CAPTURE_FALLING_EDGE);
    }
    else /*if(register_setup_timer1_capture_edge == CAPTURE_FALLING_EDGE)*/
    {
         timer1_capture_mode_edge_select(CAPTURE_RISING_EDGE);
    }
#endif
    
    //copydata_1byte_copy_interrupt();
    PORTBbits.RB5 = SET;    /*�������ԑ���*/
    
    if(sequence_num == SEQUENCE_COPY_DATA)
    {
        copydata_debug_test();
    }
    else if(sequence_num == SEQUENCE_SEND_DATA)
    {
        senddata_debug_test();
    }
    
    
    PORTBbits.RB5 = CLEAR;    /*�������ԑ���*/
}

static void interrupt_timer2_tmr2if(void)
{
    ;
}

static void interrupt_inte(void)
{
#if 0
    unsigned char edge_select_now;
    
    copydata_copy_start_flag = SET;    
    edge_select_now = OPTION_REGbits.INTEDG; /* ���[�h�͊�{�ǂ��ł��������� */
    
    if(edge_select_now == CLEAR)
    {
        CCPR1L = 0x00;
        PORTBbits.RB5 = CLEAR;
    }
    else
    {
        CCPR1L = 0x11;
        PORTBbits.RB5 = SET;
    }
    
#if 1
    if(edge_select_now == SET)
    {
        edge_select_now = CLEAR;
    }
    else
    {
        edge_select_now = SET;
    }
#endif
    
    gf_option_integ_edge_select(edge_select_now);
#endif
}