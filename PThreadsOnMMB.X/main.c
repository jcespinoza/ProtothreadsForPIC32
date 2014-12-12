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
#define LED1 LATBbits.LATB3
#define LED2 LATBbits.LATB2
#define LED3 LATBbits.LATB1
#define LED4 LATBbits.LATB0
#define LED5 LATBbits.LATB5

//Global variables
volatile int milliSec;

void __ISR(_TIMER_1_VECTOR, IPL2) Timer2Handler(void) {
    mT1ClearIntFlag();
    milliSec++;
}

static PT_THREAD(protothread1(struct pt *pt)) {
    PT_BEGIN(pt);

    while (1) {
        PT_WAIT_UNTIL(pt, milliSec == 2500);
        LD0 = !LD0;
    }

    PT_END(pt);
}

static PT_THREAD(protothread2(struct pt *pt)) {
    PT_BEGIN(pt);

    while (1) {
        PT_WAIT_UNTIL(pt, milliSec == 1000);
        milliSec = 0;
        LED1 = !LED1;
        LED2 = !LED2;
        LED3 = !LED3;
        LED4 = !LED4;
        LED5 = !LED5;
    }
    PT_END(pt);
}

void StartISRTimer1(){
    ConfigIntTimer1(T1_INT_ON | T1_INT_PRIOR_2 | T1_INT_SUB_PRIOR_1);
    OpenTimer1(T1_ON | T1_PS_1_1 | T1_SOURCE_INT, 30000);
    INTEnableSystemMultiVectoredInt();
    milliSec = 0;
}

void EnableRBPinsForLeds(){
    TRISB = 0;
    PORTB = 0;
}

static struct pt pt1, pt2;

int main(void) {
    MMBInit();
    StartISRTimer1();
    EnableRBPinsForLeds();
 
    PT_INIT(&pt1);
    PT_INIT(&pt2);

    while (1) {
        PT_SCHEDULE(protothread1(&pt1));
        PT_SCHEDULE(protothread2(&pt2));
    }
}