#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include "algorithm_by_RF.h"
#include "max30102.h"
#include <DHT.h>

DHT dht(D4, DHT11);

const char *red = "IZZI-4EB6";
const char *password = "8C61A3BB4EB6";

String urlPulso = "http://189.228.161.43/Grabar/InsertarPulso.php?idPaciente=1&idsensorpulso_oxi=1&valor=";
String urlOximetria = "http://189.228.161.43/Grabar/InsertarOximetria.php?idPaciente=1&idsensorpulso_oxi=1&valor=";
String urlTemperatura = "http://189.228.161.43/Grabar/InsertarTemperatura.php?idPaciente=1&idsensorTemperatura=1&valor=";
WiFiClient clienteWiFi;
HTTPClient http; 

#define dht_dpin 2
//#define DEBUG // Uncomment for debug output to the Serial stream
//#define USE_ADALOGGER // Comment out if you don't have ADALOGGER itself but your MCU still can handle this code
#define TEST_MAXIM_ALGORITHM // Uncomment if you want to include results returned by the original MAXIM algorithm
//#define SAVE_RAW_DATA // Uncomment if you want raw data coming out of the sensor saved to SD card. Red signal first, IR second.



#ifdef TEST_MAXIM_ALGORITHM
  #include "algorithm.h" 
#endif

// Interrupt pin
const byte oxiInt = 10; // pin connected to MAX30102 INT

uint32_t elapsedTime,timeStart;

uint32_t aun_ir_buffer[BUFFER_SIZE]; //infrared LED sensor data
uint32_t aun_red_buffer[BUFFER_SIZE];  //red LED sensor data
float old_n_spo2;  // Previous SPO2 value
uint8_t uch_dummy,k;

void setup() {
  dht.begin();
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n");
  pinMode(oxiInt, INPUT);  //pin D10 connects to the interrupt output pin of the MAX30102
  Serial.println("\nConectando a la red");
  WiFi.begin(red, password);

  //Conectarse
  while (WiFi.status() != WL_CONNECTED) {
     //Esperar medio segundo
    delay(500);
    Serial.print(".");    // Avisar que sigue conectando
  }
  // Ya se conectó, mostrar el mensaje
  Serial.printf("\nConectado a %s\n", red);
  //--------

  Wire.begin();

//#if defined(DEBUG) || !defined(USE_ADALOGGER)
  // initialize serial communication at 115200 bits per second:
  //Serial.begin(115200);
//#endif

  maxim_max30102_reset(); //resets the MAX30102
  delay(1000);

  maxim_max30102_read_reg(REG_INTR_STATUS_1,&uch_dummy);  //Reads/clears the interrupt status register
  maxim_max30102_init();  //initialize the MAX30102
  old_n_spo2=0.0;

#ifdef USE_ADALOGGER
    // Measure battery voltage
  
#else// USE_ADALOGGER

  while(Serial.available()==0)  //wait until user presses a key
  {
    Serial.println(F("Press any key to start conversion"));
    delay(1000);
  }
  uch_dummy=Serial.read();
#ifdef TEST_MAXIM_ALGORITHM
  Serial.print(F("Time[s]\tHumd\tTemp\tSpO2\tHR\tClock\tRatio\tCorr"));
#else // TEST_MAXIM_ALGORITHM
  Serial.print(F("Time[s]\tSpO2\tHR\tClock\tRatio\tCorr"));
#endif // TEST_MAXIM_ALGORITHM
#ifdef SAVE_RAW_DATA
  int32_t i;
  // These are headers for the red signal
  for(i=0;i<BUFFER_SIZE;++i) {
    Serial.print("\t");
    Serial.print(i);
  }
  // These are headers for the infrared signal
  for(i=0;i<BUFFER_SIZE;++i) {
    Serial.print("\t");
    Serial.print(i);
  }
#endif // SAVE_RAW_DATA
  Serial.println("");
  
#endif // USE_ADALOGGER
  
  timeStart=millis();
}

//Continuously taking samples from MAX30102.  Heart rate and SpO2 are calculated every ST seconds
void loop() {
  float n_spo2,ratio,correl;  //SPO2 value
  int8_t ch_spo2_valid;  //indicator to show if the SPO2 calculation is valid
  int32_t n_heart_rate; //heart rate value
  int8_t  ch_hr_valid;  //indicator to show if the heart rate calculation is valid
  int32_t i;
  char hr_str[10];
  float temperatura = dht.readTemperature();     

  //buffer length of BUFFER_SIZE stores ST seconds of samples running at FS sps
  //read BUFFER_SIZE samples, and determine the signal range
  for(i=0;i<BUFFER_SIZE;i++)
  {
    while(digitalRead(oxiInt)==0);  //wait until the interrupt pin asserts
    maxim_max30102_read_fifo((aun_red_buffer+i), (aun_ir_buffer+i));  //read from MAX30102 FIFO
  }

  //calculate heart rate and SpO2 after BUFFER_SIZE samples (ST seconds of samples) using Robert's method
  rf_heart_rate_and_oxygen_saturation(aun_ir_buffer, BUFFER_SIZE, aun_red_buffer, &n_spo2, &ch_spo2_valid, &n_heart_rate, &ch_hr_valid, &ratio, &correl); 
  elapsedTime=millis()-timeStart;
  millis_to_hours(elapsedTime,hr_str); // Time in hh:mm:ss format
  elapsedTime/=1000; // Time in seconds


#ifdef TEST_MAXIM_ALGORITHM
  //calculate heart rate and SpO2 after BUFFER_SIZE samples (ST seconds of samples) using MAXIM's method
  float n_spo2_maxim;  //SPO2 value
  int8_t ch_spo2_valid_maxim;  //indicator to show if the SPO2 calculation is valid
  int32_t n_heart_rate_maxim; //heart rate value
  int8_t  ch_hr_valid_maxim;  //indicator to show if the heart rate calculation is valid
  maxim_heart_rate_and_oxygen_saturation(aun_ir_buffer, BUFFER_SIZE, aun_red_buffer, &n_spo2_maxim, &ch_spo2_valid_maxim, &n_heart_rate_maxim, &ch_hr_valid_maxim); 
#endif // TEST_MAXIM_ALGORITHM

  //save samples and calculation result to SD card
#ifdef TEST_MAXIM_ALGORITHM
  if(ch_hr_valid && ch_spo2_valid || ch_hr_valid_maxim && ch_spo2_valid_maxim) {
#else   // TEST_MAXIM_ALGORITHM
  if(ch_hr_valid && ch_spo2_valid) { 
#endif // TEST_MAXIM_ALGORITHM

#ifdef USE_ADALOGGER
#else // USE_ADALOGGER
    Serial.println("");

    String url0 = String( urlPulso + String(n_heart_rate_maxim));
     Serial.println(url0);
    if ( http.begin(clienteWiFi, url0) ) {   // Solicitar la conexión al servicio
    int codigo = http.GET();              // Ejecutar petición y leer código de respuesta
    Serial.printf("Código: %d\n", codigo);
    if ( codigo == HTTP_CODE_OK || codigo == HTTP_CODE_MOVED_PERMANENTLY) {
        String respuesta = http.getString();  // Respuesta
        Serial.println(respuesta);
    } else {
      Serial.printf("GET falló, error: %s\n", http.errorToString(codigo).c_str());
    }
  } else {
    Serial.println("No es posible hacer la conexión");
  }
    
     String url = String(urlOximetria + String(n_spo2_maxim));
     Serial.println(url);
    if ( http.begin(clienteWiFi, url) ) {   // Solicitar la conexión al servicio
    int codigo = http.GET();              // Ejecutar petición y leer código de respuesta
    Serial.printf("Código: %d\n", codigo);
    if ( codigo == HTTP_CODE_OK || codigo == HTTP_CODE_MOVED_PERMANENTLY) {
        String respuesta = http.getString();  // Respuesta
        Serial.println(respuesta);
    } else {
      Serial.printf("GET falló, error: %s\n", http.errorToString(codigo).c_str());
    }
  } else {
    Serial.println("No es posible hacer la conexión");
  }
  String url2 = String( urlTemperatura + String(temperatura));
     Serial.println(url2);
    if ( http.begin(clienteWiFi, url2) ) {   // Solicitar la conexión al servicio
    int codigo = http.GET();              // Ejecutar petición y leer código de respuesta
    Serial.printf("Código: %d\n", codigo);
    if ( codigo == HTTP_CODE_OK || codigo == HTTP_CODE_MOVED_PERMANENTLY) {
        String respuesta = http.getString();  // Respuesta
        Serial.println(respuesta);
    } else {
      Serial.printf("GET falló, error: %s\n", http.errorToString(codigo).c_str());
    }
  } else {
    Serial.println("No es posible hacer la conexión");
  }
  http.end();
  
  delay(1000); // Cada 10 segundos

#endif // USE_ADALOGGER
    old_n_spo2=n_spo2;
  }
}

void millis_to_hours(uint32_t ms, char* hr_str)
{
  char istr[6];
  uint32_t secs,mins,hrs;
  secs=ms/1000; // time in seconds
  mins=secs/60; // time in minutes
  secs-=60*mins; // leftover seconds
  hrs=mins/60; // time in hours
  mins-=60*hrs; // leftover minutes
  itoa(hrs,hr_str,10);
  strcat(hr_str,":");
  itoa(mins,istr,10);
  strcat(hr_str,istr);
  strcat(hr_str,":");
  itoa(secs,istr,10);
  strcat(hr_str,istr);
}
