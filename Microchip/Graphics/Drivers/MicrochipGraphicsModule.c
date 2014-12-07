/*****************************************************************************
 *  Module for Microchip Graphics Library
 *  Custom display controller driver template
 *****************************************************************************
 * FileName:            MicrochipGraphicsModule.c
 * Dependencies:    Graphics.h
 * Processor:           PIC24
 * Compiler:       	MPLAB C30
 * Linker:                MPLAB LINK30
 * Company:            Microchip Technology Incorporated
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
 * Author                                    Date                               Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Pradeep Budagutta                20 Aug 2009                 Initial Version
 *****************************************************************************/
#include "Graphics/Graphics.h"

#if (DISPLAY_PANEL == TFT_G240320LTSW_118W_E)

    #include "TCON_SSD1289.c"

#elif (DISPLAY_PANEL == TFT_G320240DTSW_69W_TP_E)

    #include "TCON_HX8238.c"

#elif (DISPLAY_PANEL == PH480272T_005_I06Q)

    #include "TCON_HX8257.c"

#else

    #include "TCON_Custom.c"

#endif

#ifdef USE_PALETTE

    #include "Graphics/Palette.h"

    extern void *_palette;
    extern BYTE PaletteBpp;
    extern BYTE blPaletteChangeError;
    extern void *pPendingPalette;
    extern WORD PendingStartEntry;
    extern WORD PendingLength;

#endif

// Color
WORD_VAL    _color;
WORD_VAL    _chrcolor;

// Clipping region control
SHORT       _clipRgn;

// Clipping region borders
SHORT       _clipLeft;
SHORT       _clipTop;
SHORT       _clipRight;
SHORT       _clipBottom;

#define GFX_FLIP(a,b) { SHORT t=a; a=b; b=t; }
/*********************************************************************
* Function:  void  DelayMs(WORD time)
*
* PreCondition: none
*
* Input: time - delay in ms
*
* Output: none
*
* Side Effects: none
*
* Overview: delays execution on time specified in ms
*
* Note: none
*
********************************************************************/

#define DELAY_1MS   16000 / 5               // for 16MIPS (PIC24F)

/* */
void DelayMs(WORD time)
{
    unsigned    delay;
    while(time--)
    {
        for(delay = 0; delay < DELAY_1MS; delay++);
    }
}

void EPMP_Init(void)
{
  #if (GRAPHICS_HARDWARE_PLATFORM == DA210_DEV_BOARD)
  
	ANSDbits.ANSD7 = 0;   // PMD15
	ANSDbits.ANSD6 = 0;   // PMD14
	ANSFbits.ANSF0 = 0;   // PMD11

	ANSBbits.ANSB13 = 0;  // PMA10
	ANSBbits.ANSB12 = 0;  // PMA11
	ANSBbits.ANSB11 = 0;  // PMA12
	ANSBbits.ANSB10 = 0;  // PMA13


	PMCON1bits.ADRMUX = 0;	    // address is not multiplexed
	PMCON1bits.MODE = 3;        // master mode
	PMCON1bits.CSF = 0;         // PMCS1 pin used for chip select 1, PMCS2 pin used for chip select 2
	PMCON1bits.ALP = 1;         // set address latch strobes to high active level (for sn74lvc16373)
	PMCON1bits.ALMODE = 1;      // "smart" address strobes are not used
	PMCON1bits.BUSKEEP = 0;     // bus keeper is not used
	
	PMCS1BSbits.BASE = (GFX_DISPLAY_BUFFER_START_ADDRESS>>16);	// CS1 start address
	PMCS2BSbits.BASE = 0xff;									// set CS1 end address and CS2 start address
	PMCON2bits.RADDR = 0xff;									// set CS2 end address

	PMCON4 = 0xFFFF;            // PMA0 - PMA15 address lines are enabled
	PMCON3 |= 0x0001;           // PMA16 address line is enabled

	PMCS1CFbits.CSDIS = 1;       // disable CS1 functionality  
	PMCS2CFbits.CSDIS = 1;       // disable CS2 functionality  

	PMCS1CFbits.CSDIS = 0;      // enable CS
	PMCS1CFbits.CSP = 0;        // CS active low (for 61/62WV51216ALL)
	PMCS1CFbits.CSPTEN = 1;     // enable CS port
	PMCS1CFbits.BEP = 0;        // byte enable active low (for 61/62WV51216ALL)
	PMCS1CFbits.WRSP = 0;       // write strobe active low (for 61/62WV51216ALL)
	PMCS1CFbits.RDSP = 0;       // read strobe active low (for 61/62WV51216ALL)
	PMCS1CFbits.SM = 0;         // read and write strobes on separate lines 
	PMCS1CFbits.PTSZ = 2;       // data bus width is 8-bit 

	PMCS1MDbits.ACKM = 0;        // PMACK is not used
	PMCS1MDbits.DWAITB = 0;      // access time 1 Tcy
	PMCS1MDbits.DWAITM = 0;
	PMCS1MDbits.DWAITE = 0;
	PMCS1MDbits.AMWAIT = 2;
	
	PMCON3bits.PTWREN = 1;      // enable write strobe port
	PMCON3bits.PTRDEN = 1;      // enable read strobe port
	PMCON3bits.PTBE0EN = 1;     // enable byte enable port
	PMCON3bits.PTBE1EN = 1;     // enable byte enable port
	PMCON3bits.AWAITM = 0;      // set address latch pulses width to 1/2 Tcy
	PMCON3bits.AWAITE = 0;      // set address hold time to 1/4 Tcy
	
	PMCS1BS = 0x0188;           // The point at which external memory starts

	DelayMs(100);

	PMCON2bits.MSTSEL = 3;
	PMCON1bits.PMPEN = 1;

	DelayMs(100);

  #endif
}

/*********************************************************************
* Function:  void ResetDevice()
*
* PreCondition: none
*
* Input: none
*
* Output: none
*
* Side Effects: none
*
* Overview: resets LCD, initializes PMP
*
* Note: none
*
********************************************************************/
void ResetDevice(void)
{

/*********** Start - Project Specific Code - Relocate ***********/

    #if (GRAPHICS_HARDWARE_PLATFORM == DA210_DEV_BOARD)

        _RA5 = 1;       /* Switch On DISP_ON */
        _TRISA5 = 0;

    #endif

    #ifdef GFX_USE_EXTERNAL_RAM

        EPMP_Init();

    #endif

/************ End - Project Specific Code - Relocate ************/


    G1CON1   = 0; /* Switch off the module */
    G1CON2   = 0;
    G1CON3   = 0;
    G1IE     = 0;
    G1IR     = 0;
    G1CLUT   = 0;
    G1MRGN   = 0;
    G1CLUTWR = 0;
    G1CHRX   = 0;
    G1CHRY   = 0;
    G1CMDL   = 0;
    G1CMDH   = 0;
    
    _PUBPP   = GFX_BITS_PER_PIXEL;
    _DPBPP   = GFX_BITS_PER_PIXEL;
    _DPMODE  = GFX_LCD_TYPE;

    #if (GFX_LCD_TYPE == GFX_LCD_MSTN) || (GFX_LCD_TYPE == GFX_LCD_CSTN)

        _DPGWDTH = STN_DISPLAY_WIDTH;

    #endif

    /* Port configurations */
    #ifdef GFX_DISPLAYENABLE_ENABLE
    
        _DPENPOL = GFX_DISPLAYENABLE_POLARITY;
        _DPENOE = 1;

    #endif
    
    #ifdef GFX_HSYNC_ENABLE

        _DPHSPOL = GFX_HSYNC_POLARITY;
        _DPHSOE = 1;

    #endif
    
    #ifdef GFX_VSYNC_ENABLE
    
        _DPVSPOL = GFX_VSYNC_POLARITY;
        _DPVSOE = 1;
    
    #endif
    
    #ifdef GFX_DISPLAYPOWER_ENABLE
    
        _DPPOWER = GFX_DISPLAYPOWER_POLARITY;
        _DPPWROE = 1;
    
    #endif
    
    /* Display timing signal configurations */
    _DPCLKPOL = GFX_CLOCK_POLARITY;
    
    G1DPW = DISP_HOR_RESOLUTION;
    G1DPH = DISP_VER_RESOLUTION;
    
    G1PUW = DISP_HOR_RESOLUTION;
    G1PUH = DISP_VER_RESOLUTION;
    
    #define HT  (DISP_HOR_PULSE_WIDTH + DISP_HOR_BACK_PORCH + DISP_HOR_FRONT_PORCH + DISP_HOR_RESOLUTION)
    #define VT  (DISP_VER_PULSE_WIDTH + DISP_VER_BACK_PORCH + DISP_VER_FRONT_PORCH + DISP_VER_RESOLUTION)

    G1DPWT = HT;
    G1DPHT = VT;

    _HSLEN = DISP_HOR_PULSE_WIDTH;
    _HSST  = 0; 
    
    _VSLEN = DISP_VER_PULSE_WIDTH;
    _VSST  = 0; 

    _HENST = _HSST + DISP_HOR_PULSE_WIDTH + DISP_HOR_BACK_PORCH;
    _VENST = _VSST + DISP_VER_PULSE_WIDTH + DISP_VER_BACK_PORCH;

	_ACTPIX  = _HENST;
	_ACTLINE = _VENST;
	
    /* Start addresses in RAM */
    GFX_SetDisplayArea(GFX_DISPLAY_BUFFER_START_ADDRESS);

    /* Switch On the Graphics Module */
    _GDBEN = 0xFFFF;
    _DPPINOE = 1;

    #ifdef USE_PALETTE
    
        PaletteInit();
    
    #endif
        
    TCON_Init();    // Panel TCON Programming

    _G1CLKSEL = 1;  // 96MHz Enable
    _GCLKDIV = GFX_GCLK_DIVIDER;
    _DPSTGER = 3;

    _G1EN = 1;

    DelayMs(100);
    
    _DPPWROE = 1;
    _DPPOWER = 1;

    DelayMs(100);
    
    SetClip(0);
}

/*********************************************************************
* Function: SetClip(control)
*
* Overview: Enables/disables clipping.
*
* PreCondition: none
*
* Input: control - Enables or disables the clipping.
*			- 0: Disable clipping
*			- 1: Enable clipping
*
* Output: none
*
* Side Effects: none
*
********************************************************************/
void SetClip(BYTE control)
{
    _clipRgn=control;
    
    if(_clipRgn)
    {
        GFX_WaitForCommandQueue(2);
        GFX_SetStartXY(_clipLeft, _clipTop);
        GFX_SetEndXY(_clipRight, _clipBottom) ;
    }
    else
    {
        GFX_WaitForCommandQueue(2);
        GFX_SetStartXY(0, 0);
        GFX_SetEndXY(GetMaxX(), GetMaxY());
    }
}

/*********************************************************************
* Function: SetClipRgn(left, top, right, bottom)
*
* Overview: Sets clipping region.
*
* PreCondition: none
*
* Input: left - Defines the left clipping region border.
*		 top - Defines the top clipping region border.
*		 right - Defines the right clipping region border.
*	     bottom - Defines the bottom clipping region border.
*
* Output: none
*
* Side Effects: none
*
********************************************************************/
void SetClipRgn(SHORT left, SHORT top, SHORT right, SHORT bottom)
{
    _clipLeft=left;
    _clipTop=top;
    _clipRight=right;
    _clipBottom=bottom;

    if(_clipRgn)
    {
        SetClip(_clipRgn);
    }
}

/*********************************************************************
* Function: void PutPixel(SHORT x, SHORT y)
*
* PreCondition: none
*
* Input: x,y - pixel coordinates
*
* Output: none
*
* Side Effects: none
*
* Overview: puts pixel
*
* Note: none
*
********************************************************************/
void PutPixel(SHORT x, SHORT y)
{

    if(_clipRgn)
    {
        if(x < _clipLeft)
            return;
        if(x > _clipRight)
            return;
        if(y < _clipTop)
            return;
        if(y > _clipBottom)
            return;
    }

    #if (DISP_ORIENTATION == 90)
    {
        SHORT t = x;
        x = y;
        y = GetMaxX() - t;
    }
    #elif (DISP_ORIENTATION == 180)
    {
        x = GetMaxX() - x;
        y = GetMaxY() - y;
    }
    #elif (DISP_ORIENTATION == 270)
    {
        SHORT t = x;
        x = GetMaxY() - y;
        y = t;
    }
    #endif

    GFX_SetWorkArea2(GFX_DISPLAY_BUFFER_START_ADDRESS);

    GFX_WaitForCommandQueue(4);

    GFX_SetColor(_color.Val);
    GFX_SetDestAddress((y * (DWORD)DISP_HOR_RESOLUTION) + x);
    GFX_SetRectSize(1, 1);
    GFX_StartCopy(RCC_SOLID_FILL, RCC_ROP_C, RCC_SRC_ADDR_DISCONTUNUOUS, RCC_DEST_ADDR_DISCONTUNUOUS);

    /* Note: No need to wait for  complete execution of the command even for Blocking Mode. The next commands will be in the queue & hence will execute only after the completion of this command. */
}

/*********************************************************************
* Function: WORD GetPixel(SHORT x, SHORT y)
*
* PreCondition: none
*
* Input: x,y - pixel coordinates 
*
* Output: pixel color
*
* Side Effects: none
*
* Overview: returns pixel color at x,y position
*
* Note: none
*
********************************************************************/
WORD GetPixel(SHORT x, SHORT y)
{
    volatile WORD getcolor = 0;
    
    if(x < 0 || y < 0 || x > GetMaxX() || y > GetMaxY())
    {
        return 1; /* Don't draw but return 1 */
    }

    #if (DISP_ORIENTATION == 90)
    {
        SHORT t = x;
        x = y;
        y = GetMaxX() - t;
    }
    #elif (DISP_ORIENTATION == 180)
    {
        x = GetMaxX() - x;
        y = GetMaxY() - y;
    }
    #elif (DISP_ORIENTATION == 270)
    {
        SHORT t = x;
        x = GetMaxY() - y;
        y = t;
    }
    #endif

    /* Wait till previous commands are fully executed */
    GFX_WaitForCommandQueue(GFX_COMMAND_QUEUE_LENGTH);
    GFX_WaitForGpu();
    
    GFX_SetWorkArea1(GFX_DISPLAY_BUFFER_START_ADDRESS);
    GFX_SetWorkArea2((WORD)&getcolor);

    GFX_WaitForCommandQueue(4);
    
    GFX_SetSrcAddress((y * (DWORD)DISP_HOR_RESOLUTION) + x);
    GFX_SetDestAddress(0);
    GFX_SetRectSize(1, 1);
    GFX_StartCopy(RCC_COPY, RCC_ROP_C, RCC_SRC_ADDR_DISCONTUNUOUS, RCC_DEST_ADDR_DISCONTUNUOUS);

    /* Wait till the command is fully executed */
    GFX_WaitForCommandQueue(GFX_COMMAND_QUEUE_LENGTH);
    GFX_WaitForGpu();

    return (getcolor);
}

#ifdef USE_DRV_BAR
/*********************************************************************
* Function: WORD Bar(SHORT left, SHORT top, SHORT right, SHORT bottom)
*
* PreCondition: none
*
* Input: left,top - top left corner coordinates,
*        right,bottom - bottom right corner coordinates
*
* Output: For NON-Blocking configuration:
*         - Returns 0 when device is busy and the shape is not yet completely drawn.
*         - Returns 1 when the shape is completely drawn.
*         For Blocking configuration:
*         - Always return 1.
*
* Side Effects: none
*
* Overview: draws rectangle filled with current color
*
* Note: none
*
********************************************************************/
WORD Bar(SHORT left, SHORT top, SHORT right, SHORT bottom)
{
    DWORD address;
    SHORT width, height;

    if(left > right)
    {
        return (1); /* Don't draw but return 1 */
    }

    if(top > bottom)
    {
        return (1); /* Don't draw but return 1 */
    }
    
    if(_clipRgn)
    {
        if(left<_clipLeft)
           left = _clipLeft;
        if(right>_clipRight)
           right= _clipRight;
        if(top<_clipTop)
           top = _clipTop;
        if(bottom>_clipBottom)
           bottom = _clipBottom;
    }
    else
    {
            left    = (left < 0)    ? 0: (left > GetMaxX())  ? GetMaxX(): left;
            top     = (top < 0)     ? 0: (top > GetMaxY())   ? GetMaxY(): top;
            right   = (right < 0)   ? 0: (right > GetMaxX()) ? GetMaxX(): right;
            bottom  = (bottom < 0)  ? 0: (bottom > GetMaxY())? GetMaxY(): bottom;
    }

    #if (DISP_ORIENTATION == 0) || (DISP_ORIENTATION == 180)

        width   =   right - left + 1;
        height  =   bottom - top + 1;

    #elif (DISP_ORIENTATION == 90) || (DISP_ORIENTATION == 270)

        height  =   right - left + 1;
        width   =   bottom - top + 1;

    #endif

    #if (DISP_ORIENTATION == 90)

        bottom  =   DISP_VER_RESOLUTION - left;
        left    =   top;
        right   =   left + width;
        top     =   bottom - height;

    #elif (DISP_ORIENTATION == 180)

        right   =   DISP_HOR_RESOLUTION - left;
        bottom  =   DISP_VER_RESOLUTION - top;
        left    =   right - width;
        top     =   bottom - height;

    #elif (DISP_ORIENTATION == 270)

        right   =   DISP_HOR_RESOLUTION - top;
        top     =   left;
        bottom  =   top + height;
        left    =   right - width;

    #endif

    if(GFX_GetFreeCommandSpace() < 4)
    {
        #ifndef USE_NONBLOCKING_CONFIG
            GFX_WaitForCommandQueue(4);
        #else
            return (0);
        #endif
    }

    address = (top * (DWORD)DISP_HOR_RESOLUTION) + left;

    GFX_SetWorkArea2(GFX_DISPLAY_BUFFER_START_ADDRESS);

    GFX_SetColor(_color.Val);
    GFX_SetDestAddress(address);
    GFX_SetRectSize(width, height);
    GFX_StartCopy(RCC_SOLID_FILL, RCC_ROP_C, RCC_SRC_ADDR_DISCONTUNUOUS, RCC_DEST_ADDR_DISCONTUNUOUS);
    
    /* Note: No need to wait for  complete execution of the command even for Blocking Mode. The next commands will be in the queue & hence will execute only after the completion of this command. */
    return (1);
}
#endif

#ifdef USE_DRV_CLEARDEVICE
/*********************************************************************
* Function: void ClearDevice(void)
*
* PreCondition: none
*
* Input: none
*
* Output: none
*
* Side Effects: none
*
* Overview: clears screen with current color 
*
* Note: none
*
********************************************************************/
void ClearDevice(void)
{
    GFX_WaitForCommandQueue(4);

    GFX_SetWorkArea2(GFX_DISPLAY_BUFFER_START_ADDRESS);

    GFX_SetColor(_color.Val);
    GFX_SetDestAddress(0);
    GFX_SetRectSize(DISP_HOR_RESOLUTION, DISP_VER_RESOLUTION);
    GFX_StartCopy(RCC_SOLID_FILL, RCC_ROP_C, RCC_SRC_ADDR_DISCONTUNUOUS, RCC_DEST_ADDR_CONTUNUOUS);

    /* Note: No need to wait for  complete execution of the command even for Blocking Mode. The next commands will be in the queue & hence will execute only after the completion of this command. */
}
#endif

#ifdef USE_DRV_LINE
/*********************************************************************
* Function: WORD Line(SHORT x1, SHORT y1, SHORT x2, SHORT y2)
*
* PreCondition: none
*
* Input: x1,y1 - starting coordinates, x2,y2 - ending coordinates
*
* Output: For NON-Blocking configuration:
*         - Returns 0 when device is busy and the shape is not yet completely drawn.
*         - Returns 1 when the shape is completely drawn.
*         For Blocking configuration:
*         - Always return 1.
*
* Side Effects: none
*
* Overview: draws line
*
* Note: none
*
********************************************************************/

WORD Line(SHORT x1, SHORT y1, SHORT x2, SHORT y2)
{
    SHORT   deltaX, deltaY;
    SHORT   error, stepErrorLT, stepErrorGE;
    SHORT   stepX, stepY;
    SHORT   steep;
    SHORT   temp;
    SHORT   style, type;

    if((_lineType == 0) && ((x1 == x2) || (y1 == y2)))
    {
        if(x1 > x2)
        {
            temp = x1;
            x1 = x2;
            x2 = temp;
        }
        else if(y1 > y2)
        {
            temp = y1;
            y1 = y2;
            y2 = temp;
        }

        if(_lineThickness)
        {
            if(x1 == x2)
            {
                if(!Bar(x1 - 1, y1, x2 + 1, y2)) return (0);
            }
            else
            {
                if(!Bar(x1, y1 - 1, x2, y2 + 1)) return (0);
            }
        }
        else
        {
            if(!Bar(x1, y1, x2, y2)) return (0);
        }

        return (1);
    }

    // Move cursor
    MoveTo(x2, y2);

    if(x1 == x2)
    {
        if(y1 > y2)
        {
            temp = y1;
            y1 = y2;
            y2 = temp;
        }

        style = 0;
        type = 1;
        for(temp = y1; temp < y2 + 1; temp++)
        {
            if((++style) == _lineType)
            {
                type ^= 1;
                style = 0;
            }

            if(type)
            {
                PutPixel(x1, temp);
                if(_lineThickness)
                {
                    PutPixel(x1 + 1, temp);
                    PutPixel(x1 - 1, temp);
                }
            }
        }

        return (1);
    }

    if(y1 == y2)
    {
        if(x1 > x2)
        {
            temp = x1;
            x1 = x2;
            x2 = temp;
        }

        style = 0;
        type = 1;
        for(temp = x1; temp < x2 + 1; temp++)
        {
            if((++style) == _lineType)
            {
                type ^= 1;
                style = 0;
            }

            if(type)
            {
                PutPixel(temp, y1);
                if(_lineThickness)
                {
                    PutPixel(temp, y1 + 1);
                    PutPixel(temp, y1 - 1);
                }
            }
        }

        return (1);
    }

    stepX = 0;
    deltaX = x2 - x1;
    if(deltaX < 0)
    {
        deltaX = -deltaX;
        --stepX;
    }
    else
    {
        ++stepX;
    }

    stepY = 0;
    deltaY = y2 - y1;
    if(deltaY < 0)
    {
        deltaY = -deltaY;
        --stepY;
    }
    else
    {
        ++stepY;
    }

    steep = 0;
    if(deltaX < deltaY)
    {
        ++steep;
        temp = deltaX;
        deltaX = deltaY;
        deltaY = temp;
        temp = x1;
        x1 = y1;
        y1 = temp;
        temp = stepX;
        stepX = stepY;
        stepY = temp;
        PutPixel(y1, x1);
    }
    else
    {
        PutPixel(x1, y1);
    }

    // If the current error greater or equal zero
    stepErrorGE = deltaX << 1;

    // If the current error less than zero
    stepErrorLT = deltaY << 1;

    // Error for the first pixel
    error = stepErrorLT - deltaX;

    style = 0;
    type = 1;

    while(--deltaX >= 0)
    {
        if(error >= 0)
        {
            y1 += stepY;
            error -= stepErrorGE;
        }

        x1 += stepX;
        error += stepErrorLT;

        if((++style) == _lineType)
        {
            type ^= 1;
            style = 0;
        }

        if(type)
        {
            if(steep)
            {
                PutPixel(y1, x1);
                if(_lineThickness)
                {
                    PutPixel(y1 + 1, x1);
                    PutPixel(y1 - 1, x1);
                }
            }
            else
            {
                PutPixel(x1, y1);
                if(_lineThickness)
                {
                    PutPixel(x1, y1 + 1);
                    PutPixel(x1, y1 - 1);
                }
            }
        }
    }   // end of while

    return (1);
}

#endif

#ifdef USE_DRV_FONT

/*********************************************************************
* Function: void SetFont(void* font)
*
* PreCondition: none
*
* Input: pointer to the font image
*
* Output: none
*
* Side Effects: none
*
* Overview: defines current font
*
* Note: none
*
********************************************************************/
void SetFont(void *font)
{
    FONT_HEADER *pHeader;

        #ifdef USE_FONT_EXTERNAL
    FONT_HEADER header;
        #endif

    if(_font == font)
    {
        return;
    }

    _font = font;
    switch(*((SHORT *)font))
    {
	     		#ifdef USE_FONT_RAM
        case RAM:
            pHeader = (FONT_HEADER *) ((FONT_RAM *)font)->address;
            GFX_WaitForCommandQueue(1);
            GFX_SetFont((WORD)pHeader);
            break;
            	#endif
            	
                #ifdef USE_FONT_FLASH
        case FLASH:
            pHeader = (FONT_HEADER *) ((FONT_FLASH *)font)->address;
            break;
                #endif
                
                #ifdef USE_FONT_EXTERNAL

        case EXTERNAL:
            pHeader = &header;
            ExternalMemoryCallback(font, 0, sizeof(FONT_HEADER), pHeader);
            break;
                #endif

        default:
            return;
    }

    _fontFirstChar = pHeader->firstChar;
    _fontLastChar = pHeader->lastChar;
    _fontHeight = pHeader->height;
}

/*********************************************************************
* Function: WORD OutChar(XCHAR ch)
*
* PreCondition: none
*
* Input: character code
*
* Output: For NON-Blocking configuration:
*         - Returns 0 when device is busy and the character is not yet completely drawn.
*         - Returns 1 when the character is completely drawn.
*         For Blocking configuration:
*         - Always return 1.
*
* Side Effects: none
*
* Overview: outputs a character
*
* Note: none
*
********************************************************************/
WORD OutChar(XCHAR ch)
{
    GLYPH_ENTRY *pChTable = NULL;
    BYTE        *pChImage = NULL;

        #ifdef USE_FONT_EXTERNAL
    GLYPH_ENTRY chTable;
    BYTE        chImage[EXTERNAL_FONT_BUFFER_SIZE];
    WORD        imageSize;
    DWORD_VAL   glyphOffset;
        #endif
    SHORT       chWidth = 0;
    SHORT       xCnt, yCnt, x = 0, y;
    BYTE        temp = 0, mask;

    if(GFX_GetFreeCommandSpace() < 2)
    {
        #ifndef USE_NONBLOCKING_CONFIG
            GFX_WaitForCommandQueue(2);
        #else
            return (0);
        #endif
    }

    if((unsigned XCHAR)ch < (unsigned XCHAR)_fontFirstChar)
        return (-1);
    if((unsigned XCHAR)ch > (unsigned XCHAR)_fontLastChar)
        return (-1);

    switch(*((SHORT *)_font))
    {

        case RAM:
            if(_fontOrientation == ORIENT_HOR)
            {
	            pChTable = (GLYPH_ENTRY *) (((FONT_RAM *)_font)->address + sizeof(FONT_HEADER)) + ((unsigned XCHAR)ch - (unsigned XCHAR)_fontFirstChar);
	
	            pChImage = (BYTE *) (((FONT_RAM *)_font)->address + pChTable->offsetLSB);
	
	            chWidth = pChTable->width;
	            
	            if(_chrcolor.Val != _color.Val)
	            {
	                GFX_WaitForCommandQueue(3);  /* Already 2 commands space will be available */
	                GFX_SetCharFgColor(_color.Val);
	            }
	            
	            GFX_SetWorkArea1(GFX_DISPLAY_BUFFER_START_ADDRESS);
	            GFX_SetPrintXY(GetX(), GetY());
	            GFX_PrintChar(ch, CHR_TRANSPARENT);
            
                MoveTo(GetX() + chWidth, GetY());
            }
            else
            {
	            // if orientation is vertical, process the font using PutPixel.
	            pChTable = (GLYPH_ENTRY *) (((FONT_FLASH *)_font)->address + sizeof(FONT_HEADER)) + ((unsigned XCHAR)ch - (unsigned XCHAR)_fontFirstChar);
	            pChImage = (BYTE *) (((FONT_FLASH *)_font)->address + ((DWORD)(pChTable->offsetMSB) << 8) + pChTable->offsetLSB);
	            chWidth = pChTable->width;
                break;
            }

            return (1);


                #ifdef USE_FONT_FLASH

        case FLASH:
        
            // if font is from FLASH process the font using PutPixel.
            pChTable = (GLYPH_ENTRY *) (((FONT_FLASH *)_font)->address + sizeof(FONT_HEADER)) + ((unsigned XCHAR)ch - (unsigned XCHAR)_fontFirstChar);
            pChImage = (BYTE *) (((FONT_FLASH *)_font)->address + ((DWORD)(pChTable->offsetMSB) << 8) + pChTable->offsetLSB);
            chWidth = pChTable->width;
            break;
                #endif
                
                #ifdef USE_FONT_EXTERNAL

        case EXTERNAL:

            // get glyph entry
            ExternalMemoryCallback
            (
                _font,
                sizeof(FONT_HEADER) + ((unsigned XCHAR)ch - (unsigned XCHAR)_fontFirstChar) * sizeof(GLYPH_ENTRY),
                sizeof(GLYPH_ENTRY),
                &chTable
            );

            chWidth = chTable.width;

            // width of glyph in bytes
            imageSize = 0;
            if(chWidth & 0x0007)
                imageSize = 1;
            imageSize += (chWidth >> 3);

            // glyph image size
            imageSize *= _fontHeight;

            // get glyph image
            glyphOffset.w[1] = chTable.offsetMSB;
            glyphOffset.w[0] = chTable.offsetLSB;

            ExternalMemoryCallback(_font, glyphOffset.Val, imageSize, &chImage);
            pChImage = (BYTE *) &chImage;

            break;
                #endif

        default:
            break;
    }

    if(_fontOrientation == ORIENT_HOR)
    {
        y = GetY();
        for(yCnt = 0; yCnt < _fontHeight; yCnt++)
        {
            x = GetX();
            mask = 0;
            for(xCnt = 0; xCnt < chWidth; xCnt++)
            {
                if(mask == 0)
                {
                    temp = *pChImage++;
                    mask = 0x01;
                }

                if(temp & mask)
                {
                    PutPixel(x, y);
                }

                x++;
                mask <<= 1;
            }

            y++;
        }

        // move cursor
        _cursorX = x;
    }
    else
    {
        y = GetX();
        for(yCnt = 0; yCnt < _fontHeight; yCnt++)
        {
            x = GetY();
            mask = 0;
            for(xCnt = 0; xCnt < chWidth; xCnt++)
            {
                if(mask == 0)
                {
                    temp = *pChImage++;
                    mask = 0x01;
                }

                if(temp & mask)
                {
                    PutPixel(y, x);
                }

                x--;
                mask <<= 1;
            }

            y++;
        }

        // move cursor
        _cursorY = x;
    }

    return (1);
}

#endif

#ifdef USE_DRV_PUTIMAGE

    #ifdef USE_BITMAP_FLASH

/*********************************************************************
* Function: void PutImage1BPP(SHORT left, SHORT top, FLASH_BYTE* bitmap, BYTE stretch)
*
* PreCondition: none
*
* Input: left,top - left top image corner,
*        bitmap - image pointer,
*        stretch - image stretch factor
*
* Output: none
*
* Side Effects: none
*
* Overview: outputs monochrome image starting from left,top coordinates
*
* Note: image must be located in flash
*
********************************************************************/
void PutImage1BPP(SHORT left, SHORT top, FLASH_BYTE *bitmap, BYTE stretch)
{
    register FLASH_BYTE *flashAddress;
    register FLASH_BYTE *tempFlashAddress;
    BYTE                temp = 0;
    WORD                sizeX, sizeY;
    WORD                x, y;
    WORD                xc, yc;
    BYTE                stretchX, stretchY;
    WORD                pallete[2];
    BYTE                mask;

    // Move pointer to size information
    flashAddress = bitmap + 2;

    // Read image size
    sizeY = *((FLASH_WORD *)flashAddress);
    flashAddress += 2;
    sizeX = *((FLASH_WORD *)flashAddress);
    flashAddress += 2;
    pallete[0] = *((FLASH_WORD *)flashAddress);
    flashAddress += 2;
    pallete[1] = *((FLASH_WORD *)flashAddress);
    flashAddress += 2;

    yc = top;
    for(y = 0; y < sizeY; y++)
    {
        tempFlashAddress = flashAddress;
        for(stretchY = 0; stretchY < stretch; stretchY++)
        {
            flashAddress = tempFlashAddress;
            mask = 0;
            xc = left;
            for(x = 0; x < sizeX; x++)
            {

                // Read 8 pixels from flash
                if(mask == 0)
                {
                    temp = *flashAddress;
                    flashAddress++;
                    mask = 0x80;
                }

                // Set color
                if(mask & temp)
                {
                    SetColor(pallete[1]);
                }
                else
                {
                    SetColor(pallete[0]);
                }

                // Write pixel to screen
                for(stretchX = 0; stretchX < stretch; stretchX++)
                {
                    PutPixel(xc++, yc);
                }

                // Shift to the next pixel
                mask >>= 1;
            }

            yc++;
        }
    }
}

/*********************************************************************
* Function: void PutImage4BPP(SHORT left, SHORT top, FLASH_BYTE* bitmap, BYTE stretch)
*
* PreCondition: none
*
* Input: left,top - left top image corner, bitmap - image pointer,
*        stretch - image stretch factor
*
* Output: none
*
* Side Effects: none
*
* Overview: outputs 16 color image starting from left,top coordinates
*
* Note: image must be located in flash
*
********************************************************************/
        #if (COLOR_DEPTH >= 4)

void PutImage4BPP(SHORT left, SHORT top, FLASH_BYTE *bitmap, BYTE stretch)
{
    register FLASH_BYTE *flashAddress;
    register FLASH_BYTE *tempFlashAddress;
    WORD                sizeX, sizeY;
    register WORD       x, y;
    WORD                xc, yc;
    BYTE                temp = 0;
    register BYTE       stretchX, stretchY;
    WORD                pallete[16];
    WORD                counter;

    // Move pointer to size information
    flashAddress = bitmap + 2;

    // Read image size
    sizeY = *((FLASH_WORD *)flashAddress);
    flashAddress += 2;
    sizeX = *((FLASH_WORD *)flashAddress);
    flashAddress += 2;

    // Read pallete
    for(counter = 0; counter < 16; counter++)
    {
        pallete[counter] = *((FLASH_WORD *)flashAddress);
        flashAddress += 2;
    }

    yc = top;
    for(y = 0; y < sizeY; y++)
    {
        tempFlashAddress = flashAddress;
        for(stretchY = 0; stretchY < stretch; stretchY++)
        {
            flashAddress = tempFlashAddress;
            xc = left;
            for(x = 0; x < sizeX; x++)
            {

                // Read 2 pixels from flash
                if(x & 0x0001)
                {

                    // second pixel in byte
                    SetColor(pallete[temp >> 4]);
                }
                else
                {
                    temp = *flashAddress;
                    flashAddress++;

                    // first pixel in byte
                    SetColor(pallete[temp & 0x0f]);
                }

                // Write pixel to screen
                for(stretchX = 0; stretchX < stretch; stretchX++)
                {
                    PutPixel(xc++, yc);
                }
            }

            yc++;
        }
    }
}

        #endif

/*********************************************************************
* Function: void PutImage8BPP(SHORT left, SHORT top, FLASH_BYTE* bitmap, BYTE stretch)
*
* PreCondition: none
*
* Input: left,top - left top image corner, bitmap - image pointer,
*        stretch - image stretch factor
*
* Output: none
*
* Side Effects: none
*
* Overview: outputs 256 color image starting from left,top coordinates
*
* Note: image must be located in flash
*
********************************************************************/
        #if (COLOR_DEPTH >= 8)

void PutImage8BPP(SHORT left, SHORT top, FLASH_BYTE *bitmap, BYTE stretch)
{
    register FLASH_BYTE *flashAddress;
    register FLASH_BYTE *tempFlashAddress;
    WORD                sizeX, sizeY;
    WORD                x, y;
    WORD                xc, yc;
    BYTE                temp;
    BYTE                stretchX, stretchY;
    WORD                pallete[256];
    WORD                counter;

    // Move pointer to size information
    flashAddress = bitmap + 2;

    // Read image size
    sizeY = *((FLASH_WORD *)flashAddress);
    flashAddress += 2;
    sizeX = *((FLASH_WORD *)flashAddress);
    flashAddress += 2;

    // Read pallete
    for(counter = 0; counter < 256; counter++)
    {
        pallete[counter] = *((FLASH_WORD *)flashAddress);
        flashAddress += 2;
    }

    yc = top;
    for(y = 0; y < sizeY; y++)
    {
        tempFlashAddress = flashAddress;
        for(stretchY = 0; stretchY < stretch; stretchY++)
        {
            flashAddress = tempFlashAddress;
            xc = left;
            for(x = 0; x < sizeX; x++)
            {

                // Read pixels from flash
                temp = *flashAddress;
                flashAddress++;

                // Set color
                SetColor(pallete[temp]);

                // Write pixel to screen
                for(stretchX = 0; stretchX < stretch; stretchX++)
                {
                    PutPixel(xc++, yc);
                }
            }

            yc++;
        }
    }
}

        #endif

/*********************************************************************
* Function: void PutImage16BPP(SHORT left, SHORT top, FLASH_BYTE* bitmap, BYTE stretch)
*
* PreCondition: none
*
* Input: left,top - left top image corner, bitmap - image pointer,
*        stretch - image stretch factor
*
* Output: none
*
* Side Effects: none
*
* Overview: outputs hicolor image starting from left,top coordinates
*
* Note: image must be located in flash
*
********************************************************************/
        #if (COLOR_DEPTH == 16)

void PutImage16BPP(SHORT left, SHORT top, FLASH_BYTE *bitmap, BYTE stretch)
{
    register FLASH_WORD *flashAddress;
    register FLASH_WORD *tempFlashAddress;
    WORD                sizeX, sizeY;
    register WORD       x, y;
    WORD                xc, yc;
    WORD                temp;
    register BYTE       stretchX, stretchY;

    // Move pointer to size information
    flashAddress = (FLASH_WORD *)bitmap + 1;

    // Read image size
    sizeY = *flashAddress;
    flashAddress++;
    sizeX = *flashAddress;
    flashAddress++;
    
    if(stretch == 0)
    {
        GFX_WaitForCommandQueue(16);
        GFX_WaitForGpu(); // This is required so that Work areas aren no longer used by any GPUs

        GFX_SetWorkArea1((DWORD)flashAddress);
        GFX_SetWorkArea2(GFX_DISPLAY_BUFFER_START_ADDRESS);
        
        GFX_SetSrcAddress(0);
        GFX_SetDestAddress((top * (GetMaxX() + (DWORD) 1)) + left);
        GFX_SetRectSize(sizeX, sizeY);
        GFX_StartCopy(RCC_COPY, RCC_ROP_C, RCC_SRC_ADDR_CONTUNUOUS, RCC_DEST_ADDR_DISCONTUNUOUS);
        return;
    }

    yc = top;
    for(y = 0; y < sizeY; y++)
    {
        tempFlashAddress = flashAddress;
        for(stretchY = 0; stretchY < stretch; stretchY++)
        {
            flashAddress = tempFlashAddress;
            xc = left;
            for(x = 0; x < sizeX; x++)
            {

                // Read pixels from flash
                temp = *flashAddress;
                flashAddress++;

                // Set color
                SetColor(temp);

                // Write pixel to screen
                for(stretchX = 0; stretchX < stretch; stretchX++)
                {
                    PutPixel(xc++, yc);
                }
            }

            yc++;
        }
    }
}

        #endif
    #endif
    #ifdef USE_BITMAP_EXTERNAL

/*********************************************************************
* Function: void PutImage1BPPExt(SHORT left, SHORT top, void* bitmap, BYTE stretch)
*
* PreCondition: none
*
* Input: left,top - left top image corner, bitmap - image pointer,
*        stretch - image stretch factor
*
* Output: none
*
* Side Effects: none
*
* Overview: outputs monochrome image starting from left,top coordinates
*
* Note: image must be located in external memory
*
********************************************************************/
void PutImage1BPPExt(SHORT left, SHORT top, void *bitmap, BYTE stretch)
{
    register DWORD  memOffset;
    BITMAP_HEADER   bmp;
    WORD            pallete[2];
    BYTE            lineBuffer[((GetMaxX() + (DWORD) 1) / 8) + 1];
    BYTE            *pData;
    SHORT           byteWidth;

    BYTE            temp;
    BYTE            mask;
    WORD            sizeX, sizeY;
    WORD            x, y;
    WORD            xc, yc;
    BYTE            stretchX, stretchY;

    // Get bitmap header
    ExternalMemoryCallback(bitmap, 0, sizeof(BITMAP_HEADER), &bmp);

    // Get pallete (2 entries)
    ExternalMemoryCallback(bitmap, sizeof(BITMAP_HEADER), 2 * sizeof(WORD), pallete);

    // Set offset to the image data
    memOffset = sizeof(BITMAP_HEADER) + 2 * sizeof(WORD);

    // Line width in bytes
    byteWidth = bmp.width >> 3;
    if(bmp.width & 0x0007)
        byteWidth++;

    // Get size
    sizeX = bmp.width;
    sizeY = bmp.height;

    yc = top;
    for(y = 0; y < sizeY; y++)
    {

        // Get line
        ExternalMemoryCallback(bitmap, memOffset, byteWidth, lineBuffer);
        memOffset += byteWidth;
        for(stretchY = 0; stretchY < stretch; stretchY++)
        {
            pData = lineBuffer;
            mask = 0;
            xc = left;
            for(x = 0; x < sizeX; x++)
            {

                // Read 8 pixels from flash
                if(mask == 0)
                {
                    temp = *pData++;
                    mask = 0x80;
                }

                // Set color
                if(mask & temp)
                {
                    SetColor(pallete[1]);
                }
                else
                {
                    SetColor(pallete[0]);
                }

                // Write pixel to screen
                for(stretchX = 0; stretchX < stretch; stretchX++)
                {
                    PutPixel(xc++, yc);
                }

                // Shift to the next pixel
                mask >>= 1;
            }

            yc++;
        }
    }
}

/*********************************************************************
* Function: void PutImage4BPPExt(SHORT left, SHORT top, void* bitmap, BYTE stretch)
*
* PreCondition: none
*
* Input: left,top - left top image corner, bitmap - image pointer,
*        stretch - image stretch factor
*
* Output: none
*
* Side Effects: none
*
* Overview: outputs monochrome image starting from left,top coordinates
*
* Note: image must be located in external memory
*
********************************************************************/
        #if (COLOR_DEPTH >= 4)

void PutImage4BPPExt(SHORT left, SHORT top, void *bitmap, BYTE stretch)
{
    register DWORD  memOffset;
    BITMAP_HEADER   bmp;
    WORD            pallete[16];
    BYTE            lineBuffer[((GetMaxX() + (DWORD) 1) / 2) + 1];
    BYTE            *pData;
    SHORT           byteWidth;

    BYTE            temp = 0;
    WORD            sizeX, sizeY;
    WORD            x, y;
    WORD            xc, yc;
    BYTE            stretchX, stretchY;

    // Get bitmap header
    ExternalMemoryCallback(bitmap, 0, sizeof(BITMAP_HEADER), &bmp);

    // Get pallete (16 entries)
    ExternalMemoryCallback(bitmap, sizeof(BITMAP_HEADER), 16 * sizeof(WORD), pallete);

    // Set offset to the image data
    memOffset = sizeof(BITMAP_HEADER) + 16 * sizeof(WORD);

    // Line width in bytes
    byteWidth = bmp.width >> 1;
    if(bmp.width & 0x0001)
        byteWidth++;

    // Get size
    sizeX = bmp.width;
    sizeY = bmp.height;

    yc = top;
    for(y = 0; y < sizeY; y++)
    {

        // Get line
        ExternalMemoryCallback(bitmap, memOffset, byteWidth, lineBuffer);
        memOffset += byteWidth;
        for(stretchY = 0; stretchY < stretch; stretchY++)
        {
            pData = lineBuffer;
            xc = left;
            for(x = 0; x < sizeX; x++)
            {

                // Read 2 pixels from flash
                if(x & 0x0001)
                {

                    // second pixel in byte
                    SetColor(pallete[temp >> 4]);
                }
                else
                {
                    temp = *pData++;

                    // first pixel in byte
                    SetColor(pallete[temp & 0x0f]);
                }

                // Write pixel to screen
                for(stretchX = 0; stretchX < stretch; stretchX++)
                {
                    PutPixel(xc++, yc);
                }
            }

            yc++;
        }
    }
}

        #endif

/*********************************************************************
* Function: void PutImage8BPPExt(SHORT left, SHORT top, void* bitmap, BYTE stretch)
*
* PreCondition: none
*
* Input: left,top - left top image corner, bitmap - image pointer,
*        stretch - image stretch factor
*
* Output: none
*
* Side Effects: none
*
* Overview: outputs monochrome image starting from left,top coordinates
*
* Note: image must be located in external memory
*
********************************************************************/
        #if (COLOR_DEPTH >= 8)

void PutImage8BPPExt(SHORT left, SHORT top, void *bitmap, BYTE stretch)
{
    register DWORD  memOffset;
    BITMAP_HEADER   bmp;
    WORD            pallete[256];
    BYTE            lineBuffer[(GetMaxX() + (DWORD) 1)];
    BYTE            *pData;

    BYTE            temp;
    WORD            sizeX, sizeY;
    WORD            x, y;
    WORD            xc, yc;
    BYTE            stretchX, stretchY;

    // Get bitmap header
    ExternalMemoryCallback(bitmap, 0, sizeof(BITMAP_HEADER), &bmp);

    // Get pallete (256 entries)
    ExternalMemoryCallback(bitmap, sizeof(BITMAP_HEADER), 256 * sizeof(WORD), pallete);

    // Set offset to the image data
    memOffset = sizeof(BITMAP_HEADER) + 256 * sizeof(WORD);

    // Get size
    sizeX = bmp.width;
    sizeY = bmp.height;

    yc = top;
    for(y = 0; y < sizeY; y++)
    {

        // Get line
        ExternalMemoryCallback(bitmap, memOffset, sizeX, lineBuffer);
        memOffset += sizeX;
        for(stretchY = 0; stretchY < stretch; stretchY++)
        {
            pData = lineBuffer;
            xc = left;
            for(x = 0; x < sizeX; x++)
            {
                temp = *pData++;
                SetColor(pallete[temp]);

                // Write pixel to screen
                for(stretchX = 0; stretchX < stretch; stretchX++)
                {
                    PutPixel(xc++, yc);
                }
            }

            yc++;
        }
    }
}

        #endif

/*********************************************************************
* Function: void PutImage16BPPExt(SHORT left, SHORT top, void* bitmap, BYTE stretch)
*
* PreCondition: none
*
* Input: left,top - left top image corner, bitmap - image pointer,
*        stretch - image stretch factor
*
* Output: none
*
* Side Effects: none
*
* Overview: outputs monochrome image starting from left,top coordinates
*
* Note: image must be located in external memory
*
********************************************************************/
        #if (COLOR_DEPTH == 16)

void PutImage16BPPExt(SHORT left, SHORT top, void *bitmap, BYTE stretch)
{
    register DWORD  memOffset;
    BITMAP_HEADER   bmp;
    WORD            lineBuffer[(GetMaxX() + (DWORD) 1)];
    WORD            *pData;
    WORD            byteWidth;

    WORD            temp;
    WORD            sizeX, sizeY;
    WORD            x, y;
    WORD            xc, yc;
    BYTE            stretchX, stretchY;

    // Get bitmap header
    ExternalMemoryCallback(bitmap, 0, sizeof(BITMAP_HEADER), &bmp);

    // Set offset to the image data
    memOffset = sizeof(BITMAP_HEADER);

    // Get size
    sizeX = bmp.width;
    sizeY = bmp.height;

    byteWidth = sizeX << 1;

    yc = top;
    for(y = 0; y < sizeY; y++)
    {

        // Get line
        ExternalMemoryCallback(bitmap, memOffset, byteWidth, lineBuffer);
        memOffset += byteWidth;
        for(stretchY = 0; stretchY < stretch; stretchY++)
        {
            pData = lineBuffer;

            xc = left;
            for(x = 0; x < sizeX; x++)
            {
                temp = *pData++;
                SetColor(temp);

                // Write pixel to screen
                for(stretchX = 0; stretchX < stretch; stretchX++)
                {
                    PutPixel(xc++, yc);
                }
            }

            yc++;
        }
    }
}

        #endif
    #endif
    
/*********************************************************************
* Function: WORD PutImage(SHORT left, SHORT top, void* bitmap, BYTE stretch)
*
* PreCondition: none
*
* Input: left,top - left top image corner,
*        bitmap - image pointer,
*        stretch - image stretch factor
*
* Output: For NON-Blocking configuration:
*         - Returns 0 when device is busy and the image is not yet completely drawn.
*         - Returns 1 when the image is completely drawn.
*         For Blocking configuration:
*         - Always return 1.
*
* Side Effects: none
*
* Overview: outputs image starting from left,top coordinates
*
* Note: image must be located in flash
*
********************************************************************/
WORD PutImage(SHORT left, SHORT top, void *bitmap, BYTE stretch)
{
    FLASH_BYTE  *flashAddress;
    BYTE        colorDepth;
    WORD        colorTemp;

    if(GFX_GetFreeCommandSpace() < 4)
    {
        #ifndef USE_NONBLOCKING_CONFIG
            GFX_WaitForCommandQueue(4);
        #else
            return (0);
        #endif
    }

    // Save current color
    colorTemp = GetColor();

    switch(*((SHORT *)bitmap))
    {
                #ifdef USE_BITMAP_FLASH

        case FLASH:

            // Image address
            flashAddress = ((BITMAP_FLASH *)bitmap)->address;

            // Read color depth
            colorDepth = *(flashAddress + 1);

            // Draw picture
            switch(colorDepth)
            {
                case 1:     PutImage1BPP(left, top, flashAddress, stretch); break;
                    
                    #if (COLOR_DEPTH >= 4)
                case 4:     PutImage4BPP(left, top, flashAddress, stretch); break;
                    #endif
                    
                    #if (COLOR_DEPTH >= 8)
                case 8:     PutImage8BPP(left, top, flashAddress, stretch); break;
                    #endif
                    
                    #if (COLOR_DEPTH == 16)
                case 16:    PutImage16BPP(left, top, flashAddress, stretch); break;
                    #endif
                    
                default:    break;
            }

            break;
                #endif
                #ifdef USE_BITMAP_EXTERNAL

        case EXTERNAL:

            // Get color depth
            ExternalMemoryCallback(bitmap, 1, 1, &colorDepth);

            // Draw picture
            switch(colorDepth)
            {
                case 1:     PutImage1BPPExt(left, top, bitmap, stretch); break;
                
                    #if (COLOR_DEPTH >= 4)
                case 4:     PutImage4BPPExt(left, top, bitmap, stretch); break;
                    #endif
                
                    #if (COLOR_DEPTH >= 8)
                case 8:     PutImage8BPPExt(left, top, bitmap, stretch); break;
                    #endif
                    
                    #if (COLOR_DEPTH == 16)
                case 16:    PutImage16BPPExt(left, top, bitmap, stretch); break;
                    #endif
                    
                default:    break;
            }

            break;
                #endif

        default:
            break;
    }

    // Restore current color
    SetColor(colorTemp);
    return (1);
}

#endif // USE_DRV_PUTIMAGE

#ifdef USE_PALETTE

/*********************************************************************
* Function: void PaletteInit(void)
*
* Overview: Initializes the CLUT.
*
* PreCondition: none
*
* Input: none
*
* Output: none
*
* Side Effects: none
*
********************************************************************/
void PaletteInit(void)
{
    while(_CLUTBUSY != 0)
    {
        Nop();
    }
    _CLUTRWEN = 0;
    _CLUTEN = 0;
    _GFX1IF = 0;
    _GFX1IE = 1;
}

/*********************************************************************
* Function: void EnablePalette(void)
*
* Overview: Enables the Palette mode
*
* PreCondition: A system palette has been set using RequestPaletteChange()
*               or SetPalette() and SetPaletteBpp() must have been called
*
* Input: none
*
* Output: none
*
* Side Effects: There may be a little flicker with SetPalette() which
*               can be avoided for TFT with RequestPaletteChange()
*
********************************************************************/
void EnablePalette(void)
{
    if(PaletteBpp <= GFX_16_BPP)
    {
        _PUBPP = PaletteBpp;
        _DPBPP = PaletteBpp;
        _CLUTEN = 1;
    }    
}

/*********************************************************************
* Function: void DisablePalette(void)
*
* Overview: Disables the Palette mode
*
* PreCondition: none
*
* Input: none
*
* Output: none
*
* Side Effects: There may be a little flicker which can be avoided for TFT
*               by preceding with a dummy RequestPaletteChange()
*
********************************************************************/
void DisablePalette(void)
{
    _CLUTEN = 0;
    _PUBPP  = GFX_BITS_PER_PIXEL;
    _DPBPP  = GFX_BITS_PER_PIXEL;
}

/*********************************************************************
* Function: BYTE IsPaletteEnabled(void)
*
* Overview: Returns if the Palette mode is enabled or not
*
* PreCondition: none
*
* Input: none
*
* Output: Enabled -> 1, Disabled -> 0
*
* Side Effects:
*
********************************************************************/
BYTE IsPaletteEnabled(void)
{
    return _CLUTEN;
}

/*********************************************************************
* Function: BYTE SetPaletteBpp(BYTE bpp)
*
* Overview: Sets the CLUT's number of valid entries.
*
* PreCondition: PaletteInit() must be called before.
*
* Input: bpp -> Bits per pixel
*
* Output: Status: Zero -> Success, Non-zero -> Error.
*
* Side Effects: Drawing mode will change to support palettes
*
********************************************************************/
BYTE SetPaletteBpp(BYTE bpp)
{
    switch(bpp)
    {
        case 1:         PaletteBpp = GFX_1_BPP;
                        break;

        case 2:         PaletteBpp = GFX_2_BPP;
                        break;

        case 4:         PaletteBpp = GFX_4_BPP;
                        break;

        case 8:         PaletteBpp = GFX_8_BPP;
                        break;

        default:        PaletteBpp = GFX_16_BPP;
                        return -1;                        
    }
    
    return 0;
}

/*********************************************************************
* Function: void __GFX1Interrupt(void)
*
* Overview: This is an ISR to handle VMRGN Interrupt
*
* PreCondition: Interrupts must be enabled
*
* Input: none
*
* Output: none
*
* Side Effects: Sets the error flag blPaletteChangeError
*
********************************************************************/
void __attribute__((interrupt, shadow, no_auto_psv)) _GFX1Interrupt(void)
{
    _GFX1IF = 0;
    if(_VMRGNIF != 0)
    {
        _VMRGNIF = 0;
        blPaletteChangeError = SetPalette(pPendingPalette, PendingStartEntry, PendingLength);
        if(!blPaletteChangeError)
        {
            _palette = pPendingPalette;
        }
    }    
}

/*********************************************************************
* Function: void StartVBlankInterrupt(void)
*
* Overview: Sets up the Vertical Blanking Interrupt
*
* PreCondition: Interrupts must be enabled
*
* Input: none
*
* Output: none
*
* Side Effects: none
*
********************************************************************/
void StartVBlankInterrupt(void)
{
    if(pPendingPalette != NULL)
    {
        #if (GFX_LCD_TYPE == GFX_LCD_TFT)
        
            _VMRGNIF = 0;
            _VMRGNIE = 1;
        
        #else
        
            blPaletteChangeError = SetPalette(pPendingPalette, PendingStartEntry, PendingLength);
            if(!blPaletteChangeError)
            {
                _palette = pPendingPalette;
            }

        #endif
    }
}

/*********************************************************************
* Function: BYTE SetPaletteFlash(PALETTE_ENTRY *pPaletteEntry, WORD startEntry, WORD length)
*
* Overview: Loads the palettes from the flash immediately.
*
* PreCondition: PaletteInit() must be called before.
*
* Input: pPaletteEntry   - Pointer to the palette table in ROM
*        startEntry      - Start entry to load (inclusive)
*        length          - Number of entries
*
* Output: Status: Zero -> Success, Non-zero -> Error.
*
* Side Effects: There may be a slight flicker when the Palette entries
*               are getting loaded one by one.
*
********************************************************************/
BYTE SetPaletteFlash(PALETTE_ENTRY *pPaletteEntry, WORD startEntry, WORD length)
{
    WORD counter;

    if((pPaletteEntry == NULL) || ((startEntry + length) > 256))
    {
        return -1;
    }

    _CLUTRWEN = 1;
    for(counter = 0; counter < length; counter++)
    {
        _CLUTADR = counter;
        _CLUTWR  = pPaletteEntry[counter].value;
        while(_CLUTBUSY)
        {
            Nop();
        }
    }
    _CLUTRWEN = 0;

    return 0;
}

#endif // USE_PALETTE
