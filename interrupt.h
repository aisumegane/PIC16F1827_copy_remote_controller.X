/* 
 * File:   interrupt.h
 * Author: �XMEGANE
 *
 * Created on 2024/04/02, 17:43
 */

#ifndef INTERRUPT_H
#define	INTERRUPT_H

#ifdef	__cplusplus
extern "C" {
#endif

// Timer0 �̃I�[�o�[�t���[���J�E���g����
extern unsigned int cnt;
extern unsigned char interrupt_ccp1if_defend_cmp;   /*�ݒ�K�p��̏��񊄍��݂�h�~����֐�*/
extern unsigned char interrupt_ccp2if_defend_cmp;

#ifdef	__cplusplus
}
#endif

#endif	/* INTERRUPT_H */

