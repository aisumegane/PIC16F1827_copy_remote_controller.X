/*
 * File:   interrupt.c
 * Author: �XMEGANE
 * Mid-Range 8-bit MCUs PIC16F1827
 * Created on 2024/03/27, 23:38
 */

#include "config_bits.h"
#include <xc.h>
#include "grobal_macro.h"
#include "register_setup.h"         /* ���W�X�^�ݒ� */
#include "interrupt.h"
#include "peripheral_in.h"
#include "peripheral_out.h"
#include "copydata.h"
#include "senddata.h"
#include "i2c.h"
#include "lcdisplay.h"
#include "freq_change.h"

#include "main.h"


unsigned char main_sequence_num = 0;

void main_init(void);
void main_task(void);
void sequence_check_led(void);

void main(void) {
    main_init();
    
    rs_enable_interrupt();
    
    while(1)
    {

        main_task();
    }   
}

void main_init(void)
{
    /* PIC16F819 �n�[�h�E�F�A�ݒ� */
    pic819_register_setup();
    
    peripheral_in_init();
    peripheral_out_init();
    i2c_master_init();
    
    lcdisplay_init();       /*I2C�ʐM�ŏ���������̂�I2Cinit�̌�!*/
    
    main_sequence_num = SEQUENCE_MAIN;
}

void main_task(void)
{
   switch(main_sequence_num){
        case SEQUENCE_MAIN:
            peripheral_in_main();       /* �|�[�g�̓��� */

            if((copysw_state == SET) && (copysw_state_change == SET))       /*prevention of two consecutive  treatments*/
            {
                main_sequence_num = SEQUENCE_COPY_DATA;   
            }
            else if((sendsw_state == SET) && (sendsw_state_change == SET))  /*prevention of two consecutive treatments*/
            {
                main_sequence_num = SEQUENCE_SEND_DATA;   
            }
            else if((freqchange_sw_state == SET) && (freqchange_sw_state_change == SET))  /*prevention of two consecutive treatments*/
            {
                main_sequence_num = SEQUENCE_FREQ_CHANGE;
            }
            else
            {
                ;
            }
           
            break;
            
       case SEQUENCE_COPY_DATA:
            /* external interrupt �J�n */
            /* if external interrupt ���m �� timer1 ON */ /* 2��ڈȍ~��ON��ON�ŏ㏑�����Ă��ς��Ȃ� */
                       
            copydata_main();
            
#if 1
            if(copydata_copy_end_flag == SET)
            {
                /* external interrupt ��~ */
                /* timer1 ON */
                /* tmr1H/L�����Z�b�g ��TMR1H�͖��g�p */
                
                copydata_clear_copy_end_flag();
                main_sequence_num = SEQUENCE_MAIN;
            }
            else if(copydata_copy_fail_flag == SET)
            {
                copydata_copy_fail_flag = CLEAR;
                main_sequence_num = SEQUENCE_MAIN;
            }
#endif
            break;
            
        case SEQUENCE_SEND_DATA:
            senddata_main();
            
            if(senddata_send_end_flag == SET)
            {
                senddata_send_end_flag = CLEAR;
                main_sequence_num = SEQUENCE_MAIN;
            }
            break;
            
       case SEQUENCE_FREQ_CHANGE:
           freq_change_main();
           break;
            
       default:
           break;
    }
   
   /*common process*/
   lcdisplay_main();   
   peripheral_out_main();
}

#if 0
void sequence_check_led(void)
{
    switch(main_sequence_num){
        case SEQUENCE_MAIN:
            PORTAbits.RA2 = SET;
            PORTAbits.RA3 = CLEAR;
            PORTBbits.RB5 = CLEAR;
            break;
            
       case SEQUENCE_COPY_DATA:
            PORTAbits.RA2 = CLEAR;
            PORTAbits.RA3 = SET;
            PORTBbits.RB5 = CLEAR;
            break;
            
        case SEQUENCE_SEND_DATA:
            PORTAbits.RA2 = CLEAR;
            PORTAbits.RA3 = CLEAR;
            PORTBbits.RB5 = SET;
            break;
            
       default:
           break;
    }
}
#endif