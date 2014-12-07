/******************************************************************************
 *
 *   Terminal Emulation on MikroE PIC32MX4 MMB LCD display 
 *
 *   API compatible with Explorer16 2x16 LCD interface 
 *******************************************************************************/
//DOM-IGNORE-BEGIN
/******************************************************************************

* File Name:       LCDterminal.c
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
LDJ         18-Aug-2009 Modified for MikroE MMB 
LDJ         18-Nov-2009 Expanded for Testing MMB v5
*******************************************************************************/

#include "GenericTypeDefs.h"
#include "LCDTerminal.h"

extern const FONT_FLASH TerminalFont;

// font size is based on default font 
#define FONT_W  12
#define FONT_H  20

#define _MAX_X  GetMaxX()/FONT_W
#define _MAX_Y  GetMaxY()/FONT_H

static char page[ _MAX_Y][ _MAX_X+1];
static int _cx, _cy;    // cursor position


int LCDGetX()
{ 
    return _cx;
}

int LCDGetY()
{ 
    return _cy;
}


void LCDHome(void)
{   // set cursor to home position
    _cx = 0; 
    _cy = 0;
}


void LCDSetXY( int x, int y)
{
    _cx = x; _cy = y;
}


#define LCDL1Home   LCDHome

void LCDL2Home(void)
{
    _cx = 0;
    _cy = 1;
}

void LCDLineHome(void)
{   
    _cx = 0;
}    

void LCDClearToEOL( void)
{
    int t = _cx;
    int i;
    
    // fill with spaces until the end of the line
    for( i=_cx; i< _MAX_X; i++)
        LCDPutChar( ' ');   
        
    // return to position
    _cx = t;    
}

void LCDClear(void)
{
    int i,j;
    for (i=0; i< _MAX_X; i++)
        for( j=0; j<_MAX_Y; j++)
            page[j][i] = ' ';
            
    SetColor( LCD_BACK);
    ClearDevice();
    SetColor( LCD_FORE);
    LCDHome();
}

void LCDInit(void)
{
    InitGraph();    // initialize graphics library
    LCDClear();
    SetFont( (void *)&TerminalFont);          // set font
}


void LCDShiftCursorLeft(void)
{
    if (_cx>0) 
        _cx--;
}


void LCDShiftCursorUp(void)
{
   if ( _cy>0) _cy--;
}

void LCDShiftCursorDown(void)
{
    int i,j;
        _cy++;
#ifdef  LCD_SCROLL
    if ( _cy >= _MAX_Y) 
    {   // scroll entire screen up
        for( j=0; j<_MAX_Y-1; j++)
        {   // for each line
            // clear the line background (indipendent from LCD_OVERLAY)    
            SetColor( LCD_BACK); 
            Bar( 0, FONT_H*j, GetMaxX()-1, FONT_H*(j+1));
            SetColor( LCD_FORE); 
            // copy from next line print the new line content
            for( i=0;i<_MAX_X; i++)
            {
                page[j][i] = page[j+1][i];    
                MoveTo( FONT_W*i, FONT_H*j); OutChar( page[j][i]);
            }    
        }   
        _cy = _MAX_Y-1; 
    }
#else   // no scrolling option roll
    if ( _cy >= _MAX_Y) 
    {
        _cy = 0;
    }    

#endif        
}

void LCDShiftCursorRight(void)
{
    _cx++;
#ifdef LCD_WRAP
    if ( _cx >= _MAX_X) 
    {   // wrap to a new line
        _cx = 0;    
        LCDShiftCursorDown();
    }    
#endif
}

void LCDPutChar(char A)
{
        
#ifndef LCD_OVERLAY
    // clear the background
    SetColor( LCD_BACK); 
    Bar( _cx*FONT_W, _cy*FONT_H, (_cx+1)*FONT_W, (_cy+1)*FONT_H);
    SetColor( LCD_FORE); 
#endif

    if(( _cx<_MAX_X) && (_cy<_MAX_Y))   // clip
    { // print the new character
        MoveTo( FONT_W * _cx, FONT_H*_cy);
        OutChar( A);
        page[ _cy][ _cx] = A;
        LCDShiftCursorRight();
    }
} // LCDPutChar


void LCDPut(char A)
{
    int tab, i;
    
    switch( A)
    {
      case '\b':    // backstep
        if ( _cx>0)
            _cx--;
        break;
        
      case '\t':    // move to next tab position
        tab = (_cx/8 + 1) * 8;
        // add spaces until the next tab stop
        for( i=_cx; i<tab; i++)
            LCDPutChar(' ');
        break;

      case '\n':    // New Line
        LCDShiftCursorDown();
        // break;   // continue into Home
        
      case '\r':    // Home
        _cx = 0;
        break;

      default:      // print-able char
        LCDPutChar( A);
        break;
    } // switch
    
} // LCDPut


void LCDPutString( char* s)
{
    char c;
    while( c = *s++)
        LCDPut( c);
}

void LCDCenterString( int p, char *s)
{ 
    int height, x, y;

    // get string width & height
    height = GetTextHeight( (void *)&TerminalFont);
    y = GetMaxY()/2 - height/2 + height*p;
    x = (GetMaxX() - GetTextWidth( (void *)s, (void *)&TerminalFont))/2;
    OutTextXY(  x, y, s);
} // CenterString

 