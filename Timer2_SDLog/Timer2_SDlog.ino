#include <SD.h>
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
// ---    Parameters    --- \\
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

float static rVoltage=5.7;        //Voltage divider relation for voltage measure
float static rCurrent=5.9;        //Voltage divider relation for current measure
// -- Initial setup for the analog read---
float lePorta(uint8_t pin, float relation);

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
  

} //end setup
 

// --- Loop ---
void loop()
{
   // Wait Interrup...
   
   // Overflow = Timer2_cont x prescaler x machine_cycle
   
   // machine_cycle = 1/Fosc = 1/16E6 = 62,5ns = 62,5E-9s
   
   // Overflow = (256 - 100) x 1024 x 62,5E-9 = 9,98ms 

   // Overflow x count = 9,98ms x 100 =~ 1s
 
   File myFile = SD.open("dados2.csv", FILE_WRITE);  

   //Get the Voltage ---------------- |
   voltageValue = lePorta(Volt0, rVoltage);
   currentValue = lePorta(Volt1, rCurrent);
   currentValue = currentValue / 5.5;
  
   //End calculation ------------------------------------------ |
   //Writing... 
   if(myFile)
   {
      myFile.println(String(voltageValue) + ", " + String(currentValue)); //Save the values
   }
   else
   {
      Serial.println("Error during the opening of file to write the final value!!"); //Error
   }
   
   num=num-1; 
   while(num==0) Serial.println("/t /t END"); //Stop the loop after 1000 measurements
   myFile.close();
  
} //end loop

// --- Functions ---//
// -- Function to read and calculate the values from the analog pins
float lePorta(uint8_t pin, float relation) {
  
  float total=0;
  float aux=0;  
  
  for (int i=0; i<3; i++) {
    aux = analogRead(pin);
    aux = map(aux,0,1024,0,5000);
    aux = aux * relation;
    
    total += 1.0 * aux;
    
  }
  
  return total / 3.0;
}
