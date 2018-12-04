/*
   Author: Murilo Fabricio
   IC_IFSP
   Data Logger using SD card.
   Pin 9 will generate the PWM for the load.
   The ads1115 is responsible for taking the current and voltage from the panel.
   Hardware:
   SD Card

   > CS   - pin 10
   > MOSI - pin 11
   > MISO - pin 12
   > CLK  - pin 13

   > PWM - pin D9

   >Pino Botão - A0 - 100, 255 pontos
   >Pino Botão - A1 - 500, 1023 pontos
   >Pino Botão - A2 - Libera amostragem
   >Pino Botão - A5 - Qual Painel - ON=4/ off=1

   >A1+A0= 4095;
*/

#include <SD.h>
#include <Wire.h>
#include <Adafruit_ADS1015.h>
Adafruit_ADS1115 ads;  /* Use this for the 16-bit version */

// ---    Parameters    --- \\
// ---    Hardware Mapping    --- \\
// -- SD Card
#define CS_pin   10                 //SPI, CS_pin D10

#define set_bit(reg,bit)  (reg |= (1<<bit))
#define reset_bit(reg,bit)  (reg &= ~(1<<bit))
#define Aoff  100
#define A0on  255
#define A1on  500
#define A1A0  4095

File myFile;                           //pointer for the file

// ---    Globals Variables   --- \\

//int16_t pwmr = 4096;
//int16_t pwmr = 1023;
//int16_t pwmr = 255;
unsigned int pwmr;
byte p = 1;

void dutyPwm(int16_t _pwmresolution);
void setPwm(int16_t pwm);
void getdata();

// --- Initials Setup ---
void setup()
{
  pinMode(A0, INPUT_PULLUP);
  pinMode(A1, INPUT_PULLUP);

  pinMode(8, INPUT_PULLUP);

  set_bit(DDRD, 7);             //Led dados
  set_bit(DDRD, 6);             //Led Error
  set_bit(DDRD, 5);             //Led OK

  set_bit(DDRD, 4);             //Led OK
  set_bit(DDRD, 3);             //Led OK

  while (digitalRead(A0)) {
    set_bit(PORTD, PORTD4);             //Led aguardando
    set_bit(PORTD, PORTD3);             //Led aguardando

  }

  if (digitalRead(A1)) {
    pwmr = 4095;
    p = 1;

    reset_bit(PORTD, PORTD4);     //
    reset_bit(PORTD, PORTD3);     //

  } else {
    pwmr = 4095;
    p = 1;
  }

  ads.setGain(GAIN_TWOTHIRDS);  // 2/3x gain +/- 6.144V  1 bit = 3mV      0.1875mV (default)
  ads.begin();
  //Set how many values will gonna colect

  setPwm(pwmr);
  dutyPwm(pwmr);

  reset_bit(PORTD, PORTD7);     //
  reset_bit(PORTD, PORTD6);     //
  reset_bit(PORTD, PORTD5);     //

  //  delay(1000);

  pinMode(CS_pin, OUTPUT);                        //Setup output at Chip Select

  if (!SD.begin(CS_pin))                          //Test opening of the SD card
  {
    set_bit(PORTD, PORTD6);
    //Serial.println("Failed to open the SD!");     //Error
    return;                                       //retorn
  }

  //Success

  set_bit(PORTD, PORTD5);//success

  myFile = SD.open("dados" + String(p) + ".csv", FILE_WRITE); //Open the file to write


  if (myFile)                                     //Success to open?
  { //Yes...
    myFile.println(String(p) + " Inicio " + String(pwmr) + " pontos"); //
    myFile.println("1.08, 1.119/0.5,"); // pula linha

    myFile.println("Voltage_SOLAR_PANEL, CURRENT");// cabeçalho
    myFile.close();
    //Serial.println(header);
  } //end if
  else                                            //Nope...
  {
    set_bit(PORTD, PORTD5);
    set_bit(PORTD, PORTD6);
    //Serial.println("Error during the opening of the file...");
    //Inform about the error
  }

  getdata();

} //end setup



// --- Loop ---
void loop()
{
} //end loop

void getdata() {

  File myFile = SD.open("dados" + String(p) + ".csv", FILE_WRITE); //Open the file to write

  //Get the Voltage ---------------- |
  //delay(20);

  int16_t adc0, adc1;

  dutyPwm(pwmr);

  adc0 = ads.readADC_SingleEnded(0);
  adc1 = ads.readADC_SingleEnded(1);

  if (adc0 < 0) adc0 *= 0;
  if (myFile)
  {
    myFile.println(String(adc1) + ", " + String(adc0)); //Save the values
    set_bit(PORTD, PORTD7);                             //dados
  }
  else
  {
    set_bit(PORTD, PORTD6);       //red On
    reset_bit(PORTD, PORTD5);     //green off
    //Serial.println("Error during the opening of file to write the final value!!"); //Error
  }


  pwmr = 320;
  while (pwmr > 0) {

    dutyPwm(pwmr);


    adc0 = ads.readADC_SingleEnded(0);
    adc1 = ads.readADC_SingleEnded(1);
    if (adc0 < 0) adc0 *= 0;
    //voltageValue=(adc1*0.125*5.78/1000);
    //currentValue=(adc0*0.125*5.97);

    if (myFile)
    {
      myFile.println(String(adc1) + ", " + String(adc0)); //Save the values
      set_bit(PORTD, PORTD7);                             //dados
    }
    else
    {
      set_bit(PORTD, PORTD6);       //red On
      reset_bit(PORTD, PORTD5);     //green off
      //Serial.println("Error during the opening of file to write the final value!!"); //Error
    }


    pwmr = pwmr - 2;
    if (adc0 == 0) {
      pwmr = 0;
    }
  }

  myFile.close();
  while (pwmr <= 0) {

    set_bit(PORTD, PORTD7);             //All Led On
    set_bit(PORTD, PORTD6);
    set_bit(PORTD, PORTD5);
    set_bit(PORTD, PORTD4);             //Led aguardando
    set_bit(PORTD, PORTD3);             //Led aguardando

    while (1);
  } //Stop the loop after 1000 measurements
}


void dutyPwm(int16_t pwm) {

  // Use OCR1A and OCR1B to control the PWM
  // duty cycle. Duty cycle = OCR1A / ICR1.
  // OCR1A controls PWM on pin 9 (PORTB1).
  // OCR1B controls PWM on pin 10 (PORTB2).
  //OCR1A = 65535; // test signal
  OCR1A = pwm; // test signal

  //OCR1B = 0xf001;   //test signal
}

void setPwm(int16_t _pwmresolution) {

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
