/*
 * File:   register_setup.c
 * Author: �XMEGANE
 *
 * Created on 2024/04/11, 21:17
 */


#include <xc.h>

/*header*/
#include "grobal_macro.h"
#include "register_setup.h"

#include "peripheral_in.h"

#define SW_ACTIVE      0
#define SW_INACTIVE    1

#define SW_CHATTER_PREV_CNT     10

const unsigned char sw_chatter_prev_cnt = 10;

/* SW1:copy sw*/
static unsigned char copysw_port_data;
static unsigned char copysw_state_buffer;
static unsigned char copysw_state_before;
static unsigned char copysw_chatter_prev_cnt;
unsigned char copysw_state;
unsigned char copysw_state_change;       /*H->L, L->H change detect */


/* SW2�Fsend sw */
static unsigned char sendsw_port_data;
static unsigned char sendsw_state_buffer;
static unsigned char sendsw_state_before;
static unsigned char sendsw_chatter_prev_cnt;
unsigned char sendsw_state;
unsigned char sendsw_state_change;

/* SW3�Fpower sw */
static unsigned char freqchange_sw_port_data;
static unsigned char freqchange_sw_state_buffer;
static unsigned char freqchange_sw_state_before;
static unsigned char freqchange_sw_chatter_prev_cnt;
unsigned char freqchange_sw_state;
unsigned char freqchange_sw_state_change;


void peripheral_in_init(void);
void peripheral_in_main(void);

static void peripheral_in_get_port_state(void);
static void peripheral_in_judge_state(unsigned char *swx_port_data, unsigned char *swx_state_buffer, unsigned char *swx_chatter_prev_cnt, unsigned char *swx_state,unsigned char *swx_state_before, unsigned char *swx_state_change);
 
static void peripheral_in_get_port_state(void)
{
#if(PORTMODE == DEBUGBOARD_MODE)
    copysw_port_data = PORTAbits.RA4;
    sendsw_port_data = PORTBbits.RB2;
    freqchange_sw_port_data = PORTBbits.RB3;
#else if(PORTMODE == PRINTBOARD_MODE)
    copysw_port_data = PORTAbits.RA2;
    sendsw_port_data = PORTAbits.RA3;
    freqchange_sw_port_data = PORTBbits.RB5;
#endif
}

void peripheral_in_init(void)
{
    copysw_state = CLEAR;
    sendsw_state = CLEAR;
    freqchange_sw_state = CLEAR;
    
    copysw_state_before = CLEAR;
    sendsw_state_before = CLEAR;
    freqchange_sw_state_before = CLEAR;
    
    copysw_chatter_prev_cnt = 0x00;
    freqchange_sw_chatter_prev_cnt = 0x00;
    freqchange_sw_chatter_prev_cnt = 0x00;
    
    copysw_state_buffer = CLEAR;
    sendsw_state_buffer = CLEAR;
    freqchange_sw_state_buffer = CLEAR;
    
    
}

void peripheral_in_main(void)
{
    peripheral_in_get_port_state();
    peripheral_in_judge_state(&copysw_port_data, &copysw_state_buffer, &copysw_chatter_prev_cnt, &copysw_state,&copysw_state_before,&copysw_state_change);
    peripheral_in_judge_state(&sendsw_port_data, &sendsw_state_buffer, &sendsw_chatter_prev_cnt, &sendsw_state,&sendsw_state_before,&sendsw_state_change);
    peripheral_in_judge_state(&freqchange_sw_port_data, &freqchange_sw_state_buffer, &freqchange_sw_chatter_prev_cnt, &freqchange_sw_state, &freqchange_sw_state_before,&freqchange_sw_state_change);
}





static void peripheral_in_judge_state(unsigned char *swx_port_data, unsigned char *swx_state_buffer, unsigned char *swx_chatter_prev_cnt, unsigned char *swx_state,unsigned char *swx_state_before, unsigned char *swx_state_change)
{
    if( (*swx_port_data) == SW_ACTIVE)
    {
        if( (*swx_state_buffer) == CLEAR)
        {
            (*swx_chatter_prev_cnt) = 0x00;
            (*swx_state_buffer) = SET;
        }
        else /*(sw1_state_buffer == SET)*/
        {
            if( (*swx_chatter_prev_cnt) < sw_chatter_prev_cnt)
            {
                (*swx_chatter_prev_cnt)++;
                if( (*swx_chatter_prev_cnt) >= sw_chatter_prev_cnt)
                {
                    (*swx_state) = SET;
                }
            }
        }
    }
    else /* if(sw1_port_data = SW1_INACTIVE) */
    {
        if( (*swx_state_buffer) == SET)
        {
            (*swx_chatter_prev_cnt) = 0x00;
            (*swx_state_buffer) = CLEAR;
        }
        else /*(sw1_state_buffer == CLEAR)*/
        {
            if( (*swx_chatter_prev_cnt) < sw_chatter_prev_cnt)
            {
                (*swx_chatter_prev_cnt)++;
                if( (*swx_chatter_prev_cnt) >= sw_chatter_prev_cnt)
                {
                    (*swx_state) = CLEAR;
                }
            }
        }
    }
    
    if((*swx_state_before == CLEAR) && (*swx_state == SET))
    {
        *swx_state_change = SET;    /*rising edge detector*/
    }
    else
    {
        *swx_state_change = CLEAR;
    }
        
    *swx_state_before = *swx_state;
}