#ifndef INIT_INITSENSOR_H_
#define INIT_INITSENSOR_H_

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


#endif /* INIT_INITSENSOR_H_ */
