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
  #define VIEW_MAP_SWITCH              33

  #define LATENCY_SWITCH               22
  #define SHORTEST_PATH_SWITCH         24

  #define BARCODE_PIN                  8
  #define TIME_TESTING_PIN             9

  // Macros for virtual pins that serve as display in Blynk.
  #define SPEED_DISPLAY                1
  #define HEIGHT_OF_HUMP_DISPLAY       2
  #define DISTANCE_TRAVELLED_DISPLAY   3
  #define BARCODE_DISPLAY              4
  #define VEHICLE_STATE_DISPLAY        5
  #define SHORTEST_PATH_DISPLAY        6
  #define MAP_ROW_1                    29
  #define MAP_ROW_2                    30
  #define MAP_ROW_3                    31
  #define MAP_ROW_4                    32



  // Keys used to get MSP to send variables
  #define SPEED_FLAG                  'S'
  #define HEIGHT_OF_HUMP_FLAG         'H'
  #define DISTANCE_FLAG               'D'
  #define BARCODE_FLAG                'B'
  #define VEHICLE_STATE_DISPLAY_FLAG  'I'
  #define START_MAPPING_FLAG          'M'
  #define LATENCY_TEST                'T'
  #define SHORTEST_PATH_FLAG          'P'
  #define VIEW_MAP_FLAG               'V'


  // Comment this out to disable prints and save space
  #define BLYNK_PRINT Serial

  #define RESET_FLAG 'R'
  #define RESET_SWITCH 27
  



  #include <WiFi.h>
  #include <WiFiClient.h>
  #include <BlynkSimpleEsp32.h>
  #include <M5StickCPlus.h>
  #include <time.h>
  //auth token
  char auth[] = BLYNK_AUTH_TOKEN;
  // Replace with your own wifi credentials
  char ssid[] = "YP";
  char pass[] = "12345678";
  BlynkTimer timer;
  //declare timer variables
  unsigned long  t1;
  unsigned long t2;
  // long string variable
  
  void display_barcodeinfo(); 
  void myTimerEvent();
  char getFlag();
  void pin_map_function();
  void send_key_msp(char key_flag, int button_state);
  String read_long_string();
  //Web blynk functions

//on switch press in blynk dash board , send data to msp432
  BLYNK_WRITE(SPEED_WEB_SWITCH) 
  {
    // Set incoming value from pin V10 to a variable
    int button_state = param.asInt();//button_state = 1 if pressed 0 if not pressed
    send_key_msp(SPEED_FLAG, button_state);// pass in speed flag V10 , button_state = 1 to send_key_msp method
    Serial.print(button_state);// print in arduino serial console button state 
  }


//on switch press in blynk dash board , send data to msp432
  BLYNK_WRITE(HUMP_HEIGHT_WEB_SWITCH) 
  {
    // Set incoming value from pin V11 to a variable
    int button_state = param.asInt();//button_state = 1 if pressed 0 if not pressed
    send_key_msp(HEIGHT_OF_HUMP_FLAG, button_state); //pass in height of hump flag V11 , button_state = 1 to send_key_msp method
   
  }
  //on switch press in blynk dash board , send data to msp432
  BLYNK_WRITE(DISTANCE_TRAVELLED_SWITCH) 
  {
    // Set incoming value from pin V12 to a variable
    int button_state = param.asInt();//button_state = 1 if pressed 0 if not pressed
    send_key_msp(DISTANCE_FLAG, button_state);//pass in distance flag V12 , button_state = 1 to send_key_msp method
  
  }
  //on switch press in blynk dash board , send data to msp432
    BLYNK_WRITE(BARCODE_SWITCH) 
  {
    // Set incoming value from pin V13 to a variable
    int button_state = param.asInt();//button_state = 1 if pressed 0 if not pressed
    send_key_msp(BARCODE_FLAG, button_state);//pass in barcode flag V13 , button_state = 1 to send_key_msp method
   
  }
     //on switch press in blynk dash board , send data to msp432
      BLYNK_WRITE(START_VEHICLE_SWITCH) 
  {
    // Set incoming value from pin V14 to a variable
    int button_state = param.asInt();//button_state = 1 if pressed 0 if not pressed
    send_key_msp(VEHICLE_STATE_DISPLAY_FLAG, button_state);//pass in distance flag V14 , button_state = 1 to send_key_msp method
    
  }
  //on switch press in blynk dash board , send data to msp432
    BLYNK_WRITE(START_MAPPING_SWITCH ) 
  {
    // Set incoming value from pin V15 to a variable
    int button_state = param.asInt();//button_state = 1 if pressed 0 if not pressed
    send_key_msp(START_MAPPING_FLAG, button_state);//pass in distance flag V15 , button_state = 1 to send_key_msp method
   
  }

  BLYNK_WRITE(SHORTEST_PATH_SWITCH) 
  {
    // Set incoming value from pin V11 to a variable
    int button_state = param.asInt();//button_state = 1 if pressed 0 if not pressed
    send_key_msp(SHORTEST_PATH_FLAG, button_state); //pass in height of hump flag V11 , button_state = 1 to send_key_msp method
   
  }



   //on switch press in blynk dash board , send data to msp432
      BLYNK_WRITE(VIEW_MAP_SWITCH) 
  {
    // Set incoming value from pin V14 to a variable
    int button_state = param.asInt();//button_state = 1 if pressed 0 if not pressed
    send_key_msp(VIEW_MAP_FLAG, button_state);//pass in distance flag V14 , button_state = 1 to send_key_msp method
    
  }







  // Mobile blynk function 
  //on switch press in blynk dash board(mobile) , send data to msp432
  BLYNK_WRITE(SPEED_MOBILE_BUTTON) 
  {

    // Set incoming value from pin V16 to a variable
    int button_state = param.asInt();//button_state = 1 if pressed 0 if not pressed
    send_key_msp(SPEED_FLAG, button_state);//pass in distance flag V16 , button_state = 1 to send_key_msp method
    
  }
   //on switch press in blynk dash board(mobile) , send data to msp432
  BLYNK_WRITE(HUMP_HEIGHT_MOBILE_BUTTON) 
  {
    // Set incoming value from pin V17 to a variable
    int button_state = param.asInt();//button_state = 1 if pressed 0 if not pressed
    send_key_msp(HEIGHT_OF_HUMP_FLAG, button_state);//pass in distance flag V17 , button_state = 1 to send_key_msp method
  }
 //on switch press in blynk dash board(mobile) , send data to msp432
  BLYNK_WRITE(DISTANCE_TRAVELLED_BUTTON) 
  {
    // Set incoming value from pin V18 to a variable
    int button_state = param.asInt();//button_state = 1 if pressed 0 if not pressed
    send_key_msp(DISTANCE_FLAG, button_state);//pass in distance flag V18 , button_state = 1 to send_key_msp method
    
  }

   //on switch press in blynk dash board(mobile) , send data to msp432
  BLYNK_WRITE(BARCODE_BUTTON) 
  {
    // Set incoming value from pin V19 to a variable
    int button_state = param.asInt();//button_state = 1 if pressed 0 if not pressed
    send_key_msp(BARCODE_FLAG , button_state);//pass in distance flag V19 , button_state = 1 to send_key_msp method

  }
   //on switch press in blynk dash board(mobile) , send data to msp432
  BLYNK_WRITE(START_VEHICLE_BUTTON) 
  {
    // Set incoming value from pin V19 to a variable
    int button_state = param.asInt();//button_state = 1 if pressed 0 if not pressed

    send_key_msp(VEHICLE_STATE_DISPLAY_FLAG, button_state);//pass in distance flag V19 , button_state = 1 to send_key_msp method
 
  }
   //on switch press in blynk dash board(mobile) , send data to msp432
  BLYNK_WRITE(START_MAPPING_BUTTON) 
  {
    // Set incoming value from pin V10 to a variable
    int button_state = param.asInt();//button_state = 1 if pressed 0 if not pressed
    send_key_msp(START_MAPPING_FLAG, button_state);//pass in distance flag V19, button_state = 1 to send_key_msp method
   
  }
 
  //Latency test for blynk to msp432
 //on switch press in blynk dash board(mobile) , send data to msp432
  BLYNK_WRITE(LATENCY_SWITCH) {
  int button_state = param.asInt();//button_state = 1 if pressed 0 if not pressed
  t1 = micros();//set timer start to calculate latency
  send_key_msp(LATENCY_TEST, button_state);//pass in Latency test flag 'T', button_state = 1 to send_key_msp method
 

}
  
  BLYNK_WRITE(RESET_SWITCH) {
    int button_state = param.asInt();
    if (button_state == 1) {
      Serial2.println("R");
    }
  }
  //function to print to serial2 in msp432
  void send_key_msp(char key_flag, int button_state) {
    //if button is 1 , then print key_flag
    if (button_state == 1) {
      Serial2.println(key_flag);//print key flag in msp432
    }
      
  }

  void setup()
  {

    pinMode(9, OUTPUT);
    M5.begin(true, true, true);//start m5
    Serial2.begin(115200, SERIAL_8O1, 32,33); //rxpin = 3 (3.2), txpin =4 (3.3)
    //orientation of m5 display
    M5.Lcd.setRotation(3);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setTextColor(ORANGE);
    M5.Lcd.setCursor(0, 0);
    M5.Lcd.setTextSize(1);
    M5.Lcd.setCursor(10, 20);
    M5.Lcd.print("AutoSend Hello M5!");
    M5.Lcd.setCursor(10, 30);
    M5.Lcd.print("AutoPrint Receive");
    M5.Lcd.setCursor(10, 50);
    //begin blynk connection with wifi credentials
    Blynk.begin(auth, ssid, pass);
    M5.Lcd.print("Connected to BLYNK");
    
  }

  void loop()
  {
    // run blynk 
    Blynk.run();
    char flag = DEFAULT_FLAG;
    flag = getFlag();
  // delay(100); // Add back the delay if output contains both flag and variable
    pin_map_function(flag);
  }


//display values into label functions in blynk 
 void display_on_blynk(int pin) {
  String content = "";
  if (Serial2.available()) {// if there is data being sent from msp432
    while (Serial2.available()) {// while data is received 
      char ch = Serial2.read();// m5 read from serial 2 data sent from msp432 store in variable
      content.concat(ch);//concatnate string sent from msp432
    }
    // Display received inputs on M5 and Arduino serial console
    M5.Lcd.setTextSize(2);
    M5.Lcd.print(content);
    Serial.println(content);
     
  //converting ascii value to alphabet to display in blynk for barcode function.
  if (pin == BARCODE_DISPLAY) { 
      int t = content.toInt();
      char c = t;
      content = c;
      Blynk.virtualWrite(pin, content);
      return;
    }

    int x = content.toInt();
    Blynk.virtualWrite(pin, x);
    return;
  }
}





//function to reset dashbaord in blynk on button press in msp432 p1.4
void reset_dashboard() {
  Blynk.virtualWrite(SPEED_DISPLAY, 0);
  Blynk.virtualWrite(HEIGHT_OF_HUMP_DISPLAY, 0);
  Blynk.virtualWrite(DISTANCE_TRAVELLED_DISPLAY, 0);
  Blynk.virtualWrite(BARCODE_DISPLAY, 0);
  Blynk.virtualWrite(VEHICLE_STATE_DISPLAY, 0);
  Blynk.virtualWrite(SHORTEST_PATH_DISPLAY, "-");
  Blynk.virtualWrite(MAP_ROW_1, "-");
  Blynk.virtualWrite(MAP_ROW_2, "-");
  Blynk.virtualWrite(MAP_ROW_3, "-");
  Blynk.virtualWrite(MAP_ROW_4, "-");
  Blynk.virtualWrite(V0, 0);
  
  return;
}

//latency test to calculate and siplay on blynk upon clicking on msp432 P1.1 switch.
  void latency_test(int pin, int numBytes) {
    int i = 0;
    String content = "";
    if (Serial2.available()) {
        while(Serial2.available()){
          char ch = Serial2.read();
          content.concat(ch);
        }
        int x = content.toInt();
        while(i < numBytes)
        {
          Blynk.virtualWrite(pin, x); 
          i++;
        }
        t2 = micros();// end timer 

        x = t2-t1;// timer 2 minus timer 1 store in x value 
        float y = (float)x/1000;//convert micro to mills
        Serial.print("(MSP432 UART) Time taken to send ");
        Serial.print(numBytes);
        Serial.print(" bytes: ");
        Serial.print(y,3);
        Serial.println("ms");
    }
  }
  

  // This function sends Arduino's uptime every second to Virtual Pin 2.
  void myTimerEvent()
  {
   
  }

  char getFlag() {
      if (Serial2.available()) {
          char ch = Serial2.read();
          //Serial.print(ch);    
          M5.Lcd.setTextSize(2);
          return ch;
      }
      return DEFAULT_FLAG;
  }

  //switch case for pin map function
 void pin_map_function(char flag) {
  //Maps a key to a virtual pin to write to
  switch (flag) {
    case SPEED_FLAG:// if 'S' is sent from msp432 , display on blynk speed value. 
      display_on_blynk(SPEED_DISPLAY);
      flag = DEFAULT_FLAG;
      break;
    case HEIGHT_OF_HUMP_FLAG:// if 'H' is sent from msp432 , display on blynk height of hump value. 
      display_on_blynk(HEIGHT_OF_HUMP_DISPLAY);
      flag = DEFAULT_FLAG;
      break;
    case DISTANCE_FLAG:// if 'D' is sent from msp432 , display on blynk distance value. 
      display_on_blynk(DISTANCE_TRAVELLED_DISPLAY);
      flag = DEFAULT_FLAG;
      break;
    case BARCODE_FLAG: 
    {
      //display_on_blynk(BARCODE_DISPLAY);// if 'B' is sent from msp432 , display on blynk barcode value. 
      String new_str = read_long_string();
      Blynk.virtualWrite(BARCODE_DISPLAY, new_str);
      flag = DEFAULT_FLAG;
      break;
    }
     case VIEW_MAP_FLAG: 
    {
      //display_on_blynk(BARCODE_DISPLAY);// if 'B' is sent from msp432 , display on blynk barcode value. 
      int i = 0;
      while (i < 4) {
        String new_str = read_long_string();
        Serial.println(new_str);
        Blynk.virtualWrite(MAP_ROW_1+i, new_str);
        i++;
      }
      // Blynk.virtualWrite(VIEW_MAP_DISPLAY, new_str);
      flag = DEFAULT_FLAG;
      break;
    }
    case VEHICLE_STATE_DISPLAY_FLAG:
      Serial.println("Starting Car");// if blynk switch start vehicle is pressed ,  print in serial console starting car. 
      flag = DEFAULT_FLAG;
      break;
    case START_MAPPING_FLAG:
      Serial.println("Start Mapping");
      // if blynk switch start vehicle is pressed ,  print in serial console start mapping.  
      flag = DEFAULT_FLAG;
      break; 
      //latency testing
    case 'T':
      latency_test(V0, 1);
      flag = DEFAULT_FLAG;
      break;

    case 'A':
      Serial2.print('V');
      Serial.print("T");
      flag = DEFAULT_FLAG;
      break;
    
    case SHORTEST_PATH_FLAG:// if 'S' is sent from msp432 , display on blynk speed value. 
    {
      //display_on_blynk(BARCODE_DISPLAY);// if 'B' is sent from msp432 , display on blynk barcode value. 
      String new_str = read_long_string();
      Blynk.virtualWrite(SHORTEST_PATH_DISPLAY, new_str);
      flag = DEFAULT_FLAG;
      break;
    }
     
    case RESET_FLAG://reset dashboard on blynk
      Serial.println("Resetting Web Dashboard");
      reset_dashboard();
      break;
  }
}

String read_long_string() {
    int stop_received = 0;
    int i = 0;
    int idx = 0;
    String buf[100];
    String content = "";
    while (stop_received != 2) 
    {
        if (Serial2.available()) {
          char ch = Serial2.read();
          M5.Lcd.setTextSize(2);
          content.concat(ch);
        }
        //Serial.print(content);
        buf[idx] = content;
        if (content == "*") {
          stop_received++;
        }
        //Serial.print(c);
        //Serial.print(buf[idx]);
        idx++;
        content = "";
    }
    String new_str = "";
    while (i < idx) {
      new_str.concat(buf[i]);
      i++;
    }
    return new_str;
}
  BLYNK_CONNECTED() 
  {
    Blynk.syncAll();
    
  }
