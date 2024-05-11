/*
 * File:   i2c.c
 * Author: ïXMEGANE
 * SCK1,SDA1ÇópÇ¢ÇƒI2CÇçsÇ§
 * Created on 2024/05/08, 21:17
 */

#include <xc.h>
#include "grobal_macro.h"

#include "i2c.h"


void i2c_master_init(void);
void i2c_master_transmit_cmp_wait(void);
void i2c_master_transmit_start(void);
void i2c_master_transmit_stop(void);
void i2c_master_write(unsigned char transport_data);


void i2c_master_init(void)
{
    /*SSP1ADD*/
    SSP1ADD             = 0x4F;     /*I2C vaud rate : 100kHz*/
    
    /*SSP1STAT*/
    SSP1STATbits.SMP        = SET;      /*I2C Master mode at 100kHz*/
    SSP1STATbits.CKE        = CLEAR;
    SSP1STATbits.D_nA       = CLEAR;    /*read only*/
    SSP1STATbits.P          = CLEAR;    /*read only*/
    
    SSP1STATbits.S          = CLEAR;    /*read only*/
    SSP1STATbits.R_nW       = CLEAR;    /*read only*/
    SSP1STATbits.UA         = CLEAR;    /*read only*/
    SSP1STATbits.BF         = CLEAR;    /*read only*/
    
    /*SSP1CON1*/
    SSP1CON1bits.WCOL       = CLEAR;    /*Write collision detect bit*/
    SSP1CON1bits.SSPOV      = CLEAR;    /*Receive overflow indicator bit*/
    SSP1CON1bits.SSPEN      = SET;      /*Synchronous serial port enable bit*/  /*serial port enable*/
    SSP1CON1bits.CKP        = CLEAR;    /*Clock polarity select*/
    
    SSP1CON1bits.SSPM3      = SET;      /*I2C Master mode setup*/
    SSP1CON1bits.SSPM2      = CLEAR;
    SSP1CON1bits.SSPM1      = CLEAR;
    SSP1CON1bits.SSPM0      = CLEAR;
    
    /*SSP1CON2*/
    SSP1CON2bits.GCEN       = CLEAR;    /*General call enable bit (I2C only)*/
    SSP1CON2bits.ACKSTAT    = CLEAR;    /*Acknowledge status bit(I2C mode only)*/
    SSP1CON2bits.ACKDT      = CLEAR;    /*Acknowledge data bit (I2C mode only)*/
    SSP1CON2bits.ACKEN      = CLEAR;    /*Acknowledge sequence enable bit (I2C master mode only)*/
    
    SSP1CON2bits.RCEN       = CLEAR;    /*Receive enable bit (I2C master mode only)*/
    SSP1CON2bits.PEN        = CLEAR;    /*Stop condition enable bit (I2C master mode only)*/
    SSP1CON2bits.RSEN       = CLEAR;    /*Repeated start condition enable bit (I2C master mode only)*/
    SSP1CON2bits.SEN        = CLEAR;    /*Start condition enabled bit (I2C master mode only)*/
    
    /*SSP1CON3*/
    SSP1CON3bits.ACKTIM     = CLEAR;    /*Acknowledge time status bit (I2C mode only)*/
    SSP1CON3bits.PCIE       = CLEAR;    /*Stop condition interrupt enable bit*/
    SSP1CON3bits.SCIE       = CLEAR;    /*Start condition interrupt enable bit*/
    SSP1CON3bits.BOEN       = CLEAR;    /*Buffer overwrite enable bit*/
    
    SSP1CON3bits.SDAHT      = CLEAR;    /*SDA1 hold time selection bit (I2C mode only)*/    /*minimum of 100 ns hold*/
    SSP1CON3bits.SBCDE      = CLEAR;    /*Slave mode bus collision detect enable bit*/
    SSP1CON3bits.AHEN       = CLEAR;    /*Address hold enable bit */
    SSP1CON3bits.DHEN       = CLEAR;    /*Data hold enable bit (I2C slave mode only)*/
    
    /*SSP1MSK*/
    SSP1MSK                 = 0x00;     /*Mask bits*/    
    
    /*SSP1ADD*/
    SSP1ADD                 = 0x4F;     /*I2C Baud rate set to 100kHz*/
}

void i2c_master_transmit_cmp_wait(void)
{
  while ((SSPSTAT & 0x04) || (SSPCON2 & 0x1F));
}

void i2c_master_transmit_start(void)
{
    i2c_master_transmit_cmp_wait();
    SSP1CON2bits.SEN = SET;
}

void i2c_master_transmit_stop(void)
{
    i2c_master_transmit_cmp_wait();
    SSP1CON2bits.PEN = SET;
}

void i2c_master_write(unsigned char transport_data)
{
    i2c_master_transmit_cmp_wait();
    SSP1BUF = transport_data;
}

