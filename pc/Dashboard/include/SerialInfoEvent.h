#pragma once
#include <wx/event.h>
#include <wx/wx.h>

enum SerialTransmitEvents {
	EMPTY_CONTAINER,
	FIX_TEMPERATURE
};

struct SerialInformations {
	float temperature;
	float fillPercentage;
};

wxDECLARE_EVENT(SERIAL_INFO_EVENT, wxCommandEvent);