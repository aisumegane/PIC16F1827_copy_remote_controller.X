/*
 * File:   register_setup.c
 * Author: 氷MEGANE
 *
 * Created on 2024/03/27, 21:17
 */


#include <xc.h>
#include <pic16f1827.h>

/*header*/
#include "grobal_macro.h"
#include "register_setup.h"



typedef union {
    struct {    /*定義順序注意(下位bitから詰める)*/
        unsigned char low_ccp1x          :1; /*bit 0     */
        unsigned char low_ccp1y          :1; /*bit 1     */
        unsigned char high_8bit          :8; /*bit 2?9  */
        unsigned char unusebit           :6; /*bit 10?15*/
    }bit_field;
    
    unsigned short u_short_pwm_duty;
} DUTY_REG_SET;


/**/
unsigned char register_setup_timer1_capture_edge;

/*global func prototype*/
void pic819_register_setup_byte_set(void);
void pic819_register_setup(void);

void oscillator_setup(void);
void option_reg_setup(void);
void apfcon_reg_setup(void);
void interrupt_setup(void);
void io_port_setup(void);
void ad_converter_setup(void);

void timer0_setup(void);
void timer1_setup(void);
void timer2_pwm_mode_setup(void);
void set_pwm_duty(unsigned short duty);

void gf_enable_timer0_interrupt(void);
void gf_disable_timer0_interrupt(void);

void timer1_compare_mode_setup(void);
void timer1_capture_mode_setup(void);

void gf_enable_ccp1_interrupt(void);
void gf_disable_ccp1_interrupt(void);

void gf_enable_timer1_interrupt(void);
void gf_disable_timer1_interrupt(void);

void gf_ccp2_set_pwmduty_8bit(unsigned char duty);



void timer1_capture_mode_edge_select(unsigned char edge_select);


/*global func*/
void gf_enable_interrupt(void);    /*割込み許可関数*/
void gf_disable_interrupt(void);   /*割込み禁止関数*/
void gf_timer1_start(void);        /*timer1 動作開始関数*/
void gf_timer1_stop(void);         /*timer1 動作停止関数*/
void gf_timer2_start(void);
void gf_timer2_stop(void);

void gf_option_integ_edge_select(unsigned char edge_state);



void pic819_register_setup(void)
{         
    /*割込み禁止*/
    INTCONbits.GIE = CLEAR;         // グローバル割り込み禁止
    INTCONbits.PEIE = CLEAR;        // リフェラル割込み禁止
    
    
    oscillator_setup();         /*内蔵発振器設定*/
    option_reg_setup();         /*オプション設定*/
    apfcon_reg_setup();         /*ピン配置設定*/
    interrupt_setup();          /*割込み設定*/
    io_port_setup();            /*IOポート設定*/
    ad_converter_setup();       /*ADコンバータ設定*/
    
    //timer0_setup();             /*timer0設定*/
    //timer1_setup();             /*timer1設定*/
    timer2_pwm_mode_setup();             /*timer2設定*/
    
    //timer1_compare_mode_setup();  
    timer1_capture_mode_setup();    /*timer1(RB0=CCP1) capture mode setup*/
}

void oscillator_setup(void)
{   /*if set to 32MHz, setup with PLLEN(config_bits),IRCF3:0(OSCCON),SPLLEN(OSCCON),FOSC(config_bits)*/
    /* OSCCON */
    OSCCONbits.SPLLEN       = SET;    /*4x PLL is enabled*/
    /*internal osccilator set to 32MHz HF*/ /**/
    OSCCONbits.IRCF3        = SET;
    OSCCONbits.IRCF2        = SET;
    OSCCONbits.IRCF1        = SET;
    OSCCONbits.IRCF0        = CLEAR;
    
    OSCCONbits.SCS1         = CLEAR;
    OSCCONbits.SCS0         = CLEAR;
}

void option_reg_setup(void)
{   /*OPTION_REG*//*datasheet P.19*//*WDTタイマ、ポート初期一部設定*/
    OPTION_REGbits.nWPUEN  = SET;    /* PORT-B Pull-up Enable bit (1:Disable / 0:Enable) */
    OPTION_REGbits.INTEDG  = SET;      /* RB0/INT pin Interrupt Edge Select bit (1:rising edge / 0:falling edge) */ /* 初回は信号待ち(0→1待ち)なので立ち上がりエッジ */
    OPTION_REGbits.TMR0CS  = CLEAR;    /* TMR0 Clock Source Select bit (1:T0CKI pin / 0:Internal (CKLO)) */
    OPTION_REGbits.TMR0SE  = CLEAR;    /* TMR0 Source Edge Select bit(1:high-to-low transition on T0CKI / 0:low-to-high transition on T0CKI) */
    OPTION_REGbits.PSA     = CLEAR;    /* Prescaler Assignment bit (1:assigned to WDT / 0:assigned to Timer0) */
    OPTION_REGbits.PS2     = SET;      /* TMR0 Prescaler Rate Select bit (PS2,1,0) */
    OPTION_REGbits.PS1     = SET;
    OPTION_REGbits.PS0     = SET;
}

void apfcon_reg_setup(void)
{
    APFCON0bits.RXDTSEL     = CLEAR;    /*RX/DT function:RB1,RB2 pin selection bit */
    APFCON0bits.SDO1SEL     = CLEAR;    /*ADO1 function:RB1,RB6 pin selection bit*/
    APFCON0bits.SS1SEL      = CLEAR;    /*SS1 function:RB5,RA5 pin selection bit*/
    APFCON0bits.P2BSEL      = CLEAR;    /*P2B function:RB7,RBA6 pin selection bit*/
    APFCON0bits.CCP2SEL     = SET;      /*CCP2/P2A function:RB6,RA7 pin selection bit*/       /*CCP2 set to RA7*/
    APFCON0bits.P1DSEL      = CLEAR;    /*P1D function:RB7.RA6 pin selection bit*/
    APFCON0bits.P1CSEL      = CLEAR;    /*P1C function:RB6,RA7 pin selection bit*/
    APFCON0bits.CCP1SEL     = SET;      /*CCP1/P1A function:RB3,RB0 selection bit*/         /*set to RB0*/
    
    APFCON1bits.TXCKSEL     = CLEAR;    /*TXCKSEL pin selection bit*/                       /*TX/CK set to RB2*/
}

void interrupt_setup(void)
{   /*datasheet P.89*/
    
    /*INTCON*/
    INTCONbits.GIE          = CLEAR;    /*割込みベクタが1つしかないのでここ禁止しとくだけにする*/   /*UTF8 - shiftJIS でgitがバグるので英語に直しとく*/
    INTCONbits.PEIE         = CLEAR;
    INTCONbits.TMR0IE       = CLEAR;      /*timer0 overflow interrupt enable bit*/
    INTCONbits.INTE         = CLEAR;      /*int external interrupt enable bit*/     /*conflict with CCP1 capture mode*/
    
    INTCONbits.IOCIE        = CLEAR;    /*interrupt-on-change enable bit:disable*/
    INTCONbits.TMR0IF       = CLEAR;    /*timer0 interrupt overflow flag bit*/
    INTCONbits.INTF         = CLEAR;    /*int external interrupt flag bit*/
    INTCONbits.IOCIF        = CLEAR;    /*interrupt-on-change interrupt flag bit*/
    
    
    /*PIE1*/
    PIE1bits.TMR1GIE        = CLEAR;    /*timer1 gate interrupt enable bit*/
    PIE1bits.ADIE           = CLEAR;    /*A/D converter interrupt enable bit*/
    PIE1bits.RCIE           = CLEAR;    /*USART transmit interrupt enable bit*/
    PIE1bits.TXIE           = CLEAR;    /*USART transmit interrupt enable bit*/
    PIE1bits.SSP1IE         = CLEAR;    /*synchronous serial port1 interrupt enable bit*/
    PIE1bits.CCP1IE         = CLEAR;    /*CCP1 interrupt enable bit*/
    PIE1bits.TMR2IE         = CLEAR;      /*TMR2 to PR2 match interrupt enable bit*/
    PIE1bits.TMR1IE         = CLEAR;    /*timer1 overflow interrupt enable bit*/
    
    /*PIE2*/
    PIE2bits.OSFIE          = CLEAR;    /*oscillator fail interrupt enable bit*/
    PIE2bits.C2IE           = CLEAR;    /*comparator c2 interrupt enable bit*/
    PIE2bits.C1IE           = CLEAR;    /*comparator c1 interrupt enable bit*/
    PIE2bits.EEIE           = CLEAR;    /*EEPROM write completion interrupt enable bit*/
    PIE2bits.BCL1IE         = CLEAR;    /*MSSP1 BUS collision interrupt enable bit*/
    PIE2bits.CCP2IE         = CLEAR;    /*CCP2 interrupt enable bit*/
    
    /*PIE3*/
    PIE3bits.TMR4IE         = CLEAR;    /*CCP4 interrupt enable bit*/
    PIE3bits.CCP3IE         = CLEAR;    /*CCP3 interrupt enable bit*/
    PIE3bits.TMR6IE         = CLEAR;    /*TMR6 to PR6 match interrupt enable bit*/
    PIE3bits.CCP4IE         = CLEAR;    /*TMR4 to PR4 match interrupt enable bit*/
    
    /*PIE4*/
    PIE4bits.BCL2IE         = CLEAR;    /*MSSP2 BUS collision interrupt enable bit*/
    PIE4bits.SSP2IE         = CLEAR;    /*master synchronous serial port 2 interrupt enable bit*/
    
    /*================================================================================================*/

    /*PIR1*/
    PIR1bits.TMR1GIF        = CLEAR;    /*Timer1 gate interrupt flag bit*/
    PIR1bits.ADIF           = CLEAR;    /*A/D converter interrupt flag bit*/
    PIR1bits.RCIF           = CLEAR;    /*USART receive interrupt flag bit*/
    PIR1bits.TXIF           = CLEAR;    /*USART transmit interrupt flag bit*/
    PIR1bits.SSP1IF         = CLEAR;    /*synchronous serial port1 (MSSP1) interrupt flag bit*/
    PIR1bits.CCP1IF         = CLEAR;    /*CCP1 interrupt flag bit*/
    PIR1bits.TMR2IF         = CLEAR;    /*timer2 to PR2 interrupt flag bit*/
    PIR1bits.TMR1IF         = CLEAR;    /*timer1 overflow interrupt flag bit*/
    
    /*PIR2*/
    PIR2bits.OSFIF          = CLEAR;    /*oscillator fail interrupt flag bit*/
    PIR2bits.C2IF           = CLEAR;    /*comparator C2 interrupt flag bit*/
    PIR2bits.C1IF           = CLEAR;    /*comparator C1 interrupt flag bit*/
    PIR2bits.EEIF           = CLEAR;    /*EEPROM Write Completion interrupt flag bit*/
    PIR2bits.BCL1IF         = CLEAR;    /*MSSP1 Bus collision interrupt flag bit*/
    PIR2bits.CCP2IF         = CLEAR;    /*CCP2 interrupt flag bit*/
    
    /*PIR3*/
    PIR3bits.CCP4IF         = CLEAR;    /*CCP4 interrupt flag bit*/
    PIR3bits.CCP3IF         = CLEAR;    /*CCP3 interrupt flag bit*/
    PIR3bits.TMR6IF         = CLEAR;    /*TMR6 to PR6 match interrupt flag bit*/
    PIR3bits.TMR4IF         = CLEAR;    /*TMR4 to PR4 match interrupt flag bit*/
    
    /*PIR4*/
    PIR4bits.BCL2IF         = CLEAR;    /*MSSP2 Bus collision interrupt flag bit*/
    PIR4bits.SSP2IF         = CLEAR;    /*master synchronous serial port 2 (MSSP2) interrupt flag bit*/
}

void io_port_setup(void)
{
    /*TRISA*/
    TRISAbits.TRISA0 = CLEAR;
    TRISAbits.TRISA1 = CLEAR;
    TRISAbits.TRISA2 = CLEAR;
    TRISAbits.TRISA3 = CLEAR;
    
    TRISAbits.TRISA4 = SET;
    TRISAbits.TRISA5 = SET;
    TRISAbits.TRISA6 = CLEAR;
    TRISAbits.TRISA7 = CLEAR;
    
    /*TRISB*/   /*1:入力 / 0:出力*/
    TRISBbits.TRISB0 = SET;
    TRISBbits.TRISB1 = SET;     /*I2C:SDA*/
    TRISBbits.TRISB2 = SET;
    TRISBbits.TRISB3 = SET;
    
    TRISBbits.TRISB4 = SET;     /*I2C:SCK*/
    TRISBbits.TRISB5 = CLEAR;
    TRISBbits.TRISB6 = SET;
    TRISBbits.TRISB7 = SET;
    
    /*=====================================================*/
    
    /*PORTA*/   /*1:Hレベル出力 / 0:Lレベル出力*/ 
    PORTAbits.RA0 = CLEAR;
    PORTAbits.RA1 = CLEAR;
    PORTAbits.RA2 = CLEAR;
    PORTAbits.RA3 = CLEAR;
    
    PORTAbits.RA4 = CLEAR;
    PORTAbits.RA5 = CLEAR;
    PORTAbits.RA6 = CLEAR;
    PORTAbits.RA7 = CLEAR;
    
    /*PORTB*/   /*1:Hレベル出力 / 0:Lレベル出力*/
    PORTBbits.RB0 = CLEAR;
    PORTBbits.RB1 = CLEAR;
    PORTBbits.RB2 = CLEAR;
    PORTBbits.RB3 = CLEAR;
    
    PORTBbits.RB4 = CLEAR;
    PORTBbits.RB5 = CLEAR;
    PORTBbits.RB6 = CLEAR;
    PORTBbits.RB7 = CLEAR;
    
    /*WPUA*/
    WPUAbits.WPUA5          = CLEAR;    /*weak pull-up RA5 control bit*/
    
    /*WPUB*/
    WPUBbits.WPUB7          = CLEAR;    /*weak pull-up register bits : disabled*/
    WPUBbits.WPUB6          = CLEAR;    /*weak pull-up register bits : disabled*/
    WPUBbits.WPUB5          = CLEAR;    /*weak pull-up register bits : disabled*/
    WPUBbits.WPUB4          = CLEAR;    /*weak pull-up register bits : disabled*/
    
    WPUBbits.WPUB3          = CLEAR;    /*weak pull-up register bits : disabled*/
    WPUBbits.WPUB2          = CLEAR;    /*weak pull-up register bits : disabled*/
    WPUBbits.WPUB1          = CLEAR;    /*weak pull-up register bits : disabled*/
    WPUBbits.WPUB0          = CLEAR;    /*weak pull-up register bits : disabled*/
    
    /*ANSELA*/
    ANSELAbits.ANSA4        = CLEAR;    /*analog / digital mode selection:digital*/
    ANSELAbits.ANSA3        = CLEAR;    /*analog / digital mode selection:digital*/
    ANSELAbits.ANSA2        = CLEAR;    /*analog / digital mode selection:digital*/
    ANSELAbits.ANSA1        = CLEAR;    /*analog / digital mode selection:digital*/
    ANSELAbits.ANSA0        = CLEAR;    /*analog / digital mode selection:digital*/
    
    /*ANSELB*/
    ANSELBbits.ANSB7        = CLEAR;    /*analog / digital mode selection:digital*/
    ANSELBbits.ANSB6        = CLEAR;    /*analog / digital mode selection:digital*/
    ANSELBbits.ANSB5        = CLEAR;    /*analog / digital mode selection:digital*/
    ANSELBbits.ANSB4        = CLEAR;    /*analog / digital mode selection:digital*/
    
    ANSELBbits.ANSB3        = CLEAR;    /*analog / digital mode selection:digital*/
    ANSELBbits.ANSB2        = CLEAR;    /*analog / digital mode selection:digital*/
    ANSELBbits.ANSB1        = CLEAR;    /*analog / digital mode selection:digital*/
}

void ad_converter_setup(void)
{
    /*ADCON0*/
    ADCON0bits.CHS4         = SET;      /*analog channel select bit*/
    ADCON0bits.CHS3         = SET;      /*no channel connected*/
    ADCON0bits.CHS2         = SET;
    ADCON0bits.CHS1         = CLEAR;
    ADCON0bits.CHS0         = SET;
    
    ADCON0bits.GO_nDONE     = CLEAR;    /*A/D conversion status bit*/
    ADCON0bits.ADON         = CLEAR;    /*ADC enable bit*/
    
    /*ADCON1*/
    ADCON1bits.ADFM         = SET;      /*A/D result format select bit*/ /*右(16bitのうち10bit使用、下位ビット寄せて上位は使わない)*/
    ADCON1bits.ADCS2        = CLEAR;    /*A/D conversion clock select bit*/
    ADCON1bits.ADCS2        = CLEAR;    /*A/D conversion clock select bit*/
    ADCON1bits.ADCS2        = CLEAR;    /*A/D conversion clock select bit*/
    
    ADCON1bits.ADNREF       = CLEAR;    /*A/D negative voltage reference configuration bit*/
    ADCON1bits.ADPREF1      = CLEAR;    /*A/D positive voltage reference configuration bits*/
    ADCON1bits.ADPREF0      = CLEAR;    /*A/D positive voltage reference configuration bits*/
    
    /*ADRESH*/
    ADRESH = CLEAR;                     /*ADC result register bits*/    /*init*/
    
    /*ADRESL*/
    ADRESL = CLEAR;                     /*ADC result register bits*/    /*init*/
    
}

void timer0_setup(void)
{   /*timer0についてはオプションで設定*/
    option_reg_setup();
}


#if 0
void timer1_setup(void)
{
#if 0
    /*CCPモード設定*/
    /*T1CON*/ /*datasheet P.59*/
    /*T1CON*/
    T1CONbits.T1CKPS1 = SET;      /*(Fsoc/4)の8分周 */ /* 2MHz/8 = 250kHz(0.000,004 sec) */
    T1CONbits.T1CKPS0 = SET;
    T1CONbits.T1OSCEN = CLEAR;      /*for external clock*/        
    T1CONbits.nT1SYNC = SET;        /*for external clock*/
    T1CONbits.TMR1CS = CLEAR;       /*Internal clock*/
    T1CONbits.TMR1ON = CLEAR;

    /*CCP1CON*/ /*datasheet P.67*/
    /*CCP1CONbits_t.CCP1X = SET/CLEAR;*/
    /*CCP1CONbits_t.CCP1Y = SET/CLEAR;*/
    CCP1CONbits.CCP1M3 = SET;
    CCP1CONbits.CCP1M2 = CLEAR;
    CCP1CONbits.CCP1M1 = SET;
    CCP1CONbits.CCP1M0 = SET;
    
    /*CCPR1H,CCPR1L*/
    /* 動作周波数設定 = 50ms = 0.000,004 sec * 12500(0x30D4) */
    /* 0までロールオーバー(MAXの次に0を数えて1カウントする)ので、1引く */
    CCPR1H = 0x30;
    CCPR1L = 0xD3;  /* Timer0でも十分 */   
#endif
    /*PWMモード設定*/
    /*T1CON*/ /*datasheet P.59*/
    /*T1CON*/
    T1CONbits.T1CKPS1 = SET;      /*(Fsoc/4)の8分周 */ /* 2MHz/8 = 250kHz(0.000,004 sec) */
    T1CONbits.T1CKPS0 = SET;
    T1CONbits.T1OSCEN = CLEAR;      /*for external clock*/        
    T1CONbits.nT1SYNC = SET;        /*for external clock*/
    T1CONbits.TMR1CS = CLEAR;       /*Internal clock*/
    T1CONbits.TMR1ON = CLEAR;

    /*CCP1CON*/ /*datasheet P.67*/
    /*CCP1CONbits_t.CCP1X = SET/CLEAR;*/
    /*CCP1CONbits_t.CCP1Y = SET/CLEAR;*/
    CCP1CONbits.CCP1M3 = SET;
    CCP1CONbits.CCP1M2 = SET;
    CCP1CONbits.CCP1M1 = SET;   /*無効*/
    CCP1CONbits.CCP1M0 = SET;   /*無効*/
    
    /*CCPR1H,CCPR1L*/
    /* 動作周波数設定 = 50ms = 0.000,004 sec * 12500(0x30D4) */
    /* 0までロールオーバー(MAXの次に0を数えて1カウントする)ので、1引く */
    //CCPR1H = 0x30;  
    

#if 1
    CCPR1L = 0x11;
    CCP1CONbits.CCP1X = CLEAR;
    CCP1CONbits.CCP1Y = CLEAR;
#else
    set_pwm_duty(100);
#endif
}

#endif

void timer1_compare_mode_setup(void)
{   /*compareモード設定*/
    /*T1CON*/
    T1CONbits.TMR1CS1       = CLEAR;    /*timer1 clock source select bits*/ /*Fosc/4*/
    T1CONbits.TMR1CS0       = CLEAR;    /*timer1 clock source select bits*/
    
    T1CONbits.T1CKPS1       = SET;      /*timer1 input clock prescale select bits*/
    T1CONbits.T1CKPS0       = SET;      /*(Fsoc/4)の8分周 */ /* 8MHz/8 = 1MHz(0.000,001 sec) */
    
    T1CONbits.T1OSCEN       = CLEAR;    /*LP oscillator enable control bit*/
    T1CONbits.nT1SYNC       = SET;      /*timer1 external clock input synchronization control bit*/
    T1CONbits.TMR1ON        = CLEAR;    /*timer1 on bit*/

    T1GCONbits.TMR1GE       = CLEAR;    /*timer1 gate enable bit*/
    T1GCONbits.T1GPOL       = SET;      /*timer1 gate polarity bit*/
    T1GCONbits.T1GTM        = CLEAR;    /*timer1 gate toggle mode bit*/ /*disable*/
    T1GCONbits.T1GSPM       = CLEAR;    /*timer1 gate single-pulse mode bit*/
    T1GCONbits.T1GGO        = CLEAR;    /*timer1 gate single-pulse acquisition status bit*/
/*  T1GCONbits.T1GVAL       = CLEAR;*/  /*timer1 gate current state bit*/    /*!read only!*/
    T1GCONbits.T1GSS1       = CLEAR;    /*timer1 gate source select bits*/  /*timer0 overflow output*/
    T1GCONbits.T1GSS0       = CLEAR;    /*timer1 gate source select bits*/
    
    /*CCP1CON*/
    CCP1CONbits.P1M1        = CLEAR;    /*enhanced PWM output configuration bits*/
    CCP1CONbits.P1M0        = CLEAR;    /*enhanced PWM output configuration bits*/  /*single output*/
    
    CCP1CONbits.DC1B1       = CLEAR;    /*PWM Duty cycle least significant bits*/
    CCP1CONbits.DC1B0       = CLEAR;    /*PWM Duty cycle least significant bits*/   /*if caputure/compare mode, unused*/
    
    CCP1CONbits.CCP1M3      = SET;      /*ECCPx mode select bits*/  /*compare mode: generate software interrupt only*/
    CCP1CONbits.CCP1M2      = CLEAR;
    CCP1CONbits.CCP1M1      = SET;
    CCP1CONbits.CCP1M0      = CLEAR;
    
    
    
            
    /*CCPR1H,CCPR1L*/
    /* 動作周波数設定 = 50ms = 0.000,004 sec * 25000(0x61AB) */
    /* 0までロールオーバー(MAXの次に0を数えて1カウントする)ので、1引く */
    CCPR1H = 0xFF;
    CCPR1L = 0xFF;  /* Timer0でも十分 */   
}

void timer1_capture_mode_setup(void)
{   /*captureモード設定*/
    /*T1CON*/
    T1CONbits.TMR1CS1       = CLEAR;    /*timer1 clock source select bits*/ /*Fosc/4*/
    T1CONbits.TMR1CS0       = CLEAR;    /*timer1 clock source select bits*/
    
    T1CONbits.T1CKPS1       = SET;      /*timer1 input clock prescale select bits*/
    T1CONbits.T1CKPS0       = SET;      /*(Fsoc/4)の8分周 */ /* 8MHz/8 = 1MHz(0.000,001 sec) */
    
    T1CONbits.T1OSCEN       = CLEAR;    /*LP oscillator enable control bit*/
    T1CONbits.nT1SYNC       = SET;      /*timer1 external clock input synchronization control bit*/
    T1CONbits.TMR1ON        = CLEAR;    /*timer1 on bit*/

    T1GCONbits.TMR1GE       = CLEAR;    /*timer1 gate enable bit*/
    T1GCONbits.T1GPOL       = SET;      /*timer1 gate polarity bit*/
    T1GCONbits.T1GTM        = CLEAR;    /*timer1 gate toggle mode bit*/ /*disable*/
    T1GCONbits.T1GSPM       = CLEAR;    /*timer1 gate single-pulse mode bit*/
    T1GCONbits.T1GGO        = CLEAR;    /*timer1 gate single-pulse acquisition status bit*/
/*  T1GCONbits.T1GVAL       = CLEAR;*/  /*timer1 gate current state bit*/    /*!read only!*/
    T1GCONbits.T1GSS1       = CLEAR;    /*timer1 gate source select bits*/  /*timer0 overflow output*/
    T1GCONbits.T1GSS0       = CLEAR;    /*timer1 gate source select bits*/
    
    /*CCP1CON*/
    CCP1CONbits.P1M1        = CLEAR;    /*enhanced PWM output configuration bits*/
    CCP1CONbits.P1M0        = CLEAR;    /*enhanced PWM output configuration bits*/  /*single output*/
    
    CCP1CONbits.DC1B1       = CLEAR;    /*PWM Duty cycle least significant bits*/
    CCP1CONbits.DC1B0       = CLEAR;    /*PWM Duty cycle least significant bits*/   /*if caputure/compare mode, unused*/
#if 1
    CCP1CONbits.CCP1M3      = SET;      /*ECCPx mode select bits*/  /*compare mode: generate software interrupt only*/
    CCP1CONbits.CCP1M2      = CLEAR;
    CCP1CONbits.CCP1M1      = SET;
    CCP1CONbits.CCP1M0      = CLEAR;
#endif
    
    timer1_capture_mode_edge_select(CAPTURE_RISING_EDGE);

    /*CCPR1H,CCPR1L*/   /*capture data save register*/
    CCPR1H = 0x00;      /*initialize*/
    CCPR1L = 0x00;      /*initialize*/
}

void timer1_capture_mode_edge_select(unsigned char edge_select)
{
    if(edge_select == CAPTURE_RISING_EDGE)
    {
        CCP1CONbits.CCP1M3      = CLEAR;      /*ECCPx mode select bits*/  /*capture mode:every rising edge*/
        CCP1CONbits.CCP1M2      = SET;
        CCP1CONbits.CCP1M1      = CLEAR;
        CCP1CONbits.CCP1M0      = SET;
        register_setup_timer1_capture_edge  = SET;
    }
    else /*edge_select == CCAPTURE_FALLING_EDGE;*/
    {
        CCP1CONbits.CCP1M3      = CLEAR;      /*ECCPx mode select bits*/  /*capture mode:every falling edge*/
        CCP1CONbits.CCP1M2      = SET;
        CCP1CONbits.CCP1M1      = CLEAR;
        CCP1CONbits.CCP1M0      = CLEAR;
        register_setup_timer1_capture_edge  = CLEAR;
    }

}


void timer2_pwm_mode_setup()
{
    CCP2CONbits.P2M1        = CLEAR;
    CCP2CONbits.P2M0        = CLEAR;
    
//  CCP2CONbits.DC2B1       = CLEAR;
//  CCP2CONbits.DC2B0       = CLEAR;
    
    CCP2CONbits.CCP2M3      = SET;      /*PWM mode*/
    CCP2CONbits.CCP2M2      = SET;
    CCP2CONbits.CCP2M1      = CLEAR;
    CCP2CONbits.CCP2M0      = CLEAR;
    
    
    T2CONbits.T2OUTPS3   = CLEAR;   /*postscale:1:1*/
    T2CONbits.T2OUTPS2   = CLEAR;
    T2CONbits.T2OUTPS1   = CLEAR;
    T2CONbits.T2OUTPS0   = CLEAR;
    
    T2CONbits.T2CKPS1   = CLEAR;      /*prescale=1:1  = (32/4)/1 = 8MHz*/
    T2CONbits.T2CKPS0   = CLEAR;
    
    
    /*PWM周期(period)設定：210カウント:約38kHz*/
    PR2 = 0xCA;
    
    //CCPR2L =0x46;
    CCPR2L =0x00;
    
    /*ここの10bit拡張がいまいちわからない*/
    CCP2CONbits.DC2B1 = CLEAR;
    CCP2CONbits.DC2B0 = CLEAR;
    
    T2CONbits.TMR2ON = CLEAR;
    //T2CONbits.TMR2ON = CLEAR;
}

void set_pwm_duty(unsigned short duty)
{   /*0x0000 ? 0x03FF の範囲でduty指定*/
    DUTY_REG_SET pwm_duty;
    
    /*on-dutyを10bitで設定する:上位8bitがCCPR1L、下位2bitがCCP1XとCCP1L、なかなかキモイ*/
#if 0
    duty >>1;
    CCPR1L = (unsigned char)duty;
    CCP1CONbits.CCP1X = CLEAR;
    CCP1CONbits.CCP1Y = CLEAR;
#endif
    
#if 0
    if(duty > 0x3FF)
    {   /*オーバーフロー防止*/
        duty = 0x3FF;
    }
    
    //duty = duty << 2;
    
    /* 100%duty = 0x3FF (10bit MAX) */
    pwm_duty.u_short_pwm_duty = duty;
            
    CCPR1L = pwm_duty.bit_field.high_8bit;
    CCP1CONbits.CCP1X = pwm_duty.bit_field.low_ccp1x;
    CCP1CONbits.CCP1Y = pwm_duty.bit_field.low_ccp1y;
#endif
#if 0
    CCP1CONbits.CCP1X = duty & 0b0000000000000001;
    CCP1CONbits.CCP1Y = duty & 0b0000000000000010;
    
    CCPR1L = duty > 2;
#endif
}

void gf_enable_interrupt(void)
{   /*interrup.cでは使えない(リエントラント?)*/
    INTCONbits.GIE = SET;         // グローバル割り込み許可
    INTCONbits.PEIE = SET;        // リフェラル割込み許可 
}

void gf_disable_interrupt(void)
{   /*interrupt.cでは使えない(リエントラント?)*/
    INTCONbits.GIE = CLEAR;         // グローバル割り込み禁止
    INTCONbits.PEIE = CLEAR;        // リフェラル割込み禁止
}

void gf_enable_ccp1_interrupt(void)
{
    PIE1bits.CCP1IE         = SET;      /*CCP1 interrupt enable bit*/
}

void gf_disable_ccp1_interrupt(void)
{
    PIE1bits.CCP1IE         = CLEAR;    /*CCP1 interrupt enable bit*/
}

void gf_enable_timer1_interrupt(void)
{
    PIE1bits.TMR1IE         = SET;      /*timer1 overflow interrupt enable bit*/
}

void gf_disable_timer1_interrupt(void)
{
    PIE1bits.TMR1IE         = CLEAR;    /*timer1 overflow interrupt enable bit*/
}


void gf_enable_timer0_interrupt(void)
{
    INTCONbits.TMR0IE         = SET;      /*timer1 overflow interrupt enable bit*/
}

void gf_disable_timer0_interrupt(void)
{
    INTCONbits.TMR0IE         = CLEAR;    /*timer1 overflow interrupt enable bit*/
}


void gf_timer1_start(void)
{   /*初回起動で割込み要求等がないか確認すること*/
    T1CONbits.TMR1ON = SET;
}

void gf_timer1_stop(void)
{   /*初回起動で割込み要求等がないか確認すること*/
    T1CONbits.TMR1ON = CLEAR;
}

void gf_timer2_start()
{
    T2CONbits.TMR2ON = SET;
}

void gf_timer2_stop()
{
    T2CONbits.TMR2ON = CLEAR;
}

void gf_ccp2_set_pwmduty_8bit(unsigned char duty)
{
    CCPR2L = duty;
}

void gf_option_integ_edge_select(unsigned char edge_state)
{
    if(edge_state == SET)
    {
        OPTION_REGbits.INTEDG  = SET;      /* RB0/INT pin Interrupt Edge Select bit (1:rising edge / 0:falling edge) */ /* 初回は信号待ち(0→1待ち)なので立ち上がりエッジ */
    }
    else if(edge_state == CLEAR)
    {
        OPTION_REGbits.INTEDG  = CLEAR;
    }
}