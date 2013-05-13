#include <aspic.h>

;; Function exernal callers should use.
GLOBAL  _ShiftBuffer

;; Variables for this assembly code to use.
GLOBAL  _dataPacket

;; Tell the linker how it should be called.
SIGNAT _ShiftBuffer,4217

PSECT text,class=CODE,local,delta=2
_ShiftBuffer:
    BANKSEL (_dataPacket)
    bcf		CARRY
    rrf		_dataPacket+20,f
    rrf		_dataPacket+19,f
    rrf		_dataPacket+18,f
    rrf		_dataPacket+17,f
    rrf		_dataPacket+16,f
    rrf		_dataPacket+15,f
    rrf		_dataPacket+14,f
    rrf		_dataPacket+13,f
    rrf		_dataPacket+12,f
    rrf		_dataPacket+11,f
    rrf		_dataPacket+10,f
    rrf		_dataPacket+9,f
    rrf		_dataPacket+8,f
    rrf		_dataPacket+7,f
    rrf		_dataPacket+6,f
    rrf		_dataPacket+5,f
    rrf		_dataPacket+4,f
    rrf		_dataPacket+3,f
    rrf		_dataPacket+2,f
    rrf		_dataPacket+1,f
    rrf		_dataPacket+0,f
    RETLW   0

    END
