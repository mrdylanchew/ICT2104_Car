/*******************************************************************************
 * ICT2104 Robotic Car - Motor + PWM
 *
 *                   MSP432
 *             ------------------
 *         /|\|                  |
 *          | |                  |
 *          | |                  |
 *          | |                  |
 *          --|RST               |
 *            |            P2.4  |--> Right Motor PWM
 *            |            P2.5  |--> Left Motor PWM
 *            |            P6.0  |--> Left Speed Encoder
 *            |            P4.7  |--> Right Speed Encoder
 *
 *  PORT6_IRQHandler - Left Speed Sensor
 *  PORT4_IRQHandler - Right Speed Sensor
 *
 *******************************************************************************/

#include "initSpeed.h"


// To initialise encoders on both sides
void initSpeed(void) {

    // Left Speed Encoder
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P6, GPIO_PIN0);
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P4, GPIO_PIN7);

    // Interrupt for speed encoder
    GPIO_clearInterruptFlag(GPIO_PORT_P6, GPIO_PIN0);
    GPIO_enableInterrupt(GPIO_PORT_P6, GPIO_PIN0);
    GPIO_clearInterruptFlag(GPIO_PORT_P4, GPIO_PIN7);
    GPIO_enableInterrupt(GPIO_PORT_P4, GPIO_PIN7);

    Interrupt_enableInterrupt(INT_PORT6);
    Interrupt_enableInterrupt(INT_PORT4);

    printf("\nWheel Encoders Initialised!");
}

// When the wheel encoder triggers the interrupt, increment the count on
// that side, and check if PID needs to be triggered, or if car needs to
// be stopped.

void PORT6_IRQHandler(void) {
    uint32_t status = GPIO_getEnabledInterruptStatus(GPIO_PORT_P6);

    pulseCountL++;
    // call PID after every roation
    if ( pulseCountL % 20 == 0 && pulseCountL > 19 && turning == 0) {
        CarPID(pulseCountL);
        pulseCountR = pulseCountL;
    }
    // Once the car has travelled the specified distance, the car will stop
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
    // call PID after every roation
    if ( pulseCountR % 20 == 0 && pulseCountR > 19 && turning == 0) {
        CarPID(pulseCountR);
        pulseCountL = pulseCountR;
    }
    // Once the car has travelled the specified distance, the car will stop
    if ( pulseCountR == pulseCountTrigger ) {
        stopCar();
        pulseCountL = 0;
        pulseCountR = 0;
    }

    GPIO_clearInterruptFlag(GPIO_PORT_P4, status);
}
