#include "buzzer.h"

void playSound()   
{                  
  switch (correct){
  case 1:          
    play4Csharp(); 
    break;         
  case 2:          
    play4D();      
    break;         
  case 3:          
    play4Fsharp(); 
    break;         
  case 4:          
    play4G();      
    break;         
  case 5:          
    play4A();      
    break;         
  }                
}                  
