/*
 * initSpeed.h
 *
 *  Created on: 24 Nov 2022
 *      Author: Marcus
 */

#ifndef INIT_INITSPEED_H_
#define INIT_INITSPEED_H_

#include <stdio.h>
#include "driverlib.h"
#include "initMovement.h"
extern uint32_t pulseCountTrigger;
extern uint32_t pulseCountL;
extern uint32_t pulseCountR;
extern uint32_t pulseCountAvg;
extern bool turning;


void initSpeed(void);

void PORT6_IRQHandler(void);

void PORT4_IRQHandler(void);

void TA_3_IRQHandler(void);



#endif /* INIT_INITSPEED_H_ */
