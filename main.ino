#include "RTClib.h"
#include <OneWire.h>
#include <TinyGPS++.h> 
#include <SoftwareSerial.h>


extern char first_transmit;

extern const int status_led;
extern const int tilt_sensor;
extern const int arm_button;

extern float celsius;
extern float lattitude;
extern float longitude;

extern String current_time;
extern String tilt_time;
extern String start_time;
extern String DATA;

void blink_(int time_);
void fetch_temperature(void);
void fetch_location(void);
void inline fetch_time(String time_);
void setup_rtc(void);
void setup_temp_sensor(void);
void setup_gsm(void);
bool transmit_data();   //defined in 3rd file
void smartDelay(unsigned long ms);   //GPS function
char *ftoa(char *a, double f, int precision);   //float to integer

extern SoftwareSerial gps_uart;
//extern SoftwareSerial gsm;
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
  //gsm.begin(115200);
  setup_temp_sensor();
  setup_rtc();

  Serial.println(F("Waiting for button"));
  while(digitalRead(arm_button) == HIGH) {    //Wait till system is armed
    blink_(100);
  }

  DateTime now = rtc.now();
  start_time = String(now.year())+'/'+ String(now.month())+'/'+ String(now.day())+ " "+ String(now.hour())+':'+
               String(now.minute())+':'+ String(now.second());
  first_transmit = '1';

  Serial.println("\nLOOP STARTS");
}



void loop() 
{
  unsigned long time_now = millis();
  first_transmit = '0';
  while((millis() - time_now  <= 10000))  {   //Every 5 mins, but 10s for testing
    if(digitalRead(tilt_sensor) == HIGH){
      digitalWrite(status_led, HIGH);
      DateTime now = rtc.now();
      tilt_time = String(now.hour())+ ':' + String(now.minute())+ ':'+
                  String(now.second());  
      //transmit data if tilted here
      digitalWrite(status_led, LOW);
    }
    blink_(500);
  }
  digitalWrite(status_led, HIGH);
  fetch_temperature();
  fetch_location();
  DateTime now = rtc.now();
  current_time = String(now.hour())+ ':' + String(now.minute())+ ':'+
              String(now.second());
              
//  DATA = '{' +
//        "\"temperature\":" + String(celsius,2) + "," + 
//        "\"longitude\":" + String(longitude,4) + "," +
//        "\"latitude\":" + String(lattitude,4) + "," +
//        "\"current_time\":" + current_time + "," +
//        "\"tilt_time\":" + tilt_time + "," +
//        "\"first_transmit\":" + first_transmit + "," + 
//        "\"start_time\":" + start_time +
//        '}';
//        
  transmit_data();
  digitalWrite(status_led, LOW); 
  //  Serial.println("DATA = "); Serial.print(DATA); Serial.println('\n');
}



void blink_(int time_) 
{
  digitalWrite(status_led, HIGH);
  delay(time_);
  digitalWrite(status_led, LOW);
  delay(time_);  
}
