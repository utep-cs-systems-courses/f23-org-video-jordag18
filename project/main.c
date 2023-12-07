#include <msp430.h>
#include <libTimer.h>
#include "lcdutils.h"
#include "lcddraw.h"
#include "buzzer.h"

#define LED BIT6

#define TSW1 BIT0                            //Only for top switches so LCD won't be affected
#define TSW2 BIT1
#define TSW3 BIT2
#define TSW4 BIT3

#define SWITCHES (TSW1 | TSW2 | TSW3 | TSW4)

#define BG_COLOR COLOR_BLACK
#define LINE_COLOR COLOR_WHITE


short col = 0;
short pastRow = 0;
short currentRow = 0;
short velocity = 30;
short height = 30;
short lineRow = 100;

short controlCol = (screenWidth/2) - 5;
short controlRow = 0;

short interrupts = 1;
short drawBlock = 1;
short redrawLine = 0;
short redrawScreen = 0;

//Button pressing related states
short correct = 0;
short playTheme = 0;

short BLOCK_COLOR = COLOR_BLUE;

void draw()
{
  if (correct == 6){
    clearScreen(WHITE);
    playTheme = 1;
    interrupts = 0;
  }
  else{
    and_sr(~8);    //Masking interrupts
    if (redrawScreen){
      redrawScreen = 0;
      clearScreen(BG_COLOR);
      redrawLine = 1;
      fillRectangle(controlCol, controlRow + pastRow, 10, height, BG_COLOR);
      fillRectangle(controlCol, controlRow + currentRow, 10, height, BLOCK_COLOR);
      pastRow = currentRow;

      drawRectOutline(0, 0, 5, 50, COLOR_GREEN);
      fillRectangle(0, 0, 5, 50, BG_COLOR);
      fillRectangle(0, 0, 5, 10*correct, COLOR_GREEN);
    
    }
    else{
      fillRectangle(controlCol , controlRow + pastRow, 10, height, BG_COLOR);
      fillRectangle(controlCol, controlRow + currentRow, 10, height, BLOCK_COLOR);
      pastRow = currentRow;

      drawRectOutline(0, 0, 5, 50, COLOR_GREEN);
      fillRectangle(0, 0, 5, 50, BG_COLOR);
      fillRectangle(0, 0, 5, 10*correct, COLOR_GREEN);
    }
    or_sr(8);  //Unmasking interrupts
  }
}
  
void incorrectPress()
{
  buzzer_set_period(2000);
  if(correct > 0)
    correct --;
    
  
}    

void switch_init()
{
  P2REN |= SWITCHES;
  P2IE |= SWITCHES;
  P2OUT |= SWITCHES;
  P2DIR &= ~SWITCHES;
}
//Translate one state machine to assembly
 //Add win screen and FF victory fanfare
void wdt_c_handler()
{
  interrupts++;
  if (interrupts == 250 || interrupts == 500 || interrupts == 750){    //every second do this
    currentRow+=velocity;                                                                    
    drawBlock = 1;                                                          
  }
  if (interrupts == 750)
    interrupts = 1;

  if (currentRow > lineRow){
    currentRow = (-1*velocity);
    redrawScreen = 1;
    if (correct > 0)
      correct --;
  }

  if (playTheme){
    switch (interrupts){
    case 41:
      play5C();
      break;
    case 62:
      buzzer_set_period(0);
      break;
    case 82:
      play5C();
      break;
    case 103:
      buzzer_set_period(0);
      break;
    case 123:
      play5C();
      break;
    case 144:
      buzzer_set_period(0);
      break;
    case 165:
      play5C();
      break;
    case 210:
      buzzer_set_period(0);
      break;
    case 230:
      play4G();
      break;
    case 280:
      buzzer_set_period(0);
      break;
    case 300:
      play4A();
      break;
    case 350:
      buzzer_set_period(0);
      break;
    case 370:
      play5C();
      break;
    case 400:
      buzzer_set_period(0);
      break;
    case 420:
      play4A();
      break;
    case 450:
      buzzer_set_period(0);
      break;
    case 460:
      play5C();
      break;
    case 500:
      buzzer_set_period(0);
      correct = 0;
      playTheme = 0;
      break;
      
      
    }
    
  }
}


void __interrupt_vec(PORT2_VECTOR) Port_2()
{
  if (P2IFG & TSW1){                      //If they press it when its in the region, correct+1
    P2IFG &= ~TSW1;               
    if ((controlRow+currentRow) <= lineRow && lineRow <= ((controlRow + currentRow) + height )){
      correct++;
      currentRow = (-1*velocity);         //reset the shape's position
      redrawLine = 1;
      playSound();
    }
    else{
      currentRow = (-1*velocity);
      redrawLine = 1;
      incorrectPress();
}
  }
  else if (P2IFG & TSW2){                //Increment speed
    P2IFG &= ~TSW2;
    if (velocity < 50)
      velocity += 20;
    else
      velocity = 10;
  }
  else if (P2IFG & TSW3){                //Change color
    P2IFG &= ~TSW3;
    if (BLOCK_COLOR < COLOR_PURPLE)   
      BLOCK_COLOR = COLOR_PURPLE;
    else if (BLOCK_COLOR < COLOR_BLUE)
      BLOCK_COLOR = COLOR_BLUE;
    else
      BLOCK_COLOR = COLOR_RED;
  }
  else if (P2IFG & TSW4){                 //Increment size
    P2IFG &= ~TSW4;
    if (height < 90)
      height += 30;
    else
      height = 30;
  }
}


void main()
{
  P1DIR |= LED;
  P1OUT |= LED;
  
  configureClocks();
  lcd_init();
  switch_init();
  buzzer_init();

  enableWDTInterrupts();
  or_sr(0x8);             //enable GIE

  clearScreen(BG_COLOR);
  for (int i = 0; i < screenWidth; i++){
    drawPixel(i, lineRow, LINE_COLOR);
  }
  
  while (1){
    if (correct < 0)
      correct = 0;
    if (drawBlock){
      drawBlock = 0;
      draw(); 
    }
    if (redrawLine){
      redrawLine = 0;
      for (int i = 0; i < screenWidth; i++){ 
         drawPixel(i, lineRow, LINE_COLOR);   
      }
    }
    /*  if(correct)
      playSound();               In case he wants sound to only be done by main
    else
      incorrectPress();
    */              
    
    P1OUT &= ~LED;
    or_sr(0x10);   //turning off cpu
    P1OUT |= LED;
  }
}
