/*
 * File:   register_setup.c :���ʎq po �ɂ���
 * Author: �XMEGANE
 * ���LED�̕\���Ɏg��
 * Created on 2024/04/11, 21:17
 */

#include <xc.h>
#include "grobal_macro.h"
#include "peripheral_in.h"
#include "copydata.h"
#include "main.h"
#include "register_setup.h"

#include "peripheral_out.h"

void peripheral_out_init(void);
void peripheral_out_main(void);
void peripheral_out_infrared_led_off(void);

static void peripheral_led_test(void);
static void peripheral_infrared_led_test(void);
static void peripheral_out_busy_led(void);
static void peripheral_out_data_led(void);
static void peripheral_out_complete_led(void);
static void peripheral_out_port_set(void);


static unsigned char    po_sequence_num_before;
static unsigned char    port_red_led;
static unsigned char    port_yellow_led;
static unsigned char    port_green_led;

void peripheral_out_init(void)
{
    
}


void peripheral_out_main(void)
{
    peripheral_out_busy_led();          /*RED    LED*/
    peripheral_out_data_led();          /*YELLOW LED*/
    peripheral_out_complete_led();      /*GREEN  LED*/
    
    peripheral_out_port_set();          /*�ݒ肵���o�̓|�[�g�Ɏw�߂��Z�b�g*/
    
    po_sequence_num_before = main_sequence_num;
}


static void peripheral_out_port_set(void)
{
    if(port_red_led == SET)
    {
        PORTAbits.RA4 = SET;
    }
    else
    {
        PORTAbits.RA4 = CLEAR;
    }
    
    if(port_yellow_led == SET)
    {
        PORTBbits.RB0 = SET;
    }
    else
    {
        PORTBbits.RB0 = CLEAR;
    }
    
    if(port_green_led == SET)
    {
        PORTBbits.RB2 = SET;
    }
    else
    {
        PORTBbits.RB2 = CLEAR;
    }
}

/*BUSY�\��(�ԐF)LED�̓_������*/
/*main sequence�̂Ƃ���ŁAsequence_num �X�V�����peripheral_out��ʉ߂���̂ŁA�����O�ɓ_���ł���B*/
static void peripheral_out_busy_led(void)
{
    if((main_sequence_num == SEQUENCE_MAIN) && (po_sequence_num_before == SEQUENCE_MAIN))
    {   /*�v���Ȃ��ҋ@���*/
        port_red_led  = CLEAR;
    }
    else if((main_sequence_num == SEQUENCE_COPY_DATA) && (po_sequence_num_before == SEQUENCE_MAIN))
    {   /*�R�s�[��������*/
        port_red_led  = SET;
    }
    else if((main_sequence_num == SEQUENCE_SEND_DATA) && (po_sequence_num_before == SEQUENCE_MAIN))
    {   /*���M��������*/
        port_red_led  = SET;
    }
    else
    {   /*���̑��̏��*/
        port_red_led  = CLEAR;
    }
}

static void peripheral_out_data_led(void)
{   /*�f�[�^�L���\��(���F)LED�̓_������*/
    unsigned char check_wait    = SET;
    
    /*�R�s�[�����̂ɍŏ��̃f�[�^����A�Ƃ����Ƃ͂��蓾�Ȃ�*/
    if(copydata_led_data_ary0[0] == 0x00)
    {
        port_yellow_led     = CLEAR;
    }
    else
    {
        port_yellow_led     = SET;
    }
}


/*���������\��(�ΐF)LED�̓_������*/
/*�e�����������delay��݂���(���̊ԑ���͎󂯕t���Ȃ�)*/
static void peripheral_out_complete_led(void)
{
    if((main_sequence_num == SEQUENCE_MAIN) && (po_sequence_num_before == SEQUENCE_MAIN))
    {   /*�v���Ȃ��ҋ@���*/
        port_green_led  = CLEAR;
    }
    else if((main_sequence_num == SEQUENCE_MAIN) && (po_sequence_num_before == SEQUENCE_COPY_DATA))
    {   /*�R�s�[��������*/
        port_green_led  = SET;
    }
    else if((main_sequence_num == SEQUENCE_MAIN) && (po_sequence_num_before == SEQUENCE_SEND_DATA))
    {   /*���M��������*/
        port_green_led  = SET;
    }
    else
    {   /*���̑��̏��*/
        port_green_led  = CLEAR;
    }
    
}

void peripheral_out_infrared_led_off(void)
{
    PORTAbits.RA7   = CLEAR;
}



#if 0
static void peripheral_infrared_led_test(void)
{   

    if(PORTBbits.RB0 == SET)
    {
        //gf_disable_interrupt();
        CCPR1L = 0x11;
        //gf_enable_interrupt();
    }
    else
    {
        //gf_disable_interrupt();
        CCPR1L = 0x00;       
        //gf_enable_interrupt();
    }
}

#endif