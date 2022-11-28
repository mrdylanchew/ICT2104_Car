#define BLYNK_TEMPLATE_ID           "TMPLsBGEMBbU"
#define BLYNK_DEVICE_NAME           "Quickstart Device"
#define BLYNK_AUTH_TOKEN            "E7tIbqs6zs709_BEux7kMHRCNAOpk7JU"
#define DEFAULT_FLAG                'Z' // z is the default flag 

// Maps mobile buttons to their corresponding virtual pins
#define SPEED_MOBILE_BUTTON          16
#define HUMP_HEIGHT_MOBILE_BUTTON    17
#define DISTANCE_TRAVELLED_BUTTON    18
#define BARCODE_BUTTON               19
#define START_VEHICLE_BUTTON         20
#define START_MAPPING_BUTTON         21

// Maps web switches to their corresponding virtual pins
#define SPEED_WEB_SWITCH             10
#define HUMP_HEIGHT_WEB_SWITCH       11
#define DISTANCE_TRAVELLED_SWITCH    12
#define BARCODE_SWITCH               13
#define START_VEHICLE_SWITCH         14
#define START_MAPPING_SWITCH         15


#define BARCODE_PIN                  8
#define TIME_TESTING_PIN             9

// Macros for virtual pins that serve as display in Blynk.
#define SPEED_DISPLAY                1
#define HEIGHT_OF_HUMP_DISPLAY       2
#define DISTANCE_TRAVELLED_DISPLAY   3
#define BARCODE_DISPLAY              4
#define VEHICLE_STATE_DISPLAY        5

// Keys used to get MSP to send variables
#define SPEED_FLAG                  'S'
#define HEIGHT_OF_HUMP_FLAG         'H'
#define DISTANCE_FLAG               'D'
#define BARCODE_FLAG                'B'
#define VEHICLE_STATE_DISPLAY_FLAG  'I'
#define START_MAPPING_FLAG          'M'


// Comment this out to disable prints and save space
#define BLYNK_PRINT Serial


#include <M5StickCPlus.h>
#include <Wire.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <time.h>

char auth[] = BLYNK_AUTH_TOKEN;
// Replace with your own wifi credentials
char ssid[] = "Micow";
char pass[] = "mikhailwee";
BlynkTimer timer;
unsigned long  t1;
unsigned long t2;
int button_state=0; //Get status of latency test button
int button_state_height=0; //Get status of height button
int button_state_barcode=0; //Get status of barcode button
int sent=1; //Bool to ensure latency test is conducted once
bool readHeight = false; //bool to ensure height is only received once
bool readBarcode1 = false; //bool to read first barcode character
bool readBarcode2 = false; //bool to read second barcode character
bool readBarcode3 = false; //bool to read last barcode character
String content = ""; //global variable to hold entire barcode string

//Web blynk functions
BLYNK_WRITE(HUMP_HEIGHT_WEB_SWITCH) 
{
  // Set incoming value from pin V11 to a variable
  button_state_height = param.asInt();
  Serial.println("Height Button Pressed");
}
  BLYNK_WRITE(BARCODE_SWITCH) 
{
  // Set incoming value from pin V13 to a variable
  button_state_barcode = param.asInt();
  Serial.println("Barcode Button Pressed");
}

// Mobile blynk function 
BLYNK_WRITE(HUMP_HEIGHT_MOBILE_BUTTON) 
{
  // Set incoming value from pin V17 to a variable
  button_state_height = param.asInt();
  Serial.println("Height Button Mobile Pressed");
}
BLYNK_WRITE(BARCODE_BUTTON) 
{
  // Set incoming value from pin V19 to a variable
  button_state_barcode = param.asInt();
  Serial.println("Height Button Mobile Pressed");
}

//Handler to send data to Pico on request
void sendToPico(){
  if (button_state==1 && sent==1){
    t1 = micros(); //Starting value for latency calculation
    Wire.write('T'); //Write T to Pico
    sent++; //Change boolean value
  }
  if(button_state_height==1){
    Wire.write('H');
    button_state_height++;//m5 will only write to buffer once
    Serial.println("Requesting height from PICO");
  }
  if(button_state_barcode==1){
    Wire.write('B');
    button_state_barcode++;
    Serial.println("Requesting barcode from PICO");
  }
}
//Read value of V23 PIN of blynk
BLYNK_WRITE(V23) {
  button_state = param.asInt(); //Read value of pin 23, latency testing for Pico
  if (button_state==1){
    sent = 1; //set bool to send value
  }
}



void setup() {
  M5.begin(true, true, true);
  M5.Lcd.setRotation(3);
  M5.Lcd.setTextSize(2);
  M5.Lcd.setTextColor(ORANGE);
  M5.Lcd.setCursor(0, 0);
  M5.Lcd.println("M5Stick Started");
  Wire.begin(48); //Connect to I2C bus as #48
  Wire.onRequest(sendToPico); //Function to send data to pico
  Wire.onReceive(getFromPico); //Function to receive data from pico
  Blynk.begin(auth, ssid, pass);
  M5.Lcd.println("Connected to BLYNK");
}
int reading;
void loop() {
  Blynk.run();
}
//Get data from pico
  void getFromPico(int numOfBytes){
  int numBytes = 1; //number of bytes to send for latency testing
  int reading = 0; //to hold input from Wire.read
  int i = 0; //int for latency test looping
  while (Wire.available()){ //check if input received from pico
      reading = Wire.read(); //Read data from pico
      if((char)reading == 'H'){ //if received 'H' from pico, change bool to read height next
        readHeight = true;
      }
      //read last barcode char
      if(readBarcode3 == true){
        content.concat((char)reading);
        readBarcode3 = false;
        M5.Lcd.print("Barcode values received are ");
        M5.Lcd.println(content);
        Blynk.virtualWrite(V4,content);
        content = "";
      }
      //read second barcode char
      if(readBarcode2 == true){
        content.concat((char)reading);
        readBarcode2 = false;
        readBarcode3 = true;
      }
      //read first barcode char
      if(readBarcode1 == true){
        content.concat((char)reading);
        readBarcode1 = false;
        readBarcode2 = true;
      }
      if((char)reading == 'B'){ //if received 'B' from pico, change bool to read barcode values
        readBarcode1 = true;
      }
      //reading final height
      if(readHeight == true && (char)reading!='H'){
        float finalHeight = (float)reading/10; //converting height to cm with accuracy of 1 decimal place
        Blynk.virtualWrite(V2,finalHeight); 
        M5.Lcd.print("Height received is ");
        M5.Lcd.print(finalHeight);
        M5.Lcd.println("cm");
        Serial.println(finalHeight);
        readHeight = false;
      }
      if((char)reading == 'T'){ //perform latency test
        while(i<numBytes){
          Blynk.virtualWrite(V0,2); 
        }
        i=0;
        float x = 0;
        t2 = micros(); //End value for latency calculation
        x = t2-t1;
        float y = (float)x/1000;
        Serial.print("(Blynk initiated PICO) Time taken to send ");
        Serial.print(numBytes);
        Serial.print(" bytes: ");
        Serial.print(y,3);
        Serial.println("ms");
      }
      //reset reading
      reading = 'o';
  }
}

BLYNK_CONNECTED() 
{
  Blynk.syncAll();
  /* Change Web Link Button message to "Congratulations!"
  Blynk.setProperty(V3, "offImageUrl", "https://static-image.nyc3.cdn.digitaloceanspaces.com/general/fte/congratulations.png");
  Blynk.setProperty(V3, "onImageUrl",  "https://static-image.nyc3.cdn.digitaloceanspaces.com/general/fte/congratulations_pressed.png");
  Blynk.setProperty(V3, "url", "https://docs.google.com/presentation/d/1LgncG3i73C4eHuLK9uTsUaLBEqVJAenmGCjQGmu5V2U/edit#slide=id.p");
*/
}
