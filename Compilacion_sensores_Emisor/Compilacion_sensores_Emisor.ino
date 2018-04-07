//Este codigo consiste en la implementacion de un sensor de flujo de agua YF-S201
// puesto en arduino nano y adaptado con un modulo NRF-L2401

// CONEXIONES NRF-L201
// GND  Pin GND
// VCC Pin 3v3
// CE  Pin 9
// CSN Pin 10
// SCK Pin 13
// MOSI  Pin 11
// MISO  Pin 12

//CONEXIONES SENSOR DE FLUJO DE AGUA
// SENSOR DE FLUJO YF-S201 ARDUINO MEGA
// Pin Rojo  5V
// Pin Amarilla  D2
// Pin negro   GND

// Esta programacion se llevo a cabo basado en los siguientes tutoriales
// http://www.naylampmechatronics.com/blog/47_tutorial-sensor-de-flujo-de-agua.html
// Llamas https://www.luisllamas.es/comunicacion-inalambrica-a-2-4ghz-con-arduino-y-nrf24l01/)

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
 
const int pinCE = 9;
const int pinCSN = 10;
RF24 radio(pinCE, pinCSN);
 
// Single radio pipe address for the 2 nodes to communicate.
const uint64_t pipe = 0xE8E8F0F0E1LL;
 
float data[2];


//SENSOR DE AGUA 

volatile int NumPulsos; //variable para la cantidad de pulsos recibidos
int PinSensor = 2;    //Sensor conectado en el pin 2
float factor_conversion=7.5; //para convertir de frecuencia a caudal

//---Función que se ejecuta en interrupción---------------
void ContarPulsos ()
{ 
  NumPulsos++;  //incrementamos la variable de pulsos
} 

//---Función para obtener frecuencia de los pulsos--------
int ObtenerFrecuencia() 
{
  int frecuencia;
  NumPulsos = 0;   //Ponemos a 0 el número de pulsos
  interrupts();    //Habilitamos las interrupciones
  delay(2000);   //muestra de 1 segundo
  noInterrupts(); //Desabilitamos las interrupciones
  frecuencia=NumPulsos; //Hz(pulsos por segundo)
  return frecuencia;
}
 
void setup()
{
   radio.begin();
   radio.openWritingPipe(pipe);
   pinMode(PinSensor, INPUT); 
  attachInterrupt(0,ContarPulsos,RISING); //(Interrupcion 0(Pin2),funcion,Flanco de subida)

   
}
 
void loop()
{ 
  float frecuencia=ObtenerFrecuencia(); //obtenemos la Frecuencia de los pulsos en Hz
  float caudal_L_m=frecuencia/factor_conversion; //calculamos el caudal en L/m
  float caudal_L_h=caudal_L_m*60; //calculamos el caudal en L/h
   
   data[0]= caudal_L_h;
   data[1] = millis()/1000.0;
   
   radio.write(data, sizeof(data));
   delay(2000);
}
