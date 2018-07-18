/*
 * Author: Murilo Fabricio
 * IC_IFSP
 * Data Logger using SD card, analog pins A0 and A1.
 * D6 and D7 will control the charge of a capacitor.
 * A0 and A1 will take the value of the voltage and current of my solar panel. 
 * Hardware:
 * SD Card
 * 
 * > CS   - pin 10
 * > MOSI - pin 11
 * > MISO - pin 12
 * > CLK  - pin 13
 * 
 * > Charge - pin D7
 * > Discharge - pin D6
 * 
*/
// ---    Parameters    --- 
#include <Adafruit_ADS1015.h>
#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <SD.h>


Adafruit_ADS1115 ads;

// ---    Hardware Mapping    --- \\
// -- CAP
#define discharge 6
#define charge 7
// -- SD Card
#define CS_pin   10                 //SPI, CS_pin D10
#define Volt0    A0                 //*SUB* Voltage sensor at the solar panel for measure voltages Values
#define Volt1    A1                 //Voltage sensor at the solar panel to measure the current

File myFile;                           //pointer for the file

// ---    Globals Variables   --- \\

int unsigned count = 0;             //Auxiliar for Interrupt from Timer2

float voltageValue;                 //Voltage Value after the calculation
float currentValue;                 //Current Value after the calculation
float vTensao = 0;

float static rVoltage=5.7;        //Voltage divider relation for voltage measure
float static rCurrent=5.9;        //Voltage divider relation for current measure

// ---    Interrup Routine  ---
ISR(TIMER2_OVF_vect){
  TCNT2=100;                        // Restar reg of Timer2
    if(count==100){    
      digitalWrite(discharge, HIGH);//Discharge the CAP.
      digitalWrite(charge, LOW);    //Discharge the CAP.
      count++;                      //Let it discharge during 2 sec
      
    }else if(count<100){
      count++;
      digitalWrite(discharge, LOW);  //Charge the CAP. 
      digitalWrite(charge, HIGH);    //Charge the CAP.
                                     //Charge it for 1 sec
    }else{
      count++;                       //incrementa o valor do contador
      if(count == 300)  count=0;     //After 3 sec restart the process
    }
      
}

int num = 1001;

// --- Initials Setup ---
void setup()
{
  pinMode(charge,OUTPUT);
  pinMode(discharge,OUTPUT);
     
  TCCR2A = 0x00;   //Timer normal mode
  TCCR2B = 0x07;   //Prescaler 1:1024
  TCNT2  = 100;    //10 ms overflow again
  TIMSK2 = 0x01;   //Enable interrup from Timer2
  //Start discharging the CAP.
  digitalWrite(discharge, LOW);
  digitalWrite(charge, HIGH);

  Serial.begin(9600);
  while (!Serial) {  ;   }                        //Wait till the serial monitor

  Serial.print("Starting SD card.....");     
 
  pinMode(CS_pin, OUTPUT);                        //Setup output at Chip Select
   
  if (!SD.begin(CS_pin))                          //Test opening of the SD card
  {
    Serial.println("Failed to open the SD!");     //Error
    return;                                       //retorn
  }

  //Success
  
  Serial.println("Success starting SD Card!");    //Started Successfully
  
  
  myFile = SD.open("dados2.csv", FILE_WRITE);     //Open the file to write
  
  
  if (myFile)                                     //Success to open?
  {                                               //Yes...
    myFile.println(", ,"); // 
    String header = "Voltage_SOLAR_PANEL, CURRENT";
    myFile.println(header);
    myFile.close();
    Serial.println(header);
  } //end if
  else                                            //Nope...
  {
    Serial.println("Error during the opening of the file...");      
                                                  //Inform about the error
  }

  //  --- Begin I2C Devices ///
  ads.begin();
  ads.setGain(GAIN_ONE);        // 1x gain   +/- 4.096V  1 bit = 2mV      0.125mV
  
} //end setup
 

// --- Loop ---
void loop()
{
   // Wait Interrup...
   
   // Overflow = Timer2_cont x prescaler x machine_cycle
   
   // machine_cycle = 1/Fosc = 1/16E6 = 62,5ns = 62,5E-9s
   
   // Overflow = (256 - 100) x 1024 x 62,5E-9 = 9,98ms 

   // Overflow x count = 9,98ms x 100 =~ 1s

   int16_t adc2, adc3;
   adc2 = ads.readADC_SingleEnded(2);
   adc3 = ads.readADC_SingleEnded(3);

   File myFile = SD.open("dados2.csv", FILE_WRITE);  

   //End calculation ------------------------------------------ |
   //Writing... 
   if(myFile)
   {
      //Get the Voltage ---------------- |
      voltageValue = (adc2*0.125)/1000;
      
      currentValue = (adc3*0.125*rCurrent)/ 5.5;
      
      myFile.println(String(voltageValue) + ", " + String(currentValue)); //Save the values
   }
   else
   {  
      voltageValue = (adc2*0.125)/1000;
      
      currentValue = (adc3*0.125*rCurrent)/ 5.5;
      
      Serial.println(String(voltageValue) + ", " + String(currentValue)); //Save the values
      Serial.println("Error during the opening of file to write the final value!!"); //Error
   }
   
   num=num-1; 
   while(num==0) Serial.println("/t /t END"); //Stop the loop after 1000 measurements
   myFile.close();
  
} //end loop





