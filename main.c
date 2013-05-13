/* Firmware for the keyfob that accompanies the Keeloq Shield for the ChipKit series */

/** 
 * NOTE: There are references to using the PIC12LF1840T48A instead of the PIC12LF1840T39A
 * however, the 48A is not supported, and the current hardware schematic only supports the 39A
 * as well.
 *
 * NOTE: There are references to using multiple frequencies in the source, however the current
 * hardware design only supports the 433mhz.
 *
 * NOTE: There are refences to AES encryption in this document, this is because it is technically
 * supported within the Keeloq protocol, however due to export laws this code does not support
 * AES.
 */

#include <htc.h>
#include "hardware.h"
#include "memorymap.h"
#include "typedefs.h"
#include "keeloq.h"

#if defined(_12LF1840T39A) || defined(_12LF1840T48A) || defined(_12LF1840)
__CONFIG ( CP_ON & CPD_OFF & WRT_OFF & FOSC_INTOSC & WDTE_OFF & MCLRE_OFF & BOREN_ON & PWRTE_ON) ;
__CONFIG ( LVP_OFF & WRT_OFF & PLLEN_OFF & STVREN_ON & BORV_LO ) ;
#else
    #error unknown processor
#endif


// Counter 0
__EEPROM_DATA(0xAA, 0x55, 0x00, 0x00, 0xAA, 0x55, 0x00, 0x00);  //00
__EEPROM_DATA(0xAA, 0x55, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);  //08

// Counter 1  LSB				MSB
__EEPROM_DATA(0x11, 0x22, 0x33, 0x44, 0x11, 0x22, 0x33, 0x44);  //10
__EEPROM_DATA(0x11, 0x22, 0x33, 0x44, 0x00, 0x00, 0x00, 0x00);  //18

// KeeLoq Key
// This key needs to be generated to match a pair using the serial number and vendor code
__EEPROM_DATA(0xab, 0xe8, 0x6b, 0xf0, 0x92, 0xab, 0x55, 0x03);  //20

// KeeLoq discrimination value , AES user value LSB...MSB
__EEPROM_DATA(0x01, 0x67, 0x00, 0x00, 0x00, 0x11, 0x22, 0x00);  //28

// AES key - LSB first -- not used
__EEPROM_DATA(0xD7, 0x0B, 0x56, 0xB0, 0x7D, 0x8F, 0xBE, 0x32);  //30
__EEPROM_DATA(0xD0, 0xB2, 0x19, 0xE9, 0xFF, 0xBA, 0xB4, 0x67);  //38


//         Keeloq serial # MSB first - AES serial # LSB first, AES not used
// Serial number is first 4 bytes - first 4 bits.
__EEPROM_DATA(0x79, 0x22, 0x01, 0x06, 0x78, 0x56, 0x34, 0x12);  //40

#define TE_PER      PWM_ON
#define TE          PWM_PERIOD

#define KEELOQ_PACKET_BIT_END	264			// 66  bits

/** Storage for syncronization counter. */
UINT32_VAL counter;

enum ENCODER
{
	KEELOQ_PACKET
};

/** Encoder type currently active */
enum ENCODER encoder;

enum XMIT_STATE
{
    OFF,
    PREAMBLE,
    HEADER,
    DATA,
    GUARD
};

/** Current state of transmitter state machine. */
enum XMIT_STATE p_state = OFF;

/** Storage for encryption key. */
unsigned char key[16];

/**
 * Transmit buffer. KeeLoq transmissions use
 * 9 bytes.
 */
unsigned char dataPacket[21];

/** Current button information. */
unsigned char btn = 0;

/** Special button configuration */
unsigned char btn_cfg;

UINT16 packetNrOfBits;

/** Temporary variables used only in KLQ_Encrypt() function. */
unsigned char CNT1, CNT2, MASK;

/** Load Key from EEPROM and encrypt data buffer using KeeLoq NLF algorithm */
void KeeLoqEncryptPacket(void);

/** Shift the dataPacket buffer to the right by 1 bit. */
extern void ShiftBuffer(void);
#pragma regsused ShiftBuffer wreg, status

/** Initialize microcontroller ports. */
void initPorts()
{
	// LED
    LED_OUT = LED_OFF;
    LED_DIR = output_pin;
	
	// Transmitter control
    DATA_OUT = 0;
    DATA_DIR = output_pin;
    CTRL_OUT = 0;
    CTRL_DIR = output_pin;

	// Buttons
    SW1_DIR = input_pin;
    SW3_DIR = input_pin;
    SW4_DIR = input_pin;

	// All ports are digital
	ANSELA = 0;

	// port pull-up
    SW1_WPU = 0;
    SW3_WPU = 1;
    SW4_WPU = 1;
	OPTION_REGbits.nWPUEN = 0;
}

/** Initialize PWM module for transmit encoding. */
void initPWM()
{
    // Single output; P1A modulated; P1B assigned as port pins
    CCP1CONbits.P1M = 0b00;

    // PWM mode: P1A active-high
    CCP1CONbits.CCP1M = 0b1100;
	
	// PWM with full period 
	PR2 = TE_PER;
	CCPR1L 	= 0;

	// prescaler is 1:4
    T2CONbits.T2CKPS = 0b01;
    //T2CONbits.T2CKPS = 0b01;
    T2CONbits.T2CKPS = TMR2_PRESCALER;

	T2CONbits.TMR2ON = 1 ;
}

/** Initialize interrupts */
void initInts()
{
	TMR2IF = 0;
	TMR2IE = 0;
	PEIE = 1;
	
	// SW1 and SW2 use pull-ups
    SW1_IOC = 1;

	// SW3 and SW4 use pull-downs
	SW3_IOC = 1;
    SW4_IOC = 1;

    ei();
}

/** Interrupt vector */
static void interrupt int0()
{
    static UINT16 bitnr = 0;

	if (TMR2IF)
	{
		TMR2IF = 0;	
		if (p_state == PREAMBLE)
		{
			if (bitnr & 0x01)
            {
				CCPR1L = 0;
            }
			else
            {
				CCPR1L = TE;
            }
			bitnr++;
			if (bitnr >= 44) 
            {
                p_state = HEADER;
                bitnr = 0;
            }
		}
		else if (p_state == HEADER)
		{
			CCPR1L = 0;
			bitnr++;
			if (bitnr >= 9) { p_state = DATA ; bitnr = 0;}
		}
		else if (p_state == DATA)
		{
			if ( (bitnr&0x03)==0x00 )
			{
				// first element is always one
				CCPR1L = TE;
			}
			else if (  (bitnr&0x03)==0x01 )
			{
				if ( (dataPacket[0]&0x01)==0x01)
				{
					// it's a one
					CCPR1L = 0;
				}
				else
				{
					// it's a zero
					CCPR1L = TE;
				}
			
				// now rotate the data vector
				ShiftBuffer();
			}
			else if (  (bitnr&0x03)==0x02 )
			{
				// last element is always zero
				CCPR1L = 0;
				bitnr++;
			}

			bitnr++;

			// check if we sent all bits
			if (bitnr >= packetNrOfBits) 
			{ 
				p_state = GUARD ; 
				CCPR1L = 0; 
				bitnr = 0;
			}
		}
		else if (p_state == GUARD)
		{
			CCPR1L = 0;
			bitnr++;
			if (bitnr >= 20) 
			{ 
				p_state = OFF ; 
				CCPR1L = 0; 
				bitnr = 0; 
				LED ^= 1;
				TMR2IE = 0;
			}
		}
	}
	if ( IOCIF )
	{ // we should never get here
        IOCAF = 0;
		IOCIF = 0;
	}	
}

void sendPacket(enum ENCODER packetType)
{
	//disable CCP
    CCP1CONbits.CCP1M = 0;

    TX_Init();
	
	packetNrOfBits = KEELOQ_PACKET_BIT_END;

	//Start-up time, 2mS
	DATA_OUT = 1;
	__delay_ms(15);
	DATA_OUT = 0;

	//re-enable CCP
    CCP1CONbits.CCP1M = 0b1100;

	p_state = PREAMBLE;

	// start the interrupt
	TMR2IE = 1;
}

/** Prepare interrupt-on-change wake-up and execute sleep. */
void shutDown()
{
    di();

	// enable ints on change
	IOCIE = 1;
	IOCIF = 0;	
	IOCAF = 0;

    SLEEP();

	//disable int on change
	IOCIE = 0;
	IOCIF = 0;
	IOCAF = 0;

    ei();
}


/** Read and populate btn with current switch press seen. */
void readButtons()
{
    // delete all interrupt-on-change events
    IOCAF = 0;

    btn = 0;

    if (SW1_IN == 1)
    {   // a '1' indicates that a button is pressed
        // we need to figure out which specific button is pressed

        // make SW1 an analog channel
        SW1_ANS = 1;

        // configure ADCON1
        ADCON1bits.ADFM = 0;        // Left-justified
        ADCON1bits.ADCS = 0b010;    // Conversion clock is FOSC/32
        ADCON1bits.ADPREF = 0b00;   // Positive reference is VDD

        // configure the channel
        ADCON0bits.CHS = 0b00011;

        // turn on ADC
        ADCON0bits.ADON = 1;

        __delay_us(10);
        GO_nDONE = 1 ;
        __delay_us(10);

        while (GO_nDONE == 1);

        if (ADRESH > 210)
        {
            btn |= 0x02;
        }
        else
        {
            btn |= 0x04;
        }

        // make SW1 a digital channel
        SW1_ANS = 0;
    }
	
    // The remaining buttons are active-low.

    if (SW3_IN == 0)
    {
        btn |= 0x01;
    }
	
    if (SW4_IN == 0)
    {
        btn |= 0x08;
    }
}

/** Populate dataPacket for a standard KeeLoq packet */
void loadKeeloqEncoderPacket()
{
	//add the counter
	dataPacket[0] = counter.v[0];
	dataPacket[1] = counter.v[1];

	// LSB of discrimination
	dataPacket[2] = eeprom_read ( DISC+1 );

	// add button info
	dataPacket[3] = 0x00 | ( (btn & 0x0F) <<4 );
	// add MSB of discrimination
	dataPacket[3] |= ( eeprom_read ( DISC ) & 0x03) ;
	// add counter overflow bits
	dataPacket[3] |= ( counter.v[2] & 0x03 ) << 2;

	// serial number 
	dataPacket[4] = eeprom_read (KEELOQ_SERIAL + 3);
	dataPacket[5] = eeprom_read (KEELOQ_SERIAL + 2);
	dataPacket[6] = eeprom_read (KEELOQ_SERIAL + 1);
	dataPacket[7] = eeprom_read (KEELOQ_SERIAL + 0) & 0x0F;
	dataPacket[7] |= ( (btn & 0x0F) <<4 );  			

	// if this button is pressed start a packet
	dataPacket[8] = 0x03;			// additional flags

}

/** Read and verify sync counter from EEPROM into counter. */
void LoadAndVerifyCounter(enum ENCODER encoder)
{
    unsigned char error = 0;
    unsigned char tmp = 0;

    unsigned char addShift;

    addShift = 0x00;

    // -- read copy A from EEPROM --
    counter.v[0] = eeprom_read (CNT0_A + 0 + addShift);
    counter.v[1] = eeprom_read (CNT0_A + 1 + addShift);
    counter.v[2] = eeprom_read (CNT0_A + 2 + addShift);
    counter.v[3] = eeprom_read (CNT0_A + 3 + addShift);

    // -- read copy B and compare --
    tmp = eeprom_read ( CNT0_B + 0  + addShift);
    if ( counter.v[0] != tmp )
        error = 1;
    tmp = eeprom_read ( CNT0_B + 1  + addShift);
    if ( counter.v[1] != tmp )
        error = 1;
    tmp = eeprom_read ( CNT0_B + 2  + addShift);
    if ( counter.v[2] != tmp )
        error = 1;
    tmp = eeprom_read ( CNT0_B + 3  + addShift);
    if ( counter.v[3] != tmp )
        error = 1;

    if ( !error )
    {
        // A==B, use copy A
        // counter already has copy A
    }

    if ( error )
    {
        // -- read copy C
        counter.v[0] = eeprom_read (CNT0_C + 0 + addShift);
        counter.v[1] = eeprom_read (CNT0_C + 1 + addShift);
        counter.v[2] = eeprom_read (CNT0_C + 2 + addShift);
        counter.v[3] = eeprom_read (CNT0_C + 3 + addShift);

        error = 0;

        // Compare to copy B

        tmp = eeprom_read (CNT0_B + 0 + addShift);
        if ( counter.v[0] != tmp )
            error = 1;
        tmp = eeprom_read (CNT0_B + 1 + addShift);
        if ( counter.v[1] != tmp )
        error = 1;
        tmp = eeprom_read (CNT0_B + 2 + addShift);
        if ( counter.v[2] != tmp )
            error = 1;
        tmp = eeprom_read (CNT0_B + 3 + addShift);
        if ( counter.v[3] != tmp )
            error = 1;

        if ( error )
        {
            // -- B != C , so use copy A
            counter.v[0] = eeprom_read (CNT0_A + 0 + addShift);
            counter.v[1] = eeprom_read (CNT0_A + 1 + addShift);
            counter.v[2]	= eeprom_read (CNT0_A + 2 + addShift);
            counter.v[3] = eeprom_read (CNT0_A + 3 + addShift);
        }
        else
        {
            // B == C, so used copy C incremented by one
            counter.v[0] = eeprom_read (CNT0_C + 0 + addShift);
            counter.v[1] = eeprom_read (CNT0_C + 1 + addShift);
            counter.v[2] = eeprom_read (CNT0_C + 2 + addShift);
            counter.v[3] = eeprom_read (CNT0_C + 3 + addShift);

            counter.Val++;
        }
    }

    // increment the counter
    counter.Val++;

    // -- Now save back to eeprom
    di();

    // -- Copy A
    eeprom_write (CNT0_A + 0 + addShift, counter.v[0]);
    eeprom_write (CNT0_A + 1 + addShift, counter.v[1]);
    eeprom_write (CNT0_A + 2 + addShift, counter.v[2]);
    eeprom_write (CNT0_A + 3 + addShift, counter.v[3]);

    // -- Copy B
    eeprom_write (CNT0_B + 0 + addShift, counter.v[0]);
    eeprom_write (CNT0_B + 1 + addShift, counter.v[1]);
    eeprom_write (CNT0_B + 2 + addShift, counter.v[2]);
    eeprom_write (CNT0_B + 3 + addShift, counter.v[3]);

    // -- Copy C
    eeprom_write (CNT0_C + 0 + addShift, counter.v[0]);
    eeprom_write (CNT0_C + 1 + addShift, counter.v[1]);
    eeprom_write (CNT0_C + 2 + addShift, counter.v[2]);
    eeprom_write (CNT0_C + 3 + addShift, counter.v[3]);

    ei();
}

void main ()
{
    /** Button timeout counter */
    UINT16 timeout = 0;

    /** Storage for previous button capture. */
    unsigned char oldBtn = 0;

    PCON = 0x03;

    OSCCONbits.SCS = 0b10;
#if (_XTAL_FREQ == 8000000)
    OSCCONbits.IRCF = 0b1110;
#else
    #error unsupported clock frequency
#endif
    OSCCONbits.SPLLEN = 0;

    initPorts();
    initPWM();
    initInts();

    TX_Init();

//-- start timer0
    PSA=0;
    T0CS=0;
    PS2=1;
    PS1=1;
    PS0=1;

    btn_cfg = eeprom_read( SETTINGS );

    // first read of buttons.
    readButtons();

    // Main application loop
    while (1)
    {
        // flash LEDs
        if ( btn & 0x0F )
        {
            if (oldBtn != btn)
                LoadAndVerifyCounter(0);
            loadKeeloqEncoderPacket();

            // This is KeeLoq encoding
            KeeLoqEncryptPacket();
            sendPacket(KEELOQ_PACKET);

            if (oldBtn == btn )
            {
                // same buttons pressed, increment the timer
                // this timer increments after each data packet
                timeout++;
            }
            else
            {
                // new button pressed , reset the timer
                timeout = 0;
            }
        }
        oldBtn = btn;
        LED = LED_ON;
        while (p_state > OFF)
        {
            // during a transmission read the button values
            // if a new button was pressed compose a new packet
            readButtons();
        }

        // if timeout ocours, force the button information to zero
        if ( timeout >= 50 )
        {
            // 1. read btn_cfg
            btn_cfg = eeprom_read( SETTINGS );

            // 2. toggle button bit in btn_cfg
            btn_cfg ^= btn;

            // 3. save button configuration
            di();
            eeprom_write (SETTINGS,btn_cfg);
            ei();

            timeout = 0;
            btn = 0;
        }

        // no button pressed, data packet sending finished
        if ( ((btn & 0x0F) == 0x00 ) && (p_state == OFF)  )
        {
            LED = LED_OFF;

            do
            {
                shutDown();
                __delay_ms(1);
                oldBtn = 0;

                // here we should re-read the configuration
                btn_cfg = eeprom_read( SETTINGS );

                readButtons();
            }
            // btn bit 7 is shift key, so mask out
            while( (btn & 0x0F) == 0);
        }
    }	// end of while(1)
} // end of main

void KeeLoqEncryptPacket()
{
	key[7] = eeprom_read (KEELOQ_KEY+0);
	key[6] = eeprom_read (KEELOQ_KEY+1);
	key[5] = eeprom_read (KEELOQ_KEY+2);
	key[4] = eeprom_read (KEELOQ_KEY+3);
	key[3] = eeprom_read (KEELOQ_KEY+4);
	key[2] = eeprom_read (KEELOQ_KEY+5);
	key[1] = eeprom_read (KEELOQ_KEY+6);
	key[0] = eeprom_read (KEELOQ_KEY+7);

	KLQ_Encrypt();
}
