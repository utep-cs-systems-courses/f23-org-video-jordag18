#include <msp430.h>
#include "libTimer.h"
#include "buzzer.h"

void buzzer_init()
{
    /* 
       Direct timer A output "TA0.1" to P2.6.  
        According to table 21 from data sheet:
          P2SEL2.6, P2SEL2.7, anmd P2SEL.7 must be zero
          P2SEL.6 must be 1
        Also: P2.6 direction must be output
    */
    timerAUpmode();		/* used to drive speaker */
    P2SEL2 &= ~(BIT6 | BIT7);
    P2SEL &= ~BIT7; 
    P2SEL |= BIT6;
    P2DIR = BIT6;		/* enable output to speaker (P2.6) */
}

void buzzer_set_period(short cycles) /* buzzer clock = 2MHz.  (period of 1k results in 2kHz tone) */
{
  CCR0 = cycles; 
  CCR1 = cycles >> 1;		/* one half cycle */
}

void play4D(){
  buzzer_set_period(6811);
}

void play4F(){
  buzzer_set_period(5727);
}

void play4Fsharp(){
  buzzer_set_period(5405);
}

void play4G(){
  buzzer_set_period(5195);
}

void play4A(){
  buzzer_set_period(4630);
}

void play4Csharp(){
  buzzer_set_period(7353);
}

void play5C(){
  buzzer_set_period(3822);
}


void playState(short playTheme, short interrupts){             
  if(playTheme == 1){       
    if(interrupts == 43)   
       play4D();            
    if(interrupts == 65)   
       buzzer_set_period(0);
    if(interrupts == 86)   
       play4D();            
    if(interrupts == 108)  
       buzzer_set_period(0);
    if(interrupts == 125)  
       play4D();            
    if(interrupts == 147)  
       buzzer_set_period(0);
  }                        
}                          
