/*
 * File:   interrupt.c
 * Author: 氷MEGANE
 * Mid-Range 8-bit MCUs PIC16F1827
 * Created on 2024/03/27, 23:38
 */

#include "config_bits.h"
#include <xc.h>
#include "grobal_macro.h"
#include "register_setup.h"         /* レジスタ設定 */
#include "interrupt.h"
#include "peripheral_in.h"
#include "peripheral_out.h"
#include "copydata.h"
#include "senddata.h"
#include "i2c.h"
#include "lcdisplay.h"
#include "freq_change.h"

#include "main.h"


unsigned char           main_sequence_num = SEQUENCE_MAIN;
static unsigned char    main_sequence_num_before = SEQUENCE_MAIN;

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
    /* PIC16F819 ハードウェア設定 */
    pic819_register_setup();
    
    peripheral_in_init();
    peripheral_out_init();
    i2c_master_init();
    
    lcdisplay_init();       /*I2C通信で初期化するのでI2Cinitの後!*/
    
    freq_change_init();
    
    main_sequence_num = SEQUENCE_MAIN;
}

void main_task(void)
{    
   switch(main_sequence_num){
        case SEQUENCE_MAIN:
            peripheral_in_main();       /* ポートの入力 */

            if((copysw_state == SET) && (copysw_state_change == SET))       /*prevention of two consecutive  treatments*/
            {
                main_sequence_num = SEQUENCE_COPY_DATA;   
            }
            else if((sendsw_state == SET) && (sendsw_state_change == SET))  /*prevention of two consecutive treatments*/
            {
                if(copydata_led_data_ary0[0] != 0x00)
                {   /*コピー済み、データあり*/
                    main_sequence_num = SEQUENCE_SEND_DATA;
                }
                else
                {   /*データを一回もコピーしていない（データなし）*/
                    main_sequence_num = SEQUENCE_SEND_DEFNED_NO_DATA;
                }
                
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
            /* external interrupt 開始 */
            /* if external interrupt 検知 → timer1 ON */ /* 2回目以降はONをONで上書きしても変わらない */
                       
            copydata_main();
            
#if 1
            if(copydata_copy_end_flag == SET)
            {
                /* external interrupt 停止 */
                /* timer1 ON */
                /* tmr1H/Lをリセット ※TMR1Hは未使用 */
                
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
            
        case SEQUENCE_SEND_DEFNED_NO_DATA:
            main_sequence_num = SEQUENCE_MAIN;
           
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
           
           /*とくに他の場所に分岐しないのでそのまま戻る*/
           main_sequence_num = SEQUENCE_MAIN;
           
           break;
            
       default:
           break;
    }
   
   /*common process*/
   lcdisplay_seq_change_write_main();   
   peripheral_out_main();
   
   /*make indicate delay*/
   if(main_sequence_num != main_sequence_num_before)
   {    /*シーケンスが変わった or 各シーケンスの分岐先で処理が終わったら、表示時間追加（一瞬表示して終わっちゃうので...）*/
       __delay_ms(1500);
   }
      
   main_sequence_num_before = main_sequence_num;
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