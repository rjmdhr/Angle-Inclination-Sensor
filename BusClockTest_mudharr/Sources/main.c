/*Bus Clock test code*/
/*Raj Mudhar, mudharr, 400088877*/

#include <hidef.h>      /* common defines and macros */
#include "derivative.h"  /* derivative information */

/*funciton declerations*/
void delayby1ms(int);
void setClk(void);

void main(void) {

  setClk();             //set bus clock to 8MHz
  DDRJ = 0x01;          //set LED as output
  
  //Flash LED for 1s
  while(1){
    PTJ = 0xFF;
    delayby1ms(1000);
    PTJ= 0x00;
    delayby1ms(1000); 
  }
  
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
void delayby1ms(int time){
  int ix;
  /*enable timer and fast flag clear*/
  TSCR1 = 0x90;
  
  /*disable timer interrupt, set prescaler to 1*/
  TSCR2 = 0x00;
  
  /*enable OC0*/
  TIOS |= 0x01;
  
  /*1 bus cycle is 1/8E6, thus 1 bus cycle*8000 is 1ms*/
  for(ix = 0; ix < time; ix++){
    while(!(TFLG1_C0F));
      TC0 += 8000; 
  }
  
  /*disable OC0*/
  TIOS &= ~0x01;
}