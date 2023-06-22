;
; AssemblerLab7Application.asm
;
; Created: 4/30/2023 12:00:11 PM
; Author : Bohdan Boretskyi
;

.include "m2560def.inc"

;===========================Macros===============================================================
; ?????? ???? ???????? ?? 10 ??????????. ????????? ?? ?? ????????? ???????????? ? ???????? ??????? 
; ?? @0-@9. ??????? ?????????? ???????????? ??? ??????? ???????. ? ?????????? ???????? @0 ?? r10, 
; @1 – r18, ? @2 – r19. ??? ??? ?????????? ??? ??????????? ???? ??????? ? ????? ??????? ??????? 
; ???????????????? ??????? ?????????????? ?????????, ?? ??????? ????? ???? ????? ????? ?????­?????? 
; ???????.

; ???????? ????? ????????? ?????? ??????????, ? ??? ??? ???????????????? ???? ?????????: .else, 
; .elif, .endif, .error, .if, .ifdef, .ifndef, .message. ?? ??? ?????????? ?????? ???????? ?????? 
; ??? ?????? ???? ??????? ??.

; macros to write from constant
; start the definition of a new macro called "outi".
.macro outi															
; This line loads the constant value specified by the macro's first parameter (@1) 
; into a register called "_swap". This is done so that the constant value can be 
; easily written to the specified register, which is either an IO register (address 
; range 0x00-0x3F) or a memory-mapped register (address range 0x40-0xFF).
	ldi _swap, @1
; This line starts an if statement that checks if the address specified in 
; the macro's second parameter (@0) is in the IO register address range (less than 0x40).
	.if @0 < 0x40													; register has max of 0x3F address
; If the address is in the IO register address range, this line writes the value 
; in "_swap" to the specified IO register (@0) using the "out" instruction.
		out @0, _swap
	.else
; This line writes the value in "_swap" to the specified memory-mapped register 
; (@0) using the "sts" (store direct to memory) instruction.
		sts @0, _swap
	.endif
.endmacro

; macros to write from register
.macro outr															
	mov _swap, @1
	.if @0 < 0x40
		out @0, _swap
	.else
		sts @0, _swap
	.endif
.endmacro

; These lines are defining symbolic names for a number of registers in the AVR microcontroller's register file. 
; These symbolic names can be used throughout the code to refer to specific registers without having to remember 
; the actual register numbers.																
; registers		
.def	_low		=r16											; I/O ports states
.def	_high		=r17
.def	_swap		=r18											; register for outi and outr

.def	_logic		=r0												; 1=HIGH - button1; 2=HIGH - button2
.def	_counter1	=r19

.def	_delay1		=r20											; registers for delay
.def	_delay2		=r21

.def	_temp		=r22
.def	_start1		=r23											; PORTC states
.def	_end1		=r24
.def	_start2		=r25											; PORTK states

; =====================================================SRAM==========================================================================
.DSEG ; DATA SEGMENT



; =====================================================FLASH==========================================================================
; Define the CODE SEGMENT (CSEG) for the vector of interrupts
.CSEG



; Define the origin of the program code to address 0x000
; This is where the program starts running after power on or reset
.org 0x000
    ; Jump to the setup routine
    ; This routine is responsible for initializing the hardware and setting up the configuration
    rjmp setup

; Define the origin of the Timer/Counter 3 interrupt
; .org (set program origin) – ?? ????????? ?????????? ????????? ???????? ?????? ??? ??????? ???’???. 
.org OC3Aaddr
    ; Jump to the TIMER3_COMPA label
    ; This is the interrupt service routine that will be executed when Timer/Counter 3 triggers an interrupt
    rjmp TIMER3_COMPA

start1_subroutine:
    ; Save the return address on the stack
    push r31
    push r30

    ldi _start1, 0b00011000
	ldi _end1, 0b00000000

    ; Return from the subroutine
    pop r30
    pop r31
    ret

																	; STACK INITIALIZATION
setup:																	
		ldi		_temp, Low(RAMEND)
		out		SPL, _temp											; RAMEND is the last cell in RAM; SPL - stack point low
		ldi		_temp, High(RAMEND)
		out		SPH, _temp											; SPH - stack point high
																	; I/O PORTS INITIALIZATION
		ldi		_low, 0x00											; 0 - low, 1 (255) - high
		ldi		_high, 0xFF											; _low and _high are needed for I/O ports initialization
		
		outr	ACSR, _low											; turn off the comparator to lower the energy consumption

		; PORTC - OUTPUT, LOW (PINS PC0-PC7)
		out		DDRC, _high
		out		PORTC, _low
		
		; PORTK - OUTPUT, LOW (PINS PK0-PK7)
		outr	DDRK, _high
		outr	PORTK, _low
																	
		; PORTL - OUTPUT, LOW (BUZZER PD0)
		outr	DDRL, _high
		outr	PORTL, _low
		
		; PORTA - INPUT, PULLUP (BUTTONS PA2, PA4)
		out		DDRA, _low
		ldi		_high, (1<<DDA2) | (1<<DDA4)
		out		PORTA, _high

		; TIMER INITIALIZER
		; Clock cycles = 16,000,000 cycles/s * 0.45 s = 7,200,000 cycles
		; Effective clock cycles = 7,200,000 cycles / 1024 = 7,031.25 cycles (rounded)
		; OCR3A = 7,030 = 0x1B76
		; OCR3AH = 0x1B, OCR3AL = 0x76
		cli	; clear global interruption flag
		outi	OCR3AH, 0x1B										
		outi    OCR3AL, 0x76
		outi	TCCR3A, 0x00
		outi	TCCR3B, (1 << WGM32) | (1 << CS32) | (1 << CS30)
		outi	TIMSK3, (1 << OCIE3A)
		; WGM32: This bit enables the CTC (Clear Timer on Compare Match) mode, which causes TC3 to reset to zero 
		; when it matches the value in OCR3A

		; CS32 and CS30: These bits set the clock prescaler to 1024, meaning that the timer will increment once 
		; every 1024 clock cycles. This determines the frequency at which TC3 will run.

		; OCIE3A: This bit enables the Output Compare A Match Interrupt for TC3, which will generate an interrupt 
		; when TC3 matches the value in OCR3A.
		sei															; set global interruption flag


main:
		sbic	PINA, 4												; if button1 is pressed (bit1 == LOW),
		rjmp	trigger_button1										; goto trigger_button1
		ldi		_start1, 0b00010000
		ldi		_end1, 0b0001000

		set															; set flag T
		bld		_logic, 1											; load flag T to bit1 in _logic

		outi	PORTL, 1											; set the register as I/O										
		rcall	delay200ms											; call buzzer
		outi	PORTL, 0											; clear the bits of an I/O register		
		rjmp main									

trigger_button1:
		sbic	PINA, 2												; if button2 is pressed (bit2 == LOW),
		rjmp	trigger_button2										; goto trigger_button2
		ldi		_start2, 0b10000000

		set															; set flag T
		bld		_logic, 2											; load flag T to bit2 in _logic

		outi	PORTL, 1											; set the register as I/O										
		rcall	delay200ms											; call buzzer
		outi	PORTL, 0											; clear the bits of an I/O register												

trigger_button2:
		rjmp main													; loop main
																	; DELAYS
delay200ms:
		ldi		_delay1, 0x00
		ldi		_delay2, 0xC8										; 200 ms = 0xC8
delay:
		subi	_delay1, 1											; substract 1 from _delay1
		sbci	_delay2, 0											; substract 0 with shift
		brne	delay												; if _delay1 != _delay2, goto delay
		ret															


; TIMER3 COMPARE MATCH A
TIMER3_COMPA:
	; SBRC => skip if bit in register cleared
	SBRC _logic, 1
	rjmp algorithm1
	SBRC _logic, 2
	rjmp algorithm2


algorithm1:
		; SBRS => skip if bit in register set
		; SBRC => skip if bit in register cleared
		sbrc _start1, 0
		ldi _start1, 0b00000000
		sbrc _start1, 1
		ldi _start1, 0b10000001
		sbrc _start1, 2
		ldi _start1, 0b01000010
		sbrc _start1, 3
		ldi _start1, 0b00100100
		sbrc _end1, 3
		call start1_subroutine

		out		PORTC, _start1										; write data from _start1 to I/O port


algorithm2:
		mov		_temp, _start2										; copy _temp to _start2
		outr	PORTK, _temp
		lsr		_start2												; logic right shift
		reti


.ESEG ;EEPROM - ??????? ??? ???????????????? ?????