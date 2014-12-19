#ifndef CBMAKEFILECONSOLE_H
#define CBMAKEFILECONSOLE_H

//(*Headers(cbmakefileconsole)
#include <wx/panel.h>
//*)

class cbmakefileconsole: public wxPanel
{
	public:

		cbmakefileconsole(wxWindow* parent,wxWindowID id=wxID_ANY);
		virtual ~cbmakefileconsole();

		//(*Declarations(cbmakefileconsole)
		//*)

	protected:

		//(*Identifiers(cbmakefileconsole)
		//*)

	private:

		//(*Handlers(cbmakefileconsole)
		//*)

		DECLARE_EVENT_TABLE()
};

#endif
