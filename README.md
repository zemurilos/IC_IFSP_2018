# IC_IFSP_2018
This is the documentation about my Scientific Initiation Scholarship at IFSP. My project is about MPPT, and monitoring solar panels.
The start point of this project is to use an Arduino UNO, who will work as a controller for another circuit acting as electronic load.
## 2018-07-17
I tried to implement I2C sensor to my project, so I made an example using ADS1115 and OLED, at Arduino NANO.
## 2018-07-18
I tried to implement an interrupt using TIMER2 from Atmega, it worked fine, this interrupt will charge and discharge a capacitor at the gate of an MOSFET.
After I tried to implemtent a Datalogger using a SD Card while the running interrupt at the background.
It worked well.
After that, I made some example using I2C for the ADS1115 and SPI for the SD card.(I will test it tomorrow).
## 2018-07-19
Today I went to college and tested it out. I had to change some aspects to make it work as I planned before. First I had to implement the ADS 
to read the analog signal, because those at arduino had too much noise. After that, I realized that arduino is not fast enough to do it.
So i needed to make it slower, I was using a capacitor charging and discharging. Now I'm using PWM from arduino...
I tried and i think that worked...
## 2018-08-08
After some days, I tried again the last programm, also I had to repair some aspects about the circuit, then I programmed and bound a 16 bit PWM, DataLogger and the ADS1115 to my project, however I couldn't test it...
Tomorrow I'm gonna try to test it... 

