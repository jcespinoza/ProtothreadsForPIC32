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

//This routine gets executed every 40ms
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

// === Timer 2 interrupt handler =====================================
// ipl2 means "interrupt priority level 2"
// ASM output is 47 instructions for the ISR

void __ISR(_TIMER_2_VECTOR, ipl2) Timer2Handler(void) {
    // clear the interrupt flag
    mT2ClearIntFlag();
    // keep time
    milliSec++;
    if(milliSec % 25 == 0){
        LD3 = !LD3;
    }
}

// === Thread 1 ======================================================

/**
 * The first protothread function. A protothread function must always
 * return an integer, but must never explicitly return - returning is
 * performed inside the protothread statements.
 *
 * The protothread function is driven by the main loop further down in
 * the code.
 */
static PT_THREAD(protothread1(struct pt *pt)) {
    static int time_thread_1;
#define wait_t1 10000 // mSec

    /* A protothread function must begin with PT_BEGIN() which takes a
     pointer to a struct pt. */
    PT_BEGIN(pt);

    /* We loop forever here. */
    while (1) {
        /* Wait until the other protothread has set its flag. */

        PT_WAIT_UNTIL(pt, protothread2_flag != 0);
        LD0 = !LD0;

        /* We then reset the other protothread's flag, and set our own
           flag so that the other protothread can run. */
       protothread2_flag = 0;
       protothread1_flag = 1;


        PT_YIELD_UNTIL(pt, milliSec > time_thread_1);
        time_thread_1 = milliSec + wait_t1;
        /* And we loop. */
    } // END WHILE(1)
    LD0 = !LED_OFF;
    /* All protothread functions must end with PT_END() which takes a
       pointer to a struct pt. */
    PT_END(pt);
}

// === Thread 2 ======================================================

/**
 * The second protothread function. This is almost the same as the
 * first one.
 */
static PT_THREAD(protothread2(struct pt *pt)) {
    static int time_thread_2;
#define wait_t2 40000 // mSec

    PT_BEGIN(pt);

    while (1) {

        /* Wait until the other protothread has set its flag. */

        PT_WAIT_UNTIL(pt, protothread1_flag != 0);
        LD1 = !LD1;

        /* We then reset the other protothread's flag. */
        protothread1_flag = 0;
        protothread2_flag = 1;

        time_thread_2 = milliSec + wait_t2;
        /* And we loop. */
    } // END WHILE(1)
    LD1 = LED_OFF;
    PT_END(pt);
}

void StartTimer1() {
    ConfigIntTimer1(T1_INT_ON | T1_INT_PRIOR_3 | T1_INT_SUB_PRIOR_1);
    OpenTimer1(T1_ON | T1_IDLE_CON | T1_PS_1_256 | T1_SOURCE_INT, 12500);
}

// === Main  ======================================================
/**
 * Finally, we have the main loop. Here is where the protothreads are
 * initialized and scheduled. First, however, we define the
 * protothread state variables pt1 and pt2, which hold the state of
 * the two protothreads.
 */
static struct pt pt1, pt2;

int main(void) {

    MMBInit(); // Initialize the MikroE MMB board
    //StartTimer1(); //This timer would light all LEDs as in the class example

    // set up the timer interrupt with a priority of 2
    ConfigIntTimer2(T2_INT_ON | T2_INT_PRIOR_2);
    // Set up timer2 on,  interrupts, internal clock, prescalar 1, toggle rate
    // run at 30000 ticks is 1 mSec
    OpenTimer2(T2_ON | T2_SOURCE_INT | T2_PS_1_1, 30000);
    mT2ClearIntFlag(); // and clear the interrupt flag
    milliSec = 0;
    // setup system wide interrupts  ///
    INTEnableSystemMultiVectoredInt();

    // === now the threads ===
    // init  flags
    protothread2_flag = 1;
    protothread1_flag = 0;

    /* Initialize the protothread state variables with PT_INIT(). */
    PT_INIT(&pt1);
    PT_INIT(&pt2);
    /*
     * Then we schedule the two protothreads by repeatedly calling their
     * protothread functions and passing a pointer to the protothread
     * state variables as arguments.
     */
    while (1) {
        PT_SCHEDULE(protothread1(&pt1));
        PT_SCHEDULE(protothread2(&pt2));
    }
}

int main2(void) {
    int i;
    MMBInit(); // Initialize the MikroE MMB board
    StartTimer1(); //Start Timer1

    while (1) {
        for (i = 0; i < 1000000; i++);
    }
}



