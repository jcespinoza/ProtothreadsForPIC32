/*********************************************************************
 *
 *   LCD Access Routines header
 *
 *********************************************************************
 * FileName:        LCDBlocking.h
 * Dependencies:    None
 * Processor:       PIC18F, PIC24F
 * Compiler:        Microchip C18 v3.30 or higher
 *                  Microchip C30 v2.01 or higher
 *                  Microchip C32 v1.02 or higher
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement
 *
 * Copyright © 2002-2009 Microchip Technology Inc.  All rights 
 * reserved.
 *
 * Microchip licenses to you the right to use, modify, copy, and 
 * distribute: 
 * (i)  the Software when embedded on a Microchip microcontroller or 
 *      digital signal controller product (“Device”) which is 
 *      integrated into Licensee’s product; or
 * (ii) ONLY the Software driver source files ENC28J60.c and 
 *      ENC28J60.h ported to a non-Microchip device used in 
 *      conjunction with a Microchip ethernet controller for the 
 *      sole purpose of interfacing with the ethernet controller. 
 *
 * You should refer to the license agreement accompanying this 
 * Software for additional information regarding your rights and 
 * obligations.
 *
 * THE SOFTWARE AND DOCUMENTATION ARE PROVIDED “AS IS” WITHOUT 
 * WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT 
 * LIMITATION, ANY WARRANTY OF MERCHANTABILITY, FITNESS FOR A 
 * PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT SHALL 
 * MICROCHIP BE LIABLE FOR ANY INCIDENTAL, SPECIAL, INDIRECT OR 
 * CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF 
 * PROCUREMENT OF SUBSTITUTE GOODS, TECHNOLOGY OR SERVICES, ANY CLAIMS 
 * BY THIRD PARTIES (INCLUDING BUT NOT LIMITED TO ANY DEFENSE 
 * THEREOF), ANY CLAIMS FOR INDEMNITY OR CONTRIBUTION, OR OTHER 
 * SIMILAR COSTS, WHETHER ASSERTED ON THE BASIS OF CONTRACT, TORT 
 * (INCLUDING NEGLIGENCE), BREACH OF WARRANTY, OR OTHERWISE.
 *
 *
 * Author               Date    Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Howard Schlunder     4/03/06		Original 
 * Yifeng Yang          4/15/2009   MiMAC and MiApp revision
 * Yifeng Yang          06/20/2009  Add LCD support
 ********************************************************************/
#ifndef __LCDBLOCKING_H
#define __LCDBLOCKING_H

#include "SystemProfile.h"

#if defined(EXPLORER16)

	#define LCD_DATA0_TRIS		(TRISEbits.TRISE0)		// Multiplexed with LED6
	#define LCD_DATA0_IO		(LATEbits.LATE0)
	#define LCD_DATA1_TRIS		(TRISEbits.TRISE1)
	#define LCD_DATA1_IO		(LATEbits.LATE1)
	#define LCD_DATA2_TRIS		(TRISEbits.TRISE2)
	#define LCD_DATA2_IO		(LATEbits.LATE2)
	#define LCD_DATA3_TRIS		(TRISEbits.TRISE3)		// Multiplexed with LED3
	#define LCD_DATA3_IO		(LATEbits.LATE3)
	#define LCD_DATA4_TRIS		(TRISEbits.TRISE4)		// Multiplexed with LED2
	#define LCD_DATA4_IO		(LATEbits.LATE4)
	#define LCD_DATA5_TRIS		(TRISEbits.TRISE5)
	#define LCD_DATA5_IO		(LATEbits.LATE5)
	#define LCD_DATA6_TRIS		(TRISEbits.TRISE6)
	#define LCD_DATA6_IO		(LATEbits.LATE6)
	#define LCD_DATA7_TRIS		(TRISEbits.TRISE7)
	#define LCD_DATA7_IO		(LATEbits.LATE7)
	#define LCD_RD_WR_TRIS		(TRISDbits.TRISD5)
	#define LCD_RD_WR_IO		(LATDbits.LATD5)
	#define LCD_RS_TRIS			(TRISBbits.TRISB15)
	#define LCD_RS_IO			(LATBbits.LATB15)
	#define LCD_E_TRIS			(TRISDbits.TRISD4)
	#define LCD_E_IO			(LATDbits.LATD4)

#elif defined(PIC18_EXPLORER)

    #define LCD_CS_TRIS         (TRISAbits.TRISA2)
    #define LCD_CS              (LATAbits.LATA2)
    #define LCD_RESET_TRIS      (TRISFbits.TRISF6)
    #define LCD_RESET           (LATFbits.LATF6)

#endif


extern BYTE LCDText[16*2+1];
void LCDInit(void);
void LCDUpdate(void);
void LCDErase(void);
void LCDWriteLine(WORD number, char *line);

#endif

