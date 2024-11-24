#include "SerialThread.h"

SerialThread::SerialThread() : std::jthread()
{
	informations = {};
	running = false;
}

SerialThread::SerialThread(serial::PortInfo portInfo) 
	: std::jthread(&SerialThread::run, this, get_stop_token())
{
	portInformation = portInfo;
	informations = {20, 1};
	running = true;
}

void SerialThread::run(std::stop_token token) {
	serial::Serial connection(portInformation.port, 9600);
	if (!connection.isOpen())
		return;
	while (running) {
		mutex.lock();

		//read serial line
		std::string token = connection.readline(64);
		if (token.starts_with("T:")) {
			std::string subtoken = token.substr(2, token.find_first_of(';') - 1);
			informations.temperature = std::stof(subtoken);
			//std::cout << subtoken << std::endl << std::endl;
		} else if (token.starts_with("F:")) {
			std::string subtoken = token.substr(2, token.find_first_of(';') - 1);
			informations.fillPercentage = std::stof(subtoken);
		}
		mutex.unlock();

		//sleep for 1 second
		//std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
	connection.close();
}