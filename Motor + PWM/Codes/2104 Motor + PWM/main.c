/*******************************************************************************
 * ICT2104 Robotic Car - Motor + PWM
 *
 *
 *                   MSP432
 *             ------------------
 *         /|\|                  |
 *          | |                  |
 *          | |                  |  
 *          | |                  |
 *          --|RST               |
 *            |                  |
 *            |            P4.7  |--> Right Speed Encoder
 *            |            P2.4  |--> Right Motor PWM
 *            |            P2.5  |--> Left Motor PWM
 *            |            p6.0  |--> Left Speed Encoder
 *
 * 
 *  PORT6_IRQHandler - Left Speed Encoder
 *  PORT4_IRQHandler - Right Speed Encoder
 *  PORT2_IRQHandler - Left Line Sensor
 *  PORT5_IRQHandler - Right Line Sensor
 *
 *
 *  LEDs
 *  Red - Stop/Idle
 *  Green - Moving Straight
 *  Blue - Turning right/Turning left
 *
 *******************************************************************************/

/* DriverLib Includes */
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "driverlib.h"

// Encoder Variables
uint32_t pulseCountTrigger;
uint32_t pulseCountL = 0;
uint32_t pulseCountR = 0;
uint32_t distanceL = 0;
uint32_t distanceR = 0;
bool start_pulse = 0;

// Movement Variables
uint32_t stop = 0;
uint32_t speedL = 7500;
uint32_t speedR = 6500;
uint32_t minDist = 15;
bool turning = 0;

// PID controller Variables
uint32_t lastErrorLeft = 0;
uint32_t lastErrorRight = 0;
uint32_t totalErrorLeft = 0;
uint32_t totalErrorRight = 0;
float leftPIDOutput = 0;
float rightPIDOutput = 0;
uint32_t PWML = 0;
uint32_t PWMR = 0;

// Motor and Encoder includes
#include <init/initMovement.h>
#include <init/initSpeed.h>


int main(void)
{
    /* Halting the watchdog */
    WDT_A_holdTimer();

    // Initalise Motor and Encoder
    initMotor();
    initSpeed();

    Interrupt_enableInterrupt(INT_PORT1);
//    Interrupt_enableSleepOnIsrExit();
    Interrupt_enableMaster();

// Call functions. Depening on what the user want to do
//    turnRight(8);
//    turnRight(7);
//    moveCar(40);
//    turnLeft(8);
//    stopCar();

    while (1) {
        PCM_gotoLPM3();

//        checkPWM();
    }
}


