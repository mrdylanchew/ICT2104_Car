/*
 * initUltrasound.h
 *
 *  Created on: 16 Nov 2022
 *      Author: Marcus
 */

#ifndef INIT_INITULTRASOUND_H_
#define INIT_INITULTRASOUND_H_

#include <stdio.h>
#include "driverlib.h"

#define MIN_DISTANCE    15.0f
#define TICKPERIOD      50000
#define MICROSECONDS    10

extern bool haveWall;
extern int sensorSelect;
extern int resultAvailableFlag;
extern float result;
extern int echoReceivedFlag;

void sleep();
void initUltrasound(void);
void calculateWallDetected(void);
void sendTriggerToUltrasound(int sensorDirection);
bool checkWallLeft();
bool checkWallFront();
bool checkWallRight();



#endif /* INIT_INITULTRASOUND_H_ */
