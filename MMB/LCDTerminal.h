/******************************************************************************
 *
 *   Terminal Emulation on MikroE PIC32MX4 MMB LCD display 
 *
 *   API compatible with Explorer16 2x16 LCD interface 
 *******************************************************************************/
//DOM-IGNORE-BEGIN
/******************************************************************************

* File Name:       LCDTerminal.h
* Dependencies:    None
* Processor:       PIC32MX4
* Compiler:        C32 v1.04 or later
* Company:         Microchip Technology, Inc.

Software License Agreement

The software supplied herewith by Microchip Technology Incorporated
(the “Company”) for its PICmicro® Microcontroller is intended and
supplied to you, the Company’s customer, for use solely and
exclusively on Microchip PICmicro Microcontroller products. The
software is owned by the Company and/or its supplier, and is
protected under applicable copyright laws. All rights are reserved.
Any use in violation of the foregoing restrictions may subject the
user to criminal sanctions under applicable laws, as well as to
civil liability for the breach of the terms and conditions of this
license.

THIS SOFTWARE IS PROVIDED IN AN “AS IS” CONDITION. NO WARRANTIES,
WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.

Author          Date    Comments
--------------------------------------------------------------------------------
ADG         14-Apr-2008 First release
LDJ         18- Aug-2009 modified for QVGA display emulation on MikroE-MMB
*******************************************************************************/

// *****************************************************************************
// *****************************************************************************
// Function Prototypes
// *****************************************************************************
// *****************************************************************************
#include "GraphicsConfig.h"
#include "Graphics/Graphics.h"

void LCDInit(void);
void LCDHome(void);
void LCDL2Home(void);
void LCDClear(void);
void LCDPut( char);
void LCDPutChar( char);
void LCDPutString( char *);
void LCDCenterString( int, char *);
void LCDShiftCursorLeft(void);
void LCDShiftCursorRight(void);
void LCDShiftCursorUp(void);
void LCDShiftCursorDown(void);
void LCDLineHome(void);
void LCDClearToEOL( void);

void LCDSetXY( int, int);

#define AT( x, y)   LCDSetXY( x, y)
#define LCDL1Home   LCDHome
//#define Wait(x) 

// configuration parameters 
// found in graphics config
// LCD_WRAP      wrap to a new line when reaching the right margin
// LCD_SCROLL    scroll screen up when past the bottom margin
// LCD_OVERLAY   overwrite existing screen contents
// LCD_BACK     background color
// LCD_FORE     foreground color

