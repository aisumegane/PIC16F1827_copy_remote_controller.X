/* 
 * File:   lcdisplay_textdata.h
 * Author: •XMEGANE
 *
 * Created on 2024/05/11, 13:33
 */

#ifndef LCDISPLAY_TEXTDATA_H
#define	LCDISPLAY_TEXTDATA_H

#ifdef	__cplusplus
extern "C" {
#endif
    
    /*The 17th data should be dummy data.*/
    /*--------------------------------------------{ '0', '1', '2', '3', '4', '5', '6', '7', '8', '9','10','11','12','13','14','15',0x00}*/
    const unsigned char now_copying[17]         = { 'N', 'o', 'w', ' ', 'C', 'o', 'p', 'y', 'i', 'n', 'g', '.', '.', '.', '.',0x00,0x00};
    const unsigned char transmitting[17]        = { 'N', 'o', 'w', ' ', 'T', 'r', 'a', 'n', 's', 'm', 'i', 't', '.', '.', '.',0x00,0x00};
    const unsigned char copydata_choose[17]     = { 'C', 'o', 'p', 'y', ' ', 'd', 'a', 't', 'a', ' ', '-', '>', 'S', 'W', '1',0x00,0x00};
    const unsigned char senddata_choose[17]     = { 'S', 'e', 'n', 'd', ' ', 'd', 'a', 't', 'a', ' ', '-', '>', 'S', 'W', '2',0x00,0x00};

#ifdef	__cplusplus
}
#endif

#endif	/* LCDISPLAY_TEXTDATA_H */

