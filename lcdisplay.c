/*
 * File:   i2c.c
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

void lcdisplay_main(void);

void lcdisplay_init(void);
void lcdisplay_cmmand_4bit_commandwrite(unsigned char command,unsigned char data_command_select);
void lcdisplay_cmmand_8bit_write(unsigned char data,unsigned char data_command_select);
void lcdisplay_set_cursor(unsigned char row, unsigned char column);
void write_text_message(const unsigned char * recive_message);
void lcdisplay_clear(void);



void lcdisplay_main(void)
{   /*シーケンスによって表示内容変更*/  /*copyモードでは極力割込みさせたくないので、モードに入る前に表示処理を実行する*/
    switch(sequence_num)
    {
        case SEQUENCE_MAIN:
            lcdisplay_set_cursor(FIRST_ROW,FIRST_COLUMN);
            write_text_message(&copydata_choose);   /*Passing the first element of strig array as an argument*/
            lcdisplay_set_cursor(SECOND_ROW,FIRST_COLUMN);
            write_text_message(&senddata_choose);
            
            break;
            
        case SEQUENCE_COPY_DATA:
            
            
            
            break;
            
        case SEQUENCE_SEND_DATA:
            
            
            
            break;
            
        default:
            break;
    }
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
     __delay_us(40);     /*一応*/
     
    /*データなし*/  /*E信号立ち上げ*/
    i2c_master_transmit_start();
    i2c_master_write(SLAVE_ADDRESS);
    i2c_master_write(0x00 + data_command_select + 0x0C);
    i2c_master_transmit_stop();
    __delay_us(40);
    
    /*データ出力*/  /*E信号維持*/
    i2c_master_transmit_start();
    i2c_master_write(SLAVE_ADDRESS);
    i2c_master_write(command + data_command_select+ 0x0C);
    i2c_master_transmit_stop();
    __delay_us(40);
    
    /*データ出力*/  /*E信号立ち下げ*/
    i2c_master_transmit_start();
    i2c_master_write(SLAVE_ADDRESS);
    i2c_master_write(command + data_command_select+ 0x08);
    i2c_master_transmit_stop();
    
     __delay_us(40);     /*一応*/
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