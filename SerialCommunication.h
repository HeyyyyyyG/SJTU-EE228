
#ifndef __SERIALCOMMUNICATION_H__
#define __SERIALCOMMUNICATION_H__

#include"SerialPort.h"
#include<iostream>

using namespace std;

void openSerialPort(CSerialPort &sP)
{
	try {
		sP.Open(3);
	}
	catch (CSerialException) {
		cout << " CSerialException!\n Maybe ur bluetooth does not open.";
	}
}
void  serialCommunication(CSerialPort &sP, char CMD[])
{
	try {
		// send msg
		sP.Write((const void*)CMD, 1);
	}
	catch (CSerialException) {
		cout << " CSerialException!--> Write problem.";
	}
}
void closeSerialPort(CSerialPort &sP)
{
	try {
		sP.Close();
	}
	catch (CSerialException) {
		cout << " CSerialException!--> Close problem.";
	}
}

#endif //__SERIALCOMMUNICATION_H__