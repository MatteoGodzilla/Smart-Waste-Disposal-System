#pragma once
#include <thread>
#include <mutex>
#include <iostream>
#include "serial/serial.h"

struct SerialInformations {
	float temperature;
	float fillPercentage;
};

class SerialThread : public std::jthread {
public:
	SerialThread();
	SerialThread(serial::PortInfo portInfo);
	
	std::atomic_bool running;
	std::mutex mutex;
	SerialInformations informations;
private:
	void run(std::stop_token token);
	serial::PortInfo portInformation;
};