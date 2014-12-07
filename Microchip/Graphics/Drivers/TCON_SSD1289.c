/*****************************************************************************
 *  Module for Microchip Graphics Library
 *  Solomon Systech. SSD1289 TCON  driver
 *****************************************************************************
 * FileName:        TCON_SSD1289.c
 * Dependencies:    Graphics.h
 * Processor:       PIC24, PIC32
 * Compiler:       	MPLAB C30, MPLAB C32
 * Linker:          MPLAB LINK30, MPLAB LINK32
 * Company:         Microchip Technology Incorporated
 *
 * Software License Agreement
 *
 * Copyright © 2008 Microchip Technology Inc.  All rights reserved.
 * Microchip licenses to you the right to use, modify, copy and distribute
 * Software only when embedded on a Microchip microcontroller or digital
 * signal controller, which is integrated into your product or third party
 * product (pursuant to the sublicense terms in the accompanying license
 * agreement).  
 *
 * You should refer to the license agreement accompanying this Software
 * for additional information regarding your rights and obligations.
 *
 * SOFTWARE AND DOCUMENTATION ARE PROVIDED “AS IS” WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY
 * OF MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR
 * PURPOSE. IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR
 * OBLIGATED UNDER CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION,
 * BREACH OF WARRANTY, OR OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT
 * DAMAGES OR EXPENSES INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL,
 * INDIRECT, PUNITIVE OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA,
 * COST OF PROCUREMENT OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY
 * CLAIMS BY THIRD PARTIES (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF),
 * OR OTHER SIMILAR COSTS.
 *
 * Author               Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Anton Alkhimenok     11/17/08
 *****************************************************************************/
#include "Graphics/Graphics.h"

#define CS  0x01
#define SCL 0x02
#define SDO 0x04
#define DC  0x08

BYTE    value;

/* */

void TCON_Delay(void)
{
    WORD    timeOut;
    timeOut = 200;
    while(timeOut--);
}

/* */
void TCON_CTRL(BYTE mask, BYTE level)
{
  #if (DISPLAY_CONTROLLER == MCHP_DA210)

    switch(mask)
    {
        case CS:    CS_PORT  = level;
                    break;

        case SCL:   SCL_PORT = level;
                    break;

        case SDO:   SDO_PORT = level;
                    break;

        case DC:    DC_PORT  = level;
                    break;
    }
    
    Nop();
    
  #else

    if(level == 0)
    {
        value &= ~mask;
    }
    else
    {
        value |= mask;
    }

    SetReg(0xAC, value);

  #endif
}

/* */
void TCONWriteByte(BYTE value)
{
    BYTE    mask;

    mask = 0x80;
    while(mask)
    {
        TCON_CTRL(SCL, 0);
        TCON_Delay();
        if(mask & value)
        {
            TCON_CTRL(SDO, 1);
        }
        else
        {
            TCON_CTRL(SDO, 0);
        }

        TCON_CTRL(SCL, 1);
        mask >>= 1;
    }
}

/* */
void GPIO_TCON(WORD index, WORD value)
{
    TCON_CTRL(CS, 0);

    // Index
    TCON_CTRL(DC, 0);
    TCONWriteByte(((WORD_VAL) index).v[1]);
    TCONWriteByte(((WORD_VAL) index).v[0]);

    TCON_CTRL(CS, 1);
    TCON_Delay();
    TCON_CTRL(CS, 0);

    // Data
    TCON_CTRL(DC, 1);
    TCONWriteByte(((WORD_VAL) value).v[1]);
    TCONWriteByte(((WORD_VAL) value).v[0]);
    TCON_CTRL(CS, 1);
    TCON_Delay();
}

/* */
void TCON_Init(void)
{
    #if (DISPLAY_CONTROLLER == MCHP_DA210)
	
		WORD CS_TRIS_temp = CS_TRIS;	
		WORD SCL_TRIS_temp = SCL_TRIS;
        WORD SDO_TRIS_temp = SDO_TRIS;
        WORD DC_TRIS_temp = DC_TRIS;

        CS_DIG();
        CS_PORT  = 1;
        CS_TRIS  = 0;

        SCL_DIG();
        SCL_PORT = 1;
        SCL_TRIS = 0;

        SDO_DIG();
        SDO_PORT = 1;
        SDO_TRIS = 0;

        DC_DIG();
        DC_PORT  = 1;
        DC_TRIS  = 0;

    #else

        SetReg(0xA8, DC | CS | SDO | SCL);
        TCON_CTRL(DC, 1);
        TCON_CTRL(CS, 1);
        TCON_CTRL(SDO, 1);
        TCON_CTRL(SCL, 1);
        
    #endif
    

    DelayMs(20);

    GPIO_TCON(0x00, 0x0001);
    GPIO_TCON(0x03, 0xAAAC);
    GPIO_TCON(0x0C, 0x0002);
    DelayMs(15);
    GPIO_TCON(0x0D, 0x000A);
    GPIO_TCON(0x0E, 0x2D00);
    GPIO_TCON(0x1E, 0x00BC);
    GPIO_TCON(0x01, 0x1A0C);
    DelayMs(15);
    GPIO_TCON(0x01, 0x2B3F);
    GPIO_TCON(0x02, 0x0600);
    GPIO_TCON(0x10, 0x0000);
    GPIO_TCON(0x05, 0x0000);
    GPIO_TCON(0x06, 0x0000);

    DelayMs(20);

  #if (DISPLAY_CONTROLLER == MCHP_DA210)
		CS_TRIS = CS_TRIS_temp;	
		SCL_TRIS = SCL_TRIS_temp;
        SDO_TRIS = SDO_TRIS_temp;
        DC_TRIS = DC_TRIS_temp;
   #endif

}
