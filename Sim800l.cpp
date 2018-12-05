/* this library is writing by  Cristian Steib.
 *      steibkhriz@gmail.com
 *  Designed to work with the GSM Sim800l,maybe work with SIM900L
 *  
 *     This library use SoftwareSerial, you can define de RX and TX pin
 *     in the header "Sim800l.h" ,by default the pin is RX=10 TX=11..
 *     be sure that gnd is attached to arduino too. 
 *     You can also change the other preferred RESET_PIN
 *    
 *     Esta libreria usa SoftwareSerial, se pueden cambiar los pines de RX y TX
 *     en el archivo header, "Sim800l.h", por defecto los pines vienen configurado en
 *     RX=10 TX=11.  
 *     Tambien se puede cambiar el RESET_PIN por otro que prefiera
 *     
 *    PINOUT: 
 *        _____________________________
 *       |  ARDUINO UNO >>>   SIM800L  |
 *        -----------------------------
 *            GND      >>>   GND
 *        RX  10       >>>   TX    
 *        TX  11       >>>   RX
 *       RESET 2       >>>   RST 
 *                 
 *   POWER SOURCE 4.2V >>> VCC
 *
 *    Created on: April 20, 2016
 *        Author: Cristian Steib
 *        
 *	Modification PhC 25/10/2018
 *  Reset diode entre RESET SIM800 A et Sortie ESP32 K
 *
 * A ajouter PowerDown AT+CPOWD=1, http://m2msupport.net/m2msupport/atcpowd-power-off/
*/
#include "Arduino.h"
#include "Sim800l.h"
// #include <SoftwareSerial.h>


void Sim800l::begin() {
  #if (LED) 
    pinMode(LED_PIN,OUTPUT);
  #endif
	pinMode(RESET_PIN,OUTPUT);// PhC
  _buffer.reserve(255); //reserve memory to prevent intern fragmention
}


//
//PRIVATE METHODS
//
String Sim800l::_readSerial(){
  _timeout=0;
  while  (!Serial2.available() && _timeout < 12000  ) 
  {
    delay(13);
    _timeout++;
  }
  if (Serial2.available()) {
 	return Serial2.readString();
  }
}

//
//PUBLIC METHODS
//

int Sim800l::BattVoltage(){ // batterie en mV
	Serial2.println(F("AT+CBC"));
	_buffer=_readSerial();
	// Serial.println(_buffer);
	if(_buffer.indexOf(F("CBC:"))>-1){
		byte idx1 = _buffer.indexOf(",");
		byte idx2 = _buffer.indexOf(",",idx1 + 1);
		return _buffer.substring(idx2+1,_buffer.length()-1).toInt();
	}
}

byte Sim800l::BattPct(){ // batterie en %
	Serial2.println (F("AT+CBC"));
	_buffer=_readSerial();
	// Serial.println(_buffer);
	if(_buffer.indexOf(F("CBC:"))>-1){
		byte idx1 = _buffer.indexOf(",");
		byte idx2 = _buffer.indexOf(",",idx1 + 1);
		return _buffer.substring(idx1+1,idx2).toInt();
	}
}

byte Sim800l::getNetworkStatus(){
	Serial2.println (F("AT+CREG?"));
	_buffer=_readSerial();
	// Serial.println(_buffer);
	if(_buffer.indexOf(F("CREG:"))>-1){
		byte idx1 = _buffer.indexOf(",");
		return _buffer.substring(idx1+1,_buffer.length()).toInt();
	}
}

String Sim800l::getIMEI(){
	Serial2.println (F("AT+GSN"));
	_buffer=_readSerial();
	// Serial.println(_buffer);
	if(_buffer.indexOf(F("OK"))>-1){
		byte idx1 = _buffer.indexOf(F("OK"));
		return _buffer.substring(idx1-19, idx1-2);
	}
}

String Sim800l::getNetworkName(){
	Serial2.println (F("AT+COPS?"));
	_buffer=_readSerial();
	// Serial.println(_buffer);
	if(_buffer.indexOf(F("COPS:"))>-1){
		byte idx1 = _buffer.indexOf("\"");
		byte idx2 = _buffer.indexOf("\"",idx1+1);
		return _buffer.substring(idx1+1,idx2-1);
	}
}

bool Sim800l::getetatSIM(){
	Serial2.println (F("AT+CPIN?"));
	_buffer=_readSerial();
	// Serial.println(_buffer);
	if(_buffer.indexOf(F("READY")) >-1){
		return true;
	}
	else{
		return false;
	}
}

bool Sim800l::ModeText(){
	Serial2.println(F("AT+CMGF=1"));
	return true;
}

bool Sim800l::WritePhoneBook(String message){
	// Serial2.print(F("AT+CPBW="));
	Serial2.println(message);
	_buffer=_readSerial();
  if ( (_buffer.indexOf("OK") ) != -1){
		return true;
	}
  else{
		return false;
	}
}

byte Sim800l::ListPhoneBook(){
	Serial2.println(F("AT+CPBR=1,10"));
	_buffer=_readSerial();
	int p1 = _buffer.lastIndexOf(F("+CPBR:"));
	int p2 = _buffer.lastIndexOf(",", p1);
	byte i=(_buffer.substring(p1 + 7, p2).toInt();
	return i;	
}

byte Sim800l::getNumSms(){ // number of sms in memory
	// ModeText();
	Serial2.println(F("AT+CPMS?"));
	_buffer=_readSerial();
	byte i = (_buffer.substring(_buffer.indexOf(char(44))+1,_buffer.indexOf(","))).toInt();
	return i;
}

byte Sim800l::getIndexSms(){ // index of sms in memory
	Serial2.println(F("AT+CMGL=\"ALL\""));
	_buffer=_readSerial();
	int p1 = _buffer.lastIndexOf(F("+CMGL:"));
	int p2 = _buffer.indexOf(F(","), p1);
	byte i=(_buffer.substring(p1 + 7, p2)).toInt();
	return i;
}

String Sim800l::getPhoneBookNumber(uint8_t i) {
  // Lecture Numero dans Phone Book pos i
  Serial2.print(F("AT+CPBR="));
  Serial2.println(i);
  _buffer=_readSerial();
	// Serial.println(_buffer);
  if(_buffer.indexOf(F("+CPBR:"))>-1){// cherche 1eme champ
		byte idx1 = _buffer.indexOf("\"");
		if(idx1 > -1){
			byte idx2 = _buffer.indexOf("\"",idx1+1);
			return _buffer.substring(idx1+1,idx2);
		}
		else{
			return "0";
		}
	}  
}

String Sim800l::getPhoneBookName(uint8_t i) {
  // Lecture Numero dans Phone Book pos i
  Serial2.print(F("AT+CPBR="));
  Serial2.println(i);
  _buffer=_readSerial();
	// Serial.println(_buffer);
  if(_buffer.indexOf(F("+CPBR:"))>-1){	// cherche 3eme champ
		byte idx1 = _buffer.indexOf("\"");
		byte idx2 = _buffer.indexOf("\"",idx1+1);
		idx1 = _buffer.indexOf("\"",idx2 +1);
		idx2 = _buffer.indexOf("\"",idx1+1);
		return _buffer.substring(idx1+1,idx2);
	}  
}

void Sim800l::reset(int pin){
	int timeout = 12000;
	unsigned long time   = 0;
  #if (LED)
    digitalWrite(LED_PIN,0);
  #endif
	digitalWrite(RESET_PIN,1);
	delay(10);
  digitalWrite(RESET_PIN,0);
  delay(110);								// mini 105ms
  digitalWrite(RESET_PIN,1);// attendre mini 2.7s
  delay(5000);							// 5s pas d'erreur 
  // wait for the module response
  Serial2.print(F("AT\r\n"));
	delay(100);
	Serial2.print(F("AT\r\n"));
	delay(100);
	Serial2.print(F("AT\r\n"));
	
	_buffer=_readSerial();
	time = millis();
	while(!(_buffer.indexOf("OK") > -1) && !(_buffer.indexOf("AT") > -1)){
		_buffer=_readSerial();
		delay(100);
		if(millis()-time > timeout) break;
		// Serial.println(_buffer);
	}
	// Serial.println(_buffer);
	Serial2.print(F("AT+CPIN?\r\n"));
	_buffer=_readSerial();
	if((_buffer.indexOf("SIM PIN")>-1)){
		Serial2.print(F("AT+CPIN="));
		Serial2.print(pin);
		Serial2.print("\r\n");
	}
	Serial.println(_buffer);
	time = millis();
  while (_readSerial().indexOf("READY")==-1 ){//SMS ou CALL
		if(millis()-time > timeout) break;
	}
	
  #if (LED)
    digitalWrite(LED_PIN,1);
  #endif
}

void Sim800l::setPhoneFunctionality(int n){
  /*AT+CFUN=<fun>[,<rst>] 
  Parameters
  <fun> 0 Minimum functionality
  1 Full functionality (Default)
  4 Disable phone both transmit and receive RF circuits.
  <rst> 1 Reset the MT before setting it to <fun> power level.
  */
  // Serial2.print (F("AT+CFUN=1\r\n"));
	Serial2.print (F("AT+CFUN="));
	Serial2.print(n);
	Serial2.print(F("\r\n"));
}

void Sim800l::sleep(){
  /*AT+CSCLK=?
	0 si marche
	AT+CSCLK=1 entre en mode avion
	sleep faible consommation
	reveil par Sim800l.reset(PIN)
  */
  // Serial2.print (F("AT+CFUN=1\r\n"));
	Serial2.println (F("AT+CSCLK=1"));
}

byte Sim800l::getRSSI(){
/*Response
+CSQ: <rssi>,<ber>Parameters
<rssi>
0 -115 dBm or less
1 -111 dBm
2...30 -110... -54 dBm
31 -52 dBm or greater
99 not known or not detectable
<ber> (in percent):
0...7 As RXQUAL values in the table in GSM 05.08 [20]
subclause 7.2.4
99 Not known or not detectable 
*/
  Serial2.print (F("AT+CSQ\r\n"));
	_buffer=_readSerial();
	// Serial.println(_buffer);
	if(_buffer.indexOf(F("CSQ:")) >-1){
		byte idx1 = _buffer.indexOf(" ");
		byte idx2 = _buffer.indexOf(",");
		return _buffer.substring(idx1+1,idx2).toInt();
	}
}


void Sim800l::activateBearerProfile(){
  Serial2.print (F(" AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\" \r\n" ));_buffer=_readSerial();  // set bearer parameter 
  Serial2.print (F(" AT+SAPBR=3,1,\"APN\",\"websfr\" \r\n" ));_buffer=_readSerial(); // set apn  
  Serial2.print (F(" AT+SAPBR=1,1 \r\n"));delay(1200);_buffer=_readSerial();// activate bearer context
  Serial2.print (F(" AT+SAPBR=2,1\r\n "));delay(3000);_buffer=_readSerial(); // get context ip address
}


void Sim800l::deactivateBearerProfile(){
  Serial2.print (F("AT+SAPBR=0,1\r\n "));
  delay(1500);
}


bool Sim800l::answerCall(){
   Serial2.print (F("ATA\r\n"));
   _buffer=_readSerial();
   //Response in case of data call, if successfully connected 
   if ( (_buffer.indexOf("OK") )!=-1 ) return true;  
   else return false;
}


void  Sim800l::callNumber(char* number){
  Serial2.print (F("ATD"));
  Serial2.print (number);
  Serial2.print (F("\r\n"));
}


uint8_t Sim800l::getCallStatus(){
/*
  values of return:
 0 Ready (MT allows commands from TA/TE)
 2 Unknown (MT is not guaranteed to respond to tructions)
 3 Ringing (MT is ready for commands from TA/TE, but the ringer is active)
 4 Call in progress
*/
  Serial2.print (F("AT+CPAS\r\n"));
  _buffer=_readSerial();  
  return _buffer.substring(_buffer.indexOf("+CPAS: ")+7,_buffer.indexOf("+CPAS: ")+9).toInt();
}


bool Sim800l::hangoffCall(){
  Serial2.print (F("ATH\r\n"));
  _buffer=_readSerial();
  if ( (_buffer.indexOf("OK") ) != -1) return true;
  else return false;
}

boolean Sim800l::sendSms(char *number, char *text) {
	int timeout = 12000;
	Serial2.print (F("AT+CMGF=1\r")); //set sms to text mode  
  char sendcmd[30] = "AT+CMGS=\"";
  strncpy(sendcmd+9, number, 30-9-2);  // 9 bytes beginning, 2 bytes for close quote + null
  sendcmd[strlen(sendcmd)] = '\"';
	
	Serial2.println(sendcmd);
	_buffer=_readSerial();
	long time=millis();
	while(!(_buffer.indexOf("> ") > -1)){
		_buffer=_readSerial();
		delay(100);
		if(millis()-time > timeout) break;
	}
	// Serial.print("apres AT+CMGS="),Serial.println(_buffer);
	
  Serial2.println(text);
  Serial2.println();
  Serial2.write(0x1A);
	// Serial.print("time="),Serial.println(millis()-time);

	_buffer=_readSerial();
	time=millis();
	while(!(_buffer.indexOf("CMGS") > -1)){
		_buffer=_readSerial();
		delay(100);
		if(millis()-time > timeout) return false;
	}
	// Serial.print("time="),Serial.println(millis()-time);
	// Serial.print("apres AT+CMGS="),Serial.println(_buffer);
	return true;
}

String Sim800l::getNumberSms(uint8_t index){
  _buffer=readSms(index);
  // Serial.println(_buffer.length());
  if (_buffer.length() > 10) //avoid empty sms
  {
    uint8_t _idx1=_buffer.indexOf("+CMGR:");
    _idx1=_buffer.indexOf("\",\"",_idx1+1);
    return _buffer.substring(_idx1+3,_buffer.indexOf("\",\"",_idx1+4));
  }else{
    return "";
  }
}

String Sim800l::getNameSms(uint8_t index){//PhC
  _buffer=readSms(index);
  // Serial.println(_buffer.length());
	// Serial.println(_buffer);
  if (_buffer.length() > 10) //avoid empty sms
  {
    uint8_t _idx1=_buffer.indexOf("+CMGR:");
    _idx1=_buffer.indexOf("\",\"",_idx1+1);
		_idx1=_buffer.indexOf("\",\"",_idx1+1);
		return _buffer.substring(_idx1+3,_buffer.indexOf("\",\"",_idx1+1));
  }else{
    return "";
  }
}


String Sim800l::readSms(uint8_t index){
  Serial2.print (F("AT+CMGF=1\r")); 
  if (( _readSerial().indexOf("ER")) ==-1) {
    Serial2.print (F("AT+CMGR="));
    Serial2.print (index);
    Serial2.print("\r");
    _buffer=_readSerial();
    if (_buffer.indexOf("CMGR:")!=-1){
      return _buffer;
    }
    else return "";    
    }
  else
    return "";
}


bool Sim800l::delAllSms(){ 
  Serial2.print(F("AT+CMGDA=\"del all\"\n\r"));
  _buffer=_readSerial();
  if (_buffer.indexOf("OK")!=-1) {return true;}else {return false;}
}

bool Sim800l::delSms(int index){ 
  Serial2.print(F("AT+CMGD="));
	Serial2.print(index);
	Serial2.print("\r");
  _buffer=_readSerial();
  if (_buffer.indexOf("OK")!=-1) {return true;}else {return false;}
}

void Sim800l::RTCtime(int *day,int *month, int *year,int *hour,int *minute, int *second){
  Serial2.print(F("AT+CCLK?\r\n"));
  // if respond with ERROR try one more time. 
  _buffer=_readSerial();
  if ((_buffer.indexOf("ERR"))!=-1){
    delay(50);
    Serial2.print(F("AT+CCLK?\r\n"));
  } 
  if ((_buffer.indexOf("ERR"))==-1){
    _buffer=_buffer.substring(_buffer.indexOf("\"")+1,_buffer.lastIndexOf("\"")-1);  
    *year=_buffer.substring(0,2).toInt();
    *month= _buffer.substring(3,5).toInt();
    *day=_buffer.substring(6,8).toInt();
    *hour=_buffer.substring(9,11).toInt();
    *minute=_buffer.substring(12,14).toInt();
    *second=_buffer.substring(15,17).toInt();
 }
}

//Get the time  of the base of GSM
String Sim800l::dateNet() {
  Serial2.print(F("AT+CIPGSMLOC=2,1\r\n "));
  _buffer=_readSerial();

  if (_buffer.indexOf("OK")!=-1 ){
    return _buffer.substring(_buffer.indexOf(":")+2,(_buffer.indexOf("OK")-4));
  } else
  return "0";      
}

// Update the RTC of the module with the date of GSM. 
bool Sim800l::updateRtc(int utc){
  
  activateBearerProfile();
  _buffer=dateNet();
  deactivateBearerProfile();
  
  _buffer=_buffer.substring(_buffer.indexOf(",")+1,_buffer.length());
  String dt=_buffer.substring(0,_buffer.indexOf(","));
  String tm=_buffer.substring(_buffer.indexOf(",")+1,_buffer.length()) ;

  int hour = tm.substring(0,2).toInt();
  int day = dt.substring(8,10).toInt();

  hour=hour+utc;

  String tmp_hour;
  String tmp_day;
  //TODO : fix if the day is 0, this occur when day is 1 then decrement to 1, 
  //       will need to check the last month what is the last day .  
  if (hour<0){
    hour+=24;
    day-=1;
  }
  if (hour<10){

      tmp_hour="0"+String(hour);
  }else{
    tmp_hour=String(hour);
  }
  if (day<10){
    tmp_day="0"+String(day);    
  }else{
    tmp_day=String(day);
  }
    //for debugging
  //Serial.println("at+cclk=\""+dt.substring(2,4)+"/"+dt.substring(5,7)+"/"+tmp_day+","+tmp_hour+":"+tm.substring(3,5)+":"+tm.substring(6,8)+"-03\"\r\n");
  Serial2.print("at+cclk=\""+dt.substring(2,4)+"/"+dt.substring(5,7)+"/"+tmp_day+","+tmp_hour+":"+tm.substring(3,5)+":"+tm.substring(6,8)+"-03\"\r\n");
  if ( (_readSerial().indexOf("ER"))!=-1) {return false;}else return true;  
}