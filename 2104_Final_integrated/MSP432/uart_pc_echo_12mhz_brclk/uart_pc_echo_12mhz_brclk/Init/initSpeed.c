/*
 * initSpeed.c
 *
 *  Created on: 24 Nov 2022
 *      Author: Marcus
 */


#include "initSpeed.h"
#include "initComms.h"
const Timer_A_UpModeConfig  pwmConfigSpeed =
{
     TIMER_A_CLOCKSOURCE_SMCLK,              // SMCLK Clock Source = 3Mhz
     TIMER_A_CLOCKSOURCE_DIVIDER_64,         // TACLK = 3MHz / 64
     46875,                           // 46875 ticks (CCR0)
     TIMER_A_TAIE_INTERRUPT_DISABLE,         // Disable Timer interrupt
     TIMER_A_CCIE_CCR0_INTERRUPT_ENABLE ,    // Enable CCR0 interrupt
     TIMER_A_DO_CLEAR                        // Clear value
};

#define PI 3.14f//
volatile float timerRight = 0;//
volatile float timerLeft = 0;//
volatile int distanceLeft = 0;//
volatile int distanceRight = 0;//
volatile int countnotchesLeft = 0;
volatile int countnotchesRight = 0;
volatile int intSpeed = 0;
volatile float speedLeft = 0;
volatile float speedRight = 0;
volatile float bothSpeed = 0;
volatile int rotationRight = 0;
volatile int rotationLeft = 0;

void initSpeed(void) {

    // Left Speed Encoder
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P6, GPIO_PIN0);
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P4, GPIO_PIN7);

    // Interrupt for speed sensor
    GPIO_clearInterruptFlag(GPIO_PORT_P6, GPIO_PIN0);
    GPIO_enableInterrupt(GPIO_PORT_P6, GPIO_PIN0);
    GPIO_clearInterruptFlag(GPIO_PORT_P4, GPIO_PIN7);
    GPIO_enableInterrupt(GPIO_PORT_P4, GPIO_PIN7);

    /* Configuring Timer_A3 for Up Mode */
    Timer_A_configureUpMode(TIMER_A3_BASE, &pwmConfigSpeed);//
    Timer_A_startCounter(TIMER_A3_BASE, TIMER_A_UP_MODE);//
    Timer_A_clearCaptureCompareInterrupt(TIMER_A3_BASE,TIMER_A_CAPTURECOMPARE_REGISTER_0);

    Interrupt_enableInterrupt(INT_PORT6);
    Interrupt_enableInterrupt(INT_PORT4);
    Interrupt_enableInterrupt(INT_TA3_0);

    printf("\nWheel Encoders Initialised!");
}

// When the wheel encoder triggers the interrupt, increment the count on
// that side, and check if PID needs to be triggered, or if car needs to
// be stopped.

void PORT6_IRQHandler(void) {
    uint32_t status = GPIO_getEnabledInterruptStatus(GPIO_PORT_P6);

    pulseCountL++;
    countnotchesLeft = countnotchesLeft + 2;
    //printf("no of left notches %d \n\r", countnotchesLeft);
    distanceLeft = countnotchesLeft * 0.165 * PI; //Calculate distance of right wheel; diameter * PI where diameter = 6.6cm
    // IF
//    if ( pulseCountL % 20 == 0 && pulseCountL > 19 && turning == 0) {
//        CarPID(pulseCountL);
//        pulseCountR = pulseCountL;
//    }

    if ( pulseCountL == pulseCountTrigger ) {
        stopCar();
        pulseCountL = 0;
        pulseCountR = 0;
    }

    GPIO_clearInterruptFlag(GPIO_PORT_P6, status);
}

void PORT4_IRQHandler(void) {
    uint32_t status = GPIO_getEnabledInterruptStatus(GPIO_PORT_P4);

    pulseCountR++;
    countnotchesRight = countnotchesRight + 2;
    //printf("no of right notches %d \n\r", countnotchesRight);
    distanceRight = countnotchesRight * 0.165 * PI; //Calculate distance of right wheel; diameter * PI where diameter = 6.6cm

//    if ( pulseCountR % 20 == 0 && pulseCountR > 19 && turning == 0) {
//        CarPID(pulseCountR);
//        pulseCountL = pulseCountR;
//    }

    if ( pulseCountR == pulseCountTrigger ) {
        stopCar();
        pulseCountL = 0;
        pulseCountR = 0;
    }

    GPIO_clearInterruptFlag(GPIO_PORT_P4, status);
}

void TA3_0_IRQHandler(void){
    const status = Timer_A_getCaptureCompareEnabledInterruptStatus(TIMER_A3_BASE,TIMER_A_CAPTURECOMPARE_REGISTER_0);
    Timer_A_clearCaptureCompareInterrupt(TIMER_A3_BASE,TIMER_A_CAPTURECOMPARE_REGISTER_0);

    timerLeft++;
    timerRight++;



    if(timerLeft == 1 || timerRight == 1){

        speedLeft = distanceLeft / timerLeft;
        speedRight = distanceRight / timerRight;
        bothSpeed = (speedLeft + speedRight) / 2; //calculate average speed
        int intSpeed = (float) bothSpeed;
        SPEED = intSpeed;
        sendSpeed(SPEED);
        //printf("left speed %0.2f cm/s \n\r", speedLeft);
       // printf("right speed %0.2f cm/s \n\r", speedRight);
        printf("both speed %0.2f cm/s \n\r", bothSpeed);
        resetValues();

    }
}

void resetValues(void){
    timerLeft = 0;
    timerRight = 0;
    speedLeft = 0.00;
    speedRight = 0.00;
    bothSpeed = 0.00;
    distanceLeft = 0;
    distanceRight = 0;
    countnotchesLeft = 0;
    countnotchesRight = 0;
    SPEED = 0;
    intSpeed = 0;
}

