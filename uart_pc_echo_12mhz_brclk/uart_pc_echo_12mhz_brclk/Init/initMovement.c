/*******************************************************************************
 * ICT2104 Robotic Car
 *
 * Description: Robotic Car, controllable from web portal using
 * a scratch programming interface.
 *
 *                MSP432P401
 *             ------------------
 *         /|\|                  |
 *          | |                  |
 *          | |                  |
 *          | |                  |
 *          --|RST               |
 *            |            P2.4  |--> Left Motor PWM
 *            |            P2.5  |--> Right Motor PWM
 *            |            p6.0  |--> Speed Sensor output
 *
 *  PORT1_IRQHandler - Buttons
 *
 *******************************************************************************/

#include "initMovement.h"

/* Timer_A PWM Configuration Parameter */
// Configure for the left motor
Timer_A_PWMConfig pwmConfig =
{
        // Move the left wheel
        TIMER_A_CLOCKSOURCE_SMCLK,
        TIMER_A_CLOCKSOURCE_DIVIDER_10, // 12Mhz/64 = 0.1875s
        10000,
        // For Port2.4 as it is TA_0.1
        TIMER_A_CAPTURECOMPARE_REGISTER_1,
        TIMER_A_OUTPUTMODE_RESET_SET,
        9000
};

//Configure for the right motor
Timer_A_PWMConfig pwmConfig2 =
{
        // Move the right wheel
        // Need to increase the PWM for right motor as it is moving slower
        TIMER_A_CLOCKSOURCE_SMCLK,
        TIMER_A_CLOCKSOURCE_DIVIDER_10,
        10000,
        // For Port 2.5 as it is TA_0.2
        TIMER_A_CAPTURECOMPARE_REGISTER_2,
        TIMER_A_OUTPUTMODE_RESET_SET,
        9000
};


void initMotor(void) {

    pwmConfig.dutyCycle = stop;
    pwmConfig2.dutyCycle = stop;

    // This output allow motor to turn (clockwise direction in1->4.5 and in2->P4.4)
    // Set P4.4 and 4.5 as out for the left wheel
    // Set P4.0 and 4.2 as out for right wheel
    GPIO_setAsOutputPin(GPIO_PORT_P4, GPIO_PIN4);
    GPIO_setAsOutputPin(GPIO_PORT_P4, GPIO_PIN5);
    GPIO_setAsOutputPin(GPIO_PORT_P4, GPIO_PIN0);
    GPIO_setAsOutputPin(GPIO_PORT_P4, GPIO_PIN2);

    // Set P4.4 as low and 4.5 as high for the left wheel to turn clockwise. High on In1 and Low on In2
    // To change the direction just put Low on In1 and High on In2 (Anticlockwise)
    // Set P4.0 and 4.2 as out for right wheel to move it clockwise High on In3 and Low on In4
    // (clockwise direction in3->4.2 and in4->P4.0)
    GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN4);
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN5);
    GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN0);
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN2);


    // Set the PSEL0 & PSEL1 multiplex and alternative pin function as timer function
    // Set peripheral module function in the output direction for PWM P2.4 for left wheel & 2.5 for right wheel as primary module function modes.
    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P2, GPIO_PIN4, GPIO_PRIMARY_MODULE_FUNCTION);
    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P2, GPIO_PIN5, GPIO_PRIMARY_MODULE_FUNCTION);


    // For left motor
    Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig);
    // For right motor
    Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig2);

    printf("\nMotor initialised!");
}

void stopCar(void) {

    pwmConfig.dutyCycle = 0;
    pwmConfig2.dutyCycle = 0;
    Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig);
    Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig2);
    GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN0);
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0);
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN1);
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN2);
//    printf("Car stopped! \n");

}


void moveCar(int grid) {
    // 20cm = 1 full wheel revolution
    pulseCountTrigger = 20*grid;
    printf("pulseCountTrigger: %i \n", pulseCountTrigger);
    GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0);
    GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN1);
    GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN2);
    pwmConfig.dutyCycle = speedL;
    pwmConfig2.dutyCycle = speedR;
    Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig);
    Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig2);
    printf("Car moving %i grids! \n", grid);

    volatile uint32_t i;
    for (i = 0 ; i < grid ; i++) {
//        __delay_cycles(24000000);
        __delay_cycles(2500000);
    }

}


void turnRight(void) {

    // Set bool to prevent PID from interfering
    turning = 1;
    // 22 ticks for 1 90degree turn, since right motor is weaker
    pulseCountTrigger = 20;
    GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);
    GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN0);
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN1);
    GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN2);
    pwmConfig.dutyCycle = speedR;
    pwmConfig2.dutyCycle = stop;
    Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig);
    Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig2);
    printf("Car turning right! \n");

    //__delay_cycles(24000000);
    __delay_cycles(937500);
    turning = 0;

}


void turnLeft(void) {

    // Set bool to prevent PID from interfering
    turning = 1;
    // 18 ticks for 1 90degree turn
    pulseCountTrigger = 20;
    GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);
    GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN0);
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN1);
    GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN2);
    pwmConfig.dutyCycle = stop;
    pwmConfig2.dutyCycle = speedL;
    Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig);
    Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig2);
    printf("Car turning left! \n");

    //__delay_cycles(24000000);
    __delay_cycles(937500);
    turning = 0;

}

//void addLeft(void) {
//
//    if (pwmConfig.dutyCycle < 7000) {
//        pwmConfig.dutyCycle += 1000;
//        pwmConfig2.dutyCycle -= 1000;
//    } else {
//        pwmConfig2.dutyCycle -= 1000;
//    }
//    Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig);
//    Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig2);
//
//}
//
//void addRight(void) {
//
//    if (pwmConfig2.dutyCycle < 7000) {
//        pwmConfig.dutyCycle -= 1000;
//        pwmConfig2.dutyCycle += 1000;
//    } else {
//        pwmConfig.dutyCycle -= 1000;
//    }
//    Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig);
//    Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig2);
//
//}


void CarPID(int target) {

    leftPIDOutput = 0;
    rightPIDOutput = 0;
    PWML = 0;
    PWMR = 0;
    // Set the target count
    int targetCount = target;
    // Calculate the Proportional Control
    float Kp = 0.02; // Proportional gain
    // Get the error for left and right encoder
//    printf("LeftCount %d\n" , pulseCountL);
//    printf("RightCount %d\n" , pulseCountR);
    int errorLeft = targetCount - pulseCountL;
    int errorRight = targetCount - pulseCountR;

    float PoutLeft = Kp * errorLeft;
    float PoutRight = Kp * errorRight;
    // Ensure the KP is within the range of 0 to 1 for POUTLeft and POUTRight
    if (PoutLeft > 1) {
        PoutLeft = 1;
    } else if (PoutLeft < 0) {
        PoutLeft = 0;
    }
    if (PoutRight > 1) {
        PoutRight = 1;
    } else if (PoutRight < 0) {
        PoutRight = 0;
    }
    // Debugging Purpose
//    printf("errorLeft %d\n" , errorLeft);
//    printf("errorRight %d\n" , errorRight);
//    printf("PoutLeft %.2f\n" , PoutLeft);
//    printf("PoutRight %.2f\n" , PoutRight);

    // Calculate the Derivative Control
    float Kd = 0.01; // Derivative gain
    float DoutLeft = Kd * (errorLeft - lastErrorLeft);
    float DoutRight = Kd * (errorRight - lastErrorRight);
    if (DoutLeft > 1) {
        DoutLeft = 0.01;
    }
    if (DoutRight > 1) {
        DoutRight = 0.01;
    }
    //Debugging Purpose
//    printf("DoutLeft %.2f\n" , DoutLeft);
//    printf("DoutRight %.2f\n" , DoutRight);


// Update the last error
    lastErrorLeft = errorLeft;
    lastErrorRight = errorRight;


    // Add all the error together
    totalErrorLeft += errorLeft;
    totalErrorRight += errorRight;
    // Calculate the Integral Control
    float Ki = 0.005; // Integral gain
    float IoutLeft = Ki * totalErrorLeft;
    float IoutRight = Ki * totalErrorRight;
    //Debugging Purpose
//    printf("IoutLeft %.2f\n" , PoutLeft);
//    printf("IoutRight %.2f\n" , PoutRight);

    // Calculate the PID output

    leftPIDOutput = PoutLeft + DoutLeft + IoutLeft;
    rightPIDOutput = PoutRight + DoutRight + IoutRight;

    if (leftPIDOutput > 1) {
        leftPIDOutput = 0;
    }

    if (rightPIDOutput > 1) {
        rightPIDOutput = 0;
    }

    // Debugging Purpose
//    printf("leftPIDOutput %.2f\n" , leftPIDOutput);
//    printf("rightPIDOutput %.2f\n" , rightPIDOutput);

    // Set the PWM for the left and right motor
    PWML = leftPIDOutput * speedL;
    PWMR = rightPIDOutput * speedR;

    // Debugging Purpose
//    printf("PWMLeftInput %d\n", PWML);
//    printf("PWMRightInput %d\n", PWMR);

    // Set the PWM for the left and right motor
    // Ensure that dutyCycle does not go too high and max out

    if ( pwmConfig.dutyCycle <= 7000 && pwmConfig2.dutyCycle <= 7000 ) {
        pwmConfig.dutyCycle += PWML;
        pwmConfig2.dutyCycle += PWMR;
    } else if ( pwmConfig.dutyCycle > 7000 && pwmConfig2.dutyCycle < 7000 ) {
        pwmConfig.dutyCycle = speedL;
        pwmConfig2.dutyCycle += (PWMR - PWML);
    } else if ( pwmConfig.dutyCycle < 7000 && pwmConfig2.dutyCycle > 7000 ) {
        pwmConfig.dutyCycle += (PWML - PWMR);
        pwmConfig2.dutyCycle = speedR;
    } else if ( pwmConfig.dutyCycle > 7000 && pwmConfig2.dutyCycle > 7000 ) {
        pwmConfig.dutyCycle -= PWML;
        pwmConfig2.dutyCycle -= PWMR;
    }


//    if ( pwmConfig.dutyCycle <= 7000 && pwmConfig2.dutyCycle <= 7000 ) {
//
//        if ( pwmConfig.dutyCycle < 3000 ) {
//            pwmConfig.dutyCycle = speedL;
//        } else if ( pwmConfig2.dutyCycle < 3000 ) {
//            pwmConfig2.dutyCycle = speedR;
//        } else {
//            pwmConfig.dutyCycle += PWML;
//            pwmConfig2.dutyCycle += PWMR;
//        }
//
//    } else if ( pwmConfig.dutyCycle > 7000 && pwmConfig2.dutyCycle <= 7000 ) {
//
//        if ( pwmConfig2.dutyCycle < 3000 ) {
//            pwmConfig2.dutyCycle = speedR;
//        } else {
//            pwmConfig.dutyCycle -= PWML;
//            pwmConfig2.dutyCycle += PWMR;
//        }
//
//    } else if ( pwmConfig.dutyCycle <= 7000 && pwmConfig2.dutyCycle > 7000 ) {
//
//        if ( pwmConfig.dutyCycle < 3000 ) {
//            pwmConfig.dutyCycle = speedL;
//        } else {
//            pwmConfig.dutyCycle += PWML;
//            pwmConfig2.dutyCycle -= PWMR;
//        }
//
//    } else if ( pwmConfig.dutyCycle > 7000 && pwmConfig2.dutyCycle > 7000 ) {
//
//        pwmConfig.dutyCycle -= PWML;
//        pwmConfig2.dutyCycle -= PWMR;
//
//    }


//    if ( (pwmConfig.dutyCycle < 8000) && (pwmConfig2.dutyCycle < 8000) ) {
//        pwmConfig.dutyCycle = PWML + speedL;
//        pwmConfig2.dutyCycle = PWMR + speedR;
//    } else if ( (pwmConfig.dutyCycle > 8000) && (pwmConfig2.dutyCycle < 8000) ) {
//        pwmConfig.dutyCycle = speedL - PWML;
//        pwmConfig2.dutyCycle = PWMR + speedR;
//    } else if ( (pwmConfig.dutyCycle < 8000) && (pwmConfig2.dutyCycle > 8000) ) {
//        pwmConfig.dutyCycle = PWML + speedL;
//        pwmConfig2.dutyCycle = speedR - PWMR;
//    } else if ( (pwmConfig.dutyCycle > 8000) && (pwmConfig2.dutyCycle > 8000) ) {
//        pwmConfig.dutyCycle = speedL - PWML;
//        pwmConfig2.dutyCycle = speedR - PWMR;
//    }


//    pwmConfig.dutyCycle = PWML + speedL;
//    pwmConfig2.dutyCycle = PWMR + speedR;


    // Debugging Purpose
//    printf("pwmConfig.dutyCycle %d\n", pwmConfig.dutyCycle);
//    printf("pwmConfig2.dutyCycle %d\n", pwmConfig2.dutyCycle);

    Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig);
    Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig2);
}
