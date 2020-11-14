
// Hugh Smith, November 2020
// Use at your own risk
// Passes data to and receives data from a bluetooth module (really any serial
// device on Serial1)
// Note: for AT commands to work, set serial console to "Both NL & CR"

#include "IRremote_v2.h"
#include "CPutil.h"
#include "Bluetooth.h"
#include "Screen.h"
// characters

#define SPACE_ICON ' '
#define COLLISION_ICON 'X'
#define BLOCK_ICON 1
#define SMILEY_ICON 2
#define BODY_ICON_1 3
#define BODY_ICON_2 4
#define MIN_BLOCK 1
#define MAX_BLOCK 3.
#define MIN_SPACE 2

const int buttonPin = 24;
const int statePin = 33;
Button button(buttonPin);

// DO NOT create an LCD - Screen contains this.
Screen aScreen;


// Define pins
const int bluetoothStatePin = 33;

// Define your pins (any Digital I/O pin should work for receiving)
const int irRecieverPin = 22;

// Create your IR stuff
IRrecv irrecv(irRecieverPin);    
decode_results irResults; 


void setup()
{
  Serial.begin(9600);
  irrecv.enableIRIn(); // Start the receiver
  setupMessage(__FILE__, "Read/Write to Serial to communciate with Bluetooth module");
  delay(500);

  bluetoothSetup(bluetoothStatePin);

  // Set up serial monitor and print out program info


  // Set up our random numbers
  // randomSeed(analogRead(20));
  randomSeed(micros());


  //setupBluetooth();

  aScreen.init(SPACE_ICON);
  iconSetup();
  delay(500);

}

void loop()
{
  control();

}


void control()
{
  static MSTimer updateTimer(200);
  static int gameOn = true;
  char bodyIcon = BODY_ICON_1;
  int bodyInRow = 0;
  int score = 0;

  bodyInRow = processButtonGetRow();

  if (updateTimer.done() && gameOn)
  {
    updateTimer.set(200);

    //    // model
    shiftRowsLeft();
    score = calcScore(bodyInRow);
    //
    //    // View
    aScreen.printRowsToLcd();
    aScreen.writeChar(1, bodyInRow, bodyIcon);
    displayScore(score);

    gameOn = collisionDetection(bodyInRow);
  }
}

int collisionDetection(int bodyRow)
{
  int returnValue = true;
  int icon = 0;
  icon = aScreen.getIconAtColumnAtRow(1, bodyRow);
  if (icon == BLOCK_ICON)
  {
    aScreen.setCursor(1, bodyRow);
    aScreen.print(COLLISION_ICON);
    returnValue = false;
  }
  return returnValue;
}

int calcScore(int bodyRow)
{
  static int score = 0;
  int icon = 0;
  icon = aScreen.getIconAtColumnAtRow(1, getOtherRow(bodyRow));
  if (icon == BLOCK_ICON)
  {
    score++;
  }
  return score;
}

int processButtonGetRow()
{
  static int currentRow = 0;
  if (button.wasPushed(300) == true || getBluetoothInput() == true || irReceiving() == true)
  {
    currentRow = getOtherRow(currentRow);
  }
  return currentRow;
}

void shiftRowsLeft()
{
  static int blockCount = 0;
  static int blockTotal = 3;
  blockTotal = random(MIN_BLOCK, MAX_BLOCK);
  static int blockRow = 1;
  enum {START, BLOCKS, SPACES};
  static int state = START;
  switch (state)
  {
    case START:
      state = BLOCKS;
      break;

    case BLOCKS:
      if (blockCount < blockTotal)
      {
        aScreen.addToEndOfRow(blockRow, BLOCK_ICON);
        blockCount++;
        state = BLOCKS;
      }
      else if (blockCount == blockTotal)
      {
        blockCount = 1;
        blockTotal = random(MIN_SPACE, MAX_BLOCK);
        state = SPACES;
      }
      break;

    case SPACES:
      if (blockCount < blockTotal)
      {
        blockCount++;
        state = SPACES;
      }
      else if (blockCount == blockTotal)
      {
        blockRow = getOtherRow(blockRow);
        aScreen.addToEndOfRow(blockRow, BLOCK_ICON);
        blockCount = 1;
        blockTotal = random(MIN_BLOCK, MAX_BLOCK);
        state = BLOCKS;
      }
      break;
  }
  aScreen.shiftLeft();
}
void displayScore(int score)
{
  if (score < 10)
  {
    aScreen.setCursor(15, 0);
    aScreen.print(score);
  }
  else
  {
    aScreen.setCursor(14, 0);
    aScreen.print(score);
  }
}

int getOtherRow(int row)
{
  int returnValue = 0;
  if (row == 0)
  {
    returnValue = 1;
  }
  return returnValue;
}


void  startGameWithButton()
{
  // warning blocks on button
  aScreen.clear();
  aScreen.print("Press Button");
  aScreen.setCursor(0, 1);
  aScreen.print("To Start");
  while (button.wasPushed() != true)
  {}
}

void iconSetup()
{
  byte block[8] = {0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f};
  byte smiley[8] = {0x00, 0x11, 0x00, 0x00, 0x11, 0x0E, 0x00};
  byte body1[8] = {B01110, B01110, B00100, B01101, B10110, B00100, B01010, B01010};
  byte body2[8] = {B01110, B01110, B10100, B01100, B00110, B00101, B01010, B01010};

  aScreen.createChar(BLOCK_ICON, block);
  aScreen.createChar(SMILEY_ICON, smiley);
  aScreen.createChar(BODY_ICON_1, body1);
  aScreen.createChar(BODY_ICON_2, body2);
}

int getBluetoothInput()
{
  int returnValue = 0;

  // Check Bluetooth, send to Serial
  if (Serial1.available())
  {
    if (Serial1.read() == '1')
    {
      returnValue = 1;
    }
  }
  return returnValue;
}

void setupBluetooth()
{
  aScreen.init(SPACE_ICON);
  aScreen.println("Bluetooth Needed");
  bluetoothSetup(statePin);
  connectToBluetooth(); // this blocks
}

int irReceiving()
{
  unsigned long irResult = 0;
  int returnValue = false;
  if (irrecv.decode(&irResults))
  {
    irResult = irResults.value;

    if (irResult == 0xFF02FD)
    {
     returnValue = true; 
    }
    //delay(10);
    irrecv.resume();
  }
  return returnValue;
}
