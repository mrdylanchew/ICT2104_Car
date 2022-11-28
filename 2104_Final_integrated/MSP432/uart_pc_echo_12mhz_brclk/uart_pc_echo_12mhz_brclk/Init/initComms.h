/*
 * initComms.h
 *
 *  Created on: Nov 25, 2022
 *      Author: zongw
 */

#ifndef INIT_INITCOMMS_H_
#define INIT_INITCOMMS_H_

/* DriverLib Includes */
#include "driverlib.h"

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//for all unsigned chars must start and end with a '*' e.g. *hi*
extern int SPEED;
extern int DISTANCE_TRAVELLED;
extern int HEIGHT_OF_HUMP;
extern unsigned char* BARCODE_VALUE;
extern volatile unsigned char* SHORTEST_PATH;
extern unsigned char* MAP_ROW_1;
extern unsigned char* MAP_ROW_2;
extern unsigned char* MAP_ROW_3;
extern unsigned char* MAP_ROW_4;


void uPrintf(unsigned char * TxArray);
//send speed function
int sendSpeed(unsigned int speed);
//send barcode function
int sendBarcode(unsigned char barcode);
//send distance travelled
int sendDistancetravelled(unsigned int distance);
//send height function
int sendHeight(unsigned int height);


void initComms(void);

void itoa_z();


void EUSCIA2_IRQHandler(void);
void PORT1_IRQHandler(void);


void itoa_z(long unsigned int value, char* result, int base);



#endif /* INIT_INITCOMMS_H_ */
