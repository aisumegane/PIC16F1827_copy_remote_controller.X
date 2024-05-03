/*
 * File:   interrupt.c
 * Author: 氷MEGANE
 *
 * Created on 2024/04/14, 23:38
 */

#ifndef COPYDATA_H
#define	COPYDATA_H

#ifdef	__cplusplus
extern "C" {
#endif
    
#define COPYDATA_DATA_INDEX     40
#define COPYDATA_ARY_INDEX      6

extern unsigned char copydata_led_data_ary0[40]; /* LED信号保存用配列 */
extern unsigned char copydata_led_data_ary1[40]; /* LED信号保存用配列 */
extern unsigned char copydata_led_data_ary2[40]; /* LED信号保存用配列 */
extern unsigned char copydata_led_data_ary3[40]; /* LED信号保存用配列 */
extern unsigned char copydata_led_data_ary4[40]; /* LED信号保存用配列 */
extern unsigned char copydata_led_data_ary5[40]; /* LED信号保存用配列 */
    
extern volatile unsigned char copydata_copy_end_flag;    /*copy success*/
extern volatile unsigned char copydata_copy_fail_flag;   /*copy failed*/
extern volatile unsigned char copydata_copy_start_flag;  /*初回INTE割込み発生・コピー開始フラグ*/
extern volatile unsigned char copydata_ex_interrupt_go_flag;    /*割込み用*/

extern volatile unsigned char copydata_copy_end_req;



extern unsigned char led_data_ary_index;
extern unsigned char led_data_index;


extern void copydata_main(void);                    /*main flag reset & set*/
extern void copydata_1byte_copy_interrupt(void);    /*main data copy function*/

extern void copydata_set_copy_end_req(void);
extern void copydata_clear_copy_end_flag(void);

extern void copydata_debug_test(void);

#ifdef	__cplusplus
}
#endif

#endif	/* COPYDATA_H */

