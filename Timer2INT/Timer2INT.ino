/*
   Adapt from
   "Aula 44 - Interrupção por Estouro do Timer2 na IDE Arduino"
   www.wrkits.com.br | facebook.com/wrkits | youtube.com/user/canalwrkits
   
   Timer 2 overflow interrupt.
   
*/
// --- Parâmetros ---

#define descarga 6
#define carga 7

// --- Variaveis Globais ---

int unsigned count = 0;

// --- Rotina de Interrupção ---
ISR(TIMER2_OVF_vect)
{
    TCNT2=100;          // Reinicializa o registrador do Timer2
    if(count==100){    
      digitalWrite(descarga, HIGH);
      digitalWrite(carga, LOW);
      count++;
      
    }else if(count<100){
      count++;//incrementa o valor do contador
      digitalWrite(descarga, LOW);
      digitalWrite(carga, HIGH);
    }else{
      count++;//incrementa o valor do contador
      if(count == 300)  count=0;
    }
      
}


// --- Configurações Iniciais ---
void setup()
{
     pinMode(6,OUTPUT);
     pinMode(7,OUTPUT);
     
     TCCR2A = 0x00;   //Timer operando em modo normal
     TCCR2B = 0x07;   //Prescaler 1:1024
     TCNT2  = 100;    //10 ms overflow again
     TIMSK2 = 0x01;   //Habilita interrupção do Timer2

     digitalWrite(descarga, LOW);
     digitalWrite(carga, HIGH);

     Serial.begin(9600);
} //end setup
 

// --- Loop Infinito ---
void loop()
{
   //Aguarda Interrupção...
   
   // Estouro = Timer2_cont x prescaler x ciclo de máquina
   
   // Ciclo de máquina = 1/Fosc = 1/16E6 = 62,5ns = 62,5E-9s
   
   // Estouro = (256 - 100) x 1024 x 62,5E-9 = 9,98ms 

   // Estouro x count = 9,98ms x 100 =~ 1s

   Serial.println("...");
   delay(100);
} //end loop




