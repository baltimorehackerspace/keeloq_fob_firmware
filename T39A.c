#include <htc.h>
#include "hardware.h"

#if defined(USE_T39A)

#include "T39A.h"

/** Value to OR to the MSB of data meant for the application register. This
 * essentially sets the address bits. */
#define APP_REG_PREFIX  0

/** Value to OR to the MSB of data meant for the frequency register. This
 * essentially sets the address bits. */
#define FREQ_REG_PREFIX  0x18

void sendByteTxCommand(unsigned char cmd);

void sendTxCommand(unsigned char cmd)
{
    // The SX1243 samples data on the rising edge of clock. Clock is idle low.
	unsigned char i;
	for (i=0; i<8; i++)
	{
		if (cmd & 0x80)
			DATA_OUT = 1;
		else
			DATA_OUT = 0;

		CTRL_OUT = 1;
		NOP();
		NOP();
		CTRL_OUT = 0;
		cmd = cmd << 1;
	}
}

void TX_Init(void)
{    
    unsigned char app_high = (T39A_APP_CONFIG & 0x00FF00) >> 8;
    unsigned char app_low = (T39A_APP_CONFIG & 0x0000FF);

    unsigned char f_upper = (T39A_FREQ_CONFIG & 0x70000) >> 16;
    unsigned char f_high = (T39A_FREQ_CONFIG & 0x0FF00) >> 8;
    unsigned char f_low = (T39A_FREQ_CONFIG & 0x000FF);

    T39A_Recover();  // HACK - seems to fix POR problem when TX does not TX
    __delay_ms(1);

    sendTxCommand(APP_REG_PREFIX);
    sendTxCommand(app_high);
    sendTxCommand(app_low);

    sendTxCommand(FREQ_REG_PREFIX | f_upper);
    sendTxCommand(f_high);
    sendTxCommand(f_low);

    return;
}

void T39A_Recover(void)
{
    // General idea:
    // Keep DATA pulses below 2 us. Overall time of command does not matter.

    // Implement without a loop to avoid the overhead.
    DATA_OUT = 1;
    NOP();
    CTRL_OUT = 1;
    NOP();
    DATA_OUT = 0;
    NOP();
    CTRL_OUT = 0;
    NOP();

    DATA_OUT = 1;
    NOP();
    CTRL_OUT = 1;
    NOP();
    DATA_OUT = 0;
    NOP();
    CTRL_OUT = 0;
    NOP();

    DATA_OUT = 1;
    NOP();
    CTRL_OUT = 1;
    NOP();
    DATA_OUT = 0;
    NOP();
    CTRL_OUT = 0;
    NOP();

    DATA_OUT = 1;
    NOP();
    CTRL_OUT = 1;
    NOP();
    DATA_OUT = 0;
    NOP();
    CTRL_OUT = 0;
    NOP();

    DATA_OUT = 1;
    NOP();
    CTRL_OUT = 1;
    NOP();
    DATA_OUT = 0;
    NOP();
    CTRL_OUT = 0;
    NOP();

    DATA_OUT = 1;
    NOP();
    CTRL_OUT = 1;
    NOP();
    DATA_OUT = 0;
    NOP();
    CTRL_OUT = 0;
    NOP();

    DATA_OUT = 1;
    NOP();
    CTRL_OUT = 1;
    NOP();
    DATA_OUT = 0;
    NOP();
    CTRL_OUT = 0;
    NOP();

    DATA_OUT = 1;
    NOP();
    CTRL_OUT = 1;
    NOP();
    DATA_OUT = 0;
    NOP();
    CTRL_OUT = 0;
    NOP();

    DATA_OUT = 1;
    NOP();
    CTRL_OUT = 1;
    NOP();
    DATA_OUT = 0;
    NOP();
    CTRL_OUT = 0;
    NOP();

    DATA_OUT = 1;
    NOP();
    CTRL_OUT = 1;
    NOP();
    DATA_OUT = 0;
    NOP();
    CTRL_OUT = 0;
    NOP();

    DATA_OUT = 1;
    NOP();
    CTRL_OUT = 1;
    NOP();
    DATA_OUT = 0;
    NOP();
    CTRL_OUT = 0;
    NOP();

    DATA_OUT = 1;
    NOP();
    CTRL_OUT = 1;
    NOP();

    DATA_OUT = 0;
    NOP();
    CTRL_OUT = 0;
    NOP();

    DATA_OUT = 1;
    NOP();
    CTRL_OUT = 1;
    NOP();
    DATA_OUT = 0;
    NOP();
    CTRL_OUT = 0;
    NOP();

    DATA_OUT = 1;
    NOP();
    CTRL_OUT = 1;
    NOP();
    DATA_OUT = 0;
    NOP();
    CTRL_OUT = 0;
    NOP();

    DATA_OUT = 1;
    NOP();
    CTRL_OUT = 1;
    NOP();
    DATA_OUT = 0;
    NOP();
    CTRL_OUT = 0;
    NOP();

    DATA_OUT = 1;
    NOP();
    CTRL_OUT = 1;
    NOP();
    DATA_OUT = 0;
    NOP();
    CTRL_OUT = 0;
    NOP();

    DATA_OUT = 1;
    NOP();
    CTRL_OUT = 1;
    NOP();
    DATA_OUT = 0;
    NOP();
    CTRL_OUT = 0;
    NOP();

    DATA_OUT = 1;
    NOP();
    CTRL_OUT = 1;
    NOP();
    DATA_OUT = 0;
    NOP();
    CTRL_OUT = 0;
    NOP();

    DATA_OUT = 1;
    NOP();
    CTRL_OUT = 1;
    NOP();
    DATA_OUT = 0;
    NOP();
    CTRL_OUT = 0;
    NOP();

    DATA_OUT = 1;
    NOP();
    CTRL_OUT = 1;
    NOP();
    DATA_OUT = 0;
    NOP();
    CTRL_OUT = 0;
    NOP();

    DATA_OUT = 1;
    NOP();
    CTRL_OUT = 1;
    NOP();
    DATA_OUT = 0;
    NOP();
    CTRL_OUT = 0;
    NOP();

    DATA_OUT = 1;
    NOP();
    CTRL_OUT = 1;
    NOP();
    DATA_OUT = 0;
    NOP();
    CTRL_OUT = 0;
    NOP();

    DATA_OUT = 1;
    NOP();
    CTRL_OUT = 1;
    NOP();
    DATA_OUT = 0;
    NOP();
    CTRL_OUT = 0;
    NOP();

    DATA_OUT = 1;
    NOP();
    CTRL_OUT = 1;
    NOP();
    DATA_OUT = 0;
    NOP();
    CTRL_OUT = 0;
    NOP();

    return;
}

#endif // defined(USE_T39A)
