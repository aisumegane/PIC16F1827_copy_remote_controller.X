/*
 * File:   register_setup.c
 * Author: •XMEGANE
 *
 * Created on 2024/04/11, 21:17
 */

#include <xc.h>
#include "grobal_macro.h"
#include "peripheral_in.h"
#include "peripheral_out.h"

void peripheral_out_init(void);
void peripheral_out_main(void);

static void peripheral_led_test(void);
static void peripheral_infrared_led_test(void);


void peripheral_out_init(void)
{
    
}


void peripheral_out_main(void)
{
    //peripheral_led_test();
   //peripheral_infrared_led_test();
}

static void peripheral_led_test(void)
{
    /**/
    if(copysw_state == SET)
    {
        PORTAbits.RA2 = SET;
        //PORTBbits.RB3 = SET;
    }
    else
    {
        PORTAbits.RA2 = CLEAR;
        //PORTBbits.RB3 = CLEAR;
    }
    
    /**/
    if(sendsw_state == SET)
    {
        PORTAbits.RA3 = SET;
    }
    else
    {
        PORTAbits.RA3 = CLEAR;
    }
    
    /**/
#if 0
    if(powersw_state == SET)
    {
        PORTBbits.RB5 = SET;
    }
    else
    {
        PORTBbits.RB5 = CLEAR;
    }
#endif
}

#if 1
static void peripheral_infrared_led_test(void)
{   

    if(PORTBbits.RB0 == SET)
    {
        PORTBbits.RB5 = SET;
        //PORTBbits.RB3 = SET;
        //gf_disable_interrupt();
        CCPR1L = 0x11;
        //gf_enable_interrupt();
    }
    else
    {
        PORTBbits.RB5 = CLEAR;
        //PORTBbits.RB3 = CLEAR;
        //gf_disable_interrupt();
        CCPR1L = 0x00;       
        //gf_enable_interrupt();
    }
    //PORTBbits.RB3 = SET;
}

#endif