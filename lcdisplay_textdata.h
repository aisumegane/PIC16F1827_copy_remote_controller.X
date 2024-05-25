/* 
 * File:   lcdisplay_textdata.h
 * Author: 氷MEGANE
 *
 * Created on 2024/05/11, 13:33
 */

#ifndef LCDISPLAY_TEXTDATA_H
#define	LCDISPLAY_TEXTDATA_H

#ifdef	__cplusplus
extern "C" {
#endif
    
#include "freq_change.h"

/*The 17th data should be dummy data.*/
/*--------------------------------------------{ '0', '1', '2', '3', '4', '5', '6', '7', '8', '9','10','11','12','13','14','15',0x00}*/
const unsigned char lcdisplay_all_clear[17] = { ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',' ',0x00};   /*lcd_clearコマンドより遅い*/

const unsigned char now_copying[17]         = { 'N', 'O', 'W', ' ', 'C', 'O', 'P', 'Y', 'I', 'N', 'G', '.', '.', '.', '.',0x00,0x00};
const unsigned char transmitting[17]        = { 'N', 'o', 'w', ' ', 'T', 'r', 'a', 'n', 's', 'm', 'i', 't', '.', '.', '.',0x00,0x00};
const unsigned char copydata_choose[17]     = { 'C', 'O', 'P', 'Y', ' ', 'D', 'A', 'T', 'A', ' ',0x7E, ' ', 'S', 'W', '1',0x00,0x00};
const unsigned char senddata_choose[17]     = { 'S', 'E', 'N', 'D', ' ', 'D', 'A', 'T', 'A', ' ',0x7E, ' ', 'S', 'W', '2',0x00,0x00};

const unsigned char awaiting_signal[17]     = { 'A', 'W', 'A', 'I', 'T', 'I', 'N', 'G', ' ', 'S', 'I', 'G', 'N', 'A', 'L',0x00,0x00};
const unsigned char Transmitting[17]        = { 'T', 'R', 'A', 'N', 'S', 'M', 'I', 'T', 'T', 'I', 'N', 'G', '.', '.', '.',0x00,0x00};

const unsigned char copy_complete[17]       = { 'C', 'O', 'P', 'Y', ' ', 'C', 'O', 'M', 'P', 'L', 'E', 'T', 'E', '!', '!', '!',0x00};
const unsigned char send_complete[17]       = { 'S', 'E', 'N', 'D', ' ', 'C', 'O', 'M', 'P', 'L', 'E', 'T', 'E', '!', '!', '!',0x00};

const unsigned char nodata_available[17]    = { 'N', 'O', 'D', 'A', 'T', 'A', ' ', 'A', 'V', 'A', 'I', 'L', 'A', 'B', 'L', 'E',0x00};
const unsigned char please_copy[17]         = { '!', '!', 'P', 'L', 'E', 'A', 'S', 'E', ' ', 'C', 'O', 'P', 'Y', '!', '!',0x00,0x00};

const unsigned char carry_freq_setup[17]    = { 'C', 'A', 'R', 'R', 'Y', ' ', 'F', 'R', 'E', 'Q', ' ', 'S', 'E', 'T', 'U', 'P',0x00};
const unsigned char freq_1khz_up[17]        = { 'F', 'R', 'E', 'Q', ' ', '1', 'k', 'H', 'z', ' ', 'U', 'P', '!', '!', '!',0x00,0x00};
const unsigned char freq_set_to_min[17]     = { 'F', 'R', 'E', 'Q', ' ', 'S', 'E', 'T', ' ', 'T', 'O', ' ', 'M', 'I', 'N', '!',0x00};

const unsigned char carry_freq[17]          = { 'C', 'A', 'R', 'R', 'Y', ' ', 'F', 'R', 'E', 'Q', ':',0x00,0x00,0x00,0x00,0x00,0x00};

/*--------------------------------------------{ '0', '1', '2', '3', '4', '5', '6', '7', '8', '9','10','11','12','13','14','15',0x00}*/
/*---------------------------------------------------------------------------------------------------, '1', '2', '3', '4', '5',0x00}*/
const unsigned char freq_setting[PWM_ALL_INDEX][6] = {{ '3', '8','k','H','z',0x00},     /*1*/   /*!基本0x00で埋めていても上書きされちゃう☆*/
                                                      { '3', '9','k','H','z',0x00},     /*2*/
                                                      { '4', '0','k','H','z',0x00},     /*3*/
                                                      { '4', '1','k','H','z',0x00},     /*4*/
                                                      { '4', '2','k','H','z',0x00},     /*5*/
                                                      { '4', '3','k','H','z',0x00},     /*6*/
                                                      { '4', '4','k','H','z',0x00},     /*7*/
                                                      { '4', '5','k','H','z',0x00},     /*8*/
                                                      { '4', '6','k','H','z',0x00},     /*9*/
                                                      { '4', '7','k','H','z',0x00},     /*10*/
                                                      { '4', '8','k','H','z',0x00},     /*11*/
                                                      { '4', '9','k','H','z',0x00},     /*12*/
                                                      { '5', '0','k','H','z',0x00},     /*13*/
                                                      { '5', '1','k','H','z',0x00},     /*14*/
                                                      { '5', '2','k','H','z',0x00},     /*15*/
                                                      { '5', '3','k','H','z',0x00}};    /*16*/

/*単一文字*/
const unsigned char right_allow[2] ={0x7E,0x00};

#ifdef	__cplusplus
}
#endif

#endif	/* LCDISPLAY_TEXTDATA_H */

