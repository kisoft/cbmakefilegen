#include "cbmakefileconsole.h"

//(*InternalHeaders(cbmakefileconsole)
#include <wx/bitmap.h>
#include <wx/font.h>
#include <wx/fontenum.h>
#include <wx/fontmap.h>
#include <wx/image.h>
#include <wx/intl.h>
#include <wx/settings.h>
#include <wx/string.h>
//*)

//(*IdInit(cbmakefileconsole)
//*)

BEGIN_EVENT_TABLE(cbmakefileconsole,wxPanel)
	//(*EventTable(cbmakefileconsole)
	//*)
END_EVENT_TABLE()

cbmakefileconsole::cbmakefileconsole(wxWindow* parent,wxWindowID id)
{
	//(*Initialize(cbmakefileconsole)
	Create(parent, id, wxDefaultPosition, wxSize(667,313), wxTAB_TRAVERSAL, _T("id"));
	//*)
}

cbmakefileconsole::~cbmakefileconsole()
{
	//(*Destroy(cbmakefileconsole)
	//*)
}

