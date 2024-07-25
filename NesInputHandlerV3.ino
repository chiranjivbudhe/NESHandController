// https://docs.arduino.cc/retired/hacking/software/PortManipulation/
// Port Manipulation.
// PORTC maps to Arduino analog pins 0 to 5.
// Each bit of these registers corresponds to a single pin; 
// e.g. the low bit of DDRB, PORTB, and PINB refers to pin PB0 (digital pin 8)
// PINx x = B, C, or D
// allows are faster read input pin register.
// Reading Latch and Clock
// LATCH BIT IS THE FIRST BIT (LSB) 
//         A5 A4 A3 A2 A1 A0
// PINC & (0  0  0  0  0  1 ), same for the clock 0 0 0 0 1 0
#define readLatch() ((PINC & (1 << LATCH_BIT)) ? HIGH : LOW )
#define readClock() ((PINC & (1 << CLOCK_BIT)) ? HIGH : LOW )

// Writing Data
// for writing data we will use PORTx, x = B, C, or D
// PORTC maps to Arduino analog pins 0 to 5.
// PORTC - The Port C Data Register - read/write
// Writing to data pin = A2, whose bit is at position = 2, ex: //         A5 A4 A3 A2 A1 A0 
#define setButtonState(value) PORTC = (value << DATA_BIT)

// #define waitForClockCycle()  while(readClock() == HIGH);
#define waitForClockCycle()  while(readClock() == HIGH);while(readClock() == LOW) // Clock drops from HIGH to LOW 

#define LATCH_BIT 0
#define CLOCK_BIT 1
#define DATA_BIT 2

#define LATCH_PIN A0
#define CLOCK_PIN A1
#define DATA_PIN A2

#define A_LED 12
#define RIGHT_LED 11
#define LEFT_LED 10

const unsigned long serialBaudRate = 115200;

// LSB BITS A, B, SLCT, START, UP, DOWN, LEFT, RIGHT.
const int TEST_INPUT_LEFT = 191; // 10111111
const int TEST_INPUT_RIGHT = 127; // 01111111
const int TEST_INPUT_RIGHT_JUMP = 126; // 01111110
const int TEST_INPUT_LEFT_JUMP = 190; // 10111110

volatile int CURRENT_INPUT = 255;

void SetTestInputData(){
  CURRENT_INPUT = TEST_INPUT_RIGHT;
}

void InitPins(){
  pinMode(LATCH_PIN, INPUT);
  pinMode(CLOCK_PIN, INPUT);
  pinMode(DATA_PIN, OUTPUT);

  pinMode(A_LED, OUTPUT);
  pinMode(LEFT_LED, OUTPUT);
  pinMode(RIGHT_LED, OUTPUT);

  digitalWrite(A_LED, LOW);
  digitalWrite(LEFT_LED, LOW);
  digitalWrite(RIGHT_LED, LOW);

  digitalWrite(DATA_PIN, HIGH);

  Serial.begin(serialBaudRate);
  Serial.flush();
}

void disableTimers() {
    TCCR0A = 0;
    TCCR0B = 0;
    TCCR1A = 0;
    TCCR1B = 0;
    TCCR2A = 0;
    TCCR2B = 0;
}

void setup()
{
  // put your setup code here, to run once:
  InitPins();
  interrupts();
  disableTimers();
  // noInterrupts();
  SetTestInputData();
}

void loop() {
 HandleInput();
}

void SetButton(int pos)
{
  setButtonState(((CURRENT_INPUT>>pos) & 1));
  // Get the value of the LSB bit
  // byte b = ((CURRENT_INPUT>>pos) & 1);

  // Serial.print("Input : ");
  // Serial.println(b);

  // Set the bit value

  // Shift to get next bit value
  // CURRENT_INPUT = CURRENT_INPUT >> pos;
  // Serial.println("Clock : ");

}

void HandleInput()
{
  
  setButtonState(HIGH);
  if(Serial.available())
  {
    CURRENT_INPUT = Serial.read();
  }
  // CURRENT_INPUT = TEST_INPUT_LEFT_JUMP;

  // If latch is low return
  if(readLatch() != HIGH)
  {
    return;
  }

  // When Latch goes High, Data goes high
  // https://www.raspberryfield.life/2018/09/01/nespi-project-part-4-the-nes-controller-protocol/

  // wait for latch to go low
  while( readLatch() == HIGH);
  // SetButton(0);

  setButtonState(((CURRENT_INPUT>>0) & 1));
  waitForClockCycle();

  // Read clock values
  // Set Button B State
  // SetButton(1);
  setButtonState(((CURRENT_INPUT>>1) & 1));
  waitForClockCycle();

  // Set Button Select State
  // SetButton(2);
  setButtonState(((CURRENT_INPUT>>2) & 1));
  waitForClockCycle();

    // Set Button Start State
  // SetButton(3);
  setButtonState(((CURRENT_INPUT>>3) & 1));
  waitForClockCycle();

    // Set Button Up State
  // SetButton(4);
  setButtonState(((CURRENT_INPUT>>4) & 1));
  waitForClockCycle();

    // Set Button Down State
  // SetButton(5);
  setButtonState(((CURRENT_INPUT>>5) & 1));
  waitForClockCycle();

    // Set Button LEft State
  // SetButton(6);
  setButtonState(((CURRENT_INPUT>>6) & 1));
  waitForClockCycle();

    // Set Button Right State
  // SetButton(7);
  setButtonState(((CURRENT_INPUT>>7) & 1));
  waitForClockCycle();  

  // Data wire goes low right after last clock pulse
  // https://www.raspberryfield.life/2018/09/01/nespi-project-part-4-the-nes-controller-protocol/
  // setButtonState(HIGH); // This doesn't work

  // Serial.flush();
}
