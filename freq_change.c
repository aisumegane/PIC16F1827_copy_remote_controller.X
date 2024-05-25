/*
 * File:   freq_change.c 識別子：fc
 * Author: 氷MEGANE
 *
 * Created on 2024/05/24, 22:32
 */

#include <xc.h>

#include "register_setup.h"
#include "grobal_macro.h"
#include "interrupt.h"

#include "freq_change.h"


                                                 /* {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F} */
const unsigned char carrier_preset[PWM_ALL_INDEX] = {0xCA,0xCB,0xCD,0xCE,0xCF,0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9};
const unsigned char    duty_preset[PWM_ALL_INDEX] = {0x46,0x47,0x48,0x49,0x4A,0x4B,0x4C,0x4D,0x4E,0x4F,0x50,0x51,0x52,0x53,0x54};   /*だいたいdutyは30%に設定*/


void freq_change_init(void);
void freq_change_main(void);

unsigned char fc_current_freq_setup = PWM_38KHZ;


void freq_change_init(void)
{
    /*初期化時は最もメジャーな38kHzにする*/
    fc_current_freq_setup = PWM_38KHZ;
    
    rs_ccp2_set_pwmduty_8bit(duty_preset[PWM_38KHZ]);
    rs_ccp2_set_carrier_freq_8bit(carrier_preset[PWM_38KHZ]);
}


void freq_change_main(void)
{
    if(fc_current_freq_setup == PWM_53KHZ)
    {
        fc_current_freq_setup = PWM_38KHZ;
    }
    else
    {
        fc_current_freq_setup++;
    }
    
    /*赤外線信号のサブキャリア周波数・dutyを更新*/
    rs_disable_interrupt();
    rs_ccp2_set_pwmduty_8bit(duty_preset[fc_current_freq_setup]);
    rs_ccp2_set_carrier_freq_8bit(carrier_preset[fc_current_freq_setup]);
    rs_enable_interrupt();
}