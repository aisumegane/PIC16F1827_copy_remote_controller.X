/*
 * File:   interrupt.c
 * Author: •XMEGANE
 *
 * Created on 2024/04/14, 23:38
 */

#ifndef SENDDATA_H
#define	SENDDATA_H

#ifdef	__cplusplus
extern "C" {
#endif
    
extern unsigned char senddata_send_end_flag;

extern volatile unsigned char senddata_enddata_index;
extern volatile unsigned char senddata_endary_index;
    
extern void senddata_main(void);
extern void senddata_debug_test(void);

#ifdef	__cplusplus
}
#endif

#endif	/* SENDDATA_H */

