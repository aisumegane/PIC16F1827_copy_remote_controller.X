/* 
 * File:   register_setup.h
 * Author: ïXMEGANE
 *
 * Created on 2024/03/31, 13:18
 */

#ifndef XC_REGISTER_SETUP_H
#define	XC_REGISTER_SETUP_H


#define DEBUGBOARD_MODE     0
#define PRINTBOARD_MODE     1
    
#define PORTMODE    PRINTBOARD_MODE

#define _XTAL_FREQ  32000000     /* 32MHz */

#define CAPTURE_RISING_EDGE     1
#define CAPTURE_FALLING_EDGE    0

extern unsigned char register_setup_timer1_capture_edge;

extern void pic819_register_setup(void);
extern void timer1_capture_mode_edge_select(unsigned char edge_select);     /*CCP1 capture mode edge select*/

extern void timer1_capture_mode_setup(void);
extern void timer1_compare_mode_setup(void);

extern void rs_enable_interrupt(void);    /*äÑçûÇ›ãñâ¬ä÷êî*/
extern void rs_disable_interrupt(void);   /*äÑçûÇ›ã÷é~ä÷êî*/
extern void gf_timer1_start(void);        /*timer1 ìÆçÏäJénä÷êî*/
extern void gf_timer1_stop(void);         /*timer1 ìÆçÏí‚é~ä÷êî*/
extern void gf_timer2_start(void);
extern void gf_timer2_stop(void);
extern void gf_option_integ_edge_select(unsigned char edge_state);

extern void gf_enable_timer0_interrupt(void);
extern void gf_disable_timer0_interrupt(void);

extern void gf_enable_timer1_interrupt(void);
extern void gf_disable_timer1_interrupt(void);


extern void gf_enable_ccp1_interrupt(void);
extern void gf_disable_ccp1_interrupt(void);

extern void rs_ccp2_set_pwmduty_8bit(unsigned char duty);
extern void rs_ccp2_set_carrier_freq_8bit(unsigned char carrier_freq);

/**/


#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */


#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* XC_REGISTER_SETUP_H */

