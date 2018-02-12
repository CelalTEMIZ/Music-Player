# Music Player on HCS12 Dragon Board

  - Requirements
    - HCS12 Dragon Board
    - Freescale CodeWarrior Development Studio 5.9.0
    
  - Program is implemented with C programming language. 
  - Plays songs on PTT5 Buzzer.
  - Control PTH and get the value from PTH.
  - Plays 3 different songs for 15 seconds according to what is selected at the two least significant DIP switches (00, 01, 10, 11).
  - Light PORTB LEDs according to the music. (It is up to you but at least you can get use of the frequencies, i.e. notes.)
  - Shows the number of the song on 7-segment LEDs.
  - If 00 is selected, play all three songs and each for 5 seconds.
  - 10s and 5s delay will be implemented by using Timer Overflow Property of Timer Module.
  - The different frequencies are generated using Output Compare Property of Timer Module.
  
  
  - Demo 
    - https://www.youtube.com/watch?v=mUHa09iKu2A
