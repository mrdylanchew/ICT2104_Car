/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "pico/binary_info.h"

/* Code to communicate with M5StickC-plus via I2C

   NOTE: The panel must be capable of being driven at 3.3v NOT 5v. The Pico
   GPIO (and therefor I2C) cannot be used at 5v.

   You will need to use a level shifter on the I2C lines if you want to run the
   board at 5v.

   Connections on Raspberry Pi Pico board, other boards may vary.

   GPIO 6 -> SDA on LCD bridge board
   GPIO 7 -> SCL on LCD bridge board
   3.3v (pin 36) -> Connect to 3.3v pin on M5 Grove connector
   GND (pin 38)  -> connect to GND on M5 Grove connector
*/
// commands
// I2C communications will be on bus address 48
static int addr = 48;
uint8_t rec= 0;
//Demo values for test
uint8_t barcodeFirstChar='*'; 
uint8_t barcodeSecondChar='C';
uint8_t barcodeThirdChar='*';
uint8_t height=151;

/* Quick helper function for single byte writing */
void i2c_write_byte(uint8_t val) {
#ifdef i2c_default
    i2c_write_blocking(i2c1, addr, &val, 1, false);
#endif
}

/* Quick helper function for single byte reading */
void i2c_read_byte() {
    i2c_read_blocking(i2c1, 48, &rec, 1, false);
    printf("%c",rec);
}

// Function for latency test 2, if 'T' received from M5Stick, writes 'T' back to M5Stick
void latencyTest2(){
    i2c_read_blocking(i2c1, 48, &rec, 1, false);
    if(rec == 84){
        i2c_write_byte('T');
        printf("T\n");
    }
}
//Function to send barcode values, Sends 'B' for M5 to identify message as Barcode values, then sends barcode values
void sendBarcodeVal(){
    i2c_write_byte('B');
    i2c_write_byte(barcodeFirstChar);
    i2c_write_byte(barcodeSecondChar);
    i2c_write_byte(barcodeThirdChar);
}
//Function so send height, sends 'H' for M5 to identify message as height, then sends actual height
void sendHeight(int finalHeight){
    i2c_write_byte('H');
    i2c_write_byte(finalHeight);
}
//Function to check what input is received from M5
void checkFlag(){
    i2c_read_blocking(i2c1, 48, &rec, 1, false);
    if(rec == 84){
        i2c_write_byte('T');
    }else if(rec == 66){
        printf("Barcode values requested\n");
        sendBarcodeVal();
        printf("Sending Barcode Values\n");
    }else if(rec == 72){
        printf("Height values requested\n");
        sendHeight(height);
        printf("Sending height values\n");
    }
}

//Writes 'T' to I2C address 48
void latencyTest(){
    i2c_write_byte('T');
}

int main() {
    stdio_init_all();
#if !defined(i2c_default) || !defined(PICO_DEFAULT_I2C_SDA_PIN) || !defined(PICO_DEFAULT_I2C_SCL_PIN)
    #warning i2c/lcd_1602_i2c example requires a board with I2C pins
#else
    // This example will use I2C0 on the default SDA and SCL pins (4, 5 on a Pico) and I2C1 on GPIO 6 and 7
    i2c_init(i2c1, 100 * 1000); //Initialise at 100khz
    gpio_set_function(6, GPIO_FUNC_I2C);
    gpio_set_function(7, GPIO_FUNC_I2C);
    gpio_pull_up(6);
    gpio_pull_up(7);
    i2c_init(i2c_default, 400 * 1000); //initialise i2c default at 400khz for accelerometer 
    gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
    gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);
    // Make the I2C pins available to picotool
    bi_decl(bi_2pins_with_func(6, 7, GPIO_FUNC_I2C));
    bi_decl(bi_2pins_with_func(PICO_DEFAULT_I2C_SDA_PIN, PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C));

    while (1) {
            checkFlag();
            sleep_ms(500);
        }
        return 0;
        #endif
    }

    