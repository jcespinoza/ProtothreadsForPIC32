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

//Global variables
static int protothread1_flag, protothread2_flag;
volatile int milliSec;
volatile int countForTimer = 0;

void __ISR(_TIMER_1_VECTOR, IPL3) Timer1_ISR(void) {
    mT1ClearIntFlag();
    countForTimer++;

    if (countForTimer == 25) { //1 second delay, blink LD0
        LD0 = !LD0;
        LD1 = !LD1;
        LD2 = !LD2;
        LD3 = !LD3;
        countForTimer = 0;
    }
}

void __ISR(_TIMER_2_VECTOR, ipl2) Timer2Handler(void) {
    mT2ClearIntFlag();
    milliSec++;
    if(milliSec % 25 == 0){
        LD3 = !LD3;
    }
}

static PT_THREAD(protothread1(struct pt *pt)) {
    static int time_thread_1;
#define wait_t1 1000
    PT_BEGIN(pt);

    while (1) {

        PT_WAIT_UNTIL(pt, protothread2_flag != 0);
        LD0 = !LD0;

       protothread2_flag = 0;
       protothread1_flag = 1;

        PT_YIELD_UNTIL(pt, milliSec > time_thread_1);
        time_thread_1 = milliSec + wait_t1;
    }
    LD0 = LED_OFF;
    PT_END(pt);
}

static PT_THREAD(protothread2(struct pt *pt)) {
    static int time_thread_2;
#define wait_t2 4000

    PT_BEGIN(pt);

    while (1) {

        PT_WAIT_UNTIL(pt, protothread1_flag != 0);
        LD1 = !LD1;

        protothread1_flag = 0;
        protothread2_flag = 1;

        time_thread_2 = milliSec + wait_t2;
    }
    LD1 = LED_OFF;
    PT_END(pt);
}

void StartTimer1() {
    ConfigIntTimer1(T1_INT_ON | T1_INT_PRIOR_3 | T1_INT_SUB_PRIOR_1);
    OpenTimer1(T1_ON | T1_IDLE_CON | T1_PS_1_256 | T1_SOURCE_INT, 12500);
}

static struct pt pt1, pt2;

int main(void) {

    MMBInit();
    ConfigIntTimer2(T2_INT_ON | T2_INT_PRIOR_2);
    OpenTimer2(T2_ON | T2_SOURCE_INT | T2_PS_1_1, 30000);
    mT2ClearIntFlag();
    milliSec = 0;
    
    INTEnableSystemMultiVectoredInt();

    protothread2_flag = 1;
    protothread1_flag = 0;

    PT_INIT(&pt1);
    PT_INIT(&pt2);

    while (1) {
        PT_SCHEDULE(protothread1(&pt1));
        PT_SCHEDULE(protothread2(&pt2));
    }
}