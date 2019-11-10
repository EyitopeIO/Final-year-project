int status_led = 13;    //Actual is pin 7
int tilt_sensor = 6;
int arm_button = 7;

byte i;     //Temperature sensor main variables
byte present = 0;
byte type_s;
byte data[12];
byte addr[8];
float celsius;

String DATA = "";    //Data format for transmit
String main_time = "";    //Current time from RTC
String tilt_time = "";    //time of tilt

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

float lattitude = 0.000000;
float longitude = 0.000000;

SoftwareSerial gps_uart(9,10);   //GPS, Rx, Tx
SoftwareSerial gsm(11, 12);   //GSM, Rx, Tx
OneWire temp_sensor(8);   //temperature sensor, ds18b20
RTC_DS3231 rtc;
TinyGPSPlus gps;


void setup_temp_sensor(void) {
 if (!temp_sensor.search(addr)) {
    Serial.println(F("No more addresses."));
    Serial.println();
    temp_sensor.reset_search();
    delay(250);
    return;
  }
  Serial.print("ROM =");
  for( i = 0; i < 8; i++) {
    Serial.write(' ');
    Serial.print(addr[i], HEX);
  }
  if (OneWire::crc8(addr, 7) != addr[7]) {
    Serial.println(F("CRC is not valid!"));
    return; 
  }
  Serial.println();
  switch (addr[0]) {
    case 0x10:
      Serial.println("  Chip = DS18S20");  // or old DS1820
      type_s = 1;
      break;
    case 0x28:
      Serial.println("  Chip = DS18B20");
      type_s = 0;
      break;
    case 0x22:
      Serial.println("  Chip = DS1822");
      type_s = 0;
      break;
    default:
      Serial.println("Device is not a DS18x20 family device.");
      return;
  }
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
  Serial.print(F("  Temperature = "));
  Serial.print(celsius);
  Serial.print(F(" Celsius, "));
}



void fetch_time(String time_) 
{
  DateTime now = rtc.now();
  time_ = now.year()+'/'+ now.month()+'/'+ now.day()+'/'+' '+ now.hour()+':'+
          now.minute()+':'+ now.second();
  Serial.println(time_);
}



void setup_rtc(void)
{
  if(rtc.lostPower()) {
    Serial.println("RTC not running.");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));  
  }
  startTime = DateTime.now()
}



void fetch_location(void)
{
  gps_uart.listen();
  smartDelay(2000);
  lattitude = gps.location.lat();
  longitude = gps.location.lng();
}



void format_data(void) 
{
  /*
  char f[50];
  itoa(f, 50);*/
  DATA = String(celsius) + "," + String(longitude) + ","
  + String(lattitude) + "," + "," +
  "," + main_time + "," + tilt_time;
}



void setup_gsm(void)
{
  
}



void transmit_data(void)
{
  //
}



char *ftoa(char *a, double f, int precision) 
{
  long p[] = {0,10,100,1000,10000,100000,1000000,10000000,100000000};
  char *ret = a;
  long heiltal = (long)f;
  itoa(heiltal,a,10);
  while(*a != '\0') a++;
  *a++ = '.';
  long desimal = abs((long)((f-heiltal) * p[precision]));
  itoa(desimal,a,10);
  return ret;
}



  // This custom version of delay() ensures that the gps object
// is being "fed".
static void smartDelay(unsigned long ms) 
{
  unsigned long start = millis();
  do {
    while (gps_uart.available()) {
      gps.encode(gps_uart.read());
    }
  } while (millis() - start < ms);
}
