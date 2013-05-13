#include <aspic.h>

;; Function exernal callers should use.
GLOBAL  _KLQ_Encrypt

;; Variables for this assembly code to use.
GLOBAL  _CNT1, _CNT2, _dataPacket, _MASK, _key

;; Tell the linker how it should be called.
SIGNAT _KLQ_Encrypt,4217

PSECT text,class=CODE,local,delta=2
_KLQ_Encrypt:
ENCRYPT:
        MOVLW   0x0C            ; OUTER LOOP 11+1 TIMES
        MOVWF   _CNT1          	; OUTER LOOP 11+1 TIMES

ENCRYPT_OUTER:
        MOVLW   0x30            ; INNER LOOP 48 TIMES
        MOVWF   _CNT2          	; INNER LOOP 48 TIMES

ENCRYPT_INNER:
		CLRWDT					; RESET WATCHDOG TIMER
        MOVF   	_CNT1,W			; LAST 48 LOOPS RESTORE THE KEY
        XORLW   1               ; LAST 48 LOOPS RESTORE THE KEY
        BTFSC	ZERO            ; LAST 48 LOOPS RESTORE THE KEY
        GOTO    RRF_KEY      	; LAST 48 LOOPS RESTORE THE KEY

        ; THE LOOKUP TABLE IS COMPRESSED INTO IN 4 BYTES TO SAVE SPACE
        ; USE THE 3 LOW INDEX BITS TO MAKE UP AN 8-BIT BIT MASK
        ; USE THE 2 HIGH INDEX BITS TO LOOK UP THE VALUE IN THE TABLE
        ; USE THE BIT MASK TO ISOLATE THE CORRECT BIT IN THE BYTE
        ; PART OF THE REASON FOR THIS SCHEME IS BECAUSE NORMAL TABLE LOOKUP
        ; REQUIRES AN ADDITIONAL STACK LEVEL

        BCF		CARRY           ; CLEAR CARRY (FOR THE LEFT SHIFT)

        MOVLW   1               ; INITIALISE MASK = 1
        BTFSC   _dataPacket+2,4     	; SHIFT MASK 4X IF BIT 2 SET
        MOVLW   0x10            ; SHIFT MASK 4X IF BIT 2 SET
        MOVWF   _MASK           ; INITIALISE MASK = 1

        BTFSS   _dataPacket+1,1     	; SHIFT MASK ANOTHER 2X IF BIT 1 SET
        GOTO    $+3
        RLF     _MASK,F
        RLF     _MASK,F

        BTFSC   _dataPacket+0,1       	; SHIFT MASK ANOTHER 1X IF BIT 0 SET
        RLF     _MASK,F


; ****** GET NLF BYTE FROM LOOKUP TABLE ********************

NFL_TABLE:
        BTFSC   _dataPacket+3,7
		GOTO	TABLE2
        MOVLW   0x2E            ; BITS 4:3 WERE 00
        BTFSC   _dataPacket+3,2
        MOVLW   0x74            ; BITS 4:3 WERE 01
		GOTO	TABLE_END
TABLE2:
        MOVLW   0x5C            ; BITS 4:3 WERE 10
        BTFSC   _dataPacket+3,2
        MOVLW   0x3A            ; BITS 4:3 WERE 11

TABLE_END:
        ANDWF   _MASK,F         ; ISOLATE THE CORRECT BIT
        MOVLW   0               ; COPY THE BIT TO BIT 7
        BTFSS	ZERO            ; COPY THE BIT TO BIT 7
        MOVLW   0x80            ; COPY THE BIT TO BIT 7

        MOVWF   _MASK           ; STORE W TEMPORARILY (WE NEED BIT 7)
        RLF     _MASK,F         ; LEFT ROTATE MASK TO GET BIT 7 INTO THE CARRY
        RLF     _MASK,F         ; LEFT ROTATE MASK TO GET THE CARRY INTO BIT 0
		MOVF	_MASK,W


        XORWF   _dataPacket+0,W ; ONLY INTERESTED IN BIT CSR00
        XORWF   _dataPacket+2,W ; ONLY INTERESTED IN BIT CSR20
        XORWF   _key+0,W        ; ONLY INTERESTED IN BIT KEYREG1,0

        MOVWF   _MASK           ; STORE W TEMPORARILY (WE NEED BIT 0)
        RRF     _MASK,F         ; RIGTH ROTATE MASK TO GET BIT 0 INTO THE CARRY

        RRF     _dataPacket+3,F ; RIGHT SHIFT IN THE NEW BIT
        RRF     _dataPacket+2,F
        RRF     _dataPacket+1,F
        RRF     _dataPacket+0,F

RRF_KEY:
        BCF		CARRY           ; CLEAR CARRY
        BTFSC   _key,0          ; SET CARRY IF LEFTMOST BIT SET
        BSF		CARRY           ; SET CARRY IF LEFTMOST BIT SET

        RRF     _key+7,F        ; RIGHT-ROTATE THE 64-BIT KEY
        RRF     _key+6,F
        RRF     _key+5,F
        RRF     _key+4,F
        RRF     _key+3,F
        RRF     _key+2,F
        RRF     _key+1,F
        RRF     _key+0,F

        DECFSZ  _CNT2,F         ; INNER LOOP 48 TIMES
        GOTO    ENCRYPT_INNER   ; INNER LOOP 48 TIMES

        DECFSZ  _CNT1,F         ; OUTER LOOP 12 TIMES (11 + 1 TO RESTORE KEY)
        GOTO    ENCRYPT_OUTER   ; OUTER LOOP 12 TIMES (11 + 1 TO RESTORE KEY)
        RETLW   0
