/*
  Name:       Teensy_FMC_R11.ino
  Created:    4/17/20
  Author:     Steven Dobler
*/

/*

Programming the LCD 

    Power Button Function   Output - Teensy Pin ___
    Menu Button FUnction    Output - Teensy Pin ___
    + Button Function       Output - Teensy Pin ___
    - Buton Function        Output - Teensy Pin ___
    S/Auto Button Function  Output - Teensy Pin ___

    �	Power Button	Encoder button press & hold 10 seconds (Powers LCD ON and OFF)
    �	Power Button	Encoder button press & hold 5 seconds (LCD Mode � Flash all LEDs  ON & OFF)
    �	Menu Button 	Encoder Button Press
    �	+ Button	    Encoder rotate right
    �	- Button	    Encoder rotate left
    �	S/auto Button	EXEC button

    State = FMC
    State = LCD
   

*/

/*
To be used with the EasyEDA 737 FMC CDU Rev 3.3 circuit board

CDU physical layout of keypad
         _______________________________________________________________________________________
        |                                                                                       |
        |            _______________________________________________________________            |
        |           |                                                               |           |
        |  [LSK-1]  |                                                               |  [RSK-1]  |
        |  [SW01]   |                                                               |  [SW07]   |
        |           |                                                               |           |
        |  [LSK-2]  |                                                               |  [RSK-2]  |
        |  [SW02]   |                                                               |  [SW08]   |
        |           |                                                               |           |
        |  [LSK-3]  |                                                               |  [RKS-3]  |
        |  [SW03]   |                                                               |  [SW09]   |
        |           |                                                               |           |
        |  [LSK-4]  |                                                               |  [RSK-4]  |
        |  [SW04]   |                                                               |  [SW10]   |
        |           |                                                               |           |
        |  [LSK-5]  |                                                               |  [RSK-5]  |
        |  [SW05]   |                                                               |  [SW11]   |
        |           |                                                               |           |
        |  [LSK-6]  |                                                               |  [RSK-6]  |
        |  [SW06]   |_______________________________________________________________|  [SW12]   |
        |                                                                                       |
        |                                                                     [Encoder]         |
        |                                                                     [   O   ]         |
        |                                                                     [  SW70 ]         |
        |          [INIT  REF]  [   RTE   ]  [  CLB  ]  [  CRZ  ]  [  DES  ]                    |
        |          [   SW13  ]  [  SW14   ]  [  SW15 ]  [  SW16 ]  [  SW17 ]     LED            |
        |                                                                        (1)            |
        |          [  MENU   ]  [  LEGS   ]  [DEP ARR]  [  HOLD ]  [  PROG ]   [ EXEC ]         |
        |          [   SW18  ]  [  SW19   ]  [  SW20 ]  [  SW21 ]  [  SW22 ]   [ SW23 ]         |
        |                                                                                       |
        |          [N1  LIMIT]  [   FIX   ]  [ A  ]   [ B  ]   [ C  ]   [ D  ]  [ E  ]          |
        |          [   SW24  ]  [  SW25   ]  [SW28]   [SW29]   [SW30]   [SW31]  [SW32]          |
        |                                                                                       |
        |          [PREV PAGE]  [NEXT PAGE]  [ F  ]   [ G  ]   [ H  ]   [ I  ]  [ J  ]          |
        |     |C|  [   SW26  ]  [  SW27   ]  [SW33]   [SW34]   [SW35]   [SW36]  [SW37]          |
        | LED |A|                                                                       |M| LED |
        | (3) |L|  [ 1  ]   [ 2  ]   [ 3  ]  [ K  ]   [ L  ]   [ M  ]   [ N  ]  [ O  ]  |S| (5) |
        |     |L|  [SW67]   [SW68]   [SW69]  [SW38]   [SW39]   [SW40]   [SW41]  [SW42]  |G|     |
        |                                                                                       |
        |     |F|  [ 4  ]   [  5 ]   [ 6  ]  [ P  ]   [ Q ]    [ R  ]   [ S  ]  [ T  ]  |O|     |
        | LED |A|  [SW64]   [SW65]   [SW66]  [SW43]   [SW44]   [SW45]   [SW46]  [SW47]  |F| LED |
        | (4) |I|                                                                       |S| (6) |
        |     |L|  [ 7  ]   [  8 ]   [ 9  ]  [ U  ]   [ V  ]   [ W  ]   [ X  ]  [ Y  ]  |T|     |
        |          [SW61]   [SW62]   [SW63]  [SW48]   [SW49]   [SW50]   [SW51]  [SW52]          |
        |                                                                                       |
        |          [ .  ]   [  0 ]   [+/- ]  [ Z  ]   [ SP ]   [DEL ]   [ /  ]  [CLR ]          |
        |          [SW58]   [SW59]   [SW60]  [SW53]   [SW54]   [SW55]   [SW56]  [SW57]          |
        |_______________________________________________________________________________________|

keypad.h library keypad array layout> These are the array's rows and columns, not the keypad's

      { (R,C)...................................(R,C)} R=Row, C=Column
      { (1,1)(1,2)(1,3)(1,4)(1,5)(1,6)(1,7)(1,8)(1,9)}
      { (2,1)...................................(2,9)}
      { (3,1)...................................(3,9)}
      { (4,1)...................................(4,9)}
      { (5,1)...................................(5,9)}
      { (6,1)...................................(6,9)}
      { (7,1)...................................(7,9)}
      { (8,1)(8,2)(8,3)(8,4)(8,5)(8,6)(8,7)(8,8)(8,9)}
*/

#include <Keypad.h>

const   byte ROWS = 8;  // The CDU Keypad has 8 ROW Wires
const   byte COLS = 9;  // The CDU Keypad has 9 COL Wires
char  CodeIn;
int   OFF = 1;      // For the 5 FMC LEDs
int   ON = 0;      // For the 5 FMC LEDs 

int Encoder_Switch_State;

byte keys[ROWS][COLS] = {

    /*               COL1      COL2       COL3       COL4       COL5     COL6     COL7     COL8     COL9
                      |         |          |           |         |        |        |        |        |
         Switch-> [ SW01  ] [ SW09   ] [  SW17  ] [  SW25   ] [ SW33 ] [ SW41 ] [ SW49 ] [ SW57 ] [ SW65 ]
    ROW1 Name  -> [ LSK-1 ] [ RSK-3  ] [   DES  ] [   FIX   ] [   F  ] [  N   ] [  V   ] [ CLR  ] [   5  ] Row 0 */
                  {   1,        2,         3,         4,         5,       6,       7,      8,         9 }, // Keypad Row 1

    /*   Switch-> [ SW02  ] [ SW10   ] [  SW18  ] [  SW26   ] [ SW34 ] [ SW42 ] [ SW50 ] [ SW58 ] [ SW66 ]
    ROW2 Name  -> [ LSK-2 ] [ RSK-4  ] [  MENU  ] [PREV PAGE] [   G  ] [  O   ] [  W   ] [  .   ] [  6   ] Row 1 */
                  {   10,      11,         12,        13,         14,     15,      16,      17,      18  }, // Keypad Row 2

    /*   Switch-> [ SW03  ] [ SW11   ] [  SW19  ] [  SW27   ] [ SW35 ] [ SW43 ] [ SW51 ] [ SW59 ] [ SW67 ]
    ROW3 Name  -> [ LSK-3 ] [ RSK-5  ] [  LEGS  ] [NEXT PAGE] [   H  ] [  P   ] [  X   ] [  0   ] [  1   ] Row 2  */
                  {   19,      20,         21,        22,        23,      24,      25,      26,      27 }, // Keypad Row 3

    /*   Switch-> [ SW04  ] [ SW12   ] [  SW20  ] [  SW28   ] [ SW36 ] [ SW44 ] [ SW52 ] [ SW60 ] [ SW68 ]
    ROW4 Name  -> [ LSK-4 ] [ RSK-6  ] [DEP ARR ] [    A    ] [   I  ] [  Q   ] [  Y   ] [ +/-  ] [  2   ] Row 3  */
                  {   28,      29,        30,         31,        32,      33,      34,     35,       36  }, // Keypad Row 4

    /*   Switch-> [ SW05  ] [ SW13   ] [  SW21  ] [  SW29   ] [ SW37 ] [ SW45 ] [ SW53 ] [ SW61 ] [ SW69 ]
    ROW5 Name  -> [ LSK-5 ] [INIT REF] [  HOLD  ] [    B    ] [  J   ] [  R   ] [  Z   ] [  7   ] [  3   ] Row 4  */
                  {   37,      38,         39,         40,       41,      42,      43,      44,      45  }, // Keypad Row 5

    /*   Switch-> [ SW06  ] [ SW14   ] [  SW22  ] [  SW30   ] [ SW38 ] [ SW46 ] [ SW54 ] [ SW62 ] [ **** ]
    ROW6 Name  -> [ LSK-6 ] [  RTE   ] [  PROG  ] [    C    ] [  K   ] [  S   ] [  SP  ] [  8   ] [ **** ] Row 5  */
                  {   46,      47,         48,         49,       50,      51,      52,      53,      54 }, // Keypad Row 6

    /*   Switch-> [ SW07  ] [ SW15   ] [  SW23  ] [  SW31   ] [ SW39 ] [ SW47 ] [ SW55 ] [ SW63 ] [ **** ]
    ROW7 Name  -> [ RSK-1 ] [  CLB   ] [  EXEC  ] [    D    ] [  L   ] [  T   ] [ DEL  ] [  9   ] [ **** ] Row 6  */
                  {   55,      56,        57,          58,       59,       60,      61,     62,       63 }, // Keypad Row 7

    /*   Switch-> [ SW08  ] [ SW16   ] [  SW24  ] [  SW32   ] [ SW40 ] [ SW48 ] [ SW56 ] [ SW64 ] [ **** ]
    ROW8 Name  -> [ RSK-2 ] [  CRZ   ] [N1 LIMIT] [    E    ] [  M   ] [  U   ] [  /   ] [  4   ] [ **** ] Row 7  */
                  {   64,      65,        66,          67,       68,      69,      70,      71,      72 }  // Keypad Row 8

}; // End of keypad matrix / array

/*
In the above matrix layout the follwoing items are shown:

  - Switch Number
    - See keyboard schematic
  - Key Name
    - This is the legend engraved on the plastic keycap
  - Value
    - The number that will be returned by the keypad.h library when the key is pressed.
    - This is for the AeroSorft Avionics Software http://www.aerosoft.com.au/aerosoft_australia/home.html

The  designation [ **** ] means there is no key switch in that position, however, 222 (arbitrary number) is placed in the
matrix as a placeholder.  The keypad.h library doesn't like zeros in the matrix.

*/


/*

Looking down at the Teensy ++2.0 module, the SOCKET Pins are numbered as shown below:

                          ____________________________
                         |         |  USB  |          |
                    GND  | 1       |       |       40 | +5V
            PWM  PB7 27  | 2       |_______|       39 | 26  PB6  PWM
       PWM INT0  PD0  0  | 3                       38 | 25  PB5  PWM
       PWM INT1  PD1  1  | 4                       37 | 24  PB4  PWM
        RX INT2  PD2  2  | 5                       36 | 23  PB3
        TX INT3  PD3  3  | 6                       35 | 22  PB2
                 PD4  4  | 7                       34 | 21  PB1
                 PD5  5  | 8     Teensy++ 2.0      33 | 20  PB0
       (LED PIN) PD6  6  | 9                       32 | 19  PE7
                 PD7  7  | 10                      31 | 18  PE6
                 PE0  8  | 11                      30 | GND
                 PE1  9  | 12                      29 | AREF
                 PC0  10 | 13                      28 | 38  PF0  A0
                 PC1  11 | 14  PA4 32 O  O 28 PA0  27 | 39  PF1  A1
                 PC2  12 | 15  PA5 33 O  O 28 PA1  26 | 40  PF2  A2
                 PC3  13 | 16  PA6 34 O  O 28 PA2  25 | 41  PF3  A3
            PWM  PC4  14 | 17  PA7 35 O  O 28 PA3  24 | 42  PF4  A4
            PWM  PC5  14 | 18                      23 | 43  PF5  A5
            PWM  PC6  15 | 19                      22 | 44  PF6  A6
                 PC7  16 | 20       O  O  O        21 | 45  PF7  A7
                         | __________________________ |
                                    +  G  R
                                    5  N  S
                                    V  D  T
*/

// Teensy Programming ROW Number:         R1, R2, R3, R4, R5, R6, R7, R8
// Teensy Programming ROW Pin Names:      D6, B7, D0, D1, D2, D3, D4, D5
// Teensy Socket ROW Pin Names:           9,  2,  3,  4,  5,  6,  7,  8
                byte rowPins[ROWS] =    { 6,  27, 0,  1,  2,  3,  4,  5 };

// Teensy Programming COL Number:         C1, C2, C3, C4, C5, C6, C7, C8, C9
// Teensy Programming COL Pin Names:      B4, B3, B2, B0, E7, E6, F0, F1, F5    
// Teensy Socket COL Number:              37, 36, 35, 33, 32, 31, 28, 27, 23
                byte colPins[COLS] =    { 24, 23, 22, 20, 19, 18, 38, 39, 43 };

// Now that the row pins, column pins, number fo rows and number of columns have been defined, create the array
Keypad kpd = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// Assign Constant names and Teensy Pin Numbers for the 5 LEDs on the FMC (EXEC, MSG, DSPY, OFST, FAIL)

// Teensy MGS LED Programming Pin Name:     PC3
// Teensy MGS LED Socket Pin Number:        16
int LED_CDU_MSG = 13;

// Teensy EXEC LED Programming Pin Name:    PC4
// Teensy EXEC LED Socket Pin Number:       17
int LED_CDU_EXEC = 14;

// Teensy DSPY LED Programming Pin Name:    PC5
// Teensy DSPY LED Socket Pin Number:       18   
int LED_CDU_CALL = 15;

// Teensy FAIL LED Programming Pin Name:    PC6
// Teensy FAIL LED Socket Pin Number:       19
int LED_CDU_FAIL = 16;

// Teensy OFST LED Programming Pin Name:    PC7
// Teensy OFST LED Socket Pin Number:       20
int LED_CDU_OFST = 17;

// Teensy Backlight Programming Pin Name:   PB6
// Teensy Backlight Socket Pin Number:       39
int BACK_LIGHT_PWM = 26;


// Assign Constant names and Teensy Pin Numbers for the EC11E18244A5 Rotary Encoder used to dim the pushbutton LEDs
// There are 3 pins on the one side of the encoder with names:  ROTA, GND, ROTB

// Teensy Rotary Encoder Programming Pin Name:    ROTA
// Teensy Rotary Encoder Socket Pin Number:       14
const int EN_ROTB_Pin = 11;  // Analog input pin that the potentiometer is attached to

// Teensy Rotary Encoder Programming Pin Name:    ROTB
// Teensy Rotary Encoder Socket Pin Number:       15
const int EN_ROTA_Pin = 12; // Analog output pin that the LED is attached to

// Assign Constant names and Teensy Pin Numbers for the EC11E18244A5 Rotary Encoder's pushbutton switch
// There are 2 pins on the other side of the encoder with names:  SW, GND

// Teensy Rotary Encoder Programming Pin Name:    SW
// Teensy Rotary Encoder Socket Pin Number:       13
const int EN_SW_Pin = 10;

// Teensy Code DIP Code Select Switch #1  Pin Name: PF2
// Teensy Backlight Socket Pin Number:              40
int CODE_SELECT_SW_1 = 40;

// Teensy Code DIP Code Select Switch #1  Pin Name: PF3
// Teensy Backlight Socket Pin Number:              41
int CODE_SELECT_SW_3 = 41;

// Teensy Code DIP Code Select Switch #1  Pin Name: PF4
// Teensy Backlight Socket Pin Number:              42
int CODE_SELECT_SW_2 = 42;


// These variables are used to toggle all the LEDS ON or OFF
int state = HIGH;      // the current state of the output pin
int reading;           // the current reading from the input pin
int previous = LOW;    // the previous reading from the input pin

// Used for debouncing the pushbutton switch on the rotary encoder used to toggle all LEDs ON or OFF
// The follow variables are long's because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
long time = 0;         // The last time the output pin was toggled
long debounce = 200;   // The debounce time, increase if the output flickers

// Brightness of the backlighted pushbuttons can range from 0 to 255.  This sets initial brightnes at half 
int brightness = 50;

int currentStateCLK;
int previousStateCLK;

String encdir = "";

// These constants won't change. They're used to give names to the pins used:
const int analogInPin = A0;  // Analog input pin that the potentiometer is attached to
const int analogOutPin = 44; // Analog output pin that the LED is attached to

int DIP_SW_1 = 1;
int DIP_SW_2 = 1;
int DIP_SW_3 = 1;


// ----------------------------- Setup Routine -------------------------------

void setup()
{

    // Start serial Port 9600 baud rate
    Serial.begin(9600);

    // Configure the 5 LED pins as Output
    pinMode(LED_CDU_MSG, OUTPUT);
    pinMode(LED_CDU_EXEC, OUTPUT);
    pinMode(LED_CDU_CALL, OUTPUT);
    pinMode(LED_CDU_FAIL, OUTPUT);
    pinMode(LED_CDU_OFST, OUTPUT);

    // Set the 5 LEDs to a known state (OFF)
    digitalWrite(LED_CDU_MSG, OFF);
    digitalWrite(LED_CDU_EXEC, ON); // THIS GOES THROUGH A TRANSITOR TO DRIVE 2 LED SO THE ON / OFF IS REVERSED
    digitalWrite(LED_CDU_CALL, OFF);
    digitalWrite(LED_CDU_FAIL, OFF);
    digitalWrite(LED_CDU_OFST, OFF);

    // Pins used for the 3 DIP switches used to select the code sent to the serial port when keys are pressed
    pinMode(CODE_SELECT_SW_1, INPUT_PULLUP);
    pinMode(CODE_SELECT_SW_2, INPUT_PULLUP);
    pinMode(CODE_SELECT_SW_3, INPUT_PULLUP);

    // Pins used for the Rotary Encoder pins
    pinMode(EN_ROTA_Pin, INPUT_PULLUP);
    pinMode(EN_ROTB_Pin, INPUT_PULLUP);

    // This is the pushbutton switch on the rotary encoder used ot turn all the LEDS ON or OFF
    pinMode(EN_SW_Pin, INPUT_PULLUP);

    // This is the pushbutton switch on the rotary encoder used ot turn all the LEDS ON or OFF
    pinMode(BACK_LIGHT_PWM, OUTPUT);

    previousStateCLK = digitalRead(EN_ROTA_Pin);

    // Write to the pin that controls the backlight LEDs in each pushbutton
    analogWrite(BACK_LIGHT_PWM, brightness);
        
}

char* AeroSoft[] = {

    /*               COL1      COL2       COL3       COL4       COL5     COL6     COL7     COL8     COL9
                      |         |          |           |         |        |        |        |        |
         Switch-> [ SW01  ] [ SW09   ] [  SW17  ] [  SW25   ] [ SW33 ] [ SW41 ] [ SW49 ] [ SW57 ] [ SW65 ]
    ROW1 Name  -> [ LSK-1 ] [ RSK-3  ] [   DES  ] [   FIX   ] [   F  ] [  N   ] [  V   ] [ CLR  ] [   5  ] Row 0  */
                     "1",      "9",       "204",     "211",     "70",     "78",   "86",   "101",    "53" , // Populate Array Row 1

    /*   Switch-> [ SW02  ] [ SW10   ] [  SW18  ] [  SW26   ] [ SW34 ] [ SW42 ] [ SW50 ] [ SW58 ] [ SW66 ]
    ROW2 Name  -> [ LSK-2 ] [ RSK-4  ] [  MENU  ] [PREV PAGE] [   G  ] [  O   ] [  W   ] [  .   ] [  6   ] Row 1  */
                     "2",     "10",       "205",     "212",     "71",    "79",    "87",    "46",    "54"  , // Populate Array Row 2

    /*   Switch-> [ SW03  ] [ SW11   ] [  SW19  ] [  SW27   ] [ SW35 ] [ SW43 ] [ SW51 ] [ SW59 ] [ SW67 ]
    ROW3 Name  -> [ LSK-3 ] [ RSK-5  ] [  LEGS  ] [NEXT PAGE] [   H  ] [  P   ] [  X   ] [  0   ] [  1   ] Row 2  */
                     "3",     "11",       "206",     "213",     "72",    "80",    "88",     "48",   "49"  , // Populate Array Row 3

    /*   Switch-> [ SW04  ] [ SW12   ] [  SW20  ] [  SW28   ] [ SW36 ] [ SW44 ] [ SW52 ] [ SW60 ] [ SW68 ]
    ROW4 Name  -> [ LSK-4 ] [ RSK-6  ] [DEP ARR ] [    A    ] [   I  ] [  Q   ] [  Y   ] [ +/-  ] [  2   ] Row 3  */
                     "4",     "12",      "207",      "65",      "73",    "81",    "89",   "100",    "50"  , // Populate Array Row 4

    /*   Switch-> [ SW05  ] [ SW13   ] [  SW21  ] [  SW29   ] [ SW37 ] [ SW45 ] [ SW53 ] [ SW61 ] [ SW69 ]
    ROW5 Name  -> [ LSK-5 ] [INIT REF] [  HOLD  ] [    B    ] [  J   ] [  R   ] [  Z   ] [  7   ] [  3   ] Row 4  */
                     "5",     "200",      "208",      "66",     "74",    "82",    "90",    "55",    "51"  , // Populate Array Row 5

    /*   Switch-> [ SW06  ] [ SW14   ] [  SW22  ] [  SW30   ] [ SW38 ] [ SW46 ] [ SW54 ] [ SW62 ] [ **** ]
    ROW6 Name  -> [ LSK-6 ] [  RTE   ] [  PROG  ] [    C    ] [  K   ] [  S   ] [  SP  ] [  8   ] [ **** ] Row 5  */
                     "6",     "201",      "209",      "67",     "75",    "83",    "32",    "56",    "222" , // Populate Array Row 6

    /*   Switch-> [ SW07  ] [ SW15   ] [  SW23  ] [  SW31   ] [ SW39 ] [ SW47 ] [ SW55 ] [ SW63 ] [ **** ]
    ROW7 Name  -> [ RSK-1 ] [  CLB   ] [  EXEC  ] [    D    ] [  L   ] [  T   ] [ DEL  ] [  9   ] [ **** ] Row 6  */
                     "7",     "202",     "214",       "68",     "76",    "84",   "102",    "57",    "222" , // Populate Array Row 7

    /*   Switch-> [ SW08  ] [ SW16   ] [  SW24  ] [  SW32   ] [ SW40 ] [ SW48 ] [ SW56 ] [ SW64 ] [ **** ]
    ROW8 Name  -> [ RSK-2 ] [  CRZ   ] [N1 LIMIT] [    E    ] [  M   ] [  U   ] [  /   ] [  4   ] [ **** ] Row 7  */
                     "8",     "203",     "210",       "69",     "77",    "85",    "47",    "52",    "222"  // Populate Array Row 8
};

char* KeyName[] = {

    /*               COL1      COL2       COL3       COL4       COL5     COL6     COL7     COL8     COL9
                      |         |          |           |         |        |        |        |        |
         Switch-> [ SW01  ]   [ SW09  ]  [  SW17 ] [  SW25   ] [ SW33 ] [ SW41 ] [ SW49 ] [ SW57 ] [ SW65 ]
    ROW1 Name  -> [ LSK-1 ]   [ RSK-3 ]  [  DES  ] [   FIX   ] [   F  ] [  N   ] [  V   ] [ CLR  ] [   5  ] Row 0 */
                  "[LSK-1]",  "[RSK-3]",  "[DES]",    "[FIX]",  "[ F ]", "[ N ]", "[ V ]", "[CLR]",  "[ 5 ]" , // Populate Array Row 1

    /*   Switch-> [ SW02  ]   [ SW10   ]  [  SW18  ] [  SW26   ] [ SW34 ] [ SW42 ] [ SW50 ] [ SW58 ] [ SW66 ]
    ROW2 Name  -> [ LSK-2 ]   [ RSK-4  ]  [  MENU  ] [PREV PAGE] [   G  ] [  O   ] [  W   ] [  .   ] [  6   ] Row 1  */
                  "[LSK-2]",  "[RSK-4]",  "[MENU]", "[PREV PAGE]","[ G ]", "[ O ]", "[ W ]", "[ . ]",  "[ 6 ]", // Populate Array Row 2

    /*   Switch-> [ SW03  ]   [ SW11   ]  [  SW19  ] [  SW27   ] [ SW35 ] [ SW43 ] [ SW51 ] [ SW59 ] [ SW67 ]
    ROW3 Name  -> [ LSK-3 ]   [ RSK-5  ]  [  LEGS  ] [NEXT PAGE] [   H  ] [  P   ] [  X   ] [  0   ] [  1   ] Row 2  */
                  "[LSK-3]",  "[RSK-5]",  "[LEGS]", "[NEXT PAGE]","[ H ]","[ P ]", "[ X ]", "[ 0 ]",  "[ 1 ]", // Populate Array Row 3

    /*   Switch-> [ SW04  ]  [ SW12   ]   [  SW20  ]  [  SW28  ] [ SW36 ] [ SW44 ] [ SW52 ] [ SW60 ] [ SW68 ]
    ROW4 Name  -> [ LSK-4 ]  [ RSK-6  ]   [DEP ARR ]  [   A    ] [  I   ] [  Q   ] [  Y   ] [ +/-  ] [  2   ] Row 3  */
                 "[LSK-4]",  "[RSK-6]",  "[DEP ARR]",  "[ A ]",  "[ I ]", "[ Q ]", "[ Y ]", "[ +/- ]",  "[ 2 ]"  , // Populate Array Row 4

    /*   Switch-> [ SW05  ]   [  SW13  ]  [  SW21  ] [  SW29   ] [ SW37 ] [ SW45 ] [ SW53 ] [ SW61 ] [ SW69 ]
    ROW5 Name  -> [ LSK-5 ]   [INIT REF]  [  HOLD  ] [    B    ] [  J   ] [  R   ] [  Z   ] [  7   ] [  3   ] Row 4  */
                  "[LSK-5]", "[INIT REF]","[HOLD]",    "[ B ]",  "[ J ]", "[ R ]", "[ Z ]", "[ 7 ]", "[ 3 ]"  , // Populate Array Row 5

    /*   Switch-> [ SW06  ]  [  SW14  ]  [  SW22  ] [  SW30   ] [ SW38 ] [ SW46 ] [ SW54 ] [ SW62 ] [ **** ]
    ROW6 Name  -> [ LSK-6 ]  [  RTE   ]  [  PROG  ] [    C    ] [  K   ] [  S   ] [  SP  ] [  8   ] [ **** ] Row 5  */
                  "[LSK-6]",  "[RTE]",    "[PROG]",   "[ C ]",  "[ K ]", "[ S ]",  "[SP]", "[ 8 ]",  "[  ]", // Populate Array Row 6

    /*   Switch-> [ SW07  ]  [  SW15  ] [  SW23  ] [  SW31   ] [ SW39 ] [ SW47 ] [ SW55 ] [ SW63 ] [ **** ]
    ROW7 Name  -> [ RSK-1 ]  [  CLB   ] [  EXEC  ] [    D    ] [  L   ] [  T   ] [ DEL  ] [  9   ] [ **** ] Row 6  */
                  "[RSK-1]",  "[CLB]",   "[EXEC]",   "[ D ]",  "[ L ]", "[ T ]", "[DEL]", "[ 9 ]",  "[  ]" , // Populate Array Row 7

    /*   Switch-> [ SW08  ]  [ SW16   ] [  SW24  ] [  SW32   ] [ SW40 ] [ SW48 ] [ SW56 ] [ SW64 ] [ **** ]
    ROW8 Name  -> [ RSK-2 ]  [  CRZ   ] [N1 LIMIT] [    E    ] [  M   ] [  U   ] [  /   ] [  4   ] [ **** ] Row 7  */
                  "[RSK-2]", "[CRZ]",  "[N1 LIMIT]", "[ E ]",  "[ M ]", "[ U ]", "[ / ]", "[ 4 ]",  "[  ]"  // Populate Array Row 8
};

// ----------------------------- Loop Routine -------------------------------

void loop() {

    // Read Code DIP Switches
    DIP_SW_1 = digitalRead(CODE_SELECT_SW_1);
    DIP_SW_2 = digitalRead(CODE_SELECT_SW_2);
    DIP_SW_3 = digitalRead(CODE_SELECT_SW_3);

    // Check the keypad for button pressed

    byte key = kpd.getKey();

    if (key != NO_KEY)
    {
        Serial.println("");  // Insert blank line before next print statement

        if (DIP_SW_1 == 0)
        {
            key = key - 1;  // The KeyName and Aerosoft arrays start at "0"
            Serial.print("FMC Key Name:            ");
            Serial.println(KeyName[key]);
        }

        if (DIP_SW_2 == 0)
        {
            key = key + 1;
            Serial.print("Keypad Array Number:   ");
            Serial.println(key);
        }

        if (DIP_SW_3 == 0)
        {
            key = key - 1;  // The KeyName and Aerosoft arrays start at "0"
            Serial.print("AeroSoft Key Value:       ");
            Serial.println(AeroSoft[key]);
        }

       
        
    }

    // Start getting characters from the FSX lua script

    if (Serial.available() > 0)
    {
        CodeIn = getChar(); // read it by the "char getChar" routine

  // The < character signifies that the lua script wants to turn on an LED
        if (CodeIn == '<')
        {
            LESSTHAN(); //Identifier character it "<" goto LESSTHAN void
        }
        // The > character signifies that the lua script wants to turn on an LED                    
        if (CodeIn == '>')
        {
            MORETHAN(); //Identifier character it ">" goto MORETHAN void
        }
    } // end of if serial.available()


  // Check to see if the rotary encoder switch was pushed
    TOGGLE_LEDS();

    // Check for rotary encoder
    CHECK_ENCODER();

}// end of void loop


// ----------------------------- Subroutine getChar -------------------------------

char getChar() //  The serial buffer routine to get a character
{
    while (Serial.available() == 0); // wait for data
    return ((char)Serial.read());
} // end of getchar routine


/* ------------------------------ Subroutine LESSTHAN ------------------------------

When the flight simulator wants to control the LEDs via a .lua script it first sends
 a less than (<) character followed by a single character as shown below:

    MSG  LED  = Capital Letter M
    EXEC LED  = Capital Letter E
    CALL LED  - Capital Letter D
    FAIL LED  = Capital Letter F
    OFST LED  = Capital Letter O
*/

void LESSTHAN() // For turning LEDs "ON"
{
    char CodeIn = getChar();  // get another character from serial port

      // Look for MSG LED  on / off characters

    if (CodeIn == 'M')  // found identifier for "M" for MSG LED
    {
        digitalWrite(LED_CDU_MSG, ON); // Turn on LED
    }

    // Look for EXEC LED on / off characters

    if (CodeIn == 'E') // found identifier for "E" for EXEC LED
    {
        digitalWrite(LED_CDU_EXEC, OFF); // THIS GOES THROUGH A TRANSITOR TO DRIVE 2 LED SO THE ON / OFF IS REVERSED
    }

    // Look for CALL LED on / off characters
    if (CodeIn == 'C') // found identifier for "D" for CALL LED
    {
        digitalWrite(LED_CDU_CALL, ON); // Turn on LED
    }

    // Look for FAIL LED on / off characters

    if (CodeIn == 'F') // found identifier for "F" for FAIL LED
    {
        digitalWrite(LED_CDU_FAIL, ON); // Turn on LED
    }

    // Look for OFST LED on / off characters

    if (CodeIn == 'O') // found identifier for "O" for OFST LED
    {
        digitalWrite(LED_CDU_OFST, ON); // Turn Off LED
    }

    if (CodeIn == 'Z') // found identifier for "Z" to turn ON all LEDS
    {
        digitalWrite(LED_CDU_MSG, ON);
        digitalWrite(LED_CDU_EXEC, OFF);// THIS GOES THROUGH A TRANSITOR TO DRIVE 2 LED SO THE ON / OFF IS REVERSED
        digitalWrite(LED_CDU_CALL, ON);
        digitalWrite(LED_CDU_FAIL, ON);
        digitalWrite(LED_CDU_OFST, ON);

    }
    state = HIGH; // Set this HIGH so next time the Rotary Encoder Switch is pressed it will turn all LEDs OFF

}  // end of void LESSTHAN() 


/* ------------------------------ Subroutine MORETHAN ------------------------------

When the flight simulator wants to control the LEDs via a .lua script it first sends
 a greater than (>) character followed by a single character as shown below:

    MSG  LED  = Capital Letter M
    EXEC LED  = Capital Letter E
    CALL LED  - Capital Letter D
    FAIL LED  = Capital Letter F
    OFST LED  = Capital Letter O
*/

void MORETHAN()// For turning LEDs "OFF"
{

    char CodeIn = getChar();  // get another character from serial port

      // Look for MSG LED  on / off characters

    if (CodeIn == 'M')  // found identifier for "M" for MSG LED
    {
        digitalWrite(LED_CDU_MSG, OFF); // Turn off LED
    }

    // Look for EXEC LED on / off characters

    if (CodeIn == 'E') // found identifier for "E" for EXEC LED
    {
        digitalWrite(LED_CDU_EXEC, ON); // THIS GOES THROUGH A TRANSITOR TO DRIVE 2 LED SO THE ON / OFF IS REVERSED
    }

    // Look for CALL LED on / off characters
    if (CodeIn == 'C') // found identifier for "C" for CALL LED
    {
        digitalWrite(LED_CDU_CALL, OFF); // Turn off LED
    }

    // Look for FAIL LED on / off characters

    if (CodeIn == 'F') // found identifier for "F" for FAIL LED
    {
        digitalWrite(LED_CDU_FAIL, OFF); // Turn off LED
    }

    // Look for OFST LED on / off characters

    if (CodeIn == 'O') // found identifier for "O" for OFST LED
    {
        digitalWrite(LED_CDU_OFST, OFF); // Turn Off LED
    }

    // Look for character to turn off ALL LEDS

    if (CodeIn == 'X') // found identifier for "X" to turn off all LEDS
    {
        digitalWrite(LED_CDU_MSG, OFF);
        digitalWrite(LED_CDU_EXEC, ON);// THIS GOES THROUGH A TRANSITOR TO DRIVE 2 LED SO THE ON / OFF IS REVERSED
        digitalWrite(LED_CDU_CALL, OFF);
        digitalWrite(LED_CDU_FAIL, OFF);
        digitalWrite(LED_CDU_OFST, OFF);

    }
    state = LOW; // Set this LOW so next time the Rotary Encoder Switch is pressed it will turn all LEDs ON

}  // end of void MORETHAN()


void TOGGLE_LEDS()// For turning LEDs "OFF" and "ON"

{
    reading = digitalRead(EN_SW_Pin);
    // if the input just went from LOW and HIGH and we've waited long enough
    // to ignore any noise on the circuit, toggle the output pin and remember
    // the time
    if (reading == HIGH && previous == LOW && millis() - time > debounce)
    {
        if (state == HIGH)
        {
            state = LOW;
            digitalWrite(LED_CDU_MSG, !state);
            digitalWrite(LED_CDU_EXEC, state);// THIS GOES THROUGH A TRANSISTOR TO DRIVE 2 LED SO THE ON / OFF IS REVERSED
            digitalWrite(LED_CDU_CALL, !state);
            digitalWrite(LED_CDU_FAIL, !state);
            digitalWrite(LED_CDU_OFST, !state);
        }
        else

        {
            state = HIGH;
            digitalWrite(LED_CDU_MSG, !state);
            digitalWrite(LED_CDU_EXEC, state);// THIS GOES THROUGH A TRANSISTOR TO DRIVE 2 LED SO THE ON / OFF IS REVERSED
            digitalWrite(LED_CDU_CALL, !state);
            digitalWrite(LED_CDU_FAIL, !state);
            digitalWrite(LED_CDU_OFST, !state);
        }

        time = millis();
    }
    previous = reading;

}// end of void TOGGLE_LEDS()


void CHECK_ENCODER()
{
    // Read the current state of inputCLK
    currentStateCLK = digitalRead(EN_ROTA_Pin);

    // If the previous and the current state of the inputCLK are different then a pulse has occured
    if (currentStateCLK != previousStateCLK) {

        // If the inputDT state is different than the inputCLK state then 
        // the encoder is rotating counterclockwise
        if (digitalRead(EN_ROTB_Pin) != currentStateCLK) {

            brightness = brightness - 10;
            if (brightness < 0)
            {
                brightness = 0;
            }
            encdir = "CCW";

        }
        else {
            // Encoder is rotating clockwise

            brightness = brightness + 5;
            if (brightness > 255)
            {
                brightness = 255;
            }
            encdir = "CW";

        }
        // This is for debug purposes to see value from the rotary encoder to control the backlight LED brigtness
        Serial.print("Direction: ");
        Serial.print(encdir);
        Serial.print(" -- Value: ");
        Serial.println(brightness);

        // Write to the pin that controls the backlight LEDs in each pushbutton
        analogWrite(BACK_LIGHT_PWM, brightness);

    }
    // Update previousStateCLK with the current state
    previousStateCLK = currentStateCLK;
}