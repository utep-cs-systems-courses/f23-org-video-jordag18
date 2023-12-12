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
char* velInfo  = "10";
short controlCol = (screenWidth/2) - 20;
short controlRow = 0;

short interrupts = 1;
short drawBlock = 1;
short redrawLine = 0;
short redrawScreen = 0;

//Button pressing related states
short correct = 0;
int playSkrillex = 0;

int halfNoteLength = 200;

int quarterNoteLength = 94;

int eighthNoteLength = 48;

int seconds = 0;

int notes[]= {1275, 637, 1431, 637, 536, 637, 851,

	      956, 536, 2863, 1072, 1136, 1431, 1607,

	      1703, 1912, 1072, 1431, 1072, 1275, 1072};

int noteLength[] = {0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0};

int i = 0;

short BLOCK_COLOR = COLOR_BLUE;

void draw()
{
  if (correct == 6){
    clearScreen(WHITE);
    playSkrillex = 1;
    interrupts = 1;
  }
  else{
    and_sr(~8);    //Masking interrupts
    if (redrawScreen){
      redrawScreen = 0;
      clearScreen(BG_COLOR);
      redrawLine = 1;
    }
    fillRectangle(controlCol , controlRow + pastRow, 40, height, BG_COLOR);
    fillRectangle(controlCol, controlRow + currentRow, 40, height, BLOCK_COLOR);
    pastRow = currentRow;

    drawRectOutline(0, 0, 5, 50, COLOR_PURPLE);
    fillRectangle(0, 0, 5, 50, BG_COLOR);
    fillRectangle(0, 0, 5, 10*correct, COLOR_PURPLE);

    drawString5x7(screenWidth - 30, 10, "Speed:", COLOR_WHITE, BG_COLOR);
    drawString5x7(screenWidth - 20, 20, velInfo, COLOR_WHITE, BG_COLOR);
    
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

void play_scaryMonster()
{
  seconds++;
  if(i > 20){
    i = 0;
    playSkrillex = 0;
  }
  if(noteLength[i]){
    if(!i)
      buzzer_set_period(notes[0]);
    if(seconds >= quarterNoteLength){
      seconds = 0;
      i++;
      buzzer_set_period(notes[i]);
    }
  }
  else{
    if(seconds >= eighthNoteLength){
      seconds = 0;
      i++;
      buzzer_set_period(notes[i]);
    }
  }
}


void wdt_c_handler()
{
  interrupts++;
  if (interrupts == 250){  //every second do this
    interrupts = 0;
    currentRow+=velocity;
    drawBlock = 1;                                                          
  }

  if (currentRow > lineRow){
    currentRow = (-1*velocity);
    redrawScreen = 1;
    if (correct > 0)
      correct --;
  }

  if (playSkrillex){
    play_scaryMonster();
    correct = 0;
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
    if (velocity == 10){
      velInfo = "10";
    }
    else if (velocity == 30){
      velInfo = "30";
    }
    else
      velInfo = "50";
    
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
    P1OUT &= ~LED;
    or_sr(0x10);   //turning off cpu
    P1OUT |= LED;
  }
}
