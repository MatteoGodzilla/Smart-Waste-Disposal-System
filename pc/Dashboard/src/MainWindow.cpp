#include "MainWindow.h"

wxBEGIN_EVENT_TABLE(MainWindow, wxFrame)
	EVT_CHOICE(CHOICE_BOX, MainWindow::onComboBoxSelection)
wxEND_EVENT_TABLE()

MainWindow::MainWindow()
	: wxFrame(NULL, wxID_ANY, "SWDS Dashboard", wxDefaultPosition, wxSize(1024, 768))
{
	//Initialize Gui
	wxBoxSizer* rootSizer = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* topSizer = new wxBoxSizer(wxHORIZONTAL);

	wxStaticText* label = new wxStaticText(this, wxID_ANY, "Select device:");
	topSizer->Add(label, wxSizerFlags().Border().Center());

	//serialDevices = new wxChoice(this, CHOICE_BOX, "", wxDefaultPosition, wxSize(400,20) );
	serialDevices = new wxChoice(this, CHOICE_BOX, wxDefaultPosition, wxSize(400, 20));
	topSizer->Add(serialDevices, wxSizerFlags(1).Border().Expand());

	wxBoxSizer* mainSizer = new wxBoxSizer(wxHORIZONTAL);
	Graph* graph = new Graph(this);

	wxBoxSizer* mainRightSizer = new wxBoxSizer(wxVERTICAL);
	temperatureLabel = new wxStaticText(this, wxID_ANY, "Current temperature: ###");
	fillLabel = new wxStaticText(this, wxID_ANY, "Current fill percentage: ###");
	mainRightSizer->Add(temperatureLabel, wxSizerFlags(1).Border());
	mainRightSizer->Add(fillLabel, wxSizerFlags(1).Border());

	mainSizer->Add(graph, wxSizerFlags(1).Border(wxLEFT).Expand());
	mainSizer->Add(mainRightSizer, wxSizerFlags().Border(wxRIGHT));

	wxBoxSizer* bottomSizer = new wxBoxSizer(wxHORIZONTAL);
	wxButton* emptyButton = new wxButton(this, wxID_ANY, "Empty", wxDefaultPosition, wxSize(100, 30));
	wxButton* fixTemperatureButton = new wxButton(this, wxID_ANY, "Fix Temperature", wxDefaultPosition, wxSize(100, 30));
	bottomSizer->Add(emptyButton, wxSizerFlags(1).Border());
	bottomSizer->Add(fixTemperatureButton, wxSizerFlags(1).Border());

	rootSizer->Add(topSizer, wxSizerFlags().Expand());
	rootSizer->Add(mainSizer, wxSizerFlags(1).Expand());
	rootSizer->Add(bottomSizer, wxSizerFlags().Expand());

	SetSizerAndFit(rootSizer);

	//Initialize other stuff
	possibleConnections = serial::list_ports();
	for (auto& a : possibleConnections) {
		serialDevices->Append(a.description);
	}
	serialThread = nullptr;
	timer = new wxTimer(this);
	Bind(wxEVT_TIMER, &MainWindow::updateUI, this);
	timer->Start(100);
}

void MainWindow::updateUI(wxTimerEvent& event) {
	if (serialThread == nullptr) {
		return;
	}
	serialThread->mutex.lock();
	SerialInformations copy = serialThread->informations;
	serialThread->mutex.unlock();

	temperatureLabel->SetLabelText("Current temperature: " + std::to_string((int)copy.temperature));
	fillLabel->SetLabelText("Current fill percentage: " + std::to_string((int)(copy.fillPercentage * 100)));
}

void MainWindow::onComboBoxSelection(wxCommandEvent& event) {
	serial::PortInfo info = possibleConnections.at(serialDevices->GetSelection());
	try {
		closeActiveConnection();
		serialThread = new SerialThread(info);
	}
	catch (serial::IOException exception) {
		wxMessageBox("There was an error opening the serial port");
	}
}


void MainWindow::closeActiveConnection() {
	if (serialThread != nullptr) {
		serialThread->running = false;
		serialThread->join();
		delete serialThread;
		serialThread = nullptr;
	}
}

MainWindow::~MainWindow() {
	closeActiveConnection();
	timer->Stop();
}