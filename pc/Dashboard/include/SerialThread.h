#pragma once
#include <thread>
#include <queue>
#include <mutex>
#include <iostream>
#include "serial/serial.h"

enum SerialTransmitEvents {
	EMPTY_CONTAINER,
	FIX_TEMPERATURE
};

struct SerialInformations {
	float temperature;
	float fillPercentage;
};

class SerialThread : public std::jthread {
public:
	SerialThread();
	SerialThread(serial::PortInfo portInfo);
	
	void enqueueEvent(SerialTransmitEvents event);

	std::atomic_bool running;
	std::mutex mutexRX;
	std::mutex mutexTX;
	SerialInformations informations;
	std::queue<SerialTransmitEvents> eventQueue;
private:
	void run(std::stop_token token);
	serial::PortInfo portInformation;
};