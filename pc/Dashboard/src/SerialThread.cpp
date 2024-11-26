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
		mutexRX.lock();

		//read serial line
		try {
			std::string token = connection.readline(64);
			std::cout << token;
			if (token.starts_with("T:")) {
				std::string subtoken = token.substr(2, token.find_first_of(';') - 1);
				informations.temperature = std::stof(subtoken);
				//std::cout << subtoken << std::endl << std::endl;
			} else if (token.starts_with("F:")) {
				std::string subtoken = token.substr(2, token.find_first_of(';') - 1);
				informations.fillPercentage = std::stof(subtoken);
				//std::cout << subtoken << std::endl << std::endl;
			}
		}
		catch (std::exception e) {}
		mutexRX.unlock();

		mutexTX.lock();
		if (eventQueue.size() > 0) {
			std::cout << eventQueue.front() << std::endl;
			switch (eventQueue.front()) {
			case EMPTY_CONTAINER:
				connection.write("E;");
				break;
			case FIX_TEMPERATURE:
				connection.write("T;");
				break;
			}
			eventQueue.pop();
		}
		mutexTX.unlock();

		//sleep for 1 second
		//std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
	connection.close();
}

void SerialThread::enqueueEvent(SerialTransmitEvents event) {
	mutexTX.lock();
	std::cout << "ENQUEUED EVENT: " << event << std::endl;
	eventQueue.push(event);
	mutexTX.unlock();
}