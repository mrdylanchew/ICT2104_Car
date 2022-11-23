
/*******************************************************************************
 * ICT2104 Robotic Car
 *
 * Description: Robotic Car, controllable from web portal using
 * a scratch programming interface.
 *
 *                MSP432P401
 *             ------------------
 *         /|\|            P2.3  |--> Line Sensor Left
 *          | |            P5.0  |--> Line Sensor Right
 *          | |            P3.2  |--> WiFi Tx
 *          | |            P3.3  |--> WiFi Rx
 *          --|RST         P6.1  |--> WiFi RST
 *            |            P3.0  |--> Ultrasonic Echo
 *            |            P6.6  |--> Ultrasonic Trigger
 *            |            P2.4  |--> Left Motor PWM
 *            |            P2.5  |--> Right Motor PWM
 *            |            p6.0  |--> Speed Sensor output
 *
 *  PORT1_IRQHandler - Buttons
 *  PORT3_IRQHandler - Ultrasonic
 *  PORT6_IRQHandler - Left Speed Encoder
 *  PORT4_IRQHandler - Right Speed Encoder
 *  PORT2_IRQHandler - Left Line Sensor
 *  PORT5_IRQHandler - Right Line Sensor
 *
 *
 *  SPLITTER
 *  Blue: GROUNDING
 *  1 - Left line tracker
 *  2 - Right line tracker
 *  3 - Ultrasonic
 *  Yellow: 3v3
 *  1
 *  2 - Ultrasonic
 *  3 - Left speed encoder
 *  Red: 3v3
 *  1 - Right speed encoder
 *  2 - Left line encoder
 *  3 - Right line encoder
 *
 *
 *  LEDs
 *  Cyan - Moving forward
 *  Purple - Turning
 *  Yellow - Inside delay, waiting to execute next command
 *  Red - Stopped
 *
 *******************************************************************************/

/* DriverLib Includes */
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "driverlib.h"

// vars for ultrasonic
uint32_t pulseCountTrigger;
uint32_t pulseCountL = 0;
uint32_t pulseCountR = 0;
bool start_pulse = 0;
// vars for movement
uint32_t stop = 0;
uint32_t speedL = 6000;
uint32_t speedR = 6500;
uint32_t minDist = 15;
bool turning = 0;

uint32_t startCar = 0;

// vars for PID controller
uint32_t lastErrorLeft = 0;
uint32_t lastErrorRight = 0;
uint32_t totalErrorLeft = 0;
uint32_t totalErrorRight = 0;
float leftPIDOutput = 0;
float rightPIDOutput = 0;
uint32_t PWML = 0;
uint32_t PWMR = 0;

//var for Ultrasound
bool haveWall = 0;
int sensorSelect = 0;
int resultAvailableFlag = 0;
float result = 0.0f;
int echoReceivedFlag = 0;


#include <init/initMovement.h>
#include <init/initMapping.h>
#include <init/initUltrasound.h>
#include <init/printf.h>
//#include <init/initSpeed.h>
void uPrintf(unsigned char * TxArray);


int main(void)
{
    /* Halting the watchdog */
    WDT_A_holdTimer();
    MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN4);
    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P1, GPIO_PIN4);
    MAP_GPIO_enableInterrupt(GPIO_PORT_P1, GPIO_PIN4);
    /* Selecting P1.2 and P1.3 in UART mode */
    initprintf();
    // Initalise sensors and functions
    initMotor();
    initUltrasound();

    //Uartprintf(EUSCI_A0_BASE, "String\r\n");
    Interrupt_enableInterrupt(INT_PORT1);
    //Interrupt_enableSleepOnIsrExit();
    Interrupt_enableMaster();
    //moveCar(2);
    //turnRight();
    //stopCar();
//    Uartprintf(EUSCI_A0_BASE, "start\r\n");
//    if(checkWallRight() == 0)
//    {
//        Uartprintf(EUSCI_A0_BASE, "right\r\n");
//    }
//    if(checkWallFront() == 0)
//    {
//        Uartprintf(EUSCI_A0_BASE, "front\r\n");
//    }
//    if(checkWallLeft() == 0)
//    {
//        Uartprintf(EUSCI_A0_BASE, "left\r\n");
//    }

//    moveCar(10);
//    turnLeft();
//    moveCar(10);
//    turnRight();
//    stopCar();

    // Simulated movement
      //moveCar(1);
//    turnRight();
//    moveCar(3);
//    turnLeft();
//    moveCar(5);
      //stopCar();
    while (1) {

        if (startCar == 1) {
            //Uartprintf(EUSCI_A0_BASE, "run\r\n");
            //initMap();

            startCar = 0;
            moveCar(2);
            stopCar();
        }

        else {
            PCM_gotoLPM3();
        }

        //PCM_gotoLPM3();

//        printf("pulseCountL: %i \n", pulseCountL);
//        printf("pulseCountR: %i \n\n", pulseCountR);
//
//        __delay_cycles(6000000);

//        checkPWM();
    }
}


void PORT1_IRQHandler(void){
    uint32_t status = MAP_GPIO_getEnabledInterruptStatus(GPIO_PORT_P1);
    GPIO_clearInterruptFlag(GPIO_PORT_P1, status);
    if (status & GPIO_PIN4)
    {
        //set flag to 1
        startCar = 1;
        //Uartprintf(EUSCI_A0_BASE, "String %i\r\n",startCar);
        //initMap();
        //moveCar(1);
        //stopCar();


    }
}
