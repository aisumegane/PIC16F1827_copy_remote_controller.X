/*
 * File:   i2c.c        ：略識別子：lcd
 * Author: 氷MEGANE
 * SCK1,SDA1を用いてI2Cを行う
 * Created on 2024/05/09, 21:17
 */

#include <xc.h>
#include "grobal_macro.h"
#include "i2c.h"
#include "register_setup.h"
#include "main.h"

#include "lcdisplay_textdata.h"
#include "lcdisplay.h"

#define WRITE_DELAY_TIME    10  /*モジュール側の応答待ち、どうにか短くしたい（ボタン押しても反応しなくなるため）*/

#define SLAVE_ADDRESS   0x40    /*check the A0,A1,A2 port(PCF8574) setup!*/ /*Arduinoだとデフォで0x27らしいが全然違うぞ!*/


#define DATA_WRITE      0x01
#define COMMAND_WRITE   0x00

#define FIRST_COLUMN    0
#define SECOND_COLUMN   1
#define FIRST_ROW       0
#define SECOND_ROW      1

/*LCD command*/     /*それぞれのビットを加算する方式で、最後に8bitのデータ＋制御信号とする*/
#define SET_LCD_BACKLIGHT       0x08
#define CURSOR_SHIFT_ENABLE     0x06
#define DISPLAY_CLEAR           0x01

void lcdisplay_seq_change_write_main(void);


void lcdisplay_init(void);
void lcdisplay_cmmand_4bit_commandwrite(unsigned char command,unsigned char data_command_select);
void lcdisplay_cmmand_8bit_write(unsigned char data,unsigned char data_command_select);
void lcdisplay_set_cursor(unsigned char row, unsigned char column);
void write_text_message(const unsigned char * recive_message);
void lcdisplay_clear(void);

static unsigned char lcd_write_go_f = SET;      /*Duplicate write protection flag*/

static unsigned char lcd_sequence_num_before = SEQUENCE_MAIN;   /*Sequence of last call*/
static unsigned char lcd_main_sequence_stable_f = CLEAR;        /*No display transition after returning to "SEQUENCE_MAIN".*/


void lcdisplay_seq_change_write_main(void)
{   /*シーケンスによって表示内容変更*/  /*copyモードでは極力割込みさせたくないので、モードに入る前に表示処理を実行する*/
    
    /*シーケンス番号が変わった時だけ表示を変更?遅延防止(基本各処理終了後に表示変更なので、ボタン操作後は気にならないはず)*/
    if((lcd_sequence_num_before != main_sequence_num)||(lcd_main_sequence_stable_f == CLEAR))
    {   /*detect sequence chage*/
        lcd_write_go_f = SET;
        
        lcdisplay_set_cursor(FIRST_ROW,FIRST_COLUMN);
        write_text_message(&lcdisplay_all_clear);           /*Passing the first element of strig array as an argument*/
        lcdisplay_set_cursor(SECOND_ROW,FIRST_COLUMN);
        write_text_message(&lcdisplay_all_clear);
    }
    
    
    
    if(lcd_write_go_f == SET)
    {
        switch(main_sequence_num)
        {
            case SEQUENCE_MAIN:
                /*SEQUENCE_MAINのみ、ほかの分岐処理から戻った時に"完了"表示を残す*/
                /*残したい処理以外はなにも書かない*/
                
                if(lcd_sequence_num_before == SEQUENCE_COPY_DATA)
                {   /*コピー完了 ? MAIN*/
                    lcdisplay_set_cursor(FIRST_ROW,FIRST_COLUMN);
                    write_text_message(&copy_complete);           /*Passing the first element of strig array as an argument*/   
                }
                else if(lcd_sequence_num_before == SEQUENCE_SEND_DATA)
                {   /*送信完了 ? MAIN*/
                    lcdisplay_set_cursor(FIRST_ROW,FIRST_COLUMN);
                    write_text_message(&send_complete);           /*Passing the first element of strig array as an argument*/    
                }
                else if(lcd_sequence_num_before == SEQUENCE_FREQ_CHANGE)
                {
                    /*変更完了 ? MAIN*/
                    lcdisplay_set_cursor(FIRST_ROW,FIRST_COLUMN);
                    write_text_message(&carry_freq_setup);           /*Passing the first element of strig array as an argument*/  
                    
                    if(fc_current_freq_setup != PWM_38KHZ)
                    {
                        lcdisplay_set_cursor(SECOND_ROW,FIRST_COLUMN);
                        write_text_message(&freq_1khz_up);
                    }
                    else
                    {
                        lcdisplay_set_cursor(SECOND_ROW,FIRST_COLUMN);
                        write_text_message(&freq_set_to_min);
                    }
                    
                }
                else /*(lcd_sequence_num_before == SEQUENCE_MAIN)*/
                {
                    lcdisplay_set_cursor(FIRST_ROW,FIRST_COLUMN);
                    write_text_message(&copydata_choose);           /*Passing the first element of strig array as an argument*/
                    lcdisplay_set_cursor(SECOND_ROW,FIRST_COLUMN);
                    write_text_message(&senddata_choose);
                    
                    lcd_main_sequence_stable_f = SET;   /*SEQUENCE_MAINからSEQUENCE_MAINへ、?途中の表示遷移はない待機した状態*/
                }

                
                lcd_write_go_f = CLEAR;   /*Duplicate write protection*/
                break;
            /*============================================================================*/
            case SEQUENCE_COPY_DATA:
                
                lcdisplay_set_cursor(FIRST_ROW,FIRST_COLUMN);
                write_text_message(&awaiting_signal);           /*Passing the first element of strig array as an argument*/

                lcd_write_go_f = CLEAR;   /*Duplicate write protection*/
                lcd_main_sequence_stable_f = CLEAR;
                break;
            /*============================================================================*/
            case SEQUENCE_SEND_DATA:
                
                lcdisplay_set_cursor(FIRST_ROW,FIRST_COLUMN);
                write_text_message(&Transmitting);           /*Passing the first element of strig array as an argument*/
                
                lcdisplay_set_cursor(SECOND_ROW,FIRST_COLUMN);
                write_text_message(&carry_freq);
                lcdisplay_set_cursor(SECOND_ROW,11);
                write_text_message(&freq_setting[fc_current_freq_setup]);


                lcd_write_go_f = CLEAR;   /*Duplicate write protection*/
                lcd_main_sequence_stable_f = CLEAR;
                break;
            /*============================================================================*/
            case SEQUENCE_SEND_DEFNED_NO_DATA:
                lcdisplay_set_cursor(FIRST_ROW,FIRST_COLUMN);
                write_text_message(&nodata_available);           /*Passing the first element of strig array as an argument*/
                lcdisplay_set_cursor(SECOND_ROW,FIRST_COLUMN);
                write_text_message(&please_copy);
                
                
                lcd_main_sequence_stable_f = CLEAR;
                lcd_write_go_f = CLEAR;   /*Duplicate write protection*/
                break;
                
            /*============================================================================*/
            case SEQUENCE_FREQ_CHANGE:
                lcdisplay_set_cursor(FIRST_ROW,FIRST_COLUMN);
                write_text_message(&carry_freq_setup);           /*Passing the first element of strig array as an argument*/  
                
                
                lcdisplay_set_cursor(SECOND_ROW,FIRST_COLUMN);
                write_text_message(&freq_setting[fc_current_freq_setup]);
                
                lcdisplay_set_cursor(SECOND_ROW,6);
                write_text_message(&right_allow);
                
                lcdisplay_set_cursor(SECOND_ROW,8);
                write_text_message(&freq_setting[fc_current_freq_setup+1]);
                
            default:
                
                lcd_write_go_f = CLEAR;   /*Duplicate write protection*/
                lcd_main_sequence_stable_f = CLEAR;
                break;
        }
    }
    
    lcd_sequence_num_before = main_sequence_num;
}


void lcdisplay_init(void)
{   
    /*i2c module setup*/
    i2c_master_init();
    i2c_master_transmit_start();
    i2c_master_write(SLAVE_ADDRESS);
    i2c_master_transmit_stop();
    
    /*==================== LCD initialize =====================*/
    /*lcd display setup*/   /*delayが必要なのはここから*/
    __delay_ms(40);
    i2c_master_transmit_start();
    i2c_master_write(SLAVE_ADDRESS);
    i2c_master_write(0x30);
    i2c_master_transmit_stop();
    __delay_ms(5);
    
    /*nybble start*/    /*1st*/
    i2c_master_transmit_start();
    i2c_master_write(SLAVE_ADDRESS);
    i2c_master_write(0x30 + 0x04);
    i2c_master_transmit_stop();
    __delay_us(40);
    i2c_master_transmit_start();
    i2c_master_write(SLAVE_ADDRESS);
    i2c_master_write(0x30 + 0x00);
    i2c_master_transmit_stop();
    /*nybble end*/
    
    __delay_ms(5);
    
    /*nybble start*/    /*2nd*/
    i2c_master_transmit_start();
    i2c_master_write(SLAVE_ADDRESS);
    i2c_master_write(0x30 + 0x04);
    i2c_master_transmit_stop();
    __delay_us(1);
    i2c_master_transmit_start();
    i2c_master_write(SLAVE_ADDRESS);
    i2c_master_write(0x30 + 0x00);
    i2c_master_transmit_stop();
    /*nybble end*/
    
    __delay_ms(5);
    
    /*nybble start*/    /*3rd*/
    i2c_master_transmit_start();
    i2c_master_write(SLAVE_ADDRESS);
    i2c_master_write(0x30 + 0x04);
    i2c_master_transmit_stop();
    __delay_us(1);
    i2c_master_transmit_start();
    i2c_master_write(SLAVE_ADDRESS);
    i2c_master_write(0x30 + 0x00);
    i2c_master_transmit_stop();
    /*nybble end*/
    
    __delay_ms(1);
    
    /*================== LCD function setup ====================*/
    lcdisplay_cmmand_4bit_commandwrite(0x20,COMMAND_WRITE);  /*4bit通信選択時のみ、4bit送信を1回行う*/  /*この時のRS信号は？*/
    lcdisplay_cmmand_8bit_write(0x28,COMMAND_WRITE);
    lcdisplay_cmmand_8bit_write(0x0C,COMMAND_WRITE);
    lcdisplay_clear();
    
    lcdisplay_cmmand_8bit_write(CURSOR_SHIFT_ENABLE,COMMAND_WRITE);
}

void lcdisplay_cmmand_4bit_commandwrite(unsigned char command,unsigned char data_command_select)
{   /*受け取った8bitのcommandを送信データの上位4bitになるようにシフト（下位4bitはデータではなくLCDの制御に使うため）*/
     __delay_us(WRITE_DELAY_TIME);     /*一応*/
     
    /*データなし*/  /*E信号立ち上げ*/
    i2c_master_transmit_start();
    i2c_master_write(SLAVE_ADDRESS);
    i2c_master_write(0x00 + data_command_select + 0x0C);
    i2c_master_transmit_stop();
    __delay_us(WRITE_DELAY_TIME);
    
    /*データ出力*/  /*E信号維持*/
    i2c_master_transmit_start();
    i2c_master_write(SLAVE_ADDRESS);
    i2c_master_write(command + data_command_select+ 0x0C);
    i2c_master_transmit_stop();
    __delay_us(WRITE_DELAY_TIME);
    
    /*データ出力*/  /*E信号立ち下げ*/
    i2c_master_transmit_start();
    i2c_master_write(SLAVE_ADDRESS);
    i2c_master_write(command + data_command_select+ 0x08);
    i2c_master_transmit_stop();
    
     __delay_us(WRITE_DELAY_TIME);     /*一応*/
}

void lcdisplay_cmmand_8bit_write(unsigned char data, unsigned char data_command_select)
{   /*PCF8574の4bit通信フォーマットに合わせてデータを改造する*/
    /*8bitのデータは4bitのコマンドで2回送るイメージ・・・でも構わない*/
    unsigned char data_high4bit;
    unsigned char data_low4bit;
    
    data_high4bit   = data;
    data_high4bit   &= 0xF0; /*extract high-order byte*/
    
    data_low4bit    = data;
    data_low4bit    &= 0x0F;
    data_low4bit    = data_low4bit << 4;    /*shift the position of the lower 4bits to match the 4bit transmit func*/
    
    lcdisplay_cmmand_4bit_commandwrite(data_high4bit,data_command_select);  /*higher 4bit data*/
    lcdisplay_cmmand_4bit_commandwrite(data_low4bit,data_command_select);           /*lower  4bit data*/
}

void lcdisplay_set_cursor(unsigned char row, unsigned char column)
{
    const unsigned char lcdisplay_start_address[2] = {0x00,0x40};
    unsigned char set_address;
    
    set_address = lcdisplay_start_address[row] + column;
    lcdisplay_cmmand_8bit_write(0x80 + set_address, COMMAND_WRITE);
}

void write_text_message(const unsigned char * recive_message)
{
    while(*recive_message)  /*ポインタを進めた際、NULLデータになったら送信終了*/
    {
        lcdisplay_cmmand_8bit_write( (*recive_message++) ,DATA_WRITE); /*ポインタを1加算*/
    }
}

void lcdisplay_clear(void)
{
    lcdisplay_cmmand_8bit_write(DISPLAY_CLEAR,COMMAND_WRITE);
    __delay_ms(4);
}