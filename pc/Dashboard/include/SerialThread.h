#pragma once
#include <thread>
#include <queue>
#include <mutex>
#include <iostream>
#include <wx/event.h>
#include "serial/serial.h"
#include "SerialInfoEvent.h"

#define SERIAL_THREAD_ID 9999

class SerialThread : public std::jthread {
public:
	//ONLY USED FOR DEBUG, TO REMOVE
	SerialThread(wxFrame* mainWindow);

	SerialThread();
	SerialThread(wxEvtHandler* mainWindow, serial::PortInfo portInfo);
	
	void enqueueEvent(SerialTransmitEvents event);

	std::atomic_bool running;
	std::mutex mutexTX;
	SerialInformations informations;
	std::queue<SerialTransmitEvents> eventQueue;
private:
	void run(std::stop_token token);
	void sendEventToMainWindow();
	serial::PortInfo portInformation;
	wxEvtHandler* mainWindowRef;
};