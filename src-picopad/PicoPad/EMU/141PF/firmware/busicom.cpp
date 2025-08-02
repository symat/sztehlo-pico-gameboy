#include "../include.h"

const u8 busicom_Prog[] = {
	0xF0, 		// 000	Reset:	clb			; clear A and C
	0x11, 0x01, 	// 001	Reset2:	jt	Reset2		; wait while drum sector signal is active (wait if TEST = 0)
	0x50, 0xB0, 	// 003		call	Keyboard	; keyboard service (returns R8R9 = $00 to select KR)
	0x51, 0x5F, 	// 005		call	ShiftR		; right shift KR (addressed R8R9 = $00) -> R13 digit from KR.M0
	0xAD, 		// 007		ld	R13		; A <- R13, lower half of scan code from keyboard buffer
	0xB1, 		// 008		xch	R1		; A -> R1, save lower half of scan code
	0xF0, 		// 009		clb			; clear A = 0, Carry = 0
	0x51, 0x5F, 	// 00A		call	ShiftR		; right shift KR -> R13 digit from KR.M0
	0xAD, 		// 00C		ld	R13		; A <- R13, upper half of scan code from keyboard buffer
	0x1C, 0x29, 	// 00D		jnz	KeyService	; process if key if valid
	0x68, 		// 00F		inc	R8		; shift R8R9 to point to default WR (R8 = 1)
	0x51, 0x73, 	// 010		call	ReadSRS2	; read overflow bit, CY <- SR.S2.bit0
	0x27, 		// 012		src	R67		; selects WR from R6R7
	0xEC, 		// 013		rd0			; A <- read WR.S0, negative sign
	0xF5, 		// 014		ral			; rotate A left with C, A.bit1 = minus sign, A.bit0 = overflow
	0xB3, 		// 015		xch	R3		; R3 <- A, minus and overflow
	0x68, 		// 016		inc	R8		; increment address R8R9, R8 = 6, R9 = 0
	0xF0, 		// 017		clb			; clear A=0, CY=0
	0x51, 0xA0, 	// 018		call	Check0DR	; check if MR is zero
	0xF3, 		// 01A		cmc			; complement Carry, CY=1 if MR is not zero
	0xB3, 		// 01B		xch	R3		; A <- R3, minus and overflow flags
	0xF5, 		// 01C		ral			; rotate A left into bit 0; A.bit2 = minus sign, A.bit1 = overflow, A.bit0 = memory
	0xE1, 		// 01D		wmp			; output to RAM1 port lamps (bit 0 = memory, bit 1 = overflow, bit 2 = minut)
	0x66, 		// 01E		inc	R6		; increment R6 to 2 to select ROM2
	0x27, 		// 01F		src	R67		; selects ROM2 from R6R7
	0xEA, 		// 020		rdr			; read ROM2 port (read paper advancing button)
	0xF5, 		// 021		ral			; rotate A left, get printer paper advancing button from bit 3 -> C
	0xF7, 		// 022		tcc			; A <- C, 1=button is pressed
	0x14, 0x00, 	// 023		jz	Reset		; jump to main loop if button is not pressed
	0x52, 0x46, 	// 025		call	PaperAdv_2	; advance paper by 1 row (= 4 drum sector signals)
	0x40, 0x00, 	// 027		jmp	Reset		; jump back to main loop
	0xB0, 		// 029		xch	R0		; R0 <- A, keyboard scan code High
	0xEC, 		// 02A		rd0			; load keyboard buffer pointer KR.S0
	0xF8, 		// 02B		dec	A		; decrement A
	0xF8, 		// 02C		dec	A		; decrement A
	0xE4, 		// 02D		wr0			; save new keyboard buffer pointer to KR.S0
	0x27, 		// 02E		src	R67		; select ROM1 and WR from R6R7 (here is R6R7 = $10)
	0xEA, 		// 02F		rdr			; read ROM1 port - decimal point switch
	0xE7, 		// 030		wr3			; write it to WR.S3 - number of decimal places
	0x50, 0x64, 	// 031		call	Key1Shift	; send bit '1' to keyboard shifter, shift to select rounding switch
	0x27, 		// 033		src	R67		; select ROM1 and WR from R6R7
	0xEA, 		// 034		rdr			; read ROM1 port - runding switch
	0xE6, 		// 035		wr2			; write it to WR.S2 - rounding switch
	0x34, 		// 036		fin	R45		; fetch table PC:R1R0 to R4R5, translate scan code into function code and parameter
	0x20, 0xA0, 	// 037		fim	R01,@MacroTab0	; $A0; R0R1 <- $A0, translation table
	0xA5, 		// 039		ld	R5		; A <- R5, function code
	0xB1, 		// 03A		xch	R1		; A <-> R1
	0x30, 		// 03B		fin	R01		; fetch table PC:R0R1 to R0R1, translate function code to macro address
	0x68, 		// 03C		inc	R8		; increment R8R9 address to point default WR
	0x51, 0x73, 	// 03D		call	ReadSRS2	; read overflow bit, C <- SR.S2.bit0
	0xD0, 		// 03F		ldi	0		; A <- 0
	0xE1, 		// 040		wmp			; output RAM1, clear status lamps
	0xD1, 		// 041		ldi	1		; A <- 1
	0xF3, 		// 042		cmc			; complement C, 0 if overflow
	0xF5, 		// 043		ral			; A.bit0 <- !overflow flag (0 if overflow), bit1 = 1
	0xFC, 		// 044		kbp			; convert, A <- 15 if not overflow, 2 if overflow
	0x85, 		// 045		add	R5		; A <- A + R5 (here is C=0), add function code
	0x1A, 0x00, 	// 046		jnc	Reset		; go to main loop if function is blocked
	0xF0, 		// 048		clb			; clear A and C
	0x00, 		// 049		nop
	0x00, 		// 04A		nop
	0x11, 0x4F, 	// 04B	Exec:	jt	Exec2		; skip keyboard service if sector signal is active
	0x50, 0xB0, 	// 04D		call	Keyboard	; call keyboard service
	0x26, 0x20, 	// 04F	Exec2:	fim	R67,$20		; R6R7 <- source number address (=DR)
	0x28, 0x10, 	// 051		fim	R89,$10		; R8R9 <- destination number address (=WR)
	0x53, 0x00, 	// 053		call	CmdFetch	; fetch command code into R2R3 from page $300 = function address LOW
	0x51, 0x00, 	// 055		call	ExecCode	; execute code (command in R2R3 is LOW part of function address at page $100)
	0x71, 0x5A, 	// 057	Exec3:	ijnz	R1,Exec4	; increment register R1, skip if not zero (low part of code pointer)
	0x60, 		// 059		inc	R0		; increment register R0 (high part of code pointer)
	0x14, 0x4B, 	// 05A	Exec4:	jz	Exec		; A = 0, no jump, continue with next command
	0xF7, 		// 05C		tcc			; A <- C, A = 0 to skip next byte, A = 1 to jump
	0x14, 0x57, 	// 05D		jz	Exec3		; C = 0: skip next byte (skip jump address)
	0x43, 0x02, 	// 05F		jmp	JmpFetch	; jump to address from next byte into R0R1 (=new macro pointer)
	0xD4, 		// 061	KeyNone:ldi	4		; A <- 4, number of buttons at one row (to increase button scan code by 4)
	0x40, 0xD4, 	// 062		jmp	Keyboard5	; go back
	0xD3, 		// 064		ldi	3		; A <- 3, keyboard shifter clock = 1, shifter data = 1
	0x29, 		// 065		src	R89		; select ROM0 from R8R9 = $0x
	0xE2, 		// 066		wrr			; write shifter data from A - start write pulse
	0xD0, 		// 067		ldi	0		; A <- 0 (clock and data = 0)
	0xE2, 		// 068		wrr			; write shifter data from A - stop write pulse
	0xC0, 		// 069		ret	0		; return with A = 0
	0x6C, 		// 06A		inc	R12		; increment drum sector counter
	0x22, 0x20, 	// 06B		fim	R23,$20		; R2R3 <- $20 to select ROM2 port
	0x23, 		// 06D		src	R23		; select ROM2 from R2R3=$2x (only bits 4..7 are important)
	0xEA, 		// 06E		rdr			; A <- read ROM2 port
	0xF6, 		// 06F		rar			; A.bit0 -> C (= printer drum index signal)
	0x73, 0x6D, 	// 070		ijnz	R3,DrumSync2	; increment R3 and loop back (short wait, loop 15-times)
	0x1A, 0x76, 	// 072		jnc	DrumSync4	; jump if C = 0, printer drum index is inactive
	0xF0, 		// 074		clb			; index is active -> clear A and C
	0xBC, 		// 075		xch	R12		; clear R12 (printer drum sector counter)
	0xC0, 		// 076		ret	0		; return with A = 0, C = 0
	0xA9, 		// 077	KeyErr:	ld	R9		; A <- R9, scan code (0 = no key, 15 = multiply keys)
	0x14, 0xD9, 	// 078		jz	Keyboard6	; jump back to next row if no button is pressed in this column
	0x28, 0x00, 	// 07A	KeyErr2:fim	R89,$00		; R8 <- 0 to select KR (keyboard register), R9 <- 0 digit index
	0xF0, 		// 07C		clb			; A <- 0 (to write to digits), C <- 0
	0x51, 0x4A, 	// 07D		call	ClearWR	; clear keyboard buffer
	0x40, 0xF7, 	// 07F		jmp	Keyboard7	; exit from keyboard handling
	0xBB, 		// 081	= $bb	; $81 CM (function 11 = CM fn_clrmem, parameter 11 = print C at column 17)
	0xC7, 		// 082	= $c7	; $82 RM (function 7 = RM fn_rm, parameter 12 = print R at column 17)
	0x63, 		// 083	= $63	; $83 M- (function 3 = M+/M- fn_memadd, parameter 6 = print M- at column 18)
	0x53, 		// 084	= $53	; $84 M+ (function 3 = M+/M- fn_memadd, parameter 5 = print M+ at column 18)
	0x19, 		// 085	= $19	; $85 SQRT (function 9 = Sqrt fn_sqrt, parameter 1)
	0x1A, 		// 086	= $1a	; $86 % (function 10 = % fn_percnt, parameter 1)
	0x68, 		// 087	= $68	; $87 M=- (function 8 = =/M=+/M=- fn_memeq, parameter 6 = print M-)
	0x58, 		// 088	= $58	; $88 M=+ (function 8 = =/M=+/M=- fn_memeq, parameter 5 = print M+)
	0x05, 		// 089	= $05	; $89 diamond (function 5 = diamond fn_diamnd, parameter 0)
	0x41, 		// 08A	= $41	; $8A / (function 1 = mul/div fn_muldiv, parameter 4 = last operation /)
	0x31, 		// 08B	= $31	; $8B * (function 1 = mul/div fn_muldiv, parameter 3 = last operation *)
	0x18, 		// 08C	= $18	; $8C = (function 8 = =/M=+/M=- fn_memeq, parameter 1)
	0x22, 		// 08D	= $22	; $8D - (function 2 = +/- fn_addsub, parameter 2 = last operation -)
	0x12, 		// 08E	= $12	; $8E + (function 2 = +/- fn_addsub, parameter 1 = last operation +)
	0x05, 		// 08F	= $05	; $8F diamond2 (function 5 = diamond fn_diamnd, parameter 0)
	0x0C, 		// 090	= $0c	; $90 000 (function 12 = 1100 = enter 2 more digits 0 fn_digit, parameter 0 = enter digit 0)
	0x9D, 		// 091	= $9d	; $91 9 (function 13 = digit fn_digit, parameter 9 = enter digit 9)
	0x6D, 		// 092	= $6d	; $92 6 (function 13 = digit fn_digit, parameter 6 = enter digit 6)
	0x3D, 		// 093	= $3d	; $93 3 (function 13 = digit fn_digit, parameter 3 = enter digit 3)
	0xBD, 		// 094	= $bd	; $94 . (function 13 = digit fn_digit, parameter 11 = enter decimal point)
	0x8D, 		// 095	= $8d	; $95 8 (function 13 = digit fn_digit, parameter 8 = enter digit 8)
	0x5D, 		// 096	= $5d	; $96 5 (function 13 = digit fn_digit, parameter 5 = enter digit 5)
	0x2D, 		// 097	= $2d	; $97 2 (function 13 = digit fn_digit, parameter 2 = enter digit 2)
	0x06, 		// 098	= $06	; $98 00 (function 6 = 01100 = enter 1 more digit 0 fn_digit, parameter 0 = enter digit 0)
	0x7D, 		// 099	= $7d	; $99 7 (function 13 = digit fn_digit, parameter 7 = enter digit 7)
	0x4D, 		// 09A	= $4d	; $9A 4 (function 13 = digit fn_digit, parameter 4 = enter digit 4)
	0x1D, 		// 09B	= $1d	; $9B 1 (function 13 = digit fn_digit, parameter 1 = enter digit 1)
	0x0D, 		// 09C	= $0d	; $9C 0 (function 13 = digit fn_digit, parameter 0 = enter digit 0)
	0xAD, 		// 09D	= $ad	; $9D S (sign +/-) (function 13 = digit fn_digit, parameter 10 = sign)
	0xA4, 		// 09E	= $a4	; $9E EX (function 4 = Ex fn_ex, parameter 10 = print Ex at column 18)
	0x0E, 		// 09F	= $0e	; $9F CE (function 14 = CE fn_cleare, parameter 0)
	0xBF, 		// 0A0	= $bf	; $A0 C (function 15 = C fn_clear, parameter 11 = print C)
	0x06, 		// 0A1	= @fn_muldiv	; $06	; 1: div/mul
	0x91, 		// 0A2	= @fn_addsub	; $91	; 2: +/-
	0x98, 		// 0A3	= @fn_memadd	; $98	; 3: M+/M-
	0xF1, 		// 0A4	= @fn_ex	; $f1	; 4: Ex
	0xCD, 		// 0A5	= @fn_diamnd	; $cd	; 5: diamonds
	0xD7, 		// 0A6	= @fn_digit	; $d7	; 6: 00
	0xFD, 		// 0A7	= @fn_rm	; $fd	; 7: RM
	0x8A, 		// 0A8	= @fn_memeq	; $8a	; 8: =/M=+/M=-
	0x05, 		// 0A9	= @fn_sqrt	; $05	; 9: Sqrt
	0x61, 		// 0AA	= @fn_percnt	; $61	; 10: %
	0xF9, 		// 0AB	= @fn_clrmem	; $f9	; 11: CM
	0xD7, 		// 0AC	= @fn_digit	; $d7	; 12: 000
	0xD7, 		// 0AD	= @fn_digit	; $d7	; 13: digit/sign/DP
	0xCA, 		// 0AE	= @fn_cleare	; $ca	; 14: CE
	0xC5, 		// 0AF	= @fn_clear	; $c5	; 15: C
	0x50, 0x6A, 	// 0B0		call	DrumSync	; synchronize printer drum (increment sector counter by 1, or clear to 0)
	0x28, 0x07, 	// 0B2		fim	R89,$07		; R8 <- 0 to select ROM0 chip, R9 <- 7 loop counter (16 - 7 = 9)
	0x50, 0x64, 	// 0B4		call	Key1Shift	; send bit '1' to keyboard shifter = rows not selected
	0x79, 0xB4, 	// 0B6		ijnz	R9,Keyboard2	; loop with incrementing R9, send 9 bits '1' to unselect all rows
	0x26, 0x18, 	// 0B8		fim	R67,$18		; R6 <- $18 to select ROM1, R7 <- 8 loop counter (16 - 8 = 8 rows)
	0x22, 0x00, 	// 0BA		fim	R23,$00		; R2R3 <- 0, scan code counter
	0xD1, 		// 0BC		ldi	1		; A <- 1, keyboard shifter clock = 1, shifter data = 0
	0x50, 0x65, 	// 0BD		call	SendShift	; send '0' to keyboard shifter = select row 0
	0x27, 		// 0BF		src	R67		; select ROM1 from R6R7=$1x
	0xEA, 		// 0C0		rdr			; A <- read ROM1 port, read state of columns col0..col3
	0xFC, 		// 0C1		kbp			; decode columns (0->0, 1->1, 2->2, 4->3, 8->4, other->15)
	0xB9, 		// 0C2		xch	R9		; R9 <- A, save scan code
	0xA2, 		// 0C3		ld	R2		; A <- R2, pressed flag
	0xF5, 		// 0C4		ral			; C <- R2.bit3 (flag of pressed key in another row)
	0xF7, 		// 0C5		tcc			; A <- C (flag of pressed key), C <- 0
	0x1C, 0x77, 	// 0C6		jnz	KeyErr		; another button has been already pressed (continue to Keyboard6 if ok)
	0xA9, 		// 0C8		ld	R9		; A <- key code from R9 (15 = multiply keys)
	0x79, 0xCD, 	// 0C9		ijnz	R9,Keyboard4	; increment and jump if max. 1 column is active
	0x40, 0x7A, 	// 0CB		jmp	KeyErr2		; multiply keys pressed (code = 15), clear buffer and exit
	0x14, 0x61, 	// 0CD		jz	KeyNone		; A = 0, no key pressed (set A=4, continue at Keyboard5)
	0xB2, 		// 0CF		xch	R2		; A <- R2 (scan code counter)
	0xF5, 		// 0D0		ral			; shift left
	0xFA, 		// 0D1		stc			; set C (= key was pressed flag)
	0xF6, 		// 0D2		rar			; shift right, 1 -> A.bit3
	0xB2, 		// 0D3		xch	R2		; R2 <- A (with set bit 3)
	0x83, 		// 0D4		add	R3		; A <- R3 + A, add key column
	0xB3, 		// 0D5		xch	R3		; R3 <- A (sum of columns)
	0xD0, 		// 0D6		ldi	0		; A <- 0
	0x82, 		// 0D7		add	R2		; A <- R2 + C, add carry from R3
	0xB2, 		// 0D8		xch	R2		; R2 <- A (scan code counter HIGH)
	0x50, 0x64, 	// 0D9		call	Key1Shift	; shift next bit '1' into keyboard shifter
	0x77, 0xBF, 	// 0DB		ijnz	R7,Keyboard3	; increment R7 loop counter, go to next row
	0x29, 		// 0DD		src	R89		; select KR (keyboard register) from R8R9 (R8 = 0)
	0xA2, 		// 0DE		ld	R2		; A <- R2, R2.bit3 = button pressed flag
	0xF5, 		// 0DF		ral			; C <- A.bit3, button pressed flag
	0xF7, 		// 0E0		tcc			; A <- C, button pressed flag
	0x14, 0xF8, 	// 0E1		jz	Keyboard8	; no buttton pressed, write A = 0 into KR.S3
	0xEF, 		// 0E3		rd3			; A <- load keyboard pressing status KR.S3 (15 pressed, 0 no)
	0xF2, 		// 0E4		inc	A		; increment A, set CY if key held down (status = 15)
	0xF7, 		// 0E5		tcc			; A <- C 1=press, 0=no press, C <- 0
	0x1C, 0xF7, 	// 0E6		jnz	Keyboard7	; jump if button is already held down
	0xEC, 		// 0E8		rd0			; read A <- KR.S0, keyboard buffer pointer
	0xB9, 		// 0E9		xch	R9		; R9 <- KR.S0, keyboard buffer pointer
	0x29, 		// 0EA		src	R89		; select digit in KR, from R8R9 (here is R8 = 0)
	0xA3, 		// 0EB		ld	R3		; A <- R3, scan code LOW
	0xE0, 		// 0EC		wrm			; write scan code LOW from A into keyboard buffer
	0x69, 		// 0ED		inc	R9		; increment address R9 to point to next digit
	0x29, 		// 0EE		src	R89		; select next digit in KR, from R8R9
	0xE9, 		// 0EF		rdm			; A <- read next digit from KR
	0x1C, 0x7A, 	// 0F0		jnz	KeyErr2		; if next digit is not 0, overflow KR, jump to clear
	0xA2, 		// 0F2		ld	R2		; A <- R2, scan code HIGH
	0xE0, 		// 0F3		wrm			; write scan code HIGH
	0x69, 		// 0F4		inc	R9		; increment address to point to next digit
	0xA9, 		// 0F5		ld	R9		; A <- R9, keyboard buffer pointer
	0xE4, 		// 0F6		wr0			; write A -> KR.S0, keyboard buffer pointer
	0xDF, 		// 0F7		ldi	15		; A <- 15, flag of button is held down
	0xE7, 		// 0F8		wr3			; write new KR.S3 state of button is held down (15=held, 0=no)
	0x28, 0x00, 	// 0F9		fim	R89,$00		; R8R9 <- $00 to select KR
	0x26, 0x10, 	// 0FB		fim	R67,$10		; R6R7 <- $10 to select WR
	0x19, 0xFD, 	// 0FD		jnt	Keyboard9	; wait if TEST signal is not active (wait if TEST = 1)
	0xC0, 		// 0FF		ret	0		; return with A = 0
	0x33, 		// 100		jin	R23		; run to address PC:R2R3 (R2R3 = command code = LOW address)
	0xA5, 		// 101	MovIRWR:ld	R5		; A <- R5, function code
	0xF2, 		// 102	MovCRWR:inc	A			; A + 1
	0xF2, 		// 103	MovRRWR:inc	A			; A + 1
	0x86, 		// 104	MovDRWR:add	R6		; A <- A + R6 (C = 0)
	0xB8, 		// 105		xch	R8		; R8 <- A destination index, A <- R8 old source index
	0xB6, 		// 106		xch	R6		; R6 <- A source index
	0x41, 0x0E, 	// 107		jmp	MovWRDR	; move number
	0x66, 		// 109	MovWRMR:inc	R6		; source = MR (R6 <- 7)
	0x66, 		// 10A	MovWRTR:inc	R6		; source = TR (R6 <- 6)
	0x66, 		// 10B	MovWRSR:inc	R6		; source = SR (R6 <- 5)
	0x66, 		// 10C	MovWRCR:inc	R6		; source = CR (R6 <- 4)
	0x66, 		// 10D	MovWRRR:inc	R6		; source = RR (R6 <- 3)
	0x27, 		// 10E	MovWRDR:src	R67		; select source number register from R6R7
	0xE9, 		// 10F		rdm			; A <- read one digit from source register
	0x29, 		// 110		src	R89		; select destination register from R8R9
	0xE0, 		// 111		wrm			; write one digit from A
	0x69, 		// 112		inc	R9		; increment destination character R9
	0x77, 0x0E, 	// 113		ijnz	R7,MovWRDR	; increment destination digit index R7 (0..15), go next digit
	0x27, 		// 115		src	R67		; select source number register R6R7
	0xEC, 		// 116		rd0			; A <- read status digit S0
	0xB3, 		// 117		xch	R3		; A -> R3
	0xED, 		// 118		rd1			; A <- read status digit S1
	0x29, 		// 119		src	R89		; select destination register R8R9
	0xE5, 		// 11A		wr1			; write status digit S1 from A
	0xB3, 		// 11B		xch	R3		; A <- R3
	0xE4, 		// 11C		wr0			; write status digit S0 from A
	0xC0, 		// 11D		ret	0		; return with A = 0
	0xD4, 		// 11E	AddIRWR:ldi	4		; A <- 4, target will be IR
	0x85, 		// 11F		add	R5		; A <- 4 + R5, add function code
	0xB6, 		// 120		xch	R6		; R6 <- A, destination to IR = R5 + 4
	0x29, 		// 121	AddDRWR:src	R89		; select source number address (WR) from R8R9
	0xE9, 		// 122		rdm			; A <- read one digit
	0x27, 		// 123		src	R67		; select destination number address (DR) from R6R7
	0xEB, 		// 124		adm			; A <- A + mem (C = 0)
	0xFB, 		// 125		daa			; decimal correction
	0xE0, 		// 126		wrm			; write result into destination number address R6R7
	0x69, 		// 127		inc	R9		; increase source index R9
	0x77, 0x21, 	// 128		ijnz	R7,AddDRWR	; increase destination index R7 and loop next digit
	0xF1, 		// 12A		clc			; clear Carry
	0xC0, 		// 12B		ret	0		; return with A = 0
	0xD4, 		// 12C	SubWRIR:ldi	4		; A <- 4
	0x85, 		// 12D		add	R5		; A <- 4 + R5, function code + 4
	0xB8, 		// 12E		xch	R8		; R8 <- A, set source to IR
	0x41, 0x33, 	// 12F		jmp	SubIRWR_2	; set destination address to old R8 = 1 (WR)
	0xD4, 		// 131	SubIRWR:ldi	4		; A <- 4
	0x85, 		// 132		add	R5		; A <- 4 + R5, function code + 4
	0xB6, 		// 133		xch	R6		; R6 <- A, set destination to IR
	0xFA, 		// 134	SubDRWR:stc			; C <- 1 (= no borrow)
	0xF9, 		// 135		tcs			; transfer carry A <- 9 + C (A <- 9 or 10), CY = 0
	0x29, 		// 136		src	R89		; select source number address (WR) from R8R9
	0xE8, 		// 137		sbm			; A <- 10(9) + ~digit + ~C
	0xF1, 		// 138		clc			; clear carry C <- 0
	0x27, 		// 139		src	R67		; select destination number address (DR) from R6R7
	0xEB, 		// 13A		adm			; A <- A + digit (C = 0)
	0xFB, 		// 13B		daa			; decimal adjust
	0xE0, 		// 13C		wrm			; write digit to destination memory
	0x69, 		// 13D		inc	R9		; increase source index R9
	0x77, 0x35, 	// 13E		ijnz	R7,SubDRWR_2	; increase destination index R7 and loop next digit
	0x1A, 0x43, 	// 140		jnc	SubDRWR_7	; skip if not carry (= negative)
	0x6D, 		// 142		inc	R13		; increment R13 if positive, digit counter of div result
	0xC1, 		// 143		ret	1		; return with A = 1, C = 0 skip if negative, C = 1 jump if positive (repeat div loop)
	0x68, 		// 144	ClearMR:inc	R8		; destination = MR
	0x68, 		// 145	ClearTR:inc	R8		; destination = TR
	0x68, 		// 146	ClearSR:inc	R8		; destination = SR
	0x68, 		// 147	ClearCR:inc	R8		; destination = CR
	0x68, 		// 148	ClearRR:inc	R8		; destination = RR
	0x68, 		// 149	ClearDR:inc	R8		; destination = DR
	0x29, 		// 14A	ClearWR:src	R89		; select memory register WR from R8R9
	0xE0, 		// 14B		wrm			; write digit from A
	0x79, 0x4A, 	// 14C		ijnz	R9,ClearWR	; increment digit index R9 and loop
	0xE4, 		// 14E		wr0			; write S0 = sign
	0xE5, 		// 14F		wr1			; write S1 = place of decimal point
	0xC0, 		// 150		ret	0		; return with A = 0
	0x68, 		// 151		inc	R8		; shift target address = RR
	0x68, 		// 152		inc	R8		; shift target address = DR
	0x29, 		// 153	ShiftL:	src	R89		; select number register WR from R8R9
	0xE9, 		// 154		rdm			; A <- read one digit
	0xBD, 		// 155		xch	R13		; A <-> R13, exchange digits
	0xE0, 		// 156		wrm			; write previous digit
	0x79, 0x53, 	// 157		ijnz	R9,ShiftL	; increment digit index, loop next digit
	0xC0, 		// 159		ret	0		; return with A = 0
	0xDE, 		// 15A		ldi	14		; A <- 14, shift 14 digits
	0xB9, 		// 15B		xch	R9		; R9 <- 14, index of last digit
	0xAD, 		// 15C		ld	R13		; A <- R13, inserted digit
	0x68, 		// 15D		inc	R8		; shift target address = RR
	0x68, 		// 15E		inc	R8		; shift target address = DR
	0xBD, 		// 15F	ShiftR:	xch	R13		; R13 <- A, will write 0 to highest digit
	0xA9, 		// 160		ld	R9		; A <- R9, index of 1st digit, should be 0
	0xF8, 		// 161		dec	A		; A = A - 1, decrement index of digit
	0xF1, 		// 162		clc			; clear carry C <- 0
	0xB9, 		// 163		xch	R9		; R9 <- A, new digit index
	0x29, 		// 164		src	R89		; select number register WR from R8R9
	0xE9, 		// 165		rdm			; A <- read one digit
	0xBD, 		// 166		xch	R13		; A <- previous digit, R13 <- this digit
	0xE0, 		// 167		wrm			; write previous digit A
	0xA9, 		// 168		ld	R9		; A <- R9, index of the digit
	0x1C, 0x61, 	// 169		jnz	ShiftR_2	; loop if A != 0 (we will end at M0)
	0xC0, 		// 16B		ret	0		; return wit A = 0
	0x68, 		// 16C		inc	R8		; address = RR
	0x68, 		// 16D	JmpNMul:inc	R8		; address = DR
	0x29, 		// 16E		src	R89		; select register WR from R8R9
	0xEE, 		// 16F		rd2			; A <- read status S2
	0xF8, 		// 170		dec	A		; decrement, C <- 0 if 0, or 1 if >0
	0xF3, 		// 171		cmc			; complement C (C is 1 if S0 was 0)
	0xC1, 		// 172		ret	1		; return with A = 1, C = 1 jump if S0 = 0
	0x68, 		// 173		inc	R8		; address = SR
	0x68, 		// 174		inc	R8		; address = CR
	0x68, 		// 175		inc	R8		; address = RR
	0x68, 		// 176		inc	R8		; address = DR
	0x29, 		// 177		src	R89		; select register WR from R8R9
	0xEE, 		// 178		rd2			; A <- read status S2
	0xF6, 		// 179		rar			; rotate bit0 into C
	0xC1, 		// 17A		ret	1		; return with A = 1, C = 1 jump if S0.bit0 = 1
	0x27, 		// 17B		src	R67		; select address R6R7
	0xEE, 		// 17C		rd2			; read status S2
	0xF5, 		// 17D		ral			; rotate bit 3 into C
	0xC1, 		// 17E		ret	1		; return with A = 1, C = 1 jump if S0.bit3 = 1 (=mul/div constant CR)
	0x66, 		// 17F		inc	R6		; target = SR
	0x66, 		// 180		inc	R6		; target = CR
	0x66, 		// 181		inc	R6		; target = RR
	0x27, 		// 182		src	R67		; select register DR from R6R7
	0xE6, 		// 183		wr2			; write status 2 to 0 (from A)
	0xC0, 		// 184		ret	0		; return with A = 0
	0x66, 		// 185	SetOver:inc	R6		; target = SR
	0x66, 		// 186		inc	R6		; target = CR
	0xD1, 		// 187		ldi	1		; A <- 1
	0x41, 0x81, 	// 188		jmp	ClearRRS2	; set status RR.S2 to 1
	0xD8, 		// 18A		ldi	8		; A <- 8
	0x41, 0x81, 	// 18B		jmp	ClearRRS2	; set status RR.S2 to 8
	0xA4, 		// 18D		ld	R4		; A <- function parameter from R4
	0x41, 0x82, 	// 18E		jmp	ClearDRS2	; set status DR.S2 to R4
	0x27, 		// 190		src	R67		; select register DR from R6R7
	0xEE, 		// 191		rd2			; A <- read status 2
	0xF5, 		// 192		ral			; rotate A left, C <- A.bit3
	0xFA, 		// 193		stc			; set C
	0xF6, 		// 194		rar			; rotate A right, A.bit3 <- 1
	0xE6, 		// 195		wr2			; write status 2 (with bit3 set)
	0xC0, 		// 196		ret	0		; return with A = 0
	0xD4, 		// 197		ldi	4		; A <- 4
	0x85, 		// 198		add	R5		; A <- 4 + R5 (C = 0) = function code + 4
	0xB8, 		// 199		xch	R8		; R8 <- A, function code + 4 (register index)
	0xDE, 		// 19A		ldi	14		; A <- 14
	0xB9, 		// 19B		xch	R9		; R9 <- 14, index of first digit to test
	0x41, 0xA2, 	// 19C		jmp	Check0WR
	0xDE, 		// 19E		ldi	14		; A <- 14
	0xB9, 		// 19F		xch	R9		; R9 <- 14, index of first digit to test
	0xBD, 		// 1A0		xch	R13		; R13 <- 0 (A = 0), first digit
	0x68, 		// 1A1		inc	R8		; register + 1
	0x29, 		// 1A2		src	R89		; select register WR from R8R9
	0xDF, 		// 1A3		ldi	15		; A <- 15
	0xEB, 		// 1A4		adm			; A <- 15 + digit (C = 0), check if digit is zero, C <- 1 if digit is not 0, 0 if digit is 0
	0x79, 0xA2, 	// 1A5		ijnz	R9,Check0WR	; increment R9 and loop to next digit
	0xF3, 		// 1A7	Jump:	cmc			; complement Carry
	0xC1, 		// 1A8		ret	1		; return with A = 1, Carry = 1 to jump
	0xAD, 		// 1A9		ld	R13		; A <- R13
	0xFB, 		// 1AA		daa			; decimal correction, set Carry = 1 if R13 > 9
	0xC1, 		// 1AB		ret	1		; return with A = 1, C = 1 jump if R13 > 9
	0xAD, 		// 1AC		ld	R13		; A <- R13
	0xF8, 		// 1AD		dec	A		; decrement A (= R13 - 1), set C = 1 if R13 was not 0
	0xBD, 		// 1AE		xch	R13		; R13 <- A (= 0)
	0xF3, 		// 1AF		cmc			; complement Carry, negate jump condition
	0xC1, 		// 1B0		ret	1		; return with A = 1, C = 1 jump
	0xD7, 		// 1B1		ldi	7		; A <- 7
	0x95, 		// 1B2		sub	R5		; A <- 7 - R5 (C = 0)
	0xC1, 		// 1B3		ret	1		; return with A = 1, C = 1 jump if function code < 8
	0xDC, 		// 1B4		ldi	12		; A <- 12
	0x84, 		// 1B5		add	R4		; A <- 12 + R4 (C = 0), function parameter + 12, set Carry if R4 > 3
	0xC1, 		// 1B6		ret	1		; return with A = 1, jump if R4 > 3
	0xA5, 		// 1B7	RotFC:	ld	R5		; A <- R5 function code
	0xF6, 		// 1B8		rar			; rotate bit 0 into CY
	0xB5, 		// 1B9		xch	R5		; R5 <- A, return rotated value
	0xF3, 		// 1BA		cmc			; complement Carry
	0xC1, 		// 1BB		ret	1		; return with A = 1, C = 1 jump if bit0 was != 0
	0xA4, 		// 1BC	OddPar:	ld	R4		; A <- R4 function parameter
	0xF6, 		// 1BD		rar			; rotate bit 0 of R4 into CY
	0xC1, 		// 1BE		ret	1		; return with A = 1, jump if bit 0 of function parameter = 1
	0xD4, 		// 1BF	SetDPIR:ldi	4		; A <- 4
	0x85, 		// 1C0		add	R5		; A <- 4 + R5 (C = 0) = function code + 4
	0xB8, 		// 1C1		xch	R8		; R8 <- target address
	0x29, 		// 1C2	SetDPWR:src	R89		; select register WR from R8R9
	0xAB, 		// 1C3		ld	R11		; A <- R11, decimal point counter
	0xE5, 		// 1C4		wr1			; write decimal point counter from A to S1
	0xC0, 		// 1C5		ret	0		; return with A = 0
	0x29, 		// 1C6	GetDPWR:src	R89		; select register WR from R8R9
	0xED, 		// 1C7		rd1			; A <- read S1, position of decimal point
	0xBB, 		// 1C8		xch	R11		; R11 <- set decimal point counter
	0xC0, 		// 1C9		ret	0		; return with A = 0
	0x7B, 0xCD, 	// 1CA	IncDP:	ijnz	R11,IncDP_2	; increment decimal point counter R11 low, jump if not zero
	0x6A, 		// 1CC		inc	R10		; increment high part of decimal point counter R10
	0xC0, 		// 1CD	IncDP_2:ret	0		; return with A = 0
	0xDD, 		// 1CE		ldi	13		; A <- 13, max. value
	0x9B, 		// 1CF		sub	R11		; A <- 0,13 - R11 (C = 0), check decimal point counter, C = 1 
	0xF3, 		// 1D0		cmc			; complement carry
	0xD0, 		// 1D1		ldi	0		; A <- 0
	0x9A, 		// 1D2		sub	R10		; A <- 0 - R10 - C (= compare R10R11)
	0xC1, 		// 1D3		ret	1		; return with A = 1, C = 1 jump if decimal point counter <= 0 or 13
	0x42, 0xD3, 	// 1D4		jmp	CheckSign_2
	0x00, 		// 1D6		nop
	0x42, 0x94, 	// 1D7		jmp	DigStart_2
	0x42, 0xA3, 	// 1D9	LoadRes:jmp	LoadRes_2
	0x42, 0xAA, 	// 1DB	JmpMem:	jmp	JmpMem_2
	0x42, 0xAE, 	// 1DD	DecDP:	jmp	DecDP_2
	0x42, 0xB3, 	// 1DF	DiffDP:	jmp	DiffDP_2
	0x42, 0xB9, 	// 1E1	AdjDiv:	jmp	AdjDiv_2
	0x42, 0xCA, 	// 1E3	AdjMul:	jmp	AdjMul_2
	0x42, 0xDE, 	// 1E5		jmp	SetDMSign_2
	0x42, 0xE7, 	// 1E7	NegWR:	jmp	NegWR_2
	0x42, 0xEC, 	// 1E9	RoundWR:jmp	RoundWR_2
	0x42, 0x46, 	// 1EB		jmp	PaperAdv_2
	0x44, 0x00, 	// 1ED	Sqrt:	jmp	Sqrt_2
	0x51, 0x80, 	// 1EF	ClrOver:call	ClearCRS2		; R6 = R6 + 2, clear status digit 2 of NR(R6)
	0x51, 0x81, 	// 1F1		call	ClearRRS2		; R6 = R6 + 1, clear status RR.S2 = last operation is new number
	0x51, 0x81, 	// 1F3	ClrMode:call	ClearRRS2		; R6 = R6 + 1, clear status RR.S2 = last operation is new number
	0x2A, 0x00, 	// 1F5		fim	R1011,0			; R10R11 <- 0, reset decimal point counter
	0x40, 0x00, 	// 1F7		jmp	Reset			; exit macro interpreter, jump to main loop
	0x6F, 		// 1F9		inc	R15
	0x6F, 		// 1FA		inc	R15
	0x6F, 		// 1FB		inc	R15
	0x6F, 		// 1FC		inc	R15
	0x6F, 		// 1FD		inc	R15
	0x6F, 		// 1FE		inc	R15
	0xBF, 		// 1FF		xch	R15		; A <-> R15
	0xF4, 		// 200		cma			; complement accumulator, A <- 15 - R15
	0xBF, 		// 201		xch	R15		; R15 <- complement R15
	0x7F, 0x10, 	// 202		ijnz	R15,Print2	; check if R15 = 15, increment R15
	0xDA, 		// 204		ldi	10		; A <- 10, code of decimal point on print drum at columns 1..15
	0x51, 0x4A, 	// 205		call	ClearWR		; set WR to digits '.'
	0x2E, 0xFF, 	// 207		fim	R1415,$FF	; R14R15 <- $FF, last two columns 17 and 18 will be empty
	0xBA, 		// 209		xch	R10		; R10 <- A = 10, position of decimal point HIGH
	0xDF, 		// 20A		ldi	15		; A <- 15
	0xB9, 		// 20B		xch	R9		; R9 <- 15, 14 valid characters, A <- 0 from R9
	0x29, 		// 20C		src	R89		; select register WR from R8R9, select M15
	0xE0, 		// 20D		wrm			; write WR.M15 <- 0, column 16 is not used
	0x42, 0x2C, 	// 20E		jmp	Print6		; start printing from register WR and R1415
	0x7F, 0x17, 	// 210	Print2:	ijnz	R15,Print3	; check if R15 = 14, increment R15
	0xDF, 		// 212		ldi	15		; A <- 15
	0xBF, 		// 213		xch	R15		; R15 <- 15, empty column 18
	0xA5, 		// 214		ld	R5		; A <- function code
	0x42, 0x26, 	// 215		jmp	Print5		; save A into R14
	0xD1, 		// 217	Print3:	ldi	1		; A <- 1
	0x8F, 		// 218		add	R15		; A <- 1 + R15
	0xF7, 		// 219		tcc			; A <- C
	0x14, 0x25, 	// 21A		jz	Print4		; jump if R15 < 13
	0xA4, 		// 21C		ld	R4		; A <- R4 function parameter
	0xBF, 		// 21D		xch	R15		; R15 <- A, function parameter at column 18
	0xBE, 		// 21E		xch	R14		; A <- R14, rounding selection (0,1,8)
	0xF6, 		// 21F		rar			; get R14.bit0 -> C, "round" selected
	0xF3, 		// 220		cmc			; complement C (= !R14.bit0)
	0xDE, 		// 221		ldi	14		; A <- 14
	0xF6, 		// 222		rar			; A <- 8*CY + 7 (7 = rounding, 15 = empty char if truncated)
	0x42, 0x26, 	// 223		jmp	Print5		; save A character into R14, to print rounding mark at column 17
	0xA4, 		// 225	Print4:	ld	R4		; A <- R4, function parameter
	0xBE, 		// 226	Print5:	xch	R14		; R14 <- A, code of character in column 17
	0x29, 		// 227		src	R89		; select address R8R9 = WR
	0xED, 		// 228		rd1			; A <- read status 1 (= position of decimal point)
	0xBA, 		// 229		xch	R10		; R10 <- position of decimal point
	0xED, 		// 22A		rd1			; A <- read status 1 (= position of decimal point)
	0xBB, 		// 22B		xch	R11		; R11 <- position of decimal point
	0x11, 0x2C, 	// 22C	Print6:	jt	Print6		; wait if TEST = 0 (wait if signal is active)
	0xD2, 		// 22E		ldi	2		; A <- 2
	0xBD, 		// 22F		xch	R13		; R13 <- 2, sector loop counter (14 sectors to print)
	0xEC, 		// 230		rd0			; read A <- WR.S0 (sign)
	0xF6, 		// 231		rar			; rotate A right, sign flag -> C
	0xF7, 		// 232		tcc			; A <- C, 0 if positive, 1 if negative
	0xE1, 		// 233		wmp			; write A to RAM0 port, select color (bit 0): red if negative, black if positive
	0x50, 0xB0, 	// 234		call	Keyboard	; keyboard handling, count sector
	0x68, 		// 236		inc	R8		; restore R8R9 to point to WR again
	0x6B, 		// 237	Print7:	inc	R11		; total digits counter
	0xAB, 		// 238		ld	R11		; A <- R11, total digits counter
	0xB9, 		// 239		xch	R9		; R9 <- total digits counter, start behind highest digit
	0x51, 0xA2, 	// 23A		call	Check0WR	; check whether remainging part of WR contains any digit (= higher part before number)
	0xF7, 		// 23C		tcc			; A <- carry (1 = if number is zero)
	0x14, 0x37, 	// 23D		jz	Print7		; loop if some digits remain
	0x11, 0x3F, 	// 23F	Drum1:	jt	Drum1		; wait for inactive printer drum sector signal (wait if TEST = 0)
	0xF0, 		// 241		clb			; clear A = 0 and Carry = 0
	0xE1, 		// 242		wmp			; write RAM0 port, printer control signals are set to inactive
	0xE2, 		// 243		wrr			; write ROM0 port, clear shifter signals
	0x7D, 0x53, 	// 244		ijnz	R13,Drum2	; jump to next sector if R13 not 0
	0x2A, 0x0C, 	// 246		fim	R1011,$0C	; R10R11 <- $0C, R11 = 12 counter to send ADV signal (number of steps 16 - 12 = 4)
	0x2E, 0x00, 	// 248		fim	R1415,$00	; R14R15 <- $00
	0xD8, 		// 24A		ldi	8		; A <- 8, bit 3 = advance printer paper
	0x11, 0x4B, 	// 24B		jt	PaperAdv_3	; wait for inactive printer drum sector signal (wait if TEST = 0)
	0xE1, 		// 24D		wmp			; write RAM0 port: first 8 advance printer paper, later 0 to clear signals
	0x50, 0xB0, 	// 24E		call	Keyboard	; keyboard handling (and wait for active drum sector signal)
	0x7B, 0x4B, 	// 250		ijnz	R11,PaperAdv_3	; loop, send ADV signal (4 loops)
	0xC0, 		// 252		ret	0		; return with A = 0
	0x50, 0x6A, 	// 253	Drum2:	call	DrumSync	; R12 synchronization with printer drum sector
	0xB8, 		// 255		xch	R8		; R8 <- A = 0, reset digit pointer in WR
	0xDD, 		// 256	Drum3:	ldi	13		; A <- 13
	0x9F, 		// 257		sub	R15		; A <- 13 - R15 (C=0), check if R15 = 13
	0xF1, 		// 258		clc			; clear carry
	0x1C, 0x5F, 	// 259		jnz	Drum4		; skip if R15 is not 13
	0xBA, 		// 25B		xch	R10		; R10 <- 0, place of decimal point
	0xDF, 		// 25C	Drum34:	ldi	15		; A <- 15, print no digit
	0x42, 0x61, 	// 25D		jmp	Drum5
	0x27, 		// 25F	Drum4:	src	R67		; select register and digit WR from R6R7
	0xE9, 		// 260		rdm			; A <- read one digit
	0x77, 0x77, 	// 261	Drum5:	ijnz	R7,Drum8	; jump to next digit
	0xAA, 		// 263		ld	R10		; A <- R10, position of decimal point
	0x1C, 0x68, 	// 264		jnz	Drum6		; decimal point is already shifted (so column 16 is alredy skipped)
	0x52, 0x8F, 	// 266		call	DrumShift2	; shift one inactive column into printer shifter - skip column 16
	0xAF, 		// 268	Drum6:	ld	R15		; A <- R15, character in column 18
	0x52, 0x8A, 	// 269		call	DrumShift	; if R15=R12, shift 1 into print shifter, or 0 otherwise
	0xAE, 		// 26B		ld	R14		; A <- R14, character in column 17
	0x52, 0x8A, 	// 26C		call	DrumShift	; if R14=R12, shift 1 into print shifter, or 0 otherwise
	0x19, 0x6E, 	// 26E	Drum7:	jnt	Drum7		; wait for active printer drum sector signal
	0xD2, 		// 270		ldi	2		; A <- 2, bit 1 = fire print hammer
	0x29, 		// 271		src	R89		; select RAM0 from R8R9
	0xE1, 		// 272		wmp			; output port RAM0, fire print hammers
	0x50, 0xB2, 	// 273		call	Keyboard1	; keyboard handling (R7 is cleared)
	0x42, 0x3F, 	// 275		jmp	Drum1		; loop back to next sector
	0x52, 0x8A, 	// 277	Drum8:	call	DrumShift	; if A = R12, shift 1 into printer shifter, or 0 otherwise
	0xAA, 		// 279		ld	R10		; A <- R10
	0x14, 0x83, 	// 27A		jz	Drum9		; jump if R10 = 0 (decimal point is already sent)
	0x97, 		// 27C		sub	R7		; A <- A - R7 (C = 0)
	0xF1, 		// 27D		clc			; clear Carry
	0x1C, 0x83, 	// 27E		jnz	Drum9		; jump if R10 != R7 (decimal point is not in this position
	0xDA, 		// 280		ldi	10		; A <- 10, decimal point code character
	0x52, 0x8A, 	// 281		call	DrumShift	; if R12=10, shift 1 into printer shifter, or 0 otherwise
	0xA7, 		// 283	Drum9:	ld	R7		; A <- R7, index of the digit
	0x9B, 		// 284		sub	R11		; A <- R7 - R11
	0xF7, 		// 285		tcc			; A <- Carry
	0x14, 0x56, 	// 286		jz	Drum3		; loop back for next valid digits
	0x42, 0x5C, 	// 288		jmp	Drum34		; loop back for empty columns
	0x9C, 		// 28A		sub	R12		; A <- A - R12 - CY
	0xF1, 		// 28B		clc			; clear carry - hammer not fired
	0x1C, 0x8F, 	// 28C		jnz	DrumShift2	; skip if A != R12
	0xFA, 		// 28E		stc			; set carry - hammer will be fired
	0xD1, 		// 28F		ldi	1		; A <- 1
	0xF5, 		// 290		ral			; A <- 2 + Carry
	0xF5, 		// 291		ral			; A <- bit 2 is set (clock), bit 1 = Carry (data)
	0x40, 0x65, 	// 292		jmp	SendShift	; send bit into printer shifter
	0xA4, 		// 294		ld	R4		; A <- R4, function parameter
	0xBD, 		// 295		xch	R13		; R13 <- function parameter (digit)
	0x26, 0x40, 	// 296		fim	R67,$40		; R6R7 <- $40, address of CR
	0x27, 		// 298		src	R67		; select CR register from R6R7
	0xEE, 		// 299		rd2			; A <- read CR.S2, digit entry mode status
	0x1C, 0xA1, 	// 29A		jnz	DigStart_3	; jump if calculator is already in digit entry mode
	0xD8, 		// 29C		ldi	8		; A <- 8, mode digit without decimal point
	0xE6, 		// 29D		wr2			; write 8 into CR.S2, flag of digit entry mode
	0xF0, 		// 29E		clb			; clear A and C
	0x51, 0x4A, 	// 29F		call	ClearWR		; clear WR, WR.S0 and WR.S1
	0x41, 0xC6, 	// 2A1		jmp	GetDPWR		; R11 = WR.S1, get decimal point position
	0x51, 0x0A, 	// 2A3		call	MovWRTR		; WR <- TR
	0x51, 0x46, 	// 2A5		call	ClearSR		; clear SR (including S0 and S1)
	0x51, 0x49, 	// 2A7		call	ClearDR		; clear TR (including S0 and S1)
	0xC0, 		// 2A9		ret	0		; return with A = 0
	0xD3, 		// 2AA		ldi	3		; A <- 3
	0xB5, 		// 2AB		xch	R5		; R5 <- 3, set function code
	0xFA, 		// 2AC		stc			; set carry, jump to macro
	0xC1, 		// 2AD		ret	1		; return with A = 1, C = 1, jump to macro
	0xD1, 		// 2AE	DecDP_2:ldi	1		; A <- 1, to decrement
	0xB3, 		// 2AF		xch	R3		; R3 <- 1, amount of decrement R10R11
	0xBB, 		// 2B0		xch	R11		; A <- R11, current decimal point counter
	0x42, 0xC2, 	// 2B1		jmp	AdjDiv_3	; adjust R10R11
	0x52, 0xF9, 	// 2B3		call	ReadDP		; read decimal places of DR (->R2) and WR (->R3)
	0xEF, 		// 2B5		rd3			; A <- read decimal point switch WR.S3
	0xE5, 		// 2B6		wr1			; write decimal places to WR.S1 from A
	0x42, 0xC2, 	// 2B7		jmp	AdjDiv_3	; adjust R10R11 = required DP - current DP
	0x52, 0xF9, 	// 2B9		call	ReadDP		; read decimal places of DR (->R2) and WR (->R3)
	0xDD, 		// 2BB		ldi	13		; A <- 13, max. decimal places
	0x9B, 		// 2BC		sub	R11		; A <- 13 - R11 (C=0), 13-current decimal places
	0xF1, 		// 2BD		clc			; clear carry
	0x82, 		// 2BE		add	R2		; A <- 13 - R11 + R2 (add decimals of DR)
	0xBA, 		// 2BF		xch	R10		; R10 <- 13 - R11 + R2
	0xF7, 		// 2C0		tcc			; A <- C
	0xBA, 		// 2C1		xch	R10		; R10 <- C (carry to high nibble), A <- 13 - R11 + R2
	0x93, 		// 2C2		sub	R3		; A <- A - R3, decrease decimal point counter LOW
	0xBB, 		// 2C3		xch	R11		; R11 <- new value LOW
	0xF3, 		// 2C4		cmc			; complement carry
	0xBA, 		// 2C5		xch	R10		; A <- decimal point counter HIGH
	0x99, 		// 2C6		sub	R9		; borrow is subtracted from upper half (here is R9=0)
	0xBA, 		// 2C7		xch	R10		; R10 <- R10 - C
	0xF1, 		// 2C8		clc			; clear carry
	0xC0, 		// 2C9		ret	0		; return with A = 0
	0x52, 0xF9, 	// 2CA		call	ReadDP		; read decimal places of DR (->R2) and WR (->R3)
	0xA3, 		// 2CC		ld	R3		; A <- R3, decimal places of WR
	0x8B, 		// 2CD		add	R11		; A <- R3 + R11 (C=0), C<-0
	0x82, 		// 2CE		add	R2		; A <- R3 + R11 + R2 (C=0)
	0xBB, 		// 2CF		xch	R11		; R11 <- R3 + R11 + R2
	0xF7, 		// 2D0		tcc			; A <- C
	0xBA, 		// 2D1		xch	R10		; R10 <- high nibble
	0xC0, 		// 2D2		ret	0		; return with A = 0
	0xD4, 		// 2D3		ldi	4		; A <- 4
	0x85, 		// 2D4		add	R5		; A <- 4 + R5, function code + 4
	0xB6, 		// 2D5		xch	R6		; R6 <- function code + 4 = indirect register
	0x27, 		// 2D6		src	R67		; select IR register from R6R7
	0xEC, 		// 2D7		rd0			; A <- read sign of IR
	0xB2, 		// 2D8		xch	R2		; R2 <- sign of IR
	0x29, 		// 2D9		src	R89		; select WR register from R8R9
	0xEC, 		// 2DA		rd0			; A <- read sign of WR
	0x82, 		// 2DB		add	R2		; A <- sign IR + sign WR, bit 0 is 0 if both numbers have same sign
	0xF6, 		// 2DC		rar			; rotate result bit 0 into C
	0xC1, 		// 2DD		ret	1		; return with A = 1, C = 1 jump if numbers have different sign
	0x52, 0xD6, 	// 2DE		call	CheckSign_3	; compare WR and DR sign
	0xF7, 		// 2E0		tcc			; A <- CY, 1 if different sign
	0x66, 		// 2E1		inc	R6		; increment R6 to point to RR
	0x27, 		// 2E2		src	R67		; select RR from R6R7
	0xE4, 		// 2E3		wr0			; set sign of RR
	0xDF, 		// 2E4		ldi	15		; A <- 15
	0xBD, 		// 2E5		xch	R13		; R13 <- 15, used as loop-end indicator
	0xC0, 		// 2E6		ret	0		; return with A = 0
	0x29, 		// 2E7	NegWR_2:src	R89		; select WR register from R8R9
	0xEC, 		// 2E8		rd0			; read sign WR.S0
	0xF4, 		// 2E9		cma			; complement accumulator
	0xE4, 		// 2EA		wr0			; write new sign WR.S0
	0xC0, 		// 2EB		ret	0		; return with A = 0
	0xDB, 		// 2EC		ldi	11		; A <- 11
	0x8D, 		// 2ED		add	R13		; A <- R13 + 11 (C = 0), digit + 11, C <- 1 if R13 > 4
	0x1A, 0xF1, 	// 2EE		jnc	RoundWR_3	; skip if R1R675 (CY=0), no rounding
	0x6E, 		// 2F0		inc	R14		; increment R14, too
	0xD0, 		// 2F1		ldi	0		; A <- 0
	0x29, 		// 2F2		src	R89		; select WR register from R8R9
	0xEB, 		// 2F3		adm			; A <- A + mem + C
	0xFB, 		// 2F4		daa			; decimal adjust
	0xE0, 		// 2F5		wrm			; write new digit
	0x79, 0xF1, 	// 2F6		ijnz	R9,RoundWR_3	; increment R9 (digit pointer) and loop to next digit
	0xC0, 		// 2F8		ret	0		; return with A = 0
	0x27, 		// 2F9	ReadDP:	src	R67		; select DR from R6R7
	0xED, 		// 2FA		rd1			; read DR.S1, decimal places
	0xB2, 		// 2FB		xch	R2		; R2 <- DR.S1 decimal places
	0x29, 		// 2FC		src	R89		; select WR from R8R9
	0xED, 		// 2FD		rd1			; read WR.S1, decimal places
	0xB3, 		// 2FE		xch	R3		; R3 <- WR.S1 decimal plaves
	0xC0, 		// 2FF		ret	0		; return with A = 0
	0x32, 		// 300		fin	R23		; load code R2R3 <- [PC:R0R1]
	0xC0, 		// 301		ret	0		; return with A = 0
	0x30, 		// 302		fin	R01		; load code R0R1 <- [PC:R0R1]
	0x40, 0x4B, 	// 303		jmp	Exec		; jump to macro interpreter
	0xED, 		// 305	fn_sqrt:   = @Sqrt ;$ed    ;SQRT (+ JMP num_dpadj)			;square root of WR is placed into RR
	0x6C, 0x14, 	// 306		= @JmpNewNum, @md_prn2 ;$6c $14 ;JPC MODENN,md_prn2 ;jump, if new number is entered
	0x75, 0x0E, 	// 308		= @ReadRRS2, @md_prn1 ;$75 $0e ;JPC MODEMD,md_prn1 ;jump, if mul or div was the last operation
	0xD9, 		// 30A		= @LoadRes ;$d9 ;MOV WR,TR + CLR TR + CLR SR ;if add or sub was the last operation, then main total is recalled
	0xFC, 		// 30B		= @PrintFPar ;$fc ;PRN FPAR
	0xA7, 0x0F, 	// 30C		= @Jump, @md_exitf ; $a7 $0f ;JMP md_exitf
	0xFB, 		// 30E		= @PrintFParEmp ;$fb ;PRN FPAR,FCODE
	0x8D, 		// 30F		= @SetMulPar ;$8d ;SET MOPPAR ;keep the operation (from the parameter) for the next round
	0x04, 		// 310		= @MovDRWR ;$04 ;MOV DR,WR ;put the number into DR and CR
	0x02, 		// 311		= @MovCRWR ;$02 ;MOV CR,WR
	0x87, 		// 312		= @SetMulDiv ;$87 ;SET MODEMD
	0xEF, 		// 313		= @ClrOver ;$ef    ;CLR MENT + CLR OVFL + RET
	0xFC, 		// 314		= @PrintFPar ; $fc ;PRN FPAR
	0x6D, 0x0F, 	// 315		= @JmpNMul, @md_exitf ; $6d $0f ;JPC MOPN,md_exitf ;jump, if the other operand is not entered yet
	0x7B, 0x0F, 	// 317		= @CheckDivCR, @md_exitf ; $7b $0f ;JPC MOPCONST,md_exitf ;jump, at constant calculation (new number for calculation)
	0x76, 0x46, 	// 319		= @ReadDRS2, @mul_start ;$76 $46 ;JPC MOPMUL,mul_start ;jump, if previous operation is multiply
	0x8D, 		// 31B		= @SetMulPar ;$8d ;SET MOPPAR ;divide is marked into MOP
	0xA2, 0x3C, 	// 31C		= @Check0WR, @num_overf ;$a2 $3c ;JPC ZERO_WR,num_overf ;divide by zero would result overflow
	0x48, 		// 31E		= @ClearRR ;$48 ;CLR RR
	0xA0, 0x73, 	// 31F		= @Check0DR, @num_res ;$a0 $73 ;CLR DIGIT + JPC ZERO_DR,num_res	;if dividend is zero, the result will be zero too
	0xE1, 		// 321		= @AdjDiv ;$e1  ;GET DPCNTDIV ;digit point initialization for divide
	0x9E, 0x32, 	// 322		= @CheckSmallDR, @div_lshDR ;$9e $32 ;CLR DIGIT + JPC NBIG_DR,div_lshDR ;rotate DR into leftmost position
	0x9A, 0x36, 	// 324		= @CheckSmallWR, @div_lshWR ;$9a $36 ;JPC NBIG_WR,div_lshWR ;rotate WR into leftmost position
	0xE5, 		// 326		= @SetDMSign ;$e5  ;SET DIVMUL_SIGN + MOV DIGIT,15 ;sign of result is set
	0x51, 		// 327		= @ShiftL_RR ;$51    ;SHL RR ;15 is shifted into the cleared RR, as a mark for loop end
	0x51, 		// 328		= @ShiftL_RR ;$51    ;SHL RR
	0x34, 0x29, 	// 329		= @SubDRWR, @div_loop ;$34 $29 ;SUB DR,WR + JPC NNEG,div_loop + INC DIGIT ;find, how many times the subtraction can be done
	0x21, 		// 32B		= @AddDRWR ;$21  ;ADD DR,WR ;adding back the last unneeded subtract
	0x51, 		// 32C		= @ShiftL_RR ;$51  ;SHL RR ;next digit of result is shifted into RR
	0xA9, 0x3F, 	// 32D		= @CheckR139, @div_finsh ;$a9 $3f ;JPC BIG_DIGIT,div_finsh ;if shifted out number>9, end of division
	0x52, 		// 32F		= @ShiftL_DR ;$52  ;SHL DR ;next digit (shifted out from RR) is shifted into DR
	0xA7, 0x29, 	// 330		= @Jump, @div_loop ;$a7 $29 ;JMP div_loop
	0x52, 		// 332		= @ShiftL_DR ;$52    ;SHL DR	;one digit rotate left of DR
	0xCA, 		// 333		= @IncDP ;$ca    ;INC DPCNT
	0xA7, 0x22, 	// 334		= @Jump, @div_chkDR ;$a7 $22 ;JMP div_chkDR
	0x53, 		// 336		= @ShiftL ;$53    ;SHL WR ;one digit rotate left of WR
	0xCF, 0x3C, 	// 337		= @CheckDP0, @num_overf ;$cf $3c ;JPC ZERO_DPCNT,num_overf ;jump if rotate would cause overflow
	0xDD, 		// 339		= @DecDP ;$dd   ;DEC DPCNT
	0xA7, 0x24, 	// 33A		= @Jump, @div_chkWR ;$a7 $24 ;JMP div_chkWR
	0xFF, 		// 33C		= @PrintOvfl ;$ff  ;PRN OVFL ;print overflow
	0x85, 		// 33D		= @SetOver ;$85    ;SET OVFL					;set overflow flag
	0xF1, 		// 33E		= @ClrModeNoDP ;$f1  ;CLR MODE + CLR MENT + RET ;exit
	0x5D, 		// 33F		= @ShiftR_RR ;$5d    ;SHR RR	;rotate the number right
	0xCE, 0x73, 	// 340		= @CheckDP13, @num_res ;$ce $73 ;JPC NBIG_DPCNT,num_res	;jump, if the result contains acceptable number of digits
	0xDD, 		// 342		= @DecDP ;$dd    ;DEC DPCNT	;otherwise shift the number to right
	0x5D, 		// 343		= @ShiftR_RR ;$5d    ;SHR RR	;Note: the place of this instruction could have been saved,
	0xA7, 0x40, 	// 344		= @Jump, @num_dpadj ;$a7 $40 ;JMP num_dpadj ;  if the jump would go back to div_finsh
	0x8D, 		// 346		= @SetMulPar ;$8d  ;SET MOPPAR  ;multiplication is marked in MOP
	0x03, 		// 347		= @MovRRWR ;$03  ;MOV RR,WR
	0xE3, 		// 348		= @AdjMul ;$e3  ;GET DPCNTMUL	;digit point initialization for multiply
	0xE5, 		// 349		= @SetDMSign ;$e5  ;SET DIVMUL_SIGN + MOV DIGIT,15  ;sign of result is set
	0x0E, 		// 34A		= @MovWRDR ;$0e  ;MOV WR,DR
	0x49, 		// 34B		= @ClearDR ;$49  ;CLR DR
	0x52, 		// 34C		= @ShiftL_DR ;$52  ;SHL DR  ;shift R13=15 into DR, but it is immediately shifted into RR
	0x5E, 		// 34D		= @ShiftR_DR ;$5e  ;SHR DR  ;DR-RR is shifted right
	0x5A, 		// 34E		= @ShiftRShort_RR ;$5a  ;SSR RR
	0xA9, 0x56, 	// 34F		= @CheckR139, @mul_shres ;$a9 $56 ;JPC BIG_DIGIT,mul_shres ;jump if R13=15 was shifted out (exit from the loop)
	0xAC, 0x4D, 	// 351		= @DecDigJmp0, @mul_loopn ;$ac $4d ;JPC ZERO_DIGIT,mul_loopn + DEC DIGIT ;multiply the number with one digit
	0x21, 		// 353		= @AddDRWR ;$21  ;ADD DR,WR  ;finally DR=DR+R13*WR
	0xA7, 0x51, 	// 354		= @Jump, @mul_loopd ;$a7 $51 ;JMP mul_loopd
	0xA0, 0x40, 	// 356		= @Check0DR, @num_dpadj ;$a0 $40 ;CLR DIGIT + JPC ZERO_DR,num_dpadj ;rotate nonzero digits from DR to RR
	0xCF, 0x3C, 	// 358		= @CheckDP0, @num_overf ;$cf $3c ;JPC ZERO_DPCNT,num_overf ;jump if overflow occurred
	0x5E, 		// 35A		= @ShiftR_DR ;$5e    ;SHR DR	;DR-RR is shifted right
	0x5A, 		// 35B		= @ShiftRShort_RR ;$5a    ;SSR RR
	0xDD, 		// 35C		= @DecDP ;$dd    ;DEC DPCNT
	0xA7, 0x56, 	// 35D		= @Jump, @mul_shres ;$a7 $56 ;JMP mul_shres
	0x86, 		// 35F		= @SetNumDP ;$86    ;SET MENTDP	;digit point flag
	0xF3, 		// 360		= @ClrMode ;$f3    ;CLR MODE + RET
	0xFE, 		// 361		= @PrintFCode ;$fe    ;PRN FCODE
	0xCA, 		// 362		= @IncDP ;$ca    ;INC DPCNT	;increment the digit point place counter by 2
	0xCA, 		// 363		= @IncDP ;$ca    ;INC DPCNT
	0xA7, 0x67, 	// 364		= @Jump, @num_md ;$a7 $67 ;JMP num_md
	0xFE, 		// 366		= @PrintFCode ;$fe    ;PRN FCODE
	0x7B, 0x6F, 	// 367		= @CheckDivCR, @num_mul2 ;$7b $6f ;JPC MOPCONST,num_mul2  ;jump at const divide/multiply
	0x90, 		// 369		= @SetDivCR ;$90    ;SET MOPCONST
	0x76, 0x47, 	// 36A		= @ReadDRS2, @mul_st2 ;$76 $47 ;JPC MOPMUL,mul_st2  ;jump to multiply, if previous operation is multiply
	0x02, 		// 36C		= @MovCRWR ;$02    ;MOV CR,WR	;save the divisor for constant divide
	0xA7, 0x1C, 	// 36D		= @Jump, @div_chk0 ;$a7 $1c ;JMP div_chk0	;jump to divide
	0x04, 		// 36F		= @MovDRWR ;$04    ;MOV DR,WR	;save the number into DR
	0x0C, 		// 370		= @MovWRCR ;$0c    ;MOV WR,CR	;recall previous number from CR
	0xA7, 0x6A, 	// 371		= @Jump, @num_mul1 ;$a7 $6a ;JMP num_mul1	;jump to divide or multiply
	0x0D, 		// 373		= @MovWRRR ;$0d    ;MOV WR,RR	;copy the RR result to WR
	0xC2, 		// 374		= @SetDPWR ;$c2    ;SET DP_WR	;set the digit point position from R10R11
	0xB1, 0x10, 	// 375		= @CheckNewOp, @md_exitc ;$b1 $10 ;JPC NEWOP,md_exitc	;jump to exit at new mul and div operation
	0xB4, 0x7B, 	// 377		= @CheckMemOp, @num_adj ;$b4 $7b ;JPC MEMOP,num_adj	;jump to adjust at M=+/M=-
	0x6E, 0x9E, 	// 379		= @JmpNTrunc, @num_pra2 ;$6e $9e ;JPC NTRUNC,num_pra2	;jump to result print, if digit point should not be adjusted
	0xDF, 		// 37B		= @DiffDP ;$df    ;GET DPDIFF	;WR.S1=WR.S3, set R10R11 to the difference between required an actual digit point
	0xCF, 0x9A, 	// 37C		= @CheckDP0, @num_pra1 ;$cf $9a ;JPC ZERO_DPCNT,num_pra1  ;jump, if number is at the right digit point place
	0xCE, 0x84, 	// 37E		= @CheckDP13, @num_lrot ;$ce $84 ;JPC NBIG_DPCNT,num_lrot
	0xCA, 		// 380		= @IncDP ;$ca    ;INC DPCNT	;Rotate right
	0x5F, 		// 381		= @ShiftR ;$5f    ;SHR WR
	0xA7, 0x7C, 	// 382		= @Jump, @num_rotl ;$a7 $7c ;JMP num_rotl
	0xDD, 		// 384		= @DecDP ;$dd    ;DEC DPCNT	;Rotate left
	0x53, 		// 385		= @ShiftL ;$53    ;SHL WR
	0x9A, 0x7C, 	// 386		= @CheckSmallWR, @num_rotl ;$9a $7c ;JPC NBIG_WR,num_rotl
	0xA7, 0x3C, 	// 388		= @Jump, @num_overf ;$a7 $3c ;JMP num_overf	;print overflow
	0x6C, 0x66, 	// 38A		= @JmpNewNum, @num_prm ;$6c $66 ;JPC MODENN,num_prm	;jump, if new number is entered
	0x75, 0x66, 	// 38C		= @ReadRRS2, @num_prm ;$75 $66 ;JPC MODEMD,num_prm	;jump, if there is started mul/div operation
	0xD9, 		// 38E		= @LoadRes ;$d9    ;MOV WR,TR + CLR TR + CLR SR	  ;recall main total
	0xA7, 0x98, 	// 38F		= @Jump, @fn_memadd ;$a7 $98 ;JMP fn_memadd	;jump to add functions
	0x6C, 0x98, 	// 391		= @JmpNewNum, @fn_memadd ;$6c $98 ;JPC MODENN,fn_memadd	 ;jump, if new number is enterer
	0x75, 0x97, 	// 393		= @ReadRRS2, @clr_md ;$75 $97 ;JPC MODEMD,clr_md  ;jump, if there is started mul/div operation
	0xA7, 0x98, 	// 395		= @Jump, @fn_memadd ;$a7 $98 ;JMP fn_memadd ;jump to add functions
	0x82, 		// 397		= @ClearDRS2 ;$82    ;CLR MOP	;ignore previous mul/div operation
	0xAE, 0x7B, 	// 398		= @ClrDigJmp @num_adj ;$ae $7b ;CLR DIGIT + JMP num_adj	;jump to adjust the number to the required digits
	0xB1, 0xAA, 	// 39A		= @CheckNewOp, @num_pra3 ;$b1 $aa ;JPC NEWOP,num_pra3	;jump at new add/sub operation
	0x77, 0xA3, 	// 39C		= @ReadWRS2, @num_round ;$77 $a3 ;JPC ROUND,num_round	;jump to rounding, if rounding switch is in that position
	0xFD, 		// 39E		= @PrintRoundFPar ;$fd    ;PRN ROUND,FPAR
	0xEB, 		// 39F		= @PaperAdv ;$eb    ;PRN ADVANCE + CLR DPCNT
	0xB4, 0xA8, 	// 3A0		= @CheckMemOp, @mem_add ;$b4 $a8 ;JPC MEMOP,mem_add	;jump to change the function code at M=+/M=-/M+/M-
	0xF1, 		// 3A2		= @ClrModeNoDP ;$f1    ;CLR MODE + CLR MENT + RET
	0xE9, 		// 3A3		= @RoundWR ;$e9    ;ROUNDING	;do the rounding based on the last shifted out digit in R13
	0x9A, 0x9E, 	// 3A4		= @CheckSmallWR, @num_pra2 ;$9a $9e ;JPC NBIG_WR,num_pra2  ;may generate overflow too
	0xA7, 0x3C, 	// 3A6		= @Jump, @num_overf ;$a7 $3c ;JMP num_overf	;print overflow
	0xDB, 0xAB, 	// 3A8		= @JmpMem, @do_prpadd ;$db $ab ;SET MEMFUNC + JMP do_prpadd	;Set M+/M- function code
	0xFC, 		// 3AA		= @PrintFPar ;$fc    ;PRN FPAR
	0xC6, 		// 3AB		= @GetDPWR ;$c6    ;GET DP_WR
	0x03, 		// 3AC		= @MovRRWR ;$03    ;MOV RR,WR
	0xBC, 0xB0, 	// 3AD		= @OddPar, @skp_neg ;$bc $b0 ;JPC ODDPAR,skp_neg   ;skip negate the number at add
	0xE7, 		// 3AF		= @NegWR ;$e7    ;NEG WR	;negate the number at sub (convert it to add)
	0xD4, 0xB7, 	// 3B0		= @CheckSign, @do_sub ;$d4 $b7 ;JPC DIFF_SIGN,do_sub	;jump, when adding a negative and a positive number
	0x1E, 		// 3B2		= @AddIRWR ;$1e    ;ADD IR,WR	;ADD - may generate overflow
	0x97, 0xBD, 	// 3B3		= @CheckSmallIR, @do_next ;$97 $bd ;JPC NBIG_IR,do_next, ;jump, if there is no overflow
	0x31, 0x3C, 	// 3B5		= @SubIRWR, @num_overf ;$31 $3c ;SUB IR,WR + JPC NNEG,num_overf + INC DIGIT	;correct back IR at overflow and jump always
	0x31, 0xBD, 	// 3B7		= @SubIRWR, @do_next ;$31 $bd ;SUB IR,WR + JPC NNEG,do_next + INC DIGIT	;SUB - never generates overflow
	0x1E, 		// 3B9		= @AddIRWR ;$1e    ;ADD IR,WR
	0x2C, 0xBC, 	// 3BA		= @SubWRIR, @do_cont ;$2c $bc ;SUB WR,IR + JPC NNEG,do_cont	;always goes to the next instruction
	0x01, 		// 3BC		= @MovIRWR ;$01    ;MOV IR,WR
	0x0D, 		// 3BD		= @MovWRRR ;$0d    ;MOV WR,RR	;take the original number from RR
	0xBF, 		// 3BE		= @SetDPIR ;$bf    ;SET DP_IR	;set the place of digit point
	0xB4, 0xFF, 	// 3BF		= @CheckMemOp, @do_exit ;$b4 $ff ;JPC MEMOP,do_exit	;exit at memory function
	0xB7, 0xAC, 	// 3C1		= @RotFC, @do_addsub ;$b7 $ac ;JPC ROTFC,do_addsub	;do the addsub for the next number, if there is instruction for it
	0x8A, 		// 3C3		= @SetAddSub ;$8a    ;SET MODEAS  ;mark, that last operation was add or sub
	0xEF, 		// 3C4		= @ClrOver ;$ef    ;CLR MENT + CLR OVFL + RET	;exit
	0x82, 		// 3C5		= @ClearDRS2 ;$82    ;CLR MOP
	0x49, 		// 3C6		= @ClearDR ;$49    ;CLR DR
	0xD9, 		// 3C7		= @LoadRes ;$d9    ;MOV WR,TR + CLR TR + CLR SR
	0x4A, 		// 3C8		= @ClearWR ;$4a    ;CLR WR
	0xFC, 		// 3C9		= @PrintFPar ;$fc    ;PRN FPAR
	0x4A, 		// 3CA		= @ClearWR ;$4a    ;CLR WR
	0x7F, 		// 3CB		= @ClearSRS2 ;$7f    ;CLR OVFL
	0xF1, 		// 3CC		= @ClrModeNoDP ;$f1    ;CLR MODE + CLR MENT + RET
	0x6C, 0xD5, 	// 3CD		= @JmpNewNum, @dm_prn2 ;$6c $d5 ;JPC MODENN,dm_prn2	;jump in entry mode, print the number, and close the entry mode
	0x75, 0xD3, 	// 3CF		= @ReadRRS2, @dm_prn1 ;$75 $d3 ;JPC MODEMD,dm_prn1	;jump in mul/div mode, print the number, and init
	0x0B, 		// 3D1		= @MovWRSR ;$0b    ;MOV WR,SR	;in add/sub mode, recall the subtotal number from SR and clear SR
	0x46, 		// 3D2		= @ClearSR ;$46    ;CLR SR
	0xFC, 		// 3D3		= @PrintFPar ;$fc    ;PRN FPAR
	0xEF, 		// 3D4		= @ClrOver ;$ef    ;CLR MENT + CLR OVFL + RET
	0xFD, 		// 3D5		= @PrintRoundFPar ;$fd    ;PRN ROUND,FPAR
	0xF1, 		// 3D6		= @ClrModeNoDP ;$f1    ;CLR MODE + CLR MENT + RET
	0xD7, 		// 3D7		= @DigStart ;$d7  ;DIGIT ;save digit into R13, place of digit point (WR.S1) into R11
	0xA9, 0xDF, 	// 3D8		= @CheckR139, @dig_dpsgn ;$a9 $df ;JPC BIG_DIGIT,dig_dpsgn ;jump at digit point, minus sign
	0x53, 		// 3DA		= @ShiftL ;$53    ;SHL WR	;rotate the number into WR
	0x9A, 0xE3, 	// 3DB		= @CheckSmallWR, @dig_chkdp ;$9a $e3 ;JPC NBIG_WR,dig_chkdp  ; jump, if there is now overflow
	0x5F, 		// 3DD		= @ShiftR ;$5f    ;SHR WR	;at overflow, rotate back the number (additional digits are lost)
	0xF3, 		// 3DE		= @ClrMode ;$f3    ;CLR MODE + RET	;mark that new number is entered since the last operation, and exit
	0xBC, 0x5F, 	// 3DF		= @OddPar, @dp_mark ;$bc $5f ;JPC ODDPAR,dp_mark  ;digit point button is pressed
	0xE7, 		// 3E1		= @NegWR ;$e7    ;NEG WR	;minus sign button is pressed
	0xF3, 		// 3E2		= @ClrMode ;$f3    ;CLR MODE + RET	;mark that new number is entered since the last operation, and exit
	0x74, 0xE8, 	// 3E3		= @ReadCRS2, @dig_incdp ;$74 $e8 ;JPC MENTDP,dig_incdp	;if digit point is already entered, jump to adjust it
	0xA7, 0xEE, 	// 3E5		= @Jump, @dig_nextd ;$a7 $ee ;JMP dig_nextd
	0x00, 		// 3E7	= $00    ;(unimplemented, never used)
	0xCA, 		// 3E8		= @IncDP ;$ca    ;INC DPCNT	;adjust the digit point place with one digit more
	0xCE, 0xED, 	// 3E9		= @CheckDP13, @dig_savdp ;$ce $ed ;JPC NBIG_DPCNT,dig_savdp
	0xDD, 		// 3EB		= @DecDP ;$dd    ;DEC DPCNT	;if already too much digit entered after the digit point,
	0x5F, 		// 3EC		= @ShiftR ;$5f    ;SHR WR	; ignore the new digit
	0xC2, 		// 3ED		= @SetDPWR ;$c2    ;SET DP_WR	;save the place of digit point
	0xB7, 0xDA, 	// 3EE		= @RotFC, @dig_numsh ;$b7 $da ;JPC ROTFC,dig_numsh  ;function code contains, how many '0's has to be entered yet
	0xF3, 		// 3F0		= @ClrMode ;$f3    ;CLR MODE + RET   ;mark that new number is entered since the last operation, and exit
	0xFD, 		// 3F1		= @PrintRoundFPar ;$fd    ;PRN ROUND,FPAR
	0x02, 		// 3F2		= @MovCRWR ;$02    ;MOV CR,WR	;CR=WR (WR is saved to CR)
	0x0E, 		// 3F3		= @MovWRDR ;$0e    ;MOV WR,DR
	0x03, 		// 3F4		= @MovRRWR ;$03    ;MOV RR,WR	;RR=DR
	0x0C, 		// 3F5		= @MovWRCR ;$0c    ;MOV WR,CR
	0x04, 		// 3F6		= @MovDRWR ;$04    ;MOV DR,WR	;DR=saved WR
	0x0D, 		// 3F7		= @MovWRRR ;$0d    ;MOV WR,RR	;WR=RR
	0xF1, 		// 3F8		= @ClrModeNoDP ;$f1    ;CLR MODE + CLR MENT + RET
	0x09, 		// 3F9		= @MovWRMR ;$09    ;MOV WR,MR
	0xFA, 		// 3FA		= @PrintFParM ;$fa    ;PRN FPAR,MEM
	0x44, 		// 3FB		= @ClearMR ;$44    ;CLR MR
	0xF1, 		// 3FC		= @ClrModeNoDP ;$f1    ;CLR MODE + CLR MENT + RET
	0x09, 		// 3FD		= @MovWRMR ;$09    ;MOV WR,MR
	0xFA, 		// 3FE		= @PrintFParM ;$fa    ;PRN FPAR,MEM
	0xF1, 		// 3FF		= @ClrModeNoDP ;$f1    ;CLR MODE + CLR MENT + RET
	0x20, 0x28, 	// 400	Sqrt_2:	fim R01 @sq_start ;$28		; pseudo code entry address of the SQRT function
	0x11, 0x06, 	// 402	$402: jt $406		;wait for the inactive printer drum sector signal
	0x50, 0xB0, 	// 404	          call Keyboard		;keyboard handling
	0x26, 0x20, 	// 406	$406: fim R67 $20
	0x28, 0x10, 	// 408	          fim R89 $10
	0x32, 		// 40A	          fin R23		;fetch pseudo instruction code into R2R3
	0xF0, 		// 40B	          clb
	0x54, 0x50, 	// 40C	          call SqrtCode		;execute the associated routine
	0x71, 0x11, 	// 40E	$40e: ijnz R1 $411		;inc R0R1, pseudo code instruction pointer
	0x60, 		// 410	          inc R0
	0x14, 0x02, 	// 411	$411: jz $402		;jump back, if ACC returned by the pseudo instruction was 0
	0xF7, 		// 413	          tcc
	0x14, 0x0E, 	// 414	          jz $40e		;if CY returned by the pseudo instruction was 0, R0R1 is incremented again
	0x30, 		// 416	          fin R01		;if CY was set to 1, read the pseudo code jump address
	0x44, 0x02, 	// 417	          jmp $402		;jump to continue the pseudo code from the modified address
	0x00, 0x00, 	// 419	= 0, 0
	0x00, 0x00, 	// 41B	= 0, 0
	0x00, 0x00, 	// 41D	= 0, 0
	0x00, 0x00, 	// 41F	= 0, 0
	0x00, 0x00, 	// 421	= 0, 0
	0x00, 0x00, 	// 423	= 0, 0
	0x00, 0x00, 	// 425	= 0, 0
	0x00, 		// 427	= 0
	0x51, 		// 428		= @Sqrt_PrintFCode ;$51    ;PRN FCODE		;print number with function code (9: SQRT)
	0xA7, 		// 429		= @Sqrt_MovCRWR ;$a7    ;MOV CR,WR		;save the number to the constant register
	0x53, 		// 42A		= @Sqrt_ClearRR ;$53    ;CLR RR			;clear result register
	0x61, 0x3E, 	// 42B		= @Sqrt_Check0WR, @sq_exit ;$61 $3e ;JPC ZERO_WR,sq_exit	;jump, if number is zero (the result will be also zero)
	0x65, 		// 42D		= @Sqrt_ClrDigGetDP ;$65    ;CLR DIGIT + GET DP_WR 	;R10R11=place of digit point
	0x63, 0x44, 	// 42E		= @Sqrt_CheckSmallWR, @sq_lshift ;$63 $44 ;JPC NBIG_WR,sq_lshift  ;number is adjusted to the leftmost position
	0x9C, 		// 430		= @Sqrt_ShiftR ;$9c    ;SHR WR		;one digit overshift is corrected back
	0x5B, 		// 431		= @Sqrt_MovDRWR ;$5b    ;MOV DR,WR	;remainder (DR) is initialized to the shifted number
	0x55, 		// 432		= @Sqrt_ClearWR ;$55    ;CLR WR		;initial subtrahend (WR) is cleared
	0x6A, 0x36, 	// 433		= @Sqrt_Digit, @sq_loopns ;$6a $36 ;SET LPCSQRT + SET DPCNTSQRT + JPC EVENDP,sq_loopns	;R15=13, sqrt digit point calculation
	0x58, 		// 435		= @Sqrt_ShiftL_DR ;$58    ;SHL DR		;multiplication by 10 of the remaining part
	0x7A, 		// 436		= @Sqrt_IncWRPos ;$7a    ;INC WR_POS		;increment the subtrahend (WR from position in R15) by 1
	0x5D, 0x41, 	// 437		= @Sqrt_SubDRWR, @sq_rptinc ;$5d $41 ;SUB DR,WR + JPC NNEG,sq_rptinc + INC DIGIT;remainder is decremented by the subtrahend (DR=DR-WR)
	0x5F, 		// 439		= @Sqrt_AddDRWR ;$5f    ;ADD DR,WR		;add the subtrahend to get back the last non negative value
	0x85, 		// 43A		= @Sqrt_DecWRPos ;$85    ;DEC WR_POS		;decrement the subtrahend by one (prepare it for the next round)
	0x57, 		// 43B		= @Sqrt_ShiftL_RR ;$57    ;SHL RR		;shift the new digit into the number, R13 is cleared too
	0x98, 0x35, 	// 43C		= @Sqrt_Loop, @sq_loopsh ;$98 $35 ;JPC NZERO_LPCSQRT,sq_loopsh + DEC LPCSQRT	;decrement R15, and jump, except when R15 becomes 0
	0xA9, 		// 43E		= @Sqrt_MovDRWR2 ;$a9    ;MOV DR,WR	(MOV WR,CR ???)	  ;??? subtrahend is saved (originally it may be WR=CR)
	0x5B, 		// 43F		= @Sqrt_MovDRWR ;$5b    ;MOV DR,WR		;??? duplicated, but not disturbing code
	0x9F, 		// 440		= @Sqrt_Ret ;$9f    ;CLR MOP + RET_BPC		;return back to basic pseudo code interpreter to address $40
	0x7A, 		// 441		= @Sqrt_IncWRPos ;$7a    ;INC WR_POS		;increment the subtrahend by 1 (WR from position in R15)
	0x96, 0x36, 	// 442		= @Sqrt_Jump, @sq_loopns ;$96 $36 ;JMP sq_loopns	;jump back
	0x59, 		// 444		= @Sqrt_ShiftL ;$59    ;SHL WR		;rotate number into left position
	0x93, 0x2E, 	// 445		= @Sqrt_IncDPJmp, @sq_bshift ;$93 $2e ;INC DPCNT + JMP sq_bshift  ;increment R10R11, and jump back
	0x00, 0x00, 	// 447	= 0, 0
	0x00, 0x00, 	// 449	= 0, 0
	0x00, 0x00, 	// 44B	= 0, 0
	0x00, 0x00, 	// 44D	= 0, 0
	0x00, 		// 44F	= 0
	0x33, 		// 450		jin	R23		;jump to the pseudo instruction code associated routine
	0x41, 0xFE, 	// 451		jmp	PrintFCode	;PRN FCODE
	0x41, 0x48, 	// 453		jmp	ClearRR		;CLR RR
	0x41, 0x4A, 	// 455		jmp	ClearWR		;CLR WR
	0x68, 		// 457		inc	R8		;SHL RR
	0x68, 		// 458		inc	R8		;SHL DR
	0x41, 0x53, 	// 459		jmp	ShiftL		;SHL WR
	0x41, 0x04, 	// 45B		jmp	MovDRWR		;MOV DR,WR
	0x41, 0x34, 	// 45D		jmp	SubDRWR		;SUB DR,WR + JPC NNEG + INC DIGIT
	0x41, 0x21, 	// 45F		jmp	AddDRWR		;ADD DR,WR
	0x41, 0xA2, 	// 461		jmp	Check0WR	;JPC ZERO_WR
	0x41, 0x9A, 	// 463		jmp	CheckSmallWR	;JPC NBIG_WR
	0xBD, 		// 465		xch	R13		; R13 <- A, clear digit (R13=0)
	0x29, 		// 466		src	R89		; select WR register from R8R9 (= $10)
	0xED, 		// 467		rd1			; read status WR.S1 = decimal point position
	0xBB, 		// 468		xch	R11		; R11 <- WR.S1, decimal point position
	0xC0, 		// 469		ret	0		; return with A = 0
	0x2E, 0x6D, 	// 46A		fim	R1415,$6d	; R14 <- 6, R15 <- 13
	0xAB, 		// 46C		ld	R11		; A <- R11, decimal point position
	0xB7, 		// 46D		xch	R7		; R7 <- R11 (save original R11 into R7)
	0xBA, 		// 46E		xch	R10		; A <-> R10  (R10=0 [previous R7])
	0xF6, 		// 46F		rar			; shift A right, C <- R10.bit0, check if decimal point position is even
	0xAB, 		// 470		ld	R11		; A <- R11, decimal point position
	0xF6, 		// 471		rar			; A <- 8*(R10.bit0) + (R11 div 2),  C=(R11 mod 2)
	0x8E, 		// 472		add	R14		; A <- 8*(R10.bit0) + (R11 div 2) + (R11 mod 2) + 6, C=overflow
	0xBB, 		// 473		xch	R11		; store it to R11
	0xF7, 		// 474		tcc			; A <- C
	0xBA, 		// 475		xch	R10		; R10 <- 0 or 1
	0xB7, 		// 476		xch	R7		; A <- original R11
	0xF6, 		// 477		rar			; C=(R11 mod 2), rotate bit 0 into C
	0xF3, 		// 478		cmc			; CY=1-(R11 mod 2), negate the pseudo jump condition
	0xC1, 		// 479		ret	1		; return with A = 1, C = 1 jump
	0xAF, 		// 47A		ld	R15		; A <- R15, start digit position
	0xB9, 		// 47B		xch	R9		; R9 <- R15, start digit position
	0xFA, 		// 47C		stc			; C <- 1, overflow flag to increase digits
	0xD0, 		// 47D		ldi	0		; A <- 0
	0x29, 		// 47E		src	R89		; select WR register (and digit) from R8R9
	0xEB, 		// 47F		adm			; A <- A + digit
	0xFB, 		// 480		daa			; decimal adjust
	0xE0, 		// 481		wrm			; write digit back from A
	0x79, 0x7D, 	// 482		ijnz	R9, Sqrt_IncWRPos2 ; increment R9 position and loop back for the next digits
	0xC0, 		// 484		ret	0		; return with A = 0
	0xAF, 		// 485		ld	R15		; A <- R15, start digit position
	0xB9, 		// 486		xch	R9		; R9 <- R15, start digit position
	0xF3, 		// 487		cmc			; at first: set CY=1, later complement the borrow bit
	0x29, 		// 488		src	R89		; select WR register (and digit) from R8R9
	0xE9, 		// 489		rdm			; A <- read next digit from WR
	0x97, 		// 48A		sub	R7		; subtract R7 (=0) from it (to subtract C), A <- A + ~0 + ~C
	0x12, 0x8E, 	// 48B		jc	Sqrt_DecWRPos4	; jump, if there is no borrow
	0xD9, 		// 48D		ldi	9		; A <- set the number to 9 (BCD adjust)
	0xE0, 		// 48E		wrm			; write back the result digit
	0x79, 0x87, 	// 48F		ijnz	R9,Sqrt_DecWRPos2 ; increment R9 position and loop back for the next digits
	0xF0, 		// 491		clb			; A <- 0, C <- 0
	0xC0, 		// 492		ret	0		; return with A = 0
	0x7B, 0x96, 	// 493		ijnz	R11,Sqrt_Jump	; inc R11, and skip if result is nonzero
	0x6A, 		// 495		inc	R10		; inc R10 (increment decimal point counter HIGH)
	0xFA, 		// 496		stc			; C <- 1, flag to jump
	0xC1, 		// 497		ret	1		; return with A = 1, C = 1 (jump)
	0xAF, 		// 498		ld	R15		; A <- R15, sqrt loop counter
	0xF8, 		// 499		dec	A		; decrement sqrt loop counter, C <- 1 if A was not 0
	0xBF, 		// 49A		xch	R15		; R15 <- new loop counter
	0xC1, 		// 49B		ret	1		; return with A = 1, C = 1 if counter was not 0 (= continue to loop)
	0x41, 0x5F, 	// 49C		jmp	ShiftR		;one digit right shift of WR with R13 (0 is shifted from left)
	0x00, 		// 49E	= 0	; unused space
	0x27, 		// 49F		src	R67		; select register DR from R6R7 (=$20)
	0xE6, 		// 4A0		wr2			; clear DR.S2 = mul/div status
	0x20, 0x40, 	// 4A1		fim	R01,@num_dpadj	; entry address is num_dpadj (=$340), return to adjust result to max. 13 digits and print result
	0x26, 0x00, 	// 4A3		fim	R67,$00
	0x40, 0x4B, 	// 4A5		jmp	Exec		;jump back to basic pseudo code interpreter
	0x41, 0x02, 	// 4A7		jmp	MovCRWR		; CR <- WR
	0x41, 0x04, 	// 4A9		jmp	MovDRWR		; Maybe it is "jmp $10c"
};

const int busicom_ProgLen = 1195;
