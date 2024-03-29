/*
 Name:		Drone_Flight_Controller.ino
 Created:	11/10/2016 8:56:34 PM
 Author:	SIDUS
 Description: This is the main code for Drone_Flight_Controller Project
*/
//Local include class and files
#include "Local_Agenda.h"
#include "Config.h"
#include "cMsgT01.h"
#include "cMsgR01.h"
#include "cSerialParse.h"
//Global Class Definitions
Agenda scheduler;
cMsgT01 MsgT01;
cMsgR01 MsgR01;
cSerialParse serialParse(sizeof(MsgT01.message), MsgT01.message.startChar1, MsgT01.message.startChar2, MsgT01.message.endChar);
HardwareSerial Serial2(2);
// the setup function runs once when you press reset or power the board
void setup() {
	Serial.begin(921600);
	Serial2.begin(921600);
	
	//Configure all PINs
	pinMode(PIN_LED, OUTPUT);
	pinMode(PIN_THR, INPUT);
	pinMode(PIN_PITCH, INPUT);
	pinMode(PIN_ROLL, INPUT);
	pinMode(PIN_YAW, INPUT);


	attachInterrupt(PIN_THR, isrTHR, CHANGE);
	attachInterrupt(PIN_PITCH, isrPITCH, CHANGE);
	attachInterrupt(PIN_ROLL, isrROLL, CHANGE);
	attachInterrupt(PIN_YAW, isrYAW, CHANGE);
	

	//Insert all tasks into scheduler
	scheduler.insert(test_task, 500000);
	scheduler.insert(serialCheck, 15000);

}

// the loop function runs over and over again until power down or reset
void loop() {
	//Just call scheduler update and let it do all the process
	scheduler.update();	
}

void test_task()
{
	test_task_counter++;
	if (test_task_counter % 2 == 0)
	{
		digitalWrite(PIN_LED, HIGH);
		/*
		Serial.print("Mpu Pitch:");
		Serial.print(MsgT01.message.mpuPitch*180/M_PI);
		Serial.print("   Compass Hdg:");
		Serial.print(MsgT01.message.compassHdg*180/M_PI);
		Serial.print("   Baro Alt:");
		Serial.print(MsgT01.message.baroAlt);
		Serial.print("   Baro Temp:");
		Serial.println(MsgT01.message.baroTemp);
		*/
		/*
		Serial.print("Thr:");
		Serial.print(dutyCycle_Thr);
		Serial.print("   Pitch:");
		Serial.print(dutyCycle_Pitch);
		Serial.print("   Roll:");
		Serial.print(dutyCycle_Roll);
		Serial.print("   Yaw:");
		Serial.println(dutyCycle_Yaw);
		*/
		Serial.print("Gps Data:");
		Serial.print(MsgT01.message.gpsData);
		Serial.print("   Baro Temp");
		Serial.println(MsgT01.message.baroTemp);

		
	}
	else
	{
		digitalWrite(PIN_LED, LOW);
	}
}

void serialCheck()
{
	int numberofbytes = Serial2.available();
	if (numberofbytes > 0)
	{
		//If available number of bytes is less than our buffer size, normal case
		if (numberofbytes <= sizeof(MsgT01.message) * 3)
		{
			unsigned char buffer[sizeof(MsgT01.message) * 3];
			Serial2.readBytes(buffer, numberofbytes);
			serialParse.Push(buffer, numberofbytes);
			if (serialParse.getParsedData(MsgT01.dataBytes, sizeof(MsgT01.message)))
			{
				MsgT01.setPacket();
			}
		}
		//Else if buffer overflow, abnormal case
		else
		{
			//Just read it
			unsigned char buffer[sizeof(MsgT01.message) * 3];
			Serial2.readBytes(buffer, sizeof(MsgT01.message) * 3);
		}
	}
}

void isrTHR()
{
	if (digitalRead(PIN_THR) == HIGH)
	{
		startTime_Thr = micros();
	}
	else
	{
		dutyCycle_Thr = micros() - startTime_Thr;
	}
	rxLastDataTime = millis();  //we need to define this for each isr in order to fully get status of rx
}
void isrPITCH()
{
	if (digitalRead(PIN_PITCH) == HIGH)
	{
		startTime_Pitch = micros();
	}
	else
	{
		dutyCycle_Pitch = micros() - startTime_Pitch;
	}
}
void isrROLL()
{
	if (digitalRead(PIN_ROLL) == HIGH)
	{
		startTime_Roll = micros();
	}
	else
	{
		dutyCycle_Roll = micros() - startTime_Roll;
	}
}
void isrYAW()
{
	if (digitalRead(PIN_YAW) == HIGH)
	{
		startTime_Yaw = micros();
	}
	else
	{
		dutyCycle_Yaw = micros() - startTime_Yaw;
	}
}