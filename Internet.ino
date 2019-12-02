

//extern SoftwareSerial gsm;
extern String DATA;

String apn = "web.gprs.mtnnigeria.net";
String url = "idsapp.herokuapp.com/api/sample/details/save";

bool transmit_data()
{
  int state = 0;
  Serial.println(F("\nTRANSMIT START"));

//   DATA = '{' +
//        "\"temperature\":" + String(celsius,2) + "," + 
//        "\"longitude\":" + String(longitude,4) + "," +
//        "\"latitude\":" + String(lattitude,4) + "," +
//        "\"current_time\":" + current_time + "," +
//        "\"tilt_time\":" + tilt_time + "," +
//        "\"first_transmit\":" + first_transmit + "," + 
//        "\"start_time\":" + start_time +
//        '}';

  while(1) {
  
    switch(state) {
      case 0:
      DATA = '{' + "\"temperature\":" + String(celsius,2) + '}';
      httpSend();
      state = 1;
      break;
  
      case 1:
      DATA = '{' + "\"longitude\":" + String(longitude,4) + '}';
      httpSend();
      break;    
  
      case 2:
      DATA = '{' + "\"latitude\":" + String(lattitude,4) + '}';
      httpSend();
      state = 3;
      break;
  
      case 3:
      DATA = '{' + "\"current_time\":" + current_time + '}';
      httpSend();
      state = 4;
      break;
  
      case 4:
      DATA = '{' + "\"tilt_time\":" + tilt_time + '}';
      httpSend();
      state = 5;
      break;
  
      case 5:
      DATA = '{' + "\"first_transmit\":" + first_transmit + '}';
      httpSend();
      state = 6;
      break;
  
      case 6:
      DATA = '{' + "\"start_time\":" + start_time + '}';
      httpSend();
      state = 7;
      break;
  
      default:
      Serial.println("All data sent.");
    }
    if (state >= 7) break;
  }
}


void runsl() {
  while (gsm.available()) {
    Serial.write(gsm.read());
  }
}

void httpSend(void)
{
  gsm.println(F("AT"));
  runsl();//Print GSM Status an the Serial Output;
  delay(1000);
    
  gsm.println(F("AT+CGREG?"));
  runsl();
  delay(10000);
  
  gsm.println(F("AT+CGREG?"));
  runsl();
  delay(200);
   
  gsm.println(F("AT+CSCLK?"));
  runsl();
  delay(50);

  gsm.println(F("AT+SAPBR=3,1,Contype,GPRS"));
  runsl();
  delay(100);
  gsm.println("AT+SAPBR=3,1,APN," + apn);
  runsl();
  delay(100);
  gsm.println(F("AT+SAPBR =1,1"));
  runsl();
  delay(100);
  gsm.println(F("AT+SAPBR=2,1"));
  runsl();
  delay(2000);
  gsm.println(F("AT+HTTPINIT"));
  runsl();
  delay(100);
  gsm.println(F("AT+HTTPPARA=CID,1"));
  runsl();
  delay(100);
  gsm.println("AT+HTTPPARA=URL," + url);
  runsl();
  delay(500);
  gsm.println(F("AT+HTTPPARA=CONTENT,application/x-www-form-urlencoded"));
  runsl();
  delay(100);
  gsm.println("AT+HTTPDATA="+ String(sizeof(DATA) + 1)+ ",10000");
  runsl();
  delay(100);
  gsm.println(DATA); 
  runsl();
  delay(5000);
  gsm.println(F("AT+HTTPACTION=1"));
  runsl();
  delay(5000);
  gsm.println(F("AT+HTTPREAD"));
  runsl();
  delay(100);
  gsm.println(F("AT+HTTPDATA=0,1000"));
  runsl();
  gsm.println(F("AT+HTTPTERM"));
  runsl(); 
  delay(100);
}
