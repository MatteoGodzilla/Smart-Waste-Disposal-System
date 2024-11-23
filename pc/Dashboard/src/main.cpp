#include <wx/wx.h>
#include "MainWindow.h"

class Main : public wxApp {
public:
	bool OnInit() override {
		MainWindow* window = new MainWindow();
		window->Show();
		return true;
	};
};

wxIMPLEMENT_APP(Main);