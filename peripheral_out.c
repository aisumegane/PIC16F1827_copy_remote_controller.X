/*
 * File:   register_setup.c :識別子 po にする
 * Author: 氷MEGANE
 * 主にLEDの表示に使う
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
    
    peripheral_out_port_set();          /*設定した出力ポートに指令をセット*/
    
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

/*BUSY表示(赤色)LEDの点灯制御*/
/*main sequenceのところで、sequence_num 更新直後はperipheral_outを通過するので、処理前に点灯できる。*/
static void peripheral_out_busy_led(void)
{
    if((main_sequence_num == SEQUENCE_MAIN) && (po_sequence_num_before == SEQUENCE_MAIN))
    {   /*要求なし待機状態*/
        port_red_led  = CLEAR;
    }
    else if((main_sequence_num == SEQUENCE_COPY_DATA) && (po_sequence_num_before == SEQUENCE_MAIN))
    {   /*コピー処理完了*/
        port_red_led  = SET;
    }
    else if((main_sequence_num == SEQUENCE_SEND_DATA) && (po_sequence_num_before == SEQUENCE_MAIN))
    {   /*送信処理完了*/
        port_red_led  = SET;
    }
    else
    {   /*その他の状態*/
        port_red_led  = CLEAR;
    }
}

static void peripheral_out_data_led(void)
{   /*データ有無表示(黄色)LEDの点灯制御*/
    unsigned char check_wait    = SET;
    
    /*コピーしたのに最初のデータが空、といことはあり得ない*/
    if(copydata_led_data_ary0[0] == 0x00)
    {
        port_yellow_led     = CLEAR;
    }
    else
    {
        port_yellow_led     = SET;
    }
}


/*処理完了表示(緑色)LEDの点灯制御*/
/*各処理完了後にdelayを設ける(この間操作は受け付けない)*/
static void peripheral_out_complete_led(void)
{
    if((main_sequence_num == SEQUENCE_MAIN) && (po_sequence_num_before == SEQUENCE_MAIN))
    {   /*要求なし待機状態*/
        port_green_led  = CLEAR;
    }
    else if((main_sequence_num == SEQUENCE_MAIN) && (po_sequence_num_before == SEQUENCE_COPY_DATA))
    {   /*コピー処理完了*/
        port_green_led  = SET;
    }
    else if((main_sequence_num == SEQUENCE_MAIN) && (po_sequence_num_before == SEQUENCE_SEND_DATA))
    {   /*送信処理完了*/
        port_green_led  = SET;
    }
    else
    {   /*その他の状態*/
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