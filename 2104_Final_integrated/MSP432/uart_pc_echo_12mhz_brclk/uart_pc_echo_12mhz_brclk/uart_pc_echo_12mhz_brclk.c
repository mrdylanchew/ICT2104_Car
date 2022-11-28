
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

int SPEED = 30;
int DISTANCE_TRAVELLED = 2;
int HEIGHT_OF_HUMP = 10;
unsigned char* BARCODE_VALUE = "*A*";
unsigned volatile char* SHORTEST_PATH;
unsigned char* MAP_ROW_1 = "*0->1->2->3->4*";
unsigned char* MAP_ROW_2 = "*5->6->7->8->9*";
unsigned char* MAP_ROW_3 = "*10->11->12->13->14*";
unsigned char* MAP_ROW_4 = "*15->16->17->18->19*";


#include <init/initMovement.h>
#include <init/initMapping.h>
#include <init/initUltrasound.h>
#include <init/printf.h>
#include <init/initSpeed.h>
#include <init/initComms.h>
//#include <init/initSpeed.h>
void uPrintf(unsigned char * TxArray);


int main(void)
{
    /* Halting the watchdog */
    WDT_A_holdTimer();
    MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN4);
    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P1, GPIO_PIN4);
    MAP_GPIO_enableInterrupt(GPIO_PORT_P1, GPIO_PIN4);
    // initalise printf
    initprintf();
    // Initalise sensors and functions
    initMotor();
    // initalise speed
    initSpeed();
    // initalise sound
    initUltrasound();
    // initialise communication
    initComms();

    //Uartprintf(EUSCI_A0_BASE, "String\r\n");
    Interrupt_enableInterrupt(INT_PORT1);
    //Interrupt_enableSleepOnIsrExit();
    Interrupt_enableMaster();


    Uartprintf(EUSCI_A0_BASE, "start\r\n");

    while (1) {

        if (startCar == 1) {
            Uartprintf(EUSCI_A0_BASE, "run\r\n");
            initMap();
            startCar = 0;
        }

        else {
            PCM_gotoLPM3();
        }

    }
}


void PORT1_IRQHandler(void){
    uint32_t status = MAP_GPIO_getEnabledInterruptStatus(GPIO_PORT_P1);
    GPIO_clearInterruptFlag(GPIO_PORT_P1, status);
    if (status & GPIO_PIN4)
    {
        //set flag to 1
        startCar = 1;

    }
}
