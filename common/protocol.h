#ifndef _SERIAL_PROTOCOL_H_
#define _SERIAL_PROTOCOL_H_

// protocol

// P_PARAM 	(SPI) param changes and reads, AVR32 controlled
// P_GEN	(SPI) param changes, midi, OSC, etc generated by BF533
// P_PATCH	(UART) management of patch loading, saving, initialization, etc

//////////////////////////////////////////////////////////////////////////////
// P_PARAM	(SPI) param changes and reads, AVR32 controlled
//
// A--- B------------- C--------------------------------------
// abcd 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000
//
// (A) COMMAND (4 bit) left aligned for extensibility
//		(a) 0 = get, 1 = set
//		(b) 0 = immediate, 1 = interpolate (does not apply to all commands)
//		(c) 0 = param, 1 = interpolation time
// (B) INDEX (12 bit)
// (C) VALUE (32 bit) unsigned int or float depending on COMMAND
//
// P_PARAM COMMANDS (4-bit nibble)

/*
#define P_PARAM_COM_GETI	0b0000
#define P_PARAM_COM_GETF	0b1000
#define P_PARAM_COM_SETI	0b0100
#define P_PARAM_COM_SETF	0b1100
#define P_PARAM_COM_ISETI	0b0110
#define P_PARAM_COM_ISETF	0b1110
#define P_PARAM_COM_SETINTI	0b0101
#define P_PARAM_COM_SETINTF	0b1101
*/


// don't provide separate commands for int and float params.
// a parameter always holds value in 4 bytes.
// whether it's float or int is declared in the param's descriptor
#define P_PARAM_COM_GET	      0b0000
#define P_PARAM_COM_SET	      0b0001
#define P_PARAM_COM_SETI      0b0011
#define P_PARAM_COM_SETINT    0b0101

// parameter command nibble mask
#define P_PARAM_COM_MASK 0xF000
// parameter index nibble mask
#define P_PARAM_IDX_MASK 0x0FFF
// r-shift to get the parameter command nibble from the command word
#define P_PARAM_COM_RSHIFT 12
// l-shift to place the parameter command nibble in the command word
#define P_PARAM_COM_LSHIFT 12

// to pack for delivery:
// word_0: (index & P_PARAM_IDX_MASK) | (command << P_PARAM_COM_LSHIFT)
// word_1: value >> 16
// word_2: value & 0xffffffff

// to read:
// command = (word_0 & P_PARAM_COM_MASK) >> P_PARAM_COM_RSHIFT
// index = word_0 & P_PARAM_IDX_MASK
// value = word_1 << 16 & word_2

// and, macros:
#define P_GET_PARAM_COM(WORD) (WORD & P_PARAM_COM_MASK) >> P_PARAM_COM_RSHIFT
#define P_GET_PARAM_IDX(WORD) (WORD & P_PARAM_IDX_MASK)
#define P_GET_PARAM_DATA(WORD_H, WORD_L) (WORD_H << 16 | WORD_L)

#define P_SET_PARAM_COMMAND_WORD(COM, IDX) ((IDX & P_PARAM_IDX_MASK) | (COM << P_PARAM_COM_LSHIFT))
#define P_SET_PARAM_DATA_WORD_H(DATA) (DATA >> 16)
#define P_SET_PARAM_DATA_WORD_L(DATA) (DATA & 0xffffffff)

//// word order:
#define P_PARAM_MSG_WORD_COM     0
#define P_PARAM_MSG_WORD_DATAH   1
#define P_PARAM_MSG_WORD_DATAL   2
#define P_PARAM_MSG_WORD_COUNT   3
#define P_PARAM_MSG_WORD_COUNT_1 2

// bit depth of integer values
#define P_PARAM_MSG_INT_BIT_DEPTH  32

#endif
