#include "RTClib.h"
#include <OneWire.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>

extern char first_transmit;

extern int status_led;
extern int tilt_sensor;
extern int arm_button;

extern String main_time;
extern String tilt_time;
extern String start_time;

void fetch_temperature(void);
void fetch_location(void);
void fetch_time(String time_);
void setup_rtc(void);
void setup_temp_sensor(void);
void setup_gsm(void);
void format_data(void);
void transmit_data();
void smartDelay(unsigned long ms);   //GPS function
char *ftoa(char *a, double f, int precision);   //float to integer

extern SoftwareSerial gps_uart;
extern SoftwareSerial gsm;
extern OneWire temp_sensor;
extern RTC_DS3231 rtc;
extern TinyGPSPlus gps;

void setup()
{
  pinMode(status_led, OUTPUT);
  pinMode(tilt_sensor, INPUT);
  pinMode(arm_button, INPUT);
  Serial.begin(9600);
  gps_uart.begin(9600);
  gsm.begin(9600);
  setup_temp_sensor();
  setup_rtc();

  digitalWrite(status_led, HIGH);
  while(arm_button == HIGH);    //Wait till system is armed

  DateTime now = rtc.now();
  start_time = String(now.year())+'/'+ String(now.month())+'/'+ String(now.day())+'/'+' '+ String(now.hour())+':'+
          String(now.minute())+':'+ String(now.second());
  first_transmit = '1';
  transmit_data();
  first_transmit = '0';
  
    
}

void loop() 
{
  unsigned long time_now = millis();
  while((millis() - time_now  <= 15000)){   //Every 5 mins, but 15s for testing
    if(tilt_sensor){
      fetch_time(tilt_time);
      format_data();
      transmit_data();
    }
  }
  digitalWrite(status_led, HIGH);
  fetch_temperature();
  fetch_location();
  fetch_time(main_time);
  format_data();
  transmit_data();
  tilt_time = " ";   //reset after transmitting
  delay(100);
  digitalWrite(status_led, LOW); 
}
