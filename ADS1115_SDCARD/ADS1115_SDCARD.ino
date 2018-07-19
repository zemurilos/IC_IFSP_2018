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
#include <SD.h>
#include <Wire.h>
#include <Adafruit_ADS1015.h>
Adafruit_ADS1115 ads;  /* Use this for the 16-bit version */

// ---    Parameters    --- \\
// ---    Hardware Mapping    --- \\
// -- CAP
#define discharge 6
#define charge 7
// -- SD Card
#define CS_pin   10                 //SPI, CS_pin D10

File myFile;                           //pointer for the file

// ---    Globals Variables   --- \\

int unsigned count = 0;             //Auxiliar for Interrupt from Timer2

float voltageValue;                 //Voltage Value after the calculation
float currentValue;                 //Current Value after the calculation

float static rVoltage=5.78;        //Voltage divider relation for voltage measure
float static rCurrent=5.97;        //Voltage divider relation for current measure

// ---    Interrup Routine  ---

int unsigned num = 255;

// --- Initials Setup ---
void setup()
{
  pinMode(charge,OUTPUT);
  pinMode(discharge,OUTPUT);
  
  ads.begin();
  ads.setGain(GAIN_ONE);        // 1x gain   +/- 4.096V  1 bit = 2mV      0.125mV
  
  //Start discharging the CAP.
  digitalWrite(charge, HIGH);
  digitalWrite(discharge, !digitalRead(charge));
  
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
   delay(3000);
   
   digitalWrite(charge, LOW);
   delay(100);
   digitalWrite(discharge, HIGH);
  
   
   //digitalWrite(discharge, HIGH);
   //digitalWrite(charge, LOW);
   
} //end setup

  

// --- Loop ---
void loop()
{
   analogWrite(3, num);
   // Wait Interrup...
   
   // Overflow = Timer2_cont x prescaler x machine_cycle
   
   // machine_cycle = 1/Fosc = 1/16E6 = 62,5ns = 62,5E-9s
   
   // Overflow = (256 - 100) x 1024 x 62,5E-9 = 9,98ms 

   // Overflow x count = 9,98ms x 100 =~ 1s
 
   File myFile = SD.open("dados2.csv", FILE_WRITE);  

   //Get the Voltage ---------------- |
   int16_t adc0, adc1;

  adc0 = ads.readADC_SingleEnded(0);
  adc1 = ads.readADC_SingleEnded(1);
  //Serial.print("AIN0: "); Serial.print(adc0); Serial.print("\t "); Serial.println(adc0*0.125*5.97/1000);
  voltageValue=(adc1*0.125*5.78/1000);
  //Serial.print("AIN1: "); Serial.print(adc1); Serial.print("\t "); Serial.println(adc1*0.125*5.78/1000);
  currentValue=(adc0*0.125*5.97);
  //Serial.println(" ");

  
   //End calculation ------------------------------------------ |
   //Writing... 
   if(myFile)
   {
      myFile.println(String(voltageValue) + ", " + String(currentValue/5.5)); //Save the values
   }
   else
   {
      Serial.println("Error during the opening of file to write the final value!!"); //Error
   }
   
   num=num-1; 
   while(num==0) Serial.println("END"); //Stop the loop after 1000 measurements
   myFile.close();
  
} //end loop
