#pragma once
#include <wx/wx.h>
#include <wx/log.h>
#include <thread>
#include "Graph.h"
#include "serial/serial.h"
#include "SerialThread.h"

enum MyIds {
	CHOICE_BOX
};

class MainWindow : public wxFrame {
public:
	MainWindow();
	~MainWindow();
private:
	void updateUI(wxTimerEvent& event);
	void closeActiveConnection();
	void onComboBoxSelection(wxCommandEvent& event);

	wxChoice* serialDevices;
	wxStaticText* temperatureLabel;
	wxStaticText* fillLabel;

	std::vector<serial::PortInfo> possibleConnections;
	SerialThread* serialThread;
	wxTimer* timer;

	wxDECLARE_EVENT_TABLE();
};