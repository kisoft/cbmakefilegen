#include "cbmakefilegencons.h"

//(*InternalHeaders(cbmakefilegencons)
#include <wx/intl.h>
#include <wx/string.h>
//*)

//(*IdInit(cbmakefilegencons)
const long cbmakefilegencons::ID_STATICTEXT1 = wxNewId();
const long cbmakefilegencons::ID_BUTTON1 = wxNewId();
const long cbmakefilegencons::ID_PANEL1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(cbmakefilegencons,wxFrame)
	//(*EventTable(cbmakefilegencons)
	//*)
END_EVENT_TABLE()

cbmakefilegencons::cbmakefilegencons(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
	//(*Initialize(cbmakefilegencons)
	Create(parent, id, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE, _T("id"));
	SetClientSize(wxSize(334,247));
	Move(wxDefaultPosition);
	BoxSizer1 = new wxBoxSizer(wxHORIZONTAL);
	Panel1 = new wxPanel(this, ID_PANEL1, wxDefaultPosition, wxSize(398,214), wxTAB_TRAVERSAL, _T("ID_PANEL1"));
	BoxSizer2 = new wxBoxSizer(wxHORIZONTAL);
	StaticText1 = new wxStaticText(Panel1, ID_STATICTEXT1, _("Hello, World!"), wxDefaultPosition, wxSize(77,30), 0, _T("ID_STATICTEXT1"));
	BoxSizer2->Add(StaticText1, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	Button1 = new wxButton(Panel1, ID_BUTTON1, _("Label"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON1"));
	BoxSizer2->Add(Button1, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	Panel1->SetSizer(BoxSizer2);
	BoxSizer2->SetSizeHints(Panel1);
	BoxSizer1->Add(Panel1, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 3);
	SetSizer(BoxSizer1);
	BoxSizer1->SetSizeHints(this);
	//*)
}

cbmakefilegencons::~cbmakefilegencons()
{
	//(*Destroy(cbmakefilegencons)
	//*)
}


