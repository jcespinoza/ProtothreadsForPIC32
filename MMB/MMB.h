/*
** MMB.h 
** 
** MikroE MMB - Support library for configuration and basic peripherals access
**
** Author   	        Date        Rev    Comments
** Lucio Di Jasio   08-Dec-2009	    1.0		Restructured to match GFX 2.0
**
*/

#ifndef _MIKROE_MMB
#define _MIKROE_MMB

#include <plib.h>
#include "Graphics\Graphics.h"
#include "GenericTypeDefs.h"
#include "GraphicsConfig.h"        // need to know the display orientation
#include "HardwareProfile.h"        // need to know clock freq.

// function prototypes and macros
int MMBReadT( void);                    // read MMB Temperature Sensor
#define MMBGetTdegC()    (((3.25/1024) * MMBReadT()-.5) / 0.01)     // get temperature in deg C

int MMBReadKey( void);                  // read joystick switches (non-blocking)
int MMBGetKey( void);                   // get debounced jopystick input (blocking)

void MMBInit( void);                     // init standard hw configuration

double MMBReadStopwatch( void);         // get the stopwatch (Core Timer based) output in ms 
void MMBStartStopwatch( void);          // restart the stopwatch

void MMBUARTInit( int baudrate);           // init the UART(2) (default stdout)

int  MMBAccInit( void);                 // init accelerometer 
void MMBAccRead (int*, int*, int*);     // read accelerometer

void MMBFadeIn( int msTime);            // display fade in time (ms >100)
void MMBFadeOut( int msTime);

void MMBCenterString( int p, char *s);  // display a centered string (logo) 

#endif

