#include <RTClib.h>
#include <SoftwareSerial.h>
#include <OneWire.h>

#define GPS_WAIT_TIME 10

SoftwareSerial gps(10,9);   //GPS, Tx, Rx
SoftwareSerial gsm(11, 12);   //GSM, Tx, Rx
OneWire temp_sensor(8);   //temperature sensor, ds18b20
RTC_DS3231 rtc;

void fetch_temperature(void);
void fetch_location(void);
void fetch_time(void);
void setup_rtc(void);
void setup_temp_sensor(void);
void setup_gsm(void);
void format_data(void);
void transmit_data(void);

int status_led = 7;
int tilt_sensor = 6;

int tilt_time;

//Temperature sensor main variables
byte i;
byte present = 0;
byte type_s;
byte data[12];
byte addr[8];
float celsius;

//Data format for transmit
String DATA;

//Various status and conditions
bool tilt_state = false;

void setup()
{
  pinMode(status_led, OUTPUT);
  pinMode(tilt_sensor, INPUT);
  Serial.begin(9600);
  gps.begin(9600);
  gsm.begin(9600);
  setup_temp_sensor(); 
  setup_rtc();
}

void loop()
{
  //main
  unsigned long time_now = millis();
  while( millis() - time_now  < 300000) {   //5 mins
    fetch_temperature();
    fetch_location();
    format_data();
  }
  digitalWrite(status_led, HIGH);
  delay(500);
  digitalWrite(status_led, LOW);
  delay(2000);
}

void setup_temp_sensor(void)
{
 if ( !temp_sensor.search(addr)) {
    Serial.println(F("No more addresses."));
    Serial.println();
    temp_sensor.reset_search();
    delay(250);
    return;
  }
  Serial.print("ROM =");
  for( i = 0; i < 8; i++)
  {
    Serial.write(' ');
    Serial.print(addr[i], HEX);
  }
  if (OneWire::crc8(addr, 7) != addr[7])
  {
    Serial.println(F("CRC is not valid!"));
    return; 
  }
  Serial.println();
}

void fetch_temperature(void)
{
  temp_sensor.reset();
  temp_sensor.select(addr);
  temp_sensor.write(0x44, 1);    //start conversion, with parasite power on at the end
  delay(1000);
  Serial.print(F("  Data = "));
  Serial.print(present, HEX);
  Serial.print(" ");
  for ( i = 0; i < 9; i++ ) {         // we need 9 bytes
    data[i] = temp_sensor.read();
    Serial.print(data[i], HEX);
    Serial.print(" ");
  }
  Serial.print(F(" CRC="));
  Serial.print(OneWire::crc8(data, 8), HEX);
  Serial.println();
  int16_t raw = (data[1] << 8) | data[0];
  if (type_s) {
    raw = raw << 3;
    if (data[7] == 0x10) {
      raw = (raw & 0xFFF0) + 12 - data[6];
    }
  }
  else {
    byte cfg = (data[4] & 0x60);
    if (cfg == 0x00) raw = raw & ~7;
    else if (cfg == 0x20) raw = raw & ~3;
    else if (cfg == 0x40) raw = raw & ~1;
  }
  celsius = (float)raw / 16.0;
  fahrenheit = celsius * 1.8 + 32.0;
  Serial.print(F("  Temperature = "));
  Serial.print(celsius);
  Serial.print(F(" Celsius, "));
}

void fetch_location(void)
{
  unsigned long time_now = millis();
  gps.listen();
  while (millis() - time_now < GPS_WAIT_TIME) {
    while (gps.available() > 0) {
      gps.encode(gps.read());
    }
    if (gps.location.isUpdated()){
      lattitude = gps.location.lat();
      longitude = gps.location.lng();
      break;
    }
  }
}

void fetch_time(void)
{
  //code to grab time from rtc
}

void setup_rtc(void)
{
  
}

void setup_gsm(void)
{
  
}

void format_data(void)
{
  DATA = String(celsius) + "," + String(longitude) + ","
  + String(lattitude) + "," + String(tilt_state) + "," + tilt_time;
}

void transmit_data(void)
{
  //
}
