#include "LowPower.h"
#include <dht.h>
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include <Vcc.h>
#include "printf.h"

const float VccMin        = 2.0*0.6;  // Minimum expected Vcc level, in Volts. Example for 2xAA Alkaline.
const float VccMax        = 2.0*1.5;  // Maximum expected Vcc level, in Volts. Example for 2xAA Alkaline.
const float VccCorrection = 3.445/3.51; // Measured Vcc by multimeter divided by reported Vcc misurata/letta

const int SWITCH = 2;
#define DHT22_PIN 5
dht DHT;
Vcc vcc(VccCorrection);

const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };

typedef struct{
  float Temp;
  float Hum;
  float Battery;  
}SendPlayload;
SendPlayload sendPlayload;



void setup() {
  Serial.begin(115200);
   printf_begin();

/*
   while(1){
 //Acquisisci battery  
  sendPlayload.Battery = vcc.Read_Volts();
  Serial.println(sendPlayload.Battery);
    delay(500);   
    }
*/    
}

/*
void loop() {
  //POWER ON SENSOR
  Serial.println("LOW");
  pinMode(SWITCH,OUTPUT);
  digitalWrite(SWITCH, LOW);


  delay(2500);
  
  //POWER OFF SENSOR
  Serial.println("HIGH");
  pinMode(SWITCH,INPUT);
  digitalWrite(SWITCH, HIGH);
  
  delay(2500);


}
*/


void loop() {
  //POWER ON SENSOR
  digitalWrite(A0, LOW);  
  pinMode(A0,OUTPUT);
  digitalWrite(A0, LOW);
  digitalWrite(A0, HIGH);
  digitalWrite(A0, LOW);
  
  delay(500);
  
  //Acquisisci temp+Umidità
  int chk = DHT.read22(DHT22_PIN);
  if(chk == DHTLIB_OK){
    //Umidita = DHT.humidity;
    //Temperatura = DHT.temperature;
    sendPlayload.Temp = DHT.temperature;
    sendPlayload.Hum = DHT.humidity;
  }else{
    sendPlayload.Temp = -1000.0;
    sendPlayload.Hum = -1000.0;
  }
 
  //Acquisisci battery  
  sendPlayload.Battery = vcc.Read_Volts();
  Serial.print("<< ");
  Serial.print(sendPlayload.Temp);
  Serial.print(" ; ");
  Serial.print(sendPlayload.Hum);  
  Serial.print(" ; ");  
  Serial.print(sendPlayload.Battery);
  Serial.println(">>");

  //Trasmetti
  RF24 radio(7,8);
  radio.begin(); 

  radio.powerUp();
    
  radio.enableDynamicPayloads();
  radio.setChannel(90); 
  radio.setDataRate(RF24_2MBPS);
  radio.setPALevel(RF24_PA_MAX); 
  radio.setRetries(5,15);
  radio.openWritingPipe(pipes[0]);
  radio.openReadingPipe(1,pipes[1]);
  radio.startListening();
  Serial.println("START");
  radio.stopListening();
    
  radio.write( &sendPlayload, sizeof(sendPlayload) );
  radio.startListening();
  Serial.println("FINISH");

  radio.powerDown();

//  delay(2500);
  
  //POWER OFF SENSOR
  pinMode(A0,INPUT_PULLUP);


pinMode(DHT22_PIN,INPUT);

//delay(2500);  
  //5min pause
  //LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF); 



  //5min pause
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF); 
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF); 
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF); 
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF); 
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF); 
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF); 
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF); 
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF); 
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF); 
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF); 
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF); 
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF); 
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF); 
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF); 
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF); 
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF); 
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF); 
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF); 
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF); 
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF); 
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF); 
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF); 
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF); 
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF); 
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF); 
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF); 
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF); 
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF); 
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF); 
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF); 
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF); 
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF); 
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF); 
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF); 
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF); 
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF); 
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF); 
  LowPower.powerDown(SLEEP_4S, ADC_OFF, BOD_OFF); 
  


  
}

