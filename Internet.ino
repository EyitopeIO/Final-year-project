extern SoftwareSerial gsm;
extern String DATA;

String apn = "web.gprs.mtnnigeria.net";
String url = "idsapp.herokuapp.com/api/sample/details/save";
String gsm_response = "";

bool listen_to_modem(String response);

bool transmit_data()
{
  int trial = 3;
  Serial.println("\nTRANSMIT START");
   gsm.listen();
  
  while (trial--) {
  
    gsm.println(F("AT\r"));
    if (!listen_to_modem(F("OK")))  continue;
      
    gsm.println(F("AT+CGREG?\r"));
    if (!listen_to_modem(F("OK")))  continue;
     
  //  gsm.println("AT+CSCLK?\r");
  //  success = listen_to_modem();
    
    gsm.println(F("AT+SAPBR=3,1,Contype,GPRS\r"));
    if (!listen_to_modem(F("OK")))  continue;
  
    gsm.println("AT+SAPBR=3,1,APN," + apn + '\r');
    if (!listen_to_modem("OK")) continue;
   
    gsm.println(F("AT+SAPBR=1,1\r"));
    if (!listen_to_modem(F("OK")))  continue;
   
    gsm.println(F("AT+SAPBR=2,1\r"));
    if (!listen_to_modem(F("OK")))  continue;
   
    gsm.println(F("AT+HTTPINIT\r"));
    if (!listen_to_modem(F("OK")))  continue;
  
    gsm.println(F("AT+HTTPPARA=CID,1\r")); 
    if (!listen_to_modem(F("OK")))  continue;
    
    gsm.println("AT+HTTPPARA=URL," + url + '\r');
    if (!listen_to_modem(F("OK")))  continue;
  
    gsm.println(F("AT+HTTPPARA=CONTENT,application/x-www-form-urlencoded\r"));
    if (!listen_to_modem(F("OK"))); continue;
    
    gsm.println("AT+HTTPDATA=" + String(sizeof(DATA)) + ",5000");
    if( !listen_to_modem(F("OK")))  continue;
  
    gsm.println(DATA); 
    if (!listen_to_modem(F("OK")))  continue;
  
    gsm.println(F("AT+HTTPACTION=1\r"));
    if (!listen_to_modem(F("OK")))  continue;
    
    gsm.println(F("AT+HTTPREAD\r"));
    if (!listen_to_modem(F("OK")))  continue;
  
    gsm.println(F("AT+HTTPTERM\r"));
    if (!listen_to_modem(F("OK")))  continue;

    if (trial == 1) {
      //execute shut down code here
    }
     trial--;
  }
}



bool listen_to_modem(String response) {
  gsm.listen();
  while (gsm.available()) {
    gsm_response += gsm.read();
  }
  Serial.print("GSM: "); Serial.println(gsm_response);
  if (gsm_response.indexOf(response) >= 0) {
    return true;   //success
  }
  else {
    return false;
  }
}
