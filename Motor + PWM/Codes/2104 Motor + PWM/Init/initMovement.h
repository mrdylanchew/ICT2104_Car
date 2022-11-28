#ifndef INIT_INITMOVEMENT_H_
#define INIT_INITMOVEMENT_H_
#define DELAY_BACKWARD_COMPATIBLE

/* DriverLib Includes */
#include <stdio.h>
#include "driverlib.h"

extern uint32_t pulseCountTrigger;
extern uint32_t pulseCountL;
extern uint32_t pulseCountR;
extern uint32_t distanceL;
extern uint32_t distanceR;
extern uint32_t stop;
extern uint32_t speedL;
extern uint32_t speedR;
extern bool turning;

extern uint32_t lastErrorLeft;
extern uint32_t lastErrorRight;
extern uint32_t totalErrorLeft;
extern uint32_t totalErrorRight;
extern float leftPIDOutput;
extern float rightPIDOutput;
extern uint32_t PWML;
extern uint32_t PWMR;


void initMotor(void);

void stopCar(void);

void moveCar(int grid);

void turnLeft(int grid);

void turnRight(int grid);

void CarPID(int target);



#endif /* INIT_INITMOVEMENT_H_ */
