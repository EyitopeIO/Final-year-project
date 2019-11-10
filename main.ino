#include "RTClib.h"
#include <OneWire.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>

extern int status_led;
extern int tilt_sensor;
extern int arm_button;

extern String main_time;
extern String tilt_time;

void fetch_temperature(void);
void fetch_location(void);
void fetch_time(String time_);
void setup_rtc(void);
void setup_temp_sensor(void);
void setup_gsm(void);
void format_data(void);
void transmit_data(void);
static void smartDelay(unsigned long ms);   //GPS function
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

  digitalWrite(status_led, HIGH);
  while(arm_button == HIGH);    //System armed when button is low. Latch button used
  setup_rtc();  
}

void loop() 
{
  unsigned long time_now = millis();
  while( millis() - time_now  < 30000) {   //Every 5 mins, but 30s for testing
    digitalWrite(status_led, HIGH);
    fetch_temperature();
    fetch_location();
    fetch_time(main_time);
    format_data();
    transmit_data();
    tilt_time = "";   //reset after transmitting
    delay(100);
    digitalWrite(status_led, LOW);
  }
  if(tilt_sensor){
    fetch_time(tilt_time);
  }
}
