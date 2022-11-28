/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>

#include "pico/stdlib.h"
#include "pico/stdio.h"
#include "pico/binary_info.h"
#include "hardware/i2c.h"
#include <math.h>

#include "hardware/adc.h"
#include "hardware/gpio.h"

#define BAUD_RATE 115200

#define UART_TX_PIN 0
#define UART_RX_PIN 1

#define ADC_PIN 26
#define DIGITAL_PIN 22

#define BLACK_THRESHOLD 3000

#define WHITE_THRESHOLD 400

#define BARCODE_BUF_SIZE 10

#define BARCODE_ARR_SIZE 9

#define ADC_DIFFERENCE_THRESHHOLD 50

#define SAMPLE_SIZE 10000


// By default these devices  are on bus address 0x68
static int addr = 0x68;
static int m5addr = 48;
uint8_t rec= 0;
uint8_t barcodeFirstChar=0;
uint8_t barcodeSecondChar=0;
uint8_t barcodeThirdChar=0;
int previousHeight = 0;
#ifdef i2c_default

static void sendBarcodeVal();
static void sendHeight(int height);
static void i2c_write_byte(uint8_t byteVal);
static void i2c_read_byte();
static void mpu6050_reset() {
    // Two byte reset. First byte register, second byte data
    // There are a load more options to set up the device in different ways that could be added here
    uint8_t buf[] = {0x6B, 0x00};
    i2c_write_blocking(i2c_default, addr, buf, 2, false);
}

static void mpu6050_read_raw(int16_t accel[3], int16_t gyro[3], int16_t *temp) {
    // For this particular device, we send the device the register we want to read
    // first, then subsequently read from the device. The register is auto incrementing
    // so we don't need to keep sending the register we want, just the first.

    uint8_t buffer[6];

    // Start reading acceleration registers from register 0x3B for 6 bytes
    uint8_t val = 0x3B;
    i2c_write_blocking(i2c_default, addr, &val, 1, true); // true to keep master control of bus
    i2c_read_blocking(i2c_default, addr, buffer, 6, false);

    for (int i = 0; i < 3; i++) {
        accel[i] = (buffer[i * 2] << 8 | buffer[(i * 2) + 1]);
    }

    // Now gyro data from reg 0x43 for 6 bytes
    // The register is auto incrementing on each read
    val = 0x43;
    i2c_write_blocking(i2c_default, addr, &val, 1, true);
    i2c_read_blocking(i2c_default, addr, buffer, 6, false);  // False - finished with bus

    for (int i = 0; i < 3; i++) {
        gyro[i] = (buffer[i * 2] << 8 | buffer[(i * 2) + 1]);;
    }

    // Now temperature from reg 0x41 for 2 bytes
    // The register is auto incrementing on each read
    val = 0x41;
    i2c_write_blocking(i2c_default, addr, &val, 1, true);
    i2c_read_blocking(i2c_default, addr, buffer, 2, false);  // False - finished with bus

    *temp = buffer[0] << 8 | buffer[1];
}
#endif

enum bartype{
    THICK_BLACK, // 0
    THIN_BLACK, // 1
    THICK_WHITE, // 2
    THIN_WHITE // 3
};

//code 39 format of letter A to Z using enum bartype



//code 39 format of letter F using enum bartype
static char* A_ARRAY_MAP = "031312130";
static char* B_ARRAY_MAP = "130312130";
static char* C_ARRAY_MAP = "030312131";
static char* D_ARRAY_MAP = "131302130";
static char* E_ARRAY_MAP = "031302131";
static char* F_ARRAY_MAP = "130302131";
static char* G_ARRAY_MAP = "131312030";
static char* H_ARRAY_MAP = "031312031";
static char* I_ARRAY_MAP = "130312031";
static char* J_ARRAY_MAP = "131302031";
static char* K_ARRAY_MAP = "031313120";
static char* L_ARRAY_MAP = "130313120";
static char* M_ARRAY_MAP = "030313121";
static char* N_ARRAY_MAP = "131303120";
static char* O_ARRAY_MAP = "031303121";
static char* P_ARRAY_MAP = "130303121";
static char* Q_ARRAY_MAP = "131313020";
static char* R_ARRAY_MAP = "031313021";
static char* S_ARRAY_MAP = "130313021";
static char* T_ARRAY_MAP = "131303021";
static char* U_ARRAY_MAP = "021313130";
static char* V_ARRAY_MAP = "120313130";
static char* W_ARRAY_MAP = "020313131";
static char* X_ARRAY_MAP = "121303130";
static char* Y_ARRAY_MAP = "021303131";
static char* Z_ARRAY_MAP = "120303131";

static int waitingForStart = 1;


//code 39 format of asterisk using enum bartype
static char* ASTERISK_ARRAY_MAP = "121303031";


static int barcodeArr[BARCODE_ARR_SIZE];

static uint32_t res = 0;
static uint16_t prevAvg = 0;
static uint16_t max = 0;
static uint16_t min = 3000;

static int i = 0 ;
static int barcode_arr_index = 1;

char* outputBuffer;

static absolute_time_t blockStart;
static absolute_time_t blockEnd;


struct voltageClassification {
    uint16_t voltage;
    // 0 - white
    // 1 - black
    int blackWhite;
    absolute_time_t blockStart;
    int64_t  blockLength;
    enum bartype type;
} voltageClassification;

// queue for voltageclassifications of length 9
static struct voltageClassification voltageClassifications[BARCODE_BUF_SIZE];

// queue for barcode read of length 3
static char barcodeRead[3];

//function to append to barcodeRead Queue
static void appendToBarcodeRead(char barcodeChar){
    barcodeRead[0] = barcodeRead[1];
    barcodeRead[1] = barcodeRead[2];
    barcodeRead[2] = barcodeChar;
}

static int isValidBarcode(){
    if(barcodeRead[0] == '*' && barcodeRead[2] == '*'){
        if(barcodeRead[1] != 0)
            return 1;
    }

    return 0;
}

static int isBarcodeFull(){
    if(barcodeRead[0] != 0 && barcodeRead[1] != 0 && barcodeRead[2] != 0){
        return 1;
    }

    return 0;
}

static void clearBarcodeRead(){
    barcodeRead[0] = 0;
    barcodeRead[1] = 0;
    barcodeRead[2] = 0;
}

//function to convert array of integer to string
static char *intArrayToString(int *arr, int size){
    char *str = malloc(size + 1);
    for(int i = 0; i < size; i++){
        str[i] = arr[i] + '0';
    }
    str[size] = '\0';
    return str;
}


static int* thickThinClassification(){
    //calculate average block length
    int64_t totalBarLength = 0;
    for(int i = 0; i< BARCODE_ARR_SIZE; i++){
        totalBarLength += voltageClassifications[i].blockLength;
    }

    int *barsRead = malloc(BARCODE_ARR_SIZE * sizeof(int));

    int64_t avgBarLength = (totalBarLength/BARCODE_ARR_SIZE);
    //assign thick thin classification
    for(int i = 0; i< BARCODE_ARR_SIZE; i++){
        //printf("%"PRId64", ",voltageClassifications[i].blockLength);

        if(voltageClassifications[i].blackWhite){
            if(voltageClassifications[i].blockLength < avgBarLength){
                voltageClassifications[i].type = THIN_BLACK;
            }else{
                voltageClassifications[i].type = THICK_BLACK;
            }
        }else{
            if(voltageClassifications[i].blockLength < avgBarLength){
                voltageClassifications[i].type = THIN_WHITE;
            }else{
                voltageClassifications[i].type = THICK_WHITE;
            }
        }
        barsRead[i] = voltageClassifications[i].type;
    }
    //printf("\n\r");
    return barsRead;
}
// function to check if queue is full
static int isVoltageClassificationFull(){
    for(int i = 0; i < BARCODE_BUF_SIZE; i++){
        if(voltageClassifications[i].blackWhite == -1){
            return 0;
        }
    }
    return 1;
}

// function to flush queue 
static void flushVoltageClassification(){
    barcode_arr_index = 1;
    blockStart = get_absolute_time();

    struct voltageClassification lastReading = voltageClassifications[BARCODE_BUF_SIZE - 1];

    for(int i = 0; i < BARCODE_BUF_SIZE; i++){
        voltageClassifications[i].voltage = 0;
        voltageClassifications[i].blackWhite = -1;
        voltageClassifications[i].blockLength = 0;
        voltageClassifications[i].type = 0;
    }

    voltageClassifications[0] = lastReading;
}   

//function to compare buffer and the barcodes
static char compareTwoArray () {
    int* barsRead = thickThinClassification();

    if(voltageClassifications[0].blackWhite == 0){
        //printf("Start with white\n\r");
        return 0;
    }

    char* string = intArrayToString(barsRead, BARCODE_ARR_SIZE);
    free(barsRead);

    char* barcodes[] = {
        A_ARRAY_MAP,
        B_ARRAY_MAP,
        C_ARRAY_MAP,
        D_ARRAY_MAP,
        E_ARRAY_MAP,
        F_ARRAY_MAP,
        G_ARRAY_MAP,
        H_ARRAY_MAP,
        I_ARRAY_MAP,
        J_ARRAY_MAP,
        K_ARRAY_MAP,
        L_ARRAY_MAP,
        M_ARRAY_MAP,
        N_ARRAY_MAP,
        O_ARRAY_MAP,
        P_ARRAY_MAP,
        Q_ARRAY_MAP,
        R_ARRAY_MAP,
        S_ARRAY_MAP,
        T_ARRAY_MAP,
        U_ARRAY_MAP,
        V_ARRAY_MAP,
        W_ARRAY_MAP,
        X_ARRAY_MAP,
        Y_ARRAY_MAP,
        Z_ARRAY_MAP,
        ASTERISK_ARRAY_MAP   
    };

    
    char characters[] = {
        'A',
        'B',
        'C',
        'D',
        'E',
        'F',
        'G',
        'H',
        'I',
        'J',
        'K',
        'L',
        'M',
        'N',
        'O',
        'P',
        'Q',
        'R',
        'S',
        'T',
        'U',
        'V',
        'W',
        'X',
        'Y',
        'Z',
        '*'
    };
     

    
    for(int i = 0; i < 27; i++){
        if(strncmp(barcodes[i], string, BARCODE_ARR_SIZE) == 0){
            free(string);
            flushVoltageClassification();
            return characters[i];
        }
    }

    return 0;
    //return 1;
}


// function to append queue
static void appendVoltageClassification(struct voltageClassification voltageClassification){
    voltageClassifications[0] = voltageClassifications[1];
    voltageClassifications[1] = voltageClassifications[2];
    voltageClassifications[2] = voltageClassifications[3];
    voltageClassifications[3] = voltageClassifications[4];
    voltageClassifications[4] = voltageClassifications[5];
    voltageClassifications[5] = voltageClassifications[6];
    voltageClassifications[6] = voltageClassifications[7];
    voltageClassifications[7] = voltageClassifications[8];
    voltageClassifications[8] = voltageClassifications[9];
    voltageClassifications[9] = voltageClassification;
    if(barcode_arr_index == BARCODE_BUF_SIZE){
        char read = compareTwoArray();
        if(read != 0){
            printf("%c\0", read);

            
            appendToBarcodeRead(read);
            
        }
    }
}

static void ADC_IRQ_FIFO_HANDLER() {
    // read data from ADC FIFO
    if(!adc_fifo_is_empty()){
        uint16_t data = adc_fifo_get();
        res += data;
        if(i < 100){
            i++;
        }else{
            //uint16_t avg = res/(i);
            uint16_t avg = res/ (i);

            //printf("%d\0", gpio_get(DIGITAL_PIN));
            //printf("%"PRIu16"", data);
            //printf(",%"PRIu16"", avg);
            if(prevAvg == 0){
                prevAvg = avg;
            }else{
                if(abs(prevAvg - avg) > ADC_DIFFERENCE_THRESHHOLD){
                    prevAvg = avg;
                }else{
                    avg = prevAvg;
                }
            }
            //printf(",%"PRIu16"\n\r", avg);
            i = 0;
            res = 0;

            struct voltageClassification voltageClassification;
            voltageClassification.voltage = avg;
            
            if(avg > BLACK_THRESHOLD || gpio_get(DIGITAL_PIN) == 1){
                voltageClassification.blackWhite = 1;
            }else{
                voltageClassification.blackWhite = 0;
            }

            if(barcode_arr_index == BARCODE_BUF_SIZE){
            
                // if((abs(voltageClassifications[BARCODE_ARR_SIZE - 1].voltage - curr)/ADC_DIFFERENCE_THRESHHOLD * 100 < 10)){
                //     return;
                // }
                if(voltageClassifications[BARCODE_BUF_SIZE - 1].blackWhite != voltageClassification.blackWhite){
                    blockEnd = get_absolute_time();
                    voltageClassification.blockStart = blockEnd;
                    //printf("white block length: %"PRIu64", %"PRIu64"\n\r", blockEnd,blockStart);
                    int64_t blockLength = absolute_time_diff_us(voltageClassifications[BARCODE_BUF_SIZE - 1].blockStart, blockEnd);
                    
                    voltageClassifications[BARCODE_BUF_SIZE - 1].blockLength =  blockLength / 10000;
                    appendVoltageClassification(voltageClassification);
                }
            }else{
                if(voltageClassifications[barcode_arr_index-1].blackWhite != voltageClassification.blackWhite){
                    blockEnd = get_absolute_time();
                    voltageClassification.blockStart = blockEnd;
                    //printf("white block length: %"PRIu64", %"PRIu64"\n\r", blockEnd,blockStart);
                    if(barcode_arr_index == 0){
                        int64_t blockLength = absolute_time_diff_us(blockStart,blockEnd);
                        voltageClassification.blockLength =  blockLength / 10000;
                    }else{
                        int64_t blockLength = absolute_time_diff_us(voltageClassifications[barcode_arr_index-1].blockStart, blockEnd);
                        voltageClassifications[barcode_arr_index - 1].blockLength =  blockLength / 10000;
                    }
                    
                    voltageClassifications[barcode_arr_index] = voltageClassification;
                    barcode_arr_index++;
                }
            }
        }
    }
    irq_clear(ADC_IRQ_FIFO);
}   
/* Quick helper function for single byte transfers */
void i2c_write_byte(uint8_t byteVal) {
    i2c_write_blocking(i2c1, m5addr, &byteVal, 1, false);
}
/* Quick helper function for single byte transfers */
void i2c_read_byte() {
    i2c_read_blocking(i2c1, m5addr, &rec, 1, false);
}
//call this when all chars have been set
void sendBarcodeVal(){
    i2c_write_byte('B');
    i2c_write_byte(barcodeFirstChar);
    i2c_write_byte(barcodeSecondChar);
    i2c_write_byte(barcodeThirdChar);
}
void sendHeight(int height){
    i2c_write_byte('H');
    i2c_write_byte(height);
}
int main() {

    stdio_init_all();

    outputBuffer = malloc(3 * sizeof(char));

    #if !defined(i2c_default) || !defined(PICO_DEFAULT_I2C_SDA_PIN) || !defined(PICO_DEFAULT_I2C_SCL_PIN)
    #warning i2c/mpu6050_i2c example requires a board with I2C pins
    puts("Default I2C pins were not defined");
    #else
    printf("Hello, MPU6050! Reading raw data from registers...\n");
    #endif

    // This example will use I2C0 on the default SDA and SCL pins (4, 5 on a Pico)
    i2c_init(i2c1, 100 * 1000);
    gpio_set_function(6, GPIO_FUNC_I2C);
    gpio_set_function(7, GPIO_FUNC_I2C);
    gpio_pull_up(6);
    gpio_pull_up(7);
    i2c_init(i2c_default, 400 * 1000);
    gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN); // pull up the pins
    gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);
    
    // Make the I2C pins available to picotool
    bi_decl(bi_2pins_with_func(6, 7, GPIO_FUNC_I2C));
    bi_decl(bi_2pins_with_func(PICO_DEFAULT_I2C_SDA_PIN, PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C));

    gpio_init(DIGITAL_PIN);
    gpio_set_input_enabled(DIGITAL_PIN, true);
    gpio_pull_up(DIGITAL_PIN);


    mpu6050_reset();

    int16_t acceleration[3], gyro[3], temp;
    double pitchR;
    double height;


    adc_init();

    // init the queue
    flushVoltageClassification();

    adc_gpio_init(ADC_PIN);

    adc_select_input(0);


    adc_fifo_setup(true, false, 1, false, false);
    adc_set_clkdiv(0);
    adc_irq_set_enabled(true);

    irq_clear(ADC_IRQ_FIFO);
    irq_set_exclusive_handler(ADC_IRQ_FIFO, ADC_IRQ_FIFO_HANDLER);
    irq_set_enabled(ADC_IRQ_FIFO, true);
    
    adc_run(true);

    sleep_ms(2000);

    blockStart = get_absolute_time();

    while (true) {
        if(isValidBarcode()){
                printf("Valid Barcode\n\r");
                barcodeFirstChar = barcodeRead[0];
                barcodeSecondChar = barcodeRead[1];
                barcodeThirdChar = barcodeRead[2];
                printf("Barcode: %c%c%c\n\r", barcodeFirstChar, barcodeSecondChar, barcodeThirdChar);
                sendBarcodeVal();
                clearBarcodeRead();
                barcodeFirstChar = 0;
                barcodeSecondChar = 0;
                barcodeThirdChar = 0;
            }
        mpu6050_read_raw(acceleration, gyro, &temp);

        int AccX = acceleration[0];
        int AccY = acceleration[1];
        int AccZ = acceleration[2];
	    int	pitch = (180 * atan (AccX/sqrt(AccY*AccY + AccZ*AccZ))/M_PI)-6;
	    int roll = 180 * atan (AccY/sqrt(AccX*AccX + AccZ*AccZ))/M_PI;

        //printf("Pitch = %d deg\n", pitch);
        //printf("Roll = %d\n\n", roll);
        if (pitch < 0)
        {
            //printf("Height = 0cm \n\n");
        }
        else{
            pitchR = pitch * (M_PI / 180); // pitch in RAD because math lib sin uses RAD lol
            height = sin(pitchR) * 18.5; // mounted distance from front to sensor
            int finalHeight =(int) height*10;
            if (previousHeight != finalHeight){
                printf("Height of hump= %.2f cm\n\n", height);
                previousHeight = finalHeight;
                sendHeight(finalHeight);
            }
        }

        //sleep_ms(500); // updates every 500ms 
    }
}
