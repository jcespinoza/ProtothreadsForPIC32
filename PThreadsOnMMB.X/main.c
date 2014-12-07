#include <plib.h>
#include "PT/pt.h"
#include "MMB.h"
#include "HardwareProfile.h"
#include "GraphicsConfig.h"
#include "LCDTerminal.h"

// Configuration bits
#pragma config POSCMOD = XT, FNOSC = PRIPLL, FSOSCEN = ON
#pragma config FPLLIDIV = DIV_2, FPLLMUL = MUL_20, FPLLODIV = DIV_1, FPBDIV = DIV_1
#pragma config OSCIOFNC = ON, CP = OFF, BWP = OFF, PWP = OFF
#define CPU_FREQ (80000000ul)

// fonts
extern const FONT_FLASH TerminalFont;

//Global variables
volatile int countForTimer1 = 0;
volatile int countForTimer2 = 0;
volatile int doRefresh = 0;

//This routine gets executed every 40ms
void __ISR(_TIMER_1_VECTOR, IPL3) Timer1_ISR(void)
{
    mT1ClearIntFlag();
    countForTimer1++;

    if (countForTimer1 == 25) { //1 second delay, blink LD0
        LD0 = !LD0;
        LD1 = !LD1;
        LD2 = !LD2;
        LD3 = !LD3;
        countForTimer1 = 0;
    }
}

void StartTimer1()
{
    ConfigIntTimer1(T1_INT_ON | T1_INT_PRIOR_3 | T1_INT_SUB_PRIOR_1);
    OpenTimer1(T1_ON | T1_IDLE_CON | T1_PS_1_256 | T1_SOURCE_INT, 12500);
}

int main(void)
{
    int i;
    MMBInit(); // Initialize the MikroE MMB board
    LCDInit();

    LCDSetXY(1,5);
    LCDPutString("Move the joystick");
    MMBGetKey();

    StartTimer1(); //Start Timer1

    while (1) {
        for (i=0; i<1000000; i++){
            SetColor(WHITE);
            LCDSetXY(1,5);
            LCDPutString("Olis");
        }
    }
}



