# MSP432M5STICKCPlusUARTCommunications
1. Import Comms-module folder into Code Composer Studio
2. Ensure driverlib path is changed to own driverlib path
3. Ensure Blynk, BlynkESP32_BT_WF and M5StickCPlus libraries are installed in Arduino library manager
4. Add this line to Arduino preferences->board manager
https://m5stack.oss-cn-shenzhen.aliyuncs.com/resource/arduino/package_m5stack_index.json
5. Open wifi_m5_msp.ino in arduino, update ssid and pass with own hotspot values
6. Verify and upload wifi_m5_msp.ino to M5StickCPlus
7. Connect MSP432 to USB port and build CCS project
7. Connect M5StickCPlus to USB port and upload wifi_m5_msp.ino
8. Login to Blynk with username: zongwei123@hotmail.com password: 2104car567
9. Proceed with UART communications
10. A video showing how to use Blynk with the features can be accessed via:
https://drive.google.com/file/d/1WsZUfWp8A4PrX5rma2xbqw-oVvab1X_j/view?usp=share_link