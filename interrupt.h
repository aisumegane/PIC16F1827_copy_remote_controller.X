/* 
 * File:   interrupt.h
 * Author: 氷MEGANE
 *
 * Created on 2024/04/02, 17:43
 */

#ifndef INTERRUPT_H
#define	INTERRUPT_H

#ifdef	__cplusplus
extern "C" {
#endif

// Timer0 のオーバーフローをカウントする
extern unsigned int cnt;
extern unsigned char interrupt_ccp1if_defend_cmp;   /*設定適用後の初回割込みを防止する関数*/
extern unsigned char interrupt_ccp2if_defend_cmp;

#ifdef	__cplusplus
}
#endif

#endif	/* INTERRUPT_H */

