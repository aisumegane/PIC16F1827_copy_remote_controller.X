/*
 * File:   i2c.c
 * Author: 氷MEGANE
 * SCK1,SDA1を用いてI2Cを行う
 * Created on 2024/05/09, 21:17
 */

#ifndef LCDISPLAY_H
#define	LCDISPLAY_H

#ifdef	__cplusplus
extern "C" {
#endif
    
    extern void lcdisplay_init(void);
    extern void lcdisplay_seq_change_write_main(void);


#ifdef	__cplusplus
}
#endif

#endif	/* LCDISPLAY_H */

