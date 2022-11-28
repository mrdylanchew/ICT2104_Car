/* --COPYRIGHT--,BSD
 * Copyright (c) 2017, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --/COPYRIGHT--*/
/******************************************************************************
 * Debugging log via MSP432 UART
 *
 * Description: This demo prints debugging log via a PC serial port (i.e. COM).
 * SMCLK/DCO is used as clock source and auto-clock enable feature is used by eUSCI.
 * COM used is described as "XDS110 Class Application/User UART".
 * Check your device manager to find out which COM port is used, e.g. COM3
 *
 *               MSP432P401
 *             -----------------
 *            |                 |
 *            |                 |
 *       RST -|     P1.3/UCA0TXD|----> PC
 *            |                 |
 *            |     P1.2/UCA0RXD|<---- PC
 *            |                 |
 *
 *******************************************************************************/
/* DriverLib Includes */
#include "driverlib.h"

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void uPrintf(unsigned char * TxArray);
int sendSpeed(unsigned int speed);
int sendBarcode(unsigned char barcode);
int sendDistancetravelled(unsigned int distance);
int sendHeight(unsigned int height);

void itoa_z();
void sendVariable(unsigned int variable, unsigned char* key);



//![Simple UART Config]
/* UART Configuration Parameter. These are the configuration parameters to
 * make the eUSCI A UART module to operate with a 115200 baud rate and SMCLK at 3 MHz. These
 * values were calculated using the online calculator that TI provides at:
 * http://software-dl.ti.com/msp430/msp430_public_sw/mcu/msp430/MSP430BaudRateConverter/index.html
 */
//

const eUSCI_UART_Config uartConfig =
{
        EUSCI_A_UART_CLOCKSOURCE_SMCLK,                 // SMCLK Clock Source
        1,                                             // BRDIV = 1
        10,                                            // UCxBRF = 10
        0,                                              // UCxBRS = 0
        EUSCI_A_UART_ODD_PARITY,                        // ODD Parity
        EUSCI_A_UART_LSB_FIRST,                         // LSB First
        EUSCI_A_UART_ONE_STOP_BIT,                      // One stop bit
        EUSCI_A_UART_MODE,                              // UART mode
        EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION,  // Oversampling
};

//simulated values that will be displayed in blynk dashboard
int SPEED = 30;
int DISTANCE_TRAVELLED = 10;
int HEIGHT_OF_HUMP = 2;
unsigned char* BARCODE_VALUE = "*A*";
unsigned char* SHORTEST_PATH ="*0->1->5->9->4*" ;
unsigned char* MAP_ROW_1 ="*0->1->2->3->4*" ;
unsigned char* MAP_ROW_2 ="*5->6->7->8->9*" ;
unsigned char* MAP_ROW_3 ="*10->11->12->13->14*" ;
unsigned char* MAP_ROW_4 ="*15->16->17->18->19*" ;


int main(void)
{
    /* Halting WDT  */
    WDT_A_holdTimer();

    /* Set port 1 pin0 as output */
    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);
    GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN0);

    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN2);
    GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN2);

    /* Configuring P1.1 as an input and enabling interrupts */
    MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN1);
    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P1, GPIO_PIN1);
    MAP_GPIO_enableInterrupt(GPIO_PORT_P1, GPIO_PIN1);

    MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN4);
    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P1, GPIO_PIN4);
    MAP_GPIO_enableInterrupt(GPIO_PORT_P1, GPIO_PIN4);
    MAP_Interrupt_enableInterrupt(INT_PORT1);

    // P3.2 (RX) and P3.3 (TX) are UART pins
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P3, GPIO_PIN2 | GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);

    /* Selecting P1.2 and P1.3 in UART mode */
    // GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1, GPIO_PIN2 | GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);

    /* Configuring UART Module */
    UART_initModule(EUSCI_A2_BASE, &uartConfig);

    /* Enable UART module */
    UART_enableModule(EUSCI_A2_BASE);

    /* Enabling interrupts (Rx) */
    UART_enableInterrupt(EUSCI_A2_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT);
    Interrupt_enableInterrupt(INT_EUSCIA2);
    Interrupt_enableMaster();

    while(1)
    {
        PCM_gotoLPM3InterruptSafe();
    }

}

void uPrintf(unsigned char * TxArray)
{
// UART transmission
    unsigned short i = 0;
    while(*(TxArray+i))
    {
        UART_transmitData(EUSCI_A2_BASE, *(TxArray+i));  // Write the character at the location specified by pointer
        i++;                                             // Increment pointer to point to the next character
    }
}

/* EUSCI A2 UART ISR
 * When ISR triggers, reads the value from UART P3.2 RX Line.
 * If value recieved corresponds to a key, exectue the respective case.
 * */
void EUSCIA2_IRQHandler(void)

{
    unsigned char key = 0;
    key = UART_receiveData(EUSCI_A2_BASE);
    switch(key)
    {
        case 'S':
            printf("Calling send speed function\n");
            sendSpeed(SPEED);
            break;
        case 'H':
            printf("Calling send height function\n");
            sendHeight(HEIGHT_OF_HUMP);
            break;
        case 'D':
            printf("Calling send distance function \n");
            sendDistancetravelled(DISTANCE_TRAVELLED);
            break;
        case 'B':
            printf("Calling send barcode function\n");
            uPrintf("B");
            uPrintf(BARCODE_VALUE);
            break;
        case 'I':
            uPrintf("I");
            printf("Calling move car function\n");
            break;
        case 'P':
             uPrintf("P");
             uPrintf(SHORTEST_PATH);
             break;
        case 'M':
            uPrintf("M");
            printf("Calling start mapping\n");
            break;

        case 'V':
           uPrintf("V");
           uPrintf(MAP_ROW_1);
           uPrintf(MAP_ROW_2);
           uPrintf(MAP_ROW_3);
           uPrintf(MAP_ROW_4);
           printf("Sending Map information\n");
           break;


        case 'T':
           uPrintf("T");
           // Send 2 to Blynk
           uPrintf("2");
           break;
        case 'R':
           uPrintf("R");
           // Send 2 to Blynk
           break;

    }
}



//send speed method to send spend from msp to m5
int sendBarcode(unsigned char barcode)
// Pass speed variable here
{
    unsigned char* myString;
    myString = (unsigned char *) malloc(10*sizeof(unsigned char));
    memset(myString, 0, sizeof(myString));
    itoa_z(barcode, myString, 10);
    uPrintf("B"); // tells blynk that MSP sending speed variable
    uPrintf(myString);
    free(myString);
    return 1; // success
}

//send distance travel method to send spend from msp to m5
int sendDistancetravelled(unsigned int distance)
// Pass distance travelled variable here
{
    unsigned char* myString;
    myString = (unsigned char *) malloc(10*sizeof(unsigned char));
    memset(myString, 0, sizeof(myString));
    itoa_z(distance, myString, 10);
    uPrintf("D"); // tells blynk that MSP sending distance variable
    uPrintf(myString);
    free(myString);
    return 1; // success
}

//send speed method from msp to m5
int sendSpeed(unsigned int speed)
// Pass speed variable here
{
    unsigned char* myString;
    myString = (unsigned char *) malloc(10*sizeof(unsigned char));
    memset(myString, 0, sizeof(myString));
    itoa_z(speed, myString, 10);
    uPrintf("S"); // tells blynk that MSP sending speed variable
    uPrintf(myString);
    free(myString);
    return 1; // success
}
//send height method to
int sendHeight(unsigned int height)
{
    unsigned char* myString;
    myString = (unsigned char *) malloc(10*sizeof(unsigned char));
    memset(myString, 0, sizeof(myString));
    itoa_z(height, myString, 10);
    uPrintf("H"); // tells blynk that MSP sending speed variable
    uPrintf(myString);
    free(myString);
    return 1; // success
}


void PORT1_IRQHandler(void)
{
    uint32_t status;
    //enable intterupt status of gpio port p1
    status = MAP_GPIO_getEnabledInterruptStatus(GPIO_PORT_P1);
    //clear intterupt flag
    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P1, status);

    /* Set button P1.1 as testing button, pressing P1.1 will trigger latency test */
        if(status & GPIO_PIN1)
        {
            // T will cause arduino switch case to perform latency test
            uPrintf("T");
            // Send 2 to Blynk
            uPrintf("2");
            //toggle led output on pin 1.0
            GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN0);
        }
   // if interrupted by switch 1.4 do the following
    // if interrupted by switch 1.4 do the following
        if(status & GPIO_PIN4)
        {
            uPrintf("R");
            GPIO_toggleOutputOnPin(GPIO_PORT_P2, GPIO_PIN2);
        }

}

void itoa_z(long unsigned int value, char* result, int base)
  {
    // check that the base if valid
    if (base < 2 || base > 36) { *result = '\0';}

    char* ptr = result, *ptr1 = result, tmp_char;
    int tmp_value;

    do {
      tmp_value = value;
      value /= base;
      *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * base)];
    } while ( value );

    // Apply negative sign
    if (tmp_value < 0) *ptr++ = '-';
    *ptr-- = '\0';
    while(ptr1 < ptr) {
      tmp_char = *ptr;
      *ptr--= *ptr1;
      *ptr1++ = tmp_char;
    }
  }
