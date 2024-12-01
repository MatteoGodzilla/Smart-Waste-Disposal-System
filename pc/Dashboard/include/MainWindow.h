#pragma once
#include <wx/wx.h>
#include <wx/log.h>
#include <thread>
#include "Graph.h"
#include "serial/serial.h"
#include "SerialThread.h"
#include "SerialInfoEvent.h"

enum MyIds {
	WINDOW,
	CHOICE_BOX,
	BTN_EMPTY,
	BTN_TEMPERATURE
};

class MainWindow : public wxFrame {
public:
	MainWindow();
	~MainWindow();
private:
	void closeActiveConnection();
	void onUpdateUI(wxCommandEvent& event);
	void onComboBoxSelection(wxCommandEvent& event);
	void onButtonEmpty(wxCommandEvent& event);
	void onButtonTemperature(wxCommandEvent& event);

	wxChoice* serialDevices;
	wxStaticText* temperatureLabel;
	wxStaticText* fillLabel;

	std::vector<serial::PortInfo> possibleConnections;
	SerialThread* serialThread;

	wxDECLARE_EVENT_TABLE();
};