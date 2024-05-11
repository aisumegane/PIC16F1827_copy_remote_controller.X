/*
 * File:   i2c.c
 * Author: •XMEGANE
 *
 * Created on 2024/05/08, 21:17
 */


#ifndef I2C_H
#define	I2C_H

#ifdef	__cplusplus
extern "C" {
#endif

    
extern void i2c_master_init(void);
extern void i2c_master_transmit_cmp_wait(void);
extern void i2c_master_transmit_start(void);
extern void i2c_master_transmit_stop(void);
extern void i2c_master_write(unsigned char transport_data);



#ifdef	__cplusplus
}
#endif

#endif	/* I2C_H */

