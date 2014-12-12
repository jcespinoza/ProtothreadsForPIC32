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
#define LED1 LATBbits.LATB0
#define LED2 LATBbits.LATB1
#define LED3 LATBbits.LATB2
#define LED4 LATBbits.LATB3
#define LED5 LATBbits.LATB5
#define QUANTUM1 500
#define QUANTUM2 1000
#define QUANTUM3 1500
#define QUANTUM4 2000
#define QUANTUM5 2500

//Global variables
volatile int milliSec;

void __ISR(_TIMER_1_VECTOR, IPL2) Timer2Handler(void) {
    mT1ClearIntFlag();
    milliSec++;
}

static PT_THREAD(protothread1(struct pt *pt)) {
    static int timeToWaitThread1;
    PT_BEGIN(pt);
    while (1) {
        LD0 = !LD0;
        LD1 = !LD1;
        LD2 = !LD2;
        LD3 = !LD3;
        LED1 = !LED1;
        timeToWaitThread1 = milliSec + QUANTUM1;
        PT_YIELD_UNTIL(pt, milliSec > timeToWaitThread1);
    }
    PT_END(pt);
}

static PT_THREAD(protothread2(struct pt *pt)) {
    static int timeToWaitThread2;
    PT_BEGIN(pt);

    while (1) {       
        LED2 = !LED2;
        timeToWaitThread2 = milliSec + QUANTUM2;
        PT_YIELD_UNTIL(pt, milliSec > timeToWaitThread2);
    }
    PT_END(pt);
}

static PT_THREAD(protoLED3(struct pt *pt)){
    static int timeToWaitThread3;

    PT_BEGIN(pt);
    while(1){
        LED3 = !LED3;
        timeToWaitThread3 = milliSec + QUANTUM3;
        PT_YIELD_UNTIL(pt, milliSec > timeToWaitThread3);
    }
    PT_END(pt);
}

static PT_THREAD(protoLED4(struct pt *pt)){
    static int timeToWaitThread4;

    PT_BEGIN(pt);
    while(1){
        LED4 = !LED4;
        timeToWaitThread4 = milliSec + QUANTUM4;
        PT_YIELD_UNTIL(pt, milliSec > timeToWaitThread4);
    }
    PT_END(pt);
}

static PT_THREAD(protoLED5(struct pt *pt)){
    static int timeToWaitThread5;

    PT_BEGIN(pt);
    while(1){
        LED5 = !LED5;
        timeToWaitThread5 = milliSec + QUANTUM5;
        PT_YIELD_UNTIL(pt, milliSec > timeToWaitThread5);
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

static struct pt pt1, pt2, pt3, pt4, pt5;

int main(void) {
    MMBInit();
    StartISRTimer1();
    EnableRBPinsForLeds();
 
    PT_INIT(&pt1);
    PT_INIT(&pt2);
    PT_INIT(&pt3);
    PT_INIT(&pt4);
    PT_INIT(&pt5);

    while (1) {
        PT_SCHEDULE(protothread1(&pt1));
        PT_SCHEDULE(protothread2(&pt2));
        PT_SCHEDULE(protoLED3(&pt3));
        PT_SCHEDULE(protoLED4(&pt4));
        PT_SCHEDULE(protoLED5(&pt5));
    }
}