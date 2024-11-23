#include "MainWindow.h"

MainWindow::MainWindow()
	: wxFrame(NULL, wxID_ANY, "SWDS Dashboard", wxDefaultPosition, wxSize(1024, 768))
{
	wxBoxSizer* rootSizer = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* topSizer = new wxBoxSizer(wxHORIZONTAL);

	wxStaticText* label = new wxStaticText(this, wxID_ANY, "Select device:");
	topSizer->Add(label, wxSizerFlags().Border());

	wxComboBox* serialDevices = new wxComboBox(this, wxID_ANY, "", wxDefaultPosition, wxSize(400,20));
	topSizer->Add(serialDevices, wxSizerFlags().Border());

	wxBoxSizer* mainSizer = new wxBoxSizer(wxHORIZONTAL);
	Graph* graph = new Graph(this);

	wxBoxSizer* mainRightSizer = new wxBoxSizer(wxVERTICAL);
	wxStaticText* temperatureLabel = new wxStaticText(this, wxID_ANY, "Current temperature: ###");
	wxStaticText* fillLabel = new wxStaticText(this, wxID_ANY, "Current fill percentage: ###");
	mainRightSizer->Add(temperatureLabel, wxSizerFlags(1).Border());
	mainRightSizer->Add(fillLabel, wxSizerFlags(1).Border());

	mainSizer->Add(graph, wxSizerFlags(1).Border(wxLEFT));
	mainSizer->Add(mainRightSizer, wxSizerFlags().Border(wxRIGHT));

	wxBoxSizer* bottomSizer = new wxBoxSizer(wxHORIZONTAL);
	wxButton* emptyButton = new wxButton(this, wxID_ANY, "Empty", wxDefaultPosition, wxDefaultSize);
	wxButton* fixTemperatureButton = new wxButton(this, wxID_ANY, "Fix Temperature", wxDefaultPosition, wxDefaultSize);
	bottomSizer->Add(emptyButton, wxSizerFlags(1).Border());
	bottomSizer->Add(fixTemperatureButton, wxSizerFlags(1).Border());

	rootSizer->Add(topSizer);
	rootSizer->Add(mainSizer);
	rootSizer->Add(bottomSizer, wxSizerFlags().Expand());

	SetSizerAndFit(rootSizer);
}