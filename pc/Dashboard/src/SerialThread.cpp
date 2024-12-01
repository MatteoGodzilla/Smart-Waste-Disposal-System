#include "SerialThread.h"

//TODO: REMOVE
SerialThread::SerialThread(wxFrame* mainWindow) : std::jthread()
{
	informations = { 36, 0.6 };
	mainWindowRef = mainWindow;
	sendEventToMainWindow();
	running = false;
}


SerialThread::SerialThread() : std::jthread()
{
	informations = {};
	running = false;
}

SerialThread::SerialThread(wxEvtHandler* mainWindow, serial::PortInfo portInfo)
	: std::jthread(&SerialThread::run, this, get_stop_token())
{
	portInformation = portInfo;
	informations = {20, 1};
	mainWindowRef = mainWindow;
	running = true;
}

void SerialThread::run(std::stop_token token) {
	serial::Serial connection(portInformation.port, 9600);
	if (!connection.isOpen())
		return;
	while (running) {

		//read serial line
		try {
			std::string token = connection.readline(64);
			std::cout << token;
			if (token.starts_with("T:")) {
				std::string subtoken = token.substr(2, token.find_first_of(';') - 1);
				informations.temperature = std::stof(subtoken);
				sendEventToMainWindow();
			} else if (token.starts_with("F:")) {
				std::string subtoken = token.substr(2, token.find_first_of(';') - 1);
				informations.fillPercentage = std::stof(subtoken);
				sendEventToMainWindow();
			}
		}
		catch (std::exception e) {}
		
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

//From MainWindow to Serial
void SerialThread::enqueueEvent(SerialTransmitEvents event) {
	mutexTX.lock();
	std::cout << "ENQUEUED EVENT: " << event << std::endl;
	eventQueue.push(event);
	mutexTX.unlock();
	sendEventToMainWindow();
}

//From Serial to MainWindow
void SerialThread::sendEventToMainWindow() {
	//make copy of information struct that is present in the class
	SerialInformations* infoCopy = new SerialInformations;
	infoCopy->temperature = informations.temperature;
	infoCopy->fillPercentage = informations.fillPercentage;
	//create event
	wxCommandEvent* event = new wxCommandEvent(SERIAL_INFO_EVENT, SERIAL_THREAD_ID);
	event->SetClientData(infoCopy);
	//actually send event
	mainWindowRef->QueueEvent(event);
}