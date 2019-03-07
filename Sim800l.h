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
*/
#ifndef Sim800l_h
#define Sim800l_h
#include "Arduino.h"


#define RESET_PIN 18   // pin to the reset pin sim800l

#define LED true // used for indicator led, in case that you don want set to false . 
#define LED_PIN 5 //pin to indicate states. 


class Sim800l {
  private:
	
	int _timeout;
	String _buffer;
	String _readSerial();  	
	bool   _reponse(String rep);

  public:
	
 	void begin();	
 	void reset(int pin); 

 	// Methods for calling || Funciones de llamadas. 
 	bool answerCall(); 	
 	void callNumber(char* number);
 	bool hangoffCall();
 	uint8_t getCallStatus();   
 	//Methods for sms || Funciones de SMS.
	bool sendSms(char* number,char* text);	 
	String readSms(uint8_t index);          // return all the content of sms
	String getNumberSms(uint8_t index);     // return the number of the sms..
	String getNameSms(uint8_t index);       // return name of sms number if exist in phone book PhC
	bool   WritePhoneBook(String message);  // ecriture entrée Phone Book
	bool   delAllSms();                     // return :  OK or ERROR ..
	bool   delSms(int index);               // return :  OK or ERROR ..
	bool   ModeText();                      // mode Text
	byte   ListPhoneBook();                 // return number of entry in phone book
	byte   getNumSms();                     // return number of sms in memory
	byte   getIndexSms();                   // index of last sms in memory
	byte   getRSSI();
	void   setPhoneFunctionality(int n);
	void   activateBearerProfile();
	void   deactivateBearerProfile();
	void   RTCtime(int *day,int *month, int *year,int *hour,int *minute, int *second);//get time with the variables by reference 
	String dateNet();                       //return date,time, of the network
	bool   updateRtc(int utc);              //Update the RTC Clock with de Time AND Date of red-.
	int    BattVoltage();                   // batterie en mV
	byte   BattPct();                       // batterie en %
	byte   getNetworkStatus();
	String getNetworkName();
	bool   getetatSIM();
	String getIMEI();
	String getPhoneBookNumber(uint8_t i);
	String getPhoneBookName(uint8_t i);
	bool   sleep();                         // goto sleep mode
};

#endif 