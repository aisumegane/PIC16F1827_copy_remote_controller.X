/* 
 * File:   switch.h
 * Author: HGS01
 *
 * Created on 2024/04/11, 18:42
 */

#ifndef PERIPHERAL_IN_H
#define	PERIPHERAL_IN_H

#ifdef	__cplusplus
extern "C" {
#endif

    extern unsigned char powersw_state;
    extern unsigned char powersw_state_change;
    
    extern unsigned char copysw_state;
    extern unsigned char copysw_state_change;
    
    extern unsigned char sendsw_state;
    extern unsigned char sendsw_state_change;
    
    
    extern void peripheral_in_init(void);
    extern void peripheral_in_main(void);


#ifdef	__cplusplus
}
#endif

#endif	/* SWITCH_H */

