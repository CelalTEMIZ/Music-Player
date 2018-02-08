#include <hidef.h> /* common defines and macros */
#include "derivative.h" /* derivative-specific definitions */



// 7 Song Notes                      
#define _DO	 45977
#define _RE	 45570
#define _MI	 36363
#define _FA  34280
#define _SOL 30612
#define _LA	 27272
#define _SI	 24490


// Song List
int firstSong[15]  = {_LA,_LA,_SOL,_LA,_LA,_SOL,_SOL,_LA,_SOL,_LA,_LA,_LA,_SOL,_SI,_DO };  // 15 notes to each song
int secondSong[15] = {_DO,_RE,_MI,_FA,_SOL,_LA,_SI,_DO,_RE,_MI,_DO,_RE,_MI,_FA,_SOL };     // And each note 1 sn duration
int thirdSong[15]  = {_LA,_LA,_SOL,_LA,_SOL,_LA,_SI,_DO,_RE,_MI,_SI,_FA,_DO,_SI,_FA};


// Global Value to all changing notes
int musicNote = 0;
unsigned char readValueSC;
unsigned char overflow = 0;
 
void songPlay(int); 
// Interrupt for Output Compare            
interrupt(((0x10000 - Vtimch5)/2)-1) void TC5_ISR(void);

// Interrupt for Serial Communication
interrupt(((0x10000 - Vsci0)/2)-1) void SCI0_ISR(void);

// Interrupt for Timer Overflow 
interrupt(((0x10000 - Vtimovf)/2)-1) void TOF_ISR(void);


void SCI0_INIT(void)
{
 
 SCI0BDH = 0x00;  // Must be assigned
 SCI0BDL = 26;    // Set 9600 baud rate to new boards
 SCI0CR1 = 0x00;  // 8 Bit, no parity
 SCI0CR2 = 0xAC;  // Enable trans, recevive (0x04), send(0x08), receive+send (0x0C) 
                  // And Interrupt
}


// Interrupt Service Routine to Output Compare 
interrupt(((0x10000 - Vtimch5)/2)-1) void TC5_ISR(void)
{
 
 TC5   = TC5 + musicNote;             // Update to TC5 
 TFLG1 = 0x20;                        // Clear C5F

}

// Interrupt Service Routine to Serial Communication
interrupt (((0x10000-Vsci0)/2)-1) void SCI0_ISR(void)
{
  
   if(SCI0SR1_RDRF_MASK & SCI0SR1)   // Wait to Receive
     readValueSC = SCI0DRL;          // Save Value
}



// Interrupt Service Routine to Timer Overflow 
interrupt(((0x10000 - Vtimovf)/2)-1) void TOF_ISR(void) 
{
  overflow++;                       // Increment Count
  TFLG2 = TFLG2 | TFLG2_TOF_MASK;   // Clear Flag
}


//
// TCNT max value is 65.536
//
// So 1 cyle is equal to 65.536 / 24mHz 
//
// 1 cyle = 65.536/24.000.000 = 2.73 ms
//

// Timer Counter to 1 sn
// 1s delay implemented by using Timer Overflow Property of Timer Module.

void delay_1sn(void) 
{

 int i;
 
 TSCR1 = 0x80;                            // Enable Timer
 TSCR2 = 0x00;                            // No interrupt, No Prescale
                                          // Count Value
 for(i=0; i<366; ++i) {                  
  TFLG2 = 0x80;                           // Clear TOF
  while(!(TFLG2 & TFLG2_TOF_MASK));       // Wait for overflow flag to be raised 
 }
 
}

// 5s delay implemented by using Timer Overflow Property of Timer Module.
// Timer Counter to 5 sn
// 
//    2.73  ms    ==  65536
//    5.000 ms    == 120029304,029304 is bigger than 65536
//
// Thus, count value is eqaul to 120029304 / 65536 = 1831.5018

void delay_5sn(void) 
{

 int i;

 TSCR1 = 0x80;                            // Enable Timer 
 TSCR2 = 0x00;                            // No interrupt, No Prescale
 
 for(i=0; i<1832; ++i) {                  // Count Value
 
  TFLG2 = 0x80;                           // Clear TOF
 
  while(!(TFLG2 & TFLG2_TOF_MASK));       // Wait for overflow flag to be raised
 }
 
}

// 10s delay implemented by using Timer Overflow Property of Timer Module.
// Timer Counter to 10 sn
// 
// 2.73  ms   ==    65536
// 10.000 ms  ==    240058608,058660 is bigger than 65536
//
// Thus, count value is equal to 240058608,058660 / 65536 = 3663.0036

void delay_10sn(void) 
{

 int i;
 TSCR1 = 0x80;                        // Timer Enable
 TSCR2 = 0x00;                        // No Interrupt, No Prescale
 
 for(i=0; i<3663; ++i) {              // Count Value
 
  TFLG2 = 0x80;                       // Clear TOF
 
  while(!(TFLG2 & TFLG2_TOF_MASK));   // Wait for overflow flag to be raised
 }
 
}


//////////////////////////// Main Function //////////////////////////////////////////////

void main(void) 
{
    
    
    int i;              // Counter to song number
    
    __asm(sei);         //  Disable Interrupts Globally 
    DDRB = 0xFF;        //  PORTB Output
    DDRH = 0x00;        //  PORTH Input
    DDRP = 0xFF;        //  PORTP Output to Seven Segment Display
    PTP  = 0x07;        //  Write number of the song on 7-segment LEDs.
      
  
    TSCR1 = 0x80;       // Timer Enable
    TIOS  = 0x20;       // Select Channel 5 for output compare
    TCTL1 = 0x04;       // Toggle PT5 pin upon match
    TIE   = 0x20;       // Enable Interrupt for Channel 5
    TFLG1 = 0x20;       // Clear C5F
    __asm(cli);          // Enable Interrupts Globally
   
   
/////////////////////////////////  Sound All Songs   //////////////////////////////////////////////////
//                                                                                                   //
//            If 00 is selected, play all three songs and each for 5 seconds.                        //
//                                                                                                   //
////////////////////////////////////////////////////////////////////////////////////////////////////////


      if(PTH == 0x00)                       
      {                                     
        
            PORTB = 0x3F;    
         
            for(i=0; i<5; ++i){             // Song 1 Operation , Play Song 5s
               	musicNote = firstSong[i];   // Song value to global variable
      	      	PORTB = firstSong[i];       // Light PORTB LEDs according to the music.	
      		      PORTB = 0x06;               // Write 1 number to number of song on 7-segment LEDs.
                delay_1sn();                // Delay duration
            }
            
            for(i=0; i<5; ++i){             // Song 2 Operation , Play Song 5s
               musicNote = secondSong[i];   // Song value to global variable
      	       PORTB = secondSong[i];       // Light PORTB LEDs according to the music.
      		     PORTB = 0x5B;                // Write 2 number to number of song on 7-segment LEDs.
               delay_1sn();                 // Delay duration
      
            }
            
            for(i=0; i<5; ++i)   {         // Song 3 Operation , Play Song 5s
               	musicNote = thirdSong[i];
      	      	PORTB = thirdSong[i];       // Light PORTB LEDs according to the music.
      		      PORTB = 0x4F;               // Write 3 number to number of song on 7-segment LEDs.
                delay_1sn();                // Delay duration  
            }
      
      }
      
///////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                   //
// Play 3 different songs for 15 seconds according to what is selected at the two least significant  //
// DIP switches (00, 01, 10, 11).                                                                    //
//                                                                                                   //
///////////////////////////////////////////////////////////////////////////////////////////////////////
      
      if(PTH == 0x01){                  // If PTH value is 1, then sound first song
         songPlay(1);                   // Call Sound Function
      }
     
         
      if(PTH == 0x02){                  // If PTH value is 2, then sound second song
         songPlay(2);                   // Call Sound Function to second song
      }
           
      if(PTH == 0x03)  {                // If PTH value is 3, then sound third song
        songPlay(3);                    // Call Sound Function to third song
      }
      
     // End 
  
} 




//
// Sound Function
// Play 3 different songs for 15 seconds according to what is selected 
// at the two least significant DIP switches (00, 01, 10, 11).
//

void songPlay(int songNumber)
{
  int i;
	
    	if(songNumber == 1)                      // PTH = 00000001 
    	{   
    	
    	  	for(i = 0; i < 15; ++i){                             
        		musicNote = firstSong[i];         // Sound First Song
        	  PORTB = musicNote;                // Light PORTB LEDs according to the music.
        	  PORTB = 0x06;                     // Write 1 number to number of song on 7-segment LEDs.
        		delay_1sn();	                    // Delay duration
    	  	}
    	}
    	
    	
    	if (songNumber == 2)                    // PTH = 00000010
    	{  
      	  for(i = 0; i < 15; ++i){            
              musicNote = secondSong[i];      // Sound Second Song
              PORTB = musicNote;	            // Light PORTB LEDs according to the music.
              PORTB = 0x5B;                   // Write 2 number to number of song on 7-segment LEDs.
          		delay_1sn();                    // Delay duration
      	  }
    	}
    	
      if (songNumber == 3)                    //  PTH = 00000011
    	{ 
      	  for(i = 0; i < 15; ++i){ 
              musicNote = thirdSong[i];       //  Sound Third Song
          	 	PORTB = musicNote;              //  Light PORTB LEDs according to the music.
          	 	PORTB = 0x4F;                   //  Write 2 number to number of song on 7-segment LEDs.
              delay_1sn();                    //  Delay duration
      	  }
    	}
  	
}


