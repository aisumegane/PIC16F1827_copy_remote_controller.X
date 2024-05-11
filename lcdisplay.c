/*
 * File:   i2c.c
 * Author: �XMEGANE
 * SCK1,SDA1��p����I2C���s��
 * Created on 2024/05/09, 21:17
 */

#include <xc.h>
#include "grobal_macro.h"
#include "i2c.h"
#include "register_setup.h"
#include "main.h"

#include "lcdisplay_textdata.h"
#include "lcdisplay.h"

#define SLAVE_ADDRESS   0x40    /*check the A0,A1,A2 port(PCF8574) setup!*/ /*Arduino���ƃf�t�H��0x27�炵�����S�R�Ⴄ��!*/


#define DATA_WRITE      0x01
#define COMMAND_WRITE   0x00

#define FIRST_COLUMN    0
#define SECOND_COLUMN   1
#define FIRST_ROW       0
#define SECOND_ROW      1

/*LCD command*/     /*���ꂼ��̃r�b�g�����Z��������ŁA�Ō��8bit�̃f�[�^�{����M���Ƃ���*/
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
{   /*�V�[�P���X�ɂ���ĕ\�����e�ύX*/  /*copy���[�h�ł͋ɗ͊����݂��������Ȃ��̂ŁA���[�h�ɓ���O�ɕ\�����������s����*/
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
    /*lcd display setup*/   /*delay���K�v�Ȃ̂͂�������*/
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
    lcdisplay_cmmand_4bit_commandwrite(0x20,COMMAND_WRITE);  /*4bit�ʐM�I�����̂݁A4bit���M��1��s��*/  /*���̎���RS�M���́H*/
    lcdisplay_cmmand_8bit_write(0x28,COMMAND_WRITE);
    lcdisplay_cmmand_8bit_write(0x0C,COMMAND_WRITE);
    lcdisplay_clear();
    
    lcdisplay_cmmand_8bit_write(CURSOR_SHIFT_ENABLE,COMMAND_WRITE);
}

void lcdisplay_cmmand_4bit_commandwrite(unsigned char command,unsigned char data_command_select)
{   /*�󂯎����8bit��command�𑗐M�f�[�^�̏��4bit�ɂȂ�悤�ɃV�t�g�i����4bit�̓f�[�^�ł͂Ȃ�LCD�̐���Ɏg�����߁j*/
     __delay_us(40);     /*�ꉞ*/
     
    /*�f�[�^�Ȃ�*/  /*E�M�������グ*/
    i2c_master_transmit_start();
    i2c_master_write(SLAVE_ADDRESS);
    i2c_master_write(0x00 + data_command_select + 0x0C);
    i2c_master_transmit_stop();
    __delay_us(40);
    
    /*�f�[�^�o��*/  /*E�M���ێ�*/
    i2c_master_transmit_start();
    i2c_master_write(SLAVE_ADDRESS);
    i2c_master_write(command + data_command_select+ 0x0C);
    i2c_master_transmit_stop();
    __delay_us(40);
    
    /*�f�[�^�o��*/  /*E�M����������*/
    i2c_master_transmit_start();
    i2c_master_write(SLAVE_ADDRESS);
    i2c_master_write(command + data_command_select+ 0x08);
    i2c_master_transmit_stop();
    
     __delay_us(40);     /*�ꉞ*/
}

void lcdisplay_cmmand_8bit_write(unsigned char data, unsigned char data_command_select)
{   /*PCF8574��4bit�ʐM�t�H�[�}�b�g�ɍ��킹�ăf�[�^����������*/
    /*8bit�̃f�[�^��4bit�̃R�}���h��2�񑗂�C���[�W�E�E�E�ł��\��Ȃ�*/
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
    while(*recive_message)  /*�|�C���^��i�߂��ہANULL�f�[�^�ɂȂ����瑗�M�I��*/
    {
        lcdisplay_cmmand_8bit_write( (*recive_message++) ,DATA_WRITE); /*�|�C���^��1���Z*/
    }
}

void lcdisplay_clear(void)
{
    lcdisplay_cmmand_8bit_write(DISPLAY_CLEAR,COMMAND_WRITE);
    __delay_ms(4);
}