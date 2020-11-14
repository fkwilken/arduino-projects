
#include <CPutil.h>
#include "Bluetooth.h"

int statePin = 0;

void bluetoothSetup(int bluetoothStatePin)
{
	// Bluetooth setup
	statePin = bluetoothStatePin;
	Serial1.begin(9600);
	pinMode(statePin, INPUT);

}

void monitorBluetoothConnection()
{
	// make sure still connected via bluetooth
	if (digitalRead(statePin) == LOW)
	{
		connectToBluetooth(); // this blocks
	}
}

void connectToBluetooth()
{
  //  blocks until connected via bluetooth
  Serial.println("Blocking - Waiting to Connect to Bluetooth (AT commands should work)");
  Serial.println("For AT commands set the serial console to: Both NL & CR");
  
	while (digitalRead(statePin) == LOW)
	{
		// even though bluetooth is down still want to process AT commands
		atCommandProcessing(); 
	}  // warning blocking while loop

	Serial.println("Connected... (AT commands will NOT work now)");

}

void atCommandProcessing()
{
	// Use for AT commands

	char aChar = 0;

	// check serial console for AT command and send to bluetooth (Serial1)
	if (Serial.available()) 
	{      
		aChar = Serial.read();
		Serial1.write(aChar);   
		Serial.print(aChar);    // also echo out to serial 
	}

	// Check Bluetooth respone and send to Serial
	if (Serial1.available()) 
	{        
		Serial.write(Serial1.read());   
	}
	
}
