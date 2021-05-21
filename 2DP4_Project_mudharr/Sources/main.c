/*2DP4 Project Code*/
/*Author: Raj Mudhar, mudharr, 400088877*/

#include <hidef.h>      /* common defines and macros */
#include "derivative.h"  /* derivative information */
#include "SCI.h"

/*funciton declerations*/
void delayby1ms(int);
void setClk(void);
void OutCRLF(void);
unsigned int mode;

void main(void) {
  
  /*setup variables, ports and bus speed*/
  unsigned short x;       //x analog reading for mode 0
  unsigned short y;       //y analog reading for mode 1
  double g = 0;           //acceleration to be calculated
  double conversion = 0;  //converting g to angle
  int angle = 0;          //the angle to be displayed
  setClk();               //set bus clock to 8MHz
  DDRJ = 0x01;            //set LED as output
  DDR01AD = 0b1111001111; //set LED outputs
  
  /*setup and enable ADC channel 5 (mode 0 - x) and channel 4 (mode 1 - y)*/
  ATDCTL1 = 0x4F; //12-bit resolution
  ATDCTL3 = 0x90; //right justified, two samples per sequence
  ATDCTL5 = 0x34; //continuous conversion on channel 4 and 5
  
  /*setup and configure timer input capture*/
  TSCR1 = 0x90; //timer system control register 1
                //timer enable and runs during WAI
  TSCR2 = 0x03; //timer system control register 2
                //timer prescaler select set to bus clock/8 = 1MHz
  TIOS = 0xFE;  //timer input capture/output compare
                //set TIC[0] and input
  PERT = 0x01;  //enable pull up resistor on TIC[0]
  TCTL3 = 0x00; //TCTLX configures which edge(s) to capture
  TCTL4 = 0x02; //configured to capture falling edges on TIC[0]
  TIE = 0x01;   //timer interrupt enable for TIC[0]
  EnableInterrupts;       
  
  SCI_Init(38400); //initialize baudrate
  
  //infinte loop
  for(;;){
    //read channel 5 (x)
    if(mode == 0){
      x = ATDDR1;
      g = (double)x*6/4095-3;
      conversion = g*2.34*180/3.14;
      if(conversion <= 0)
        conversion = 0;
      angle = (int)conversion; 
      SCI_OutUDec(angle);
      OutCRLF();
      delayby1ms(250); //MATLAB Samplig rate ~ 4 samples per second
    }
    //read channel 4 (y)
    else if(mode == 1){
      y = ATDDR0;
      g = (double)y*6/4095-3;
      conversion = g*2.34*180/3.14;
      if(conversion <= 0)
        conversion = 0;
      angle = (int)conversion; 
      SCI_OutUDec(angle);
      OutCRLF();
      delayby1ms(250); //MATLAB sampling rate ~ 4 samples per second
    }
    //momentarily halt serial communications 
    else if(mode == 2){
      delayby1ms(250); //MATLAB samping rate ~ 4 samples per second
    }
    PT01AD = (angle/10%10)<<6;  //output 10s column on BCD display
    PT01AD = PT01AD + angle%10; //output 1s column on BCD display   
  }
}

/*interrupt service routine for TC0*/
static interrupt  VectorNumber_Vtimch0 void ISR_Vtimch0(void){
  unsigned int temp;
  //resets and increments modes
  //mode 0 reads x
  //mode 1 reads y
  //mode 2 halts serial communication
  mode++;
  if(mode == 3){
    mode = 0;  
  }
  temp = TC0;  
}

/*sets bus-clk to 8MHz*/
void setClk(void){
  CPMUCLKS = 0x80; //PLLSEL = 1
  CPMUOSC = 0x00; //OSCE = 0
  CPMUSYNR = 0x07; //fref = 2*1*(7+1)=16
  CPMUFLG = 0x08; //lock = 1, PLLCLK = VCOCLK/(POSTDIV+1)
  CPMUPOSTDIV = 0x00; //POSTDIV = 0, so BUSCLK = PLL/2 = 8MHz  
}

/*delay function for 1ms*/
/*this was verified to be correct for 1s using the AD2*/
void delayby1ms(int time)
{
  unsigned int j,k;
  for(j=0;j<time;j++){
    for(k=0;k<1333;k++){
    }
  }
}

/*Output a CR,LF to SCI to move cursor to a new line*/
void OutCRLF(void){
  SCI_OutChar(CR);
  SCI_OutChar(LF);
}