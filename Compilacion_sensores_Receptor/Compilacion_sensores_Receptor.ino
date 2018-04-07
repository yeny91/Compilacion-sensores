// Este codigo realiza la compilacion de 4 sensores, 3 de los cuales se conectan 
// en este mismo arduino mega y del cual se muestran las conexiones y el otro se recibe por medio de un emisor NRF-L2401
// Conectado a un arduino nano (Codigo Emisor). 

// LA CONEXION DE LOS SENSORES ES LA SIGUIENTE 

//SENSOR MAGNETICO DE PUERTA MC-38  ARDUINO MEGA
//Pin 1 D3
//Pin 2 GND
//SENSOR DE CORRIENTE ACS712  ARDUINO MEGA
//VCC 5V
//OUT A0
//GND GND
//SENSOR DE MOVIMIENTO HC-SR501 ARDUINO MEGA
//VCC 5V
//OUT D4
//GND GND

// CONEXION MODULO NRF-L2401 CON ARDUINO MEGA
 
// GND= Pin GND
// VCC = Pin 3v3
// CE = Pin 9
// CSN = Pin 10
// SCK = Pin 52
// MOSI = Pin 51
// MISO = Pin 50

// PARA ESTA PROGRAMACION SE VISITARON LOS SIGUIENTES TUTORIALES

// Llamas https://www.luisllamas.es/comunicacion-inalambrica-a-2-4ghz-con-arduino-y-nrf24l01/)
// codigo adapatado de: http://www.naylampmechatronics.com/blog/48_tutorial-sensor-de-corriente-acs712.html
// Adaptado de https://www.luisllamas.es/detector-de-movimiento-con-arduino-y-sensor-pir/
// # http://arubia45.blogspot.com.es

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
 
const int pinCE = 9;
const int pinCSN = 10;
RF24 radio(pinCE, pinCSN);
 
// Single radio pipe address for the 2 nodes to communicate.
const uint64_t pipe = 0xE8E8F0F0E1LL;
 
float data[3];

//DECLARACION DE VARIABLES PARA EL SENSOR DE MOVIMIENTO
       // pin para el LED
const int PIRPin = 4;         // pin de entrada (for PIR sensor)
 


// INICIALIZACION DE VARIABLES PARA EL SENSOR DE MOVIMIENTO
int pirState = LOW;           // de inicio no hay movimiento
int val = 0;                 // estado del pin

// INICIALIZACION DE VARIABLES PARA EL SENSOR DE CORRIENTE
float Sensibilidad=0.66; //sensibilidad en V/A para nuestro sensor
float offset=0; // Equivale a la amplitud del ruido

//DECLARACION DE VARIABLES PARA EL SENSOR MAGNETICO 

// Definiciones
boolean abierto = false;
int cambios;

// Rutina de Interrupción
void interrupcion()
{
  abierto = true;
//  Serial.print("cambio ");
  if (cambios == 0)
    cambios ++;
  else {
    cambios = 0;
    abierto = false;
  }
  //Serial.println(cambios);
}


 
void setup()
{
  //COMUNICACION POR RADIO
   radio.begin();
   Serial.begin(9600); 
   radio.openReadingPipe(1, pipe);
   radio.startListening();

//declaracion sensor de movimiento
 
   pinMode(PIRPin, INPUT);

   //declaracion sensor de flujo

   digitalWrite(3, HIGH);
  attachInterrupt(1,interrupcion,HIGH); // Attaching the ISR to INT0


   
}
 
void loop()
{


  //proceso del sensor de corriente
  float Ip=get_corriente();//obtenemos la corriente pico
  float Irms=Ip*0.707; //Intensidad RMS = Ipico/(2^1/2)
  float P=Irms*110.0; // P=IV watts
// profeso del sensor de movimiento
  
  val = digitalRead(PIRPin);

// filtro sensor magnetico 
    
if (abierto)
    Serial.println("P1");
  else
    Serial.println("P0");
 

// filtro sensor de corriente
  if (P>13){
  
 
  Serial.println("L1");
  }

  else 
  {

  Serial.println("L0");
    
    }

     // filtro sensor de movimiento
   if (val == HIGH)   //si está activado
   {     
      
      Serial.println("M1");
            
      }
      else 
      {   
        Serial.println("M0");       
        }

   if (radio.available())
   {    
      radio.read(data,sizeof(data));
      if (data[0]==0) {     
      Serial.println("LV0");
      
   }
   else {
    Serial.println("LV1");
   }
   }
   delay(2000);
}

float get_corriente()
{
  float voltajeSensor;
  float corriente=0;
  long tiempo=millis();
  float Imax=0;
  float Imin=0;
  while(millis()-tiempo<500)//realizamos mediciones durante 0.5 segundos
  { 
    voltajeSensor = analogRead(A0) * (5.0 / 1023.0);//lectura del sensor
    corriente=0.9*corriente+0.1*((voltajeSensor-2.5)/Sensibilidad); //Ecuación  para obtener la corriente
    if(corriente>Imax)Imax=corriente;
    if(corriente<Imin)Imin=corriente;
  }
  return(((Imax-Imin)/2)-offset);
}
