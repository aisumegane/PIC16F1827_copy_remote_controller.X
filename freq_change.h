/*
 * File:   register_setup.c
 * Author: •XMEGANE
 *
 * Created on 2024/05/24, 22:32
 */

#ifndef FREQ_CHANGE_H
#define	FREQ_CHANGE_H

#ifdef	__cplusplus
extern "C" {
#endif

#define     PWM_ALL_INDEX   16

#define     PWM_38KHZ   0
#define     PWM_39KHZ   1
#define     PWM_40KHZ   2
#define     PWM_41KHZ   3
#define     PWM_42KHZ   4
#define     PWM_43KHZ   5
#define     PWM_44KHZ   6
#define     PWM_45KHZ   7
#define     PWM_46KHZ   8
#define     PWM_47KHZ   9
#define     PWM_48KHZ   10
#define     PWM_49KHZ   11
#define     PWM_50KHZ   12
#define     PWM_51KHZ   13
#define     PWM_52KHZ   14
#define     PWM_53KHZ   15
    
    
unsigned char fc_current_freq_setup;

extern void freq_change_init(void);
extern void freq_change_main(void);


#ifdef	__cplusplus
}
#endif

#endif	/* FREQ_CHANGE_H */

