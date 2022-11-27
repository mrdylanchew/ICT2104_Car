/* --COPYRIGHT--,BSD
 * Copyright (c) 2017, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --/COPYRIGHT--*/
/*******************************************************************************
 * MSP432 Timer Interrupt
 *
 *
 *                MSP432P401
 *             ------------------
 *         /|\|                  |
 *          | |                  |
 *          --|RST         P3.6  |---> Trigger
 *            |                  |
 *            |            P3.7  |<--- Echo
 *            |                  |
 *            |                  |
 *
 ******************************************************************************/
/* DriverLib Includes */
#include "driverlib.h"
#include <stdbool.h>
#include <stdio.h>

#define MIN_DISTANCE    8.0f // Min distance in cm to set wall detection. Tweak this to change min distance before a wall is considered to be detected
#define TICKPERIOD      50000
#define MICROSECONDS    10

bool haveWall = 0;
int sensorSelect = 0;
int resultAvailableFlag = 0;
float result = 0.0f;
int echoReceivedFlag = 0;

// -------------------------------------------------------------------------------------------------------------------

/*this function is required in main init for ultrasonic*/
void sleep()
{
    int i = 0;
    for (i; i < 100000; i++)
    {

    }
}

void Initalise_Ultrasound(void)
{
    Interrupt_disableMaster();
    /* Timer_A UpMode Configuration Parameter */
    // TA0, used for counter value to store ultrasonic echo pulse
    const Timer_A_UpModeConfig upConfig = {
    TIMER_A_CLOCKSOURCE_SMCLK,              // SMCLK Clock Source
            TIMER_A_CLOCKSOURCE_DIVIDER_3,          // SMCLK/3 = 1MHz
            TICKPERIOD,                             // tick period
            TIMER_A_TAIE_INTERRUPT_DISABLE,         // Disable Timer interrupt
            TIMER_A_CCIE_CCR0_INTERRUPT_DISABLE,    // Enable CCR0 interrupt
            TIMER_A_DO_CLEAR                        // Clear value
            };

    //TA1, used for trigger delay, minimally 10us
    const Timer_A_UpModeConfig upConfig2 = {
    TIMER_A_CLOCKSOURCE_SMCLK,              // SMCLK Clock Source
            TIMER_A_CLOCKSOURCE_DIVIDER_3,          // SMCLK/3 = 1MHz
            MICROSECONDS,                           // tick period
            TIMER_A_TAIE_INTERRUPT_DISABLE,         // Disable Timer interrupt
            TIMER_A_CCIE_CCR0_INTERRUPT_ENABLE,    // Enable CCR0 interrupt
            TIMER_A_DO_CLEAR                        // Clear value
            };

    /* Configuring P3.6 as Output */
    GPIO_setAsOutputPin(GPIO_PORT_P3, GPIO_PIN6);
    GPIO_setOutputLowOnPin(GPIO_PORT_P3, GPIO_PIN6);

    /* Configuring P3.7 as input */
    GPIO_setAsInputPinWithPullDownResistor(GPIO_PORT_P3, GPIO_PIN7);

    /* Configuring P5.2 as Output */
    GPIO_setAsOutputPin(GPIO_PORT_P5, GPIO_PIN2);
    GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN2);

    /* Configuring P3.5 as input */
    GPIO_setAsInputPinWithPullDownResistor(GPIO_PORT_P3, GPIO_PIN5);

    /* Configuring P5.0 as Output */
    GPIO_setAsOutputPin(GPIO_PORT_P5, GPIO_PIN0);
    GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN0);

    /* Configuring P5.1 as input */
    GPIO_setAsInputPinWithPullDownResistor(GPIO_PORT_P5, GPIO_PIN1);

    /* Configuring Timer_A0 and Timer_A1 for Up Mode */
    Timer_A_configureUpMode(TIMER_A0_BASE, &upConfig);
    Timer_A_configureUpMode(TIMER_A1_BASE, &upConfig2);
    Timer_A_clearTimer(TIMER_A0_BASE);
    Timer_A_clearTimer(TIMER_A1_BASE);

    /* Configure P2.0/1/2 and set it to LOW LED for debugging */
    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2);
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2);

    /* this dumb sleep function is needed in init as somehow
     * the ultrasonic will set an interrupt if a delay is not set
     * before configuring interrupts*/
    sleep();

    /*Configure 3.7 interrupt*/
    GPIO_interruptEdgeSelect(GPIO_PORT_P3, GPIO_PIN7,
    GPIO_LOW_TO_HIGH_TRANSITION);
    GPIO_clearInterruptFlag(GPIO_PORT_P3, GPIO_PIN7);

    /*Configure 3.5 interrupt*/
    GPIO_interruptEdgeSelect(GPIO_PORT_P3, GPIO_PIN5,
    GPIO_LOW_TO_HIGH_TRANSITION);
    GPIO_clearInterruptFlag(GPIO_PORT_P3, GPIO_PIN5);

    /*Configure 5.1 interrupt*/
    GPIO_interruptEdgeSelect(GPIO_PORT_P5, GPIO_PIN1,
    GPIO_LOW_TO_HIGH_TRANSITION);
    GPIO_clearInterruptFlag(GPIO_PORT_P5, GPIO_PIN1);

    /*Set all to low intially for sanity check*/
    GPIO_setOutputLowOnPin(
            GPIO_PORT_P5,
            GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3 | GPIO_PIN4
                    | GPIO_PIN5 | GPIO_PIN6 | GPIO_PIN7);
    GPIO_setOutputLowOnPin(
            GPIO_PORT_P3,
            GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3 | GPIO_PIN4
                    | GPIO_PIN5 | GPIO_PIN6 | GPIO_PIN7);

    /*Clear all interrupt flags for port used*/
    GPIO_clearInterruptFlag(GPIO_PORT_P5, 0xFF);
    GPIO_clearInterruptFlag(GPIO_PORT_P3, 0xFF);

    /*Enable Interrupts*/
    GPIO_enableInterrupt(GPIO_PORT_P1, GPIO_PIN1 | GPIO_PIN4);
    GPIO_enableInterrupt(GPIO_PORT_P3, GPIO_PIN7);
    GPIO_enableInterrupt(GPIO_PORT_P3, GPIO_PIN5);
    GPIO_enableInterrupt(GPIO_PORT_P5, GPIO_PIN1);

    /* Enabling interrupts */
    Interrupt_enableInterrupt(INT_TA0_0);
    Interrupt_enableInterrupt(INT_TA1_0);
    Interrupt_enableInterrupt(INT_PORT3);
    Interrupt_enableInterrupt(INT_PORT5);
    Interrupt_enableMaster();

}
// -------------------------------------------------------------------------------------------------------------------

/*Function to calculate if a wall has been detected based on value received from ultrasonic and set minimum distance */
void calculateWallDetected(void)
{
    float calculatedDistance = 0;

    /* Calculating distance in cm */
    calculatedDistance = result / 58.0f;

    if (calculatedDistance < MIN_DISTANCE)
    {
        haveWall = 1;
    }
    else
    {
        haveWall = 0;
    }

    resultAvailableFlag = 1;
}

/*Trigger function for ultrasonic, to start sending a pulse*/
void sendTriggerToUltrasound(int sensorDirection)
{   // 0 = left, 1 = front, 2 = right
    if (sensorDirection == 0)
    {
        GPIO_setOutputHighOnPin(GPIO_PORT_P3, GPIO_PIN6);
    }
    else if (sensorDirection == 1)
    {
        GPIO_setOutputHighOnPin(GPIO_PORT_P5, GPIO_PIN2);
    }
    else if (sensorDirection == 2)
    {
        GPIO_setOutputHighOnPin(GPIO_PORT_P5, GPIO_PIN0);
    }
    Timer_A_clearTimer(TIMER_A1_BASE);
    Timer_A_startCounter(TIMER_A1_BASE, TIMER_A_UP_MODE);
}

/*api function to be called in main, checks left of wall*/
bool checkWallLeft()
{
    sensorSelect = 0;
    sendTriggerToUltrasound(0);
    while (1)
    {
        if (echoReceivedFlag)
        {
            calculateWallDetected();
            echoReceivedFlag = 0;
        }
        if (resultAvailableFlag)
        {
            resultAvailableFlag = 0;
            result = 0.0f;
            echoReceivedFlag = 0;
            return haveWall;
        }
    }
}

/*api function to be called in main, checks front of wall*/
bool checkWallFront()
{
    sensorSelect = 1;
    sendTriggerToUltrasound(1);
    while (1)
    {
        if (echoReceivedFlag)
        {
            calculateWallDetected();
            echoReceivedFlag = 0;
        }
        if (resultAvailableFlag)
        {
            resultAvailableFlag = 0;
            result = 0.0f;
            echoReceivedFlag = 0;
            return haveWall;
        }
    }
}

/*api function to be called in main, checks right of wall*/
bool checkWallRight()
{
    sensorSelect = 2;
    sendTriggerToUltrasound(2);
    while (1)
    {
        if (echoReceivedFlag)
        {
            calculateWallDetected();
            echoReceivedFlag = 0;
        }
        if (resultAvailableFlag)
        {
            resultAvailableFlag = 0;
            result = 0.0f;
            echoReceivedFlag = 0;
            return haveWall;
        }
    }
}
// -------------------------------------------------------------------------------------------------------------------

/*TA1 used to complete the trigger pulse*/
void TA1_0_IRQHandler(void)
{
    Timer_A_clearCaptureCompareInterrupt(TIMER_A1_BASE,
    TIMER_A_CAPTURECOMPARE_REGISTER_0);
    Timer_A_stopTimer(TIMER_A1_BASE);
    Timer_A_clearTimer(TIMER_A1_BASE);
    if (sensorSelect == 0)
    {
        GPIO_setOutputLowOnPin(GPIO_PORT_P3, GPIO_PIN6);
    }
    else if (sensorSelect == 1)
    {
        GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN2);
    }
    else if (sensorSelect == 2)
    {
        GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN0);
    }
}

/*Ultrasonic P3 ISR handler. These pins used to record length of echo pulse and store the result. Only P3.5 and P3.7 used.*/
void PORT3_IRQHandler(void)
{

    uint32_t status = MAP_GPIO_getEnabledInterruptStatus(GPIO_PORT_P3);

    if (status & GPIO_PIN7)
    {
        if (GPIO_getInputPinValue(GPIO_PORT_P3, GPIO_PIN7) == 1)
        {
            GPIO_interruptEdgeSelect(GPIO_PORT_P3, GPIO_PIN7,
            GPIO_HIGH_TO_LOW_TRANSITION);
            Timer_A_clearTimer(TIMER_A0_BASE);
            Timer_A_startCounter(TIMER_A0_BASE, TIMER_A_UP_MODE);
        }
        else if (GPIO_getInputPinValue(GPIO_PORT_P3, GPIO_PIN7) == 0)
        {
            Timer_A_stopTimer(TIMER_A0_BASE);
            GPIO_interruptEdgeSelect(GPIO_PORT_P3, GPIO_PIN7,
            GPIO_LOW_TO_HIGH_TRANSITION);
            result = Timer_A_getCounterValue(TIMER_A0_BASE);
            Timer_A_clearTimer(TIMER_A0_BASE);
            echoReceivedFlag = 1;
        }
    }
    if (status & GPIO_PIN5)
    {
        if (GPIO_getInputPinValue(GPIO_PORT_P3, GPIO_PIN5) == 1)
        {
            GPIO_interruptEdgeSelect(GPIO_PORT_P3, GPIO_PIN5,
            GPIO_HIGH_TO_LOW_TRANSITION);
            Timer_A_clearTimer(TIMER_A0_BASE);
            Timer_A_startCounter(TIMER_A0_BASE, TIMER_A_UP_MODE);
        }
        else if (GPIO_getInputPinValue(GPIO_PORT_P3, GPIO_PIN5) == 0)
        {
            Timer_A_stopTimer(TIMER_A0_BASE);
            GPIO_interruptEdgeSelect(GPIO_PORT_P3, GPIO_PIN5,
            GPIO_LOW_TO_HIGH_TRANSITION);
            result = Timer_A_getCounterValue(TIMER_A0_BASE);
            Timer_A_clearTimer(TIMER_A0_BASE);
            echoReceivedFlag = 1;
        }
    }
    GPIO_clearInterruptFlag(GPIO_PORT_P3, status);
}

/*Ultrasonic P5 ISR handler. These pins used to record length of echo pulse and store the result. Only P5.1 used.*/
void PORT5_IRQHandler(void)
{

    uint32_t status = MAP_GPIO_getEnabledInterruptStatus(GPIO_PORT_P5);

    if (status & GPIO_PIN1)
    {
        if (GPIO_getInputPinValue(GPIO_PORT_P5, GPIO_PIN1) == 1)
        {
            GPIO_interruptEdgeSelect(GPIO_PORT_P5, GPIO_PIN1,
            GPIO_HIGH_TO_LOW_TRANSITION);
            Timer_A_clearTimer(TIMER_A0_BASE);
            Timer_A_startCounter(TIMER_A0_BASE, TIMER_A_UP_MODE);
        }
        else if (GPIO_getInputPinValue(GPIO_PORT_P5, GPIO_PIN1) == 0)
        {
            Timer_A_stopTimer(TIMER_A0_BASE);
            GPIO_interruptEdgeSelect(GPIO_PORT_P5, GPIO_PIN1,
            GPIO_LOW_TO_HIGH_TRANSITION);
            result = Timer_A_getCounterValue(TIMER_A0_BASE);
            Timer_A_clearTimer(TIMER_A0_BASE);
            echoReceivedFlag = 1;
        }
    }
    GPIO_clearInterruptFlag(GPIO_PORT_P5, status);
}

// -------------------------------------------------------------------------------------------------------------------

/*Sample usage in main function*/
uint32_t main(void)
{
    bool leftHaveWall = 0;
    bool frontHaveWall = 0;
    bool rightHaveWall = 0;
    Initalise_Ultrasound();

    while (1)
    {
        // if state is checkWall call ultrasound functions
        leftHaveWall = checkWallLeft();
        frontHaveWall = checkWallFront();
        rightHaveWall = checkWallRight();

        /*Turns led on or off if there is a wall on the left/front/right,
         * otherwise turn led off. Remove this if not needed, used for debugging and development only*/
        if (leftHaveWall) //
        {
            GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN0);
        }
        else
        {
            GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0);
        }
        if (frontHaveWall)
        {
            GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN1);
        }
        else
        {
            GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN1);
        }
        if (rightHaveWall)
        {
            GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN2);
        }
        else
        {
            GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN2);
        }
    }
}
