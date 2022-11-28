/*******************************************************************************
 * ICT2104 Robotic Car - Motor + PWM
 *
 *                  MSP432
 *             ------------------
 *         /|\|                  |
 *          | |                  |
 *          | |                  |
 *          | |                  |
 *          --|RST               |
 *            |            P2.4  |--> Right Motor PWM
 *            |            P2.5  |--> Left Motor PWM
 *            |                  |
 *
 *******************************************************************************/

#include "initMovement.h"

/* Timer_A PWM Configuration Parameter */
// Configure for the RIGHT motor
Timer_A_PWMConfig pwmConfig =
{
        // Move the RIGHT wheel
        TIMER_A_CLOCKSOURCE_SMCLK,
        TIMER_A_CLOCKSOURCE_DIVIDER_64, // 12Mhz/64 = 0.1875s
        10000,
        // For Port2.4 as it is TA_0.1
        TIMER_A_CAPTURECOMPARE_REGISTER_1,
        TIMER_A_OUTPUTMODE_RESET_SET,
        0
};

//Configure for the LEFT motor
Timer_A_PWMConfig pwmConfig2 =
{
        // Move the LEFT wheel
        TIMER_A_CLOCKSOURCE_SMCLK,
        TIMER_A_CLOCKSOURCE_DIVIDER_64,
        10000,
        // For Port 2.5 as it is TA_0.2
        TIMER_A_CAPTURECOMPARE_REGISTER_2,
        TIMER_A_OUTPUTMODE_RESET_SET,
        0
};

// Function to Initialise motor. This represents the Idle state of the car. Once motor has been initialised,
// it is ready to move. initMotor is called at the end of every function to ensure motor is always ready
// If init correctly, motor should be vibrating
void initMotor(void) {

    pwmConfig.dutyCycle = stop;
    pwmConfig2.dutyCycle = stop;

    // Motor to turn (clockwise direction in1->4.5 and in2->P4.4)
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

    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN0);
    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN1);
    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN2);

    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0);
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN1);
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN2);

    GPIO_toggleOutputOnPin(GPIO_PORT_P2, GPIO_PIN0);


    // Left Motor
    Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig);
    // Right motor
    Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig2);

    printf("\nMotor initialised!");
}

// Function to stop the car. Speed for both wheels will be adjusted to 0
void stopCar(void) {
// Speed set to 0
    pwmConfig.dutyCycle = 0;
    pwmConfig2.dutyCycle = 0;
    Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig);
    Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig2);
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0);
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN1);
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN2);
    GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN0);
    printf("Car stopped! \n");
    initMotor();

}

// This function is to move car depending on the grid specified.
void moveCar(int grid) {
    // 20cm = 1 full wheel revolution
    // Used to measure the number of wheel rotations travelled by the car
    pulseCountTrigger = 20*grid;
    printf("pulseCountTrigger: %i \n", pulseCountTrigger);
    pwmConfig.dutyCycle = speedL;
    pwmConfig2.dutyCycle = speedR;
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0);
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN1);
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN2);
    GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN1);

    Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig);
    Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig2);
    printf("Car moving %i grids! \n", grid);

// Longer delay, = longer distance for each grid count
    volatile uint32_t i;
    for (i = grid ; i > 0 ; i--) {
//        __delay_cycles(24000000);
        __delay_cycles(12000000);
    }
// Call back initMotor to get car ready
    initMotor();
}

// This fucntion enables the car to turn left
// If angle = 1: Car turns left by 45deg. If want turn more, increase angle 
void turnLeft(int angle) {
    // 20cm = 1 full wheel revolution
    // this is to prevent pid from being called. PID only called when turning = 0(car moving straight)
    turning = 1;
    pulseCountTrigger = 20*angle;
    printf("pulseCountTrigger: %i \n", pulseCountTrigger);
    // Only the left
    pwmConfig.dutyCycle = speedR;
    pwmConfig2.dutyCycle = 0;
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0);
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN1);
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN2);
    GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN1);

    Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig);
    Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig2);
    printf("Car turning left %i degree! \n", angle * 45);

    volatile uint32_t i;
    for (i = angle ; i > 0 ; i--) {
//        __delay_cycles(24000000);
        __delay_cycles(3350000);
    }
//    initMotor();
    turning = 0;

}


// This fucntion enables the car to turn Right
// If angle = 1: Car turns Right by 45deg. If want turn more, increase angle 
void turnRight(int angle) {
    // 20cm = 1 full wheel revolution
    // this is to prevent pid from being called. PID only called when turning = 0(car moving straight)
    turning = 1;
    pulseCountTrigger = 20*angle;
    printf("pulseCountTrigger: %i \n", pulseCountTrigger);
    pwmConfig.dutyCycle = 0;
    pwmConfig2.dutyCycle = speedL;
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0);
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN1);
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN2);
    GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN1);

    Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig);
    Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig2);
    printf("Car turning right %i degree! \n", angle * 45);

    volatile uint32_t i;
    for (i = angle ; i > 0 ; i--) {
//        __delay_cycles(24000000);
        __delay_cycles(3350000);
    }
   initMotor();
    turning = 0;

}

// This function will be called each time either wheel makes a full rotation to adjust the speed of both wheels
void CarPID(int target) {
    leftPIDOutput = 0;
    rightPIDOutput = 0;
    PWML = 0;
    PWMR = 0;
    // Set the target count
    int targetCount = target;

    
// Calculate the Proportional Control
//  float Kp = 0.065; // Proportional gain
    float Kp = 0.075; // Proportional gain
    // Get the error for left and right encoder
    printf("LeftCount %d\n" , pulseCountL);
    printf("RightCount %d\n" , pulseCountR);
    int errorLeft = targetCount - pulseCountL;
    int errorRight = targetCount - pulseCountR;

    float PoutLeft = Kp * errorLeft;
    float PoutRight = Kp * errorRight;

    // Update the last error
    lastErrorLeft = errorLeft;
    lastErrorRight = errorRight;

    // Add all the error together
    totalErrorLeft += errorLeft;
    totalErrorRight += errorRight;


    //Calculate the Integral Control
    float Ki = 0.005; // Integral gain
    float IoutLeft = Ki * totalErrorLeft;
    float IoutRight = Ki * totalErrorRight;

//        float Kd = 0.38; // Derivative gain
    float Kd = 0.35; // Derivative gain
    float DoutLeft = Kd * (errorLeft - lastErrorLeft);
    float DoutRight = Kd * (errorRight - lastErrorRight);

    leftPIDOutput = PoutLeft + DoutLeft + IoutLeft;
    rightPIDOutput = PoutRight + DoutRight + IoutRight;

    if (leftPIDOutput > 1) {
        leftPIDOutput = 1;
    }

    if (rightPIDOutput > 1) {
        rightPIDOutput = 1;
    }

    if (leftPIDOutput < 0) {
        leftPIDOutput = 0;
    }

    if (rightPIDOutput < 0) {
        rightPIDOutput = 0;
    }

    PWML = leftPIDOutput * speedL;
    PWMR = rightPIDOutput * speedR;

    // for debugging purposes
    //printf("PWMLeftInput %d\n", PWML);
    //printf("PWMRightInput %d\n", PWMR);


    if ( pwmConfig.dutyCycle <= 7000 && pwmConfig2.dutyCycle <= 7000 ) {
        pwmConfig.dutyCycle += PWML;
        pwmConfig2.dutyCycle += PWMR;
    } else if ( pwmConfig.dutyCycle > 7000 && pwmConfig2.dutyCycle <= 7000 ) {
//            pwmConfig.dutyCycle = speedL;
        pwmConfig.dutyCycle += (PWMR - PWML);
        pwmConfig2.dutyCycle += (PWML - PWMR);
    } else if ( pwmConfig.dutyCycle <= 7000 && pwmConfig2.dutyCycle > 7000 ) {
        pwmConfig.dutyCycle += (PWMR - PWML);
        pwmConfig2.dutyCycle += (PWML - PWMR);
//            pwmConfig2.dutyCycle = speedR;
    } else if ( pwmConfig.dutyCycle > 7000 && pwmConfig2.dutyCycle > 7000 ) {
        pwmConfig.dutyCycle -= PWML;
        pwmConfig2.dutyCycle -= PWMR;
    }

    // for debugging purposes
    //printf("SpeedL %d\n", pwmConfig.dutyCycle);
    //printf("SpeedR %d\n", pwmConfig2.dutyCycle);

    Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig);
    Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig2);
}

