#include <wx/wx.h>

class MyFrame : public wxFrame {
public:
	MyFrame()
		: wxFrame(NULL, wxID_ANY, "Hello World", wxDefaultPosition, wxSize(1024,768))
	{}
};

class Main : public wxApp {
public:
	bool OnInit() override {
		MyFrame* frame = new MyFrame();
		frame->Show();
		return true;
	};
};

wxIMPLEMENT_APP(Main);