/* 
 * File:   register_setup.h
 * Author: •XMEGANE
 *
 * Created on 2024/03/31, 13:18
 */

#ifndef XC_REGISTER_SETUP_H
#define	XC_REGISTER_SETUP_H

#define _XTAL_FREQ  32000000     /* 32MHz */

#define CAPTURE_RISING_EDGE     1
#define CAPTURE_FALLING_EDGE    0

extern unsigned char register_setup_timer1_capture_edge;

extern void pic819_register_setup(void);
extern void timer1_capture_mode_edge_select(unsigned char edge_select);     /*CCP1 capture mode edge select*/

extern void timer1_capture_mode_setup(void);
extern void timer1_compare_mode_setup(void);

extern void gf_enable_interrupt(void);    /*Š„‚İ‹–‰ÂŠÖ”*/
extern void gf_disable_interrupt(void);   /*Š„‚İ‹Ö~ŠÖ”*/
extern void gf_timer1_start(void);        /*timer1 “®ìŠJnŠÖ”*/
extern void gf_timer1_stop(void);         /*timer1 “®ì’â~ŠÖ”*/
extern void gf_timer2_start(void);
extern void gf_timer2_stop(void);
extern void gf_option_integ_edge_select(unsigned char edge_state);

extern void gf_enable_timer0_interrupt(void);
extern void gf_disable_timer0_interrupt(void);

extern void gf_enable_timer1_interrupt(void);
extern void gf_disable_timer1_interrupt(void);


extern void gf_enable_ccp1_interrupt(void);
extern void gf_disable_ccp1_interrupt(void);

extern void gf_ccp2_set_pwmduty_8bit(unsigned char duty);

/**/


#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */


#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* XC_REGISTER_SETUP_H */

