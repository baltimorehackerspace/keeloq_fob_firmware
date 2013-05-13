
#if !defined(_T39A_H) && defined(USE_T39A)
#define _T39A_H_

// <editor-fold desc="APPLICATION register parameters">
/** Automatic transmit mode */
#define T39_MODE_AUTO (0)

/** Forced transmit mode */
#define T39_MODE_FORCED (1 << 15)

/** FSK modulation scheme */
#define T39_MOD_FSK (0)

/** OOK modulation scheme */
#define T39_MOD_OOK (1 << 14)

/** Select band 0, for frequencies 310 to 450 MHz */
#define T39_BAND0   (0)

/** Select band 1, for frequencies 860 to 870 MHz and 902 to 928 MHz */
#define T39_BAND1   (1 << 13)

/** Frequency deviation of approximatly 20 kHz. Actual deviation is 19.04 kHz */
#define T39_FDEV_20kHz (6 << 5)

/** Frequency deviation of approximatly 40 kHz. Actual deviation is 41.26 kHz */
#define T39_FDEV_40kHz (13 << 5)

/** Output power 0 dBm */
#define T39_TX_0dB (0)

/** Output power +10 dBm */
#define T39_TX_10dB (1 << 4)

/** 2 ms of inactivity before entering sleep mode. Only used in Automatic mode. */
#define T39_TX_2ms 0

/** 20 ms of inactivity before entering sleep mode. Only used in Automatic mode. */
#define T39_TX_20ms (1 << 3)

/** Bits 2:0 are reserved and should be maintained at their data sheet value. */
#define T39_RESERVED_BITS (0b100)
// </editor-fold>

// <editor-fold desc="Frequency register values">
#if RF_XTAL == 24000000

    /** The value to load into the Frequency registers for 433.92 MHz, assuming
     * a reference clock of 24 MHz. */
    #define FREQUENCY_43392 0x4851F

    /** 315.0 MHz @ Fref = 24 MHz, band = 0 */
    #define FREQUENCY_31500 0x34800

    /** 315.5 MHz @ Fref = 24 MHz, band = 0 */
    #define FREQUENCY_31550 0x34955

#elif RF_XTAL == 26000000

    /** 390 MHz @ Fref = 26 MHz, band = 0 */
    #define FREQUENCY_39000 0x3C000

    /** The value to load into the Frequency registers for 433.92 MHz, assuming
     * a reference clock of 26 MHz. */
    #define FREQUENCY_43392 0x42C1C

    /** 868.3 MHz @ Fref = 26 MHz, band = 1 */
    #define FREQUENCY_86830 0x42CAD

    /** 915 MHz @ Fref = 26 MHz, band = 1 */
    #define FREQUENCY_91500 0x46627

#else
    #error  Unknown RF_XTAL value
#endif  // RF_XTAL ==
// </editor-fold>

/** Initialize the transmitter. */
void TX_Init(void);

/** Issue a soft reset command. */
void T39A_Recover(void);

#endif