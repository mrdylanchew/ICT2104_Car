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


//![Simple UART Config]
/* UART Configuration Parameter. These are the configuration parameters to
 * make the eUSCI A UART module to operate with a 115200 baud rate and SMCLK at 3 MHz. These
 * values were calculated using the online calculator that TI provides at:
 * http://software-dl.ti.com/msp430/msp430_public_sw/mcu/msp430/MSP430BaudRateConverter/index.html
 */
//
#include "initComms.h"
#include "initMapping.h"
#include "initMovement.h"
const eUSCI_UART_Config uartConfig2 =
{
        EUSCI_A_UART_CLOCKSOURCE_SMCLK,                 // SMCLK Clock Source
        6,                                             // BRDIV = 1
        8,                                            // UCxBRF = 10
        32,                                              // UCxBRS = 0
        EUSCI_A_UART_ODD_PARITY,                        // ODD Parity
        EUSCI_A_UART_LSB_FIRST,                         // LSB First
        EUSCI_A_UART_ONE_STOP_BIT,                      // One stop bit
        EUSCI_A_UART_MODE,                              // UART mode
        EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION,  // Oversampling
};

void initComms(void)
    {


    // P3.2 (RX) and P3.3 (TX) are UART pins
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P3, GPIO_PIN2 | GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);

    /* Selecting P1.2 and P1.3 in UART mode */
    // GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1, GPIO_PIN2 | GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);

    /* Configuring UART Module */
    UART_initModule(EUSCI_A2_BASE, &uartConfig2);

    /* Enable UART module */
    UART_enableModule(EUSCI_A2_BASE);

    /* Enabling interrupts (Rx) */
    UART_enableInterrupt(EUSCI_A2_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT);
    Interrupt_enableInterrupt(INT_EUSCIA2);
    Interrupt_enableMaster();
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

{unsigned char key = 0;
key = UART_receiveData(EUSCI_A2_BASE);
switch(key)
{
    case 'S':
        printf("Calling send speed function\n");
        sendSpeed(SPEED);
        break;
    case 'H':
        printf("Calling send height function\n");
        // sendHeight(HEIGHT_OF_HUMP);
        break;
    case 'D':
        printf("Calling send distance function \n");
        // sendDistancetravelled(DISTANCE_TRAVELLED);
        break;
    case 'B':
        printf("Calling send barcode function\n");
        //uPrintf("B");
        //uPrintf(BARCODE_VALUE);
        break;
    case 'I':
        uPrintf("I");
        printf("Calling move car function\n");
        moveCar(2);
        break;
    case 'P':
         uPrintf("P");
         uPrintf(string);
         break;
    case 'M':
        uPrintf("M");
        printf("Calling start mapping\n");
        initMap();
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
    printf("%s\n", myString);
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
