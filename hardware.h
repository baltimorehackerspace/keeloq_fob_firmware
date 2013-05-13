#ifndef _HARDWARE_H_
#define _HARDWARE_H_

#define USE_T39A

/** MCU oscillator frequency. Used in PICC __delay macro. */
#define _XTAL_FREQ  8000000

#define CTRL        CTRL_OUT
#define CTRL_OUT    LATAbits.LATA5
#define CTRL_DIR    TRISAbits.TRISA5

#define DATA_OUT    LATAbits.LATA2
#define DATA_DIR    TRISAbits.TRISA2

#define SW1_IN      PORTAbits.RA4
#define SW1_DIR     TRISAbits.TRISA4
#define SW1_IOC     IOCAPbits.IOCAP4
#define SW1_ANS     ANSELAbits.ANSA4
#define SW1_WPU     WPUAbits.WPUA4


#define output_pin	0
#define input_pin	1

#define LED_OFF     1
#define LED_ON      0

#define LED         LED_OUT
#define LED_OUT     LATAbits.LATA0
#define LED_DIR     TRISAbits.TRISA0

#define SW3_IN      PORTAbits.RA3
#define SW3_DIR     TRISAbits.TRISA3
#define SW3_WPU     WPUAbits.WPUA3
#define SW3_IOC     IOCANbits.IOCAN3

#define SW4_IN      PORTAbits.RA1
#define SW4_DIR     TRISAbits.TRISA1
#define SW4_WPU     WPUAbits.WPUA1
#define SW4_IOC     IOCANbits.IOCAN1

/* Data rate configuration */
#if(_XTAL_FREQ == 8000000ul)
    #define TMR2_PRESCALER      0b01
    #define PWM_ON              100
    #define PWM_PERIOD          (PWM_ON+1)
#else
    #error unsupported system clock frequency
#endif

// Choose the fundamental frequency
#if 1
    #define USE_434MHZ
#elif 1
    #define USE_868MHZ
#else
    #define USE_915MHZ
#endif

// Choose the modulation
#if 0
    #define USE_OOK
#else
    #define USE_FSK
#endif

#if defined(USE_T48A)
    #include "T48A.h"
#elif defined(USE_T39A)
    /* Choose the frequency of the reference oscillator. Typically 24 or
     * 26 MHz. This is used to determine the correct BAND setting for the
     * transmitter configuration. */
    #if 0
        #define RF_XTAL 24000000
    #else
        #define RF_XTAL 26000000
    #endif

    #include "T39A.h"
#else
    #error need to define USE_T48A or USE_T39A
#endif

// Determine the transmitter configuration settings
#if defined(USE_T48A)
    #warning T48A configuration does not yet adjust to other #define settings like T39A does.
    #define T48A_APP_CONFIG (T48_POWER_AND_GO | T48_FREQ_43392 | T48_MOD_OOK | T48_DEV_20K | T48_TX_10dB | T48_TX_20mS | T48_TUNE_0PLL)

#elif defined(USE_T39A)
    #if defined(USE_434MHZ)
        // Note that this assumes a reference clock of 26 MHz, and therefore sets the band to 0.
        #if defined(USE_OOK)
            #define T39A_APP_CONFIG (T39_MODE_AUTO | T39_MOD_OOK | T39_FDEV_40kHz | T39_BAND0 | T39_TX_10dB | T39_TX_20ms | T39_RESERVED_BITS)
        #else
            // Assume FSK if not using OOK
            // Assume frequency deviation of 40 kHz.
            #define T39A_APP_CONFIG (T39_MODE_AUTO | T39_MOD_FSK | T39_FDEV_40kHz | T39_BAND0 | T39_TX_10dB | T39_TX_20ms | T39_RESERVED_BITS)
        #endif
        #define T39A_FREQ_CONFIG (FREQUENCY_43392)
    #elif defined(USE_868MHZ) | defined (USE_915MHZ)
         // Assume 26 MHz Reference, and Band 1 operation.
         // Assume auto-transmit mode with 20 ms sleep delay.
        #if defined(USE_OOK)
            #define T39A_APP_CONFIG (T39_MODE_AUTO | T39_MOD_OOK | T39_BAND1 | T39_FDEV_40kHz | T39_TX_10dB | T39_TX_20ms | T39_RESERVED_BITS)
        #else
            // Assume FSK if not using OOK.
            // Assume frequency deviation of 40 kHz.
            #define T39A_APP_CONFIG (T39_MODE_AUTO | T39_MOD_FSK | T39_BAND1 | T39_FDEV_40kHz | T39_TX_10dB | T39_TX_20ms | T39_RESERVED_BITS)
        #endif

        #if defined (USE_868MHZ)
            #define T39A_FREQ_CONFIG (FREQUENCY_86830)
        #else
            #define T39A_FREQ_CONFIG (FREQUENCY_91500)
        #endif
    #else
        #error Unknown frequency band
    #endif
#endif


#endif  // _HARDWARE_H_

