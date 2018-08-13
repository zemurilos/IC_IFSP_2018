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
// -- SD Card
#define CS_pin   10                 //SPI, CS_pin D10

File myFile;                           //pointer for the file

// ---    Globals Variables   --- \\

float voltageValue;                 //Voltage Value after the calculation
float currentValue;                 //Current Value after the calculation

float static rVoltage=5.78;        //Voltage divider relation for voltage measure
float static rCurrent=5.97;        //Voltage divider relation for current measure

// ---    Interrup Routine  ---
//int16_t pwmd = 0xffff;
int16_t pwmd = 4096;

void dutyPwm(int16_t _pwmresolution);
void setPwm(int16_t pwm);  
void getdata();

// --- Initials Setup ---
void setup()
{
  
  setPwm(pwmd);
  ads.begin();
  ads.setGain(GAIN_ONE);        // 1x gain   +/- 4.096V  1 bit = 2mV      0.125mV
  
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
  
  getdata();
   
} //end setup

  

// --- Loop ---
void loop()
{
} //end loop

void getdata(){
  while(pwmd>0){
    
    dutyPwm(pwmd);
   
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
     
     pwmd=pwmd-5;
     myFile.close();
     
  }
  
  while(pwmd==0) {  Serial.println("END"); while(1);} //Stop the loop after 1000 measurements
}


void dutyPwm(int16_t pwm){
  
  // Use OCR1A and OCR1B to control the PWM
  // duty cycle. Duty cycle = OCR1A / ICR1.
  // OCR1A controls PWM on pin 9 (PORTB1).
  // OCR1B controls PWM on pin 10 (PORTB2).
  //OCR1A = 65535; // test signal
    OCR1A = pwm; // test signal
    
  //OCR1B = 0xf001;   //test signal
}

void setPwm(int16_t _pwmresolution){
  
  // Set PB1/2 as outputs.
  DDRB |= (1 << DDB1) | (1 << DDB2);

  TCCR1A =
      (1 << COM1A1) | (1 << COM1B1) |
      // Fast PWM mode.
      (1 << WGM11);
  TCCR1B =
      // Fast PWM mode.
      (1 << WGM12) | (1 << WGM13) |
      // No clock prescaling (fastest possible
      // freq).
      (1 << CS10);
  OCR1A = 0;
  // Set the counter value that corresponds to
  // full duty cycle. For 15-bit PWM use
  // 0x7fff, etc. A lower value for ICR1 will
  // allow a faster PWM frequency.
  //ICR1 = 0xffff;
  ICR1 = _pwmresolution;

}
