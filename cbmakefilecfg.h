#ifndef CBMAKEFILECFG_H
#define CBMAKEFILECFG_H

#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

//(*Headers(cbmakefilecfg)
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/checkbox.h>
#include <wx/panel.h>
//*)

#include <configurationpanel.h>

class cbmakefilecfg: public cbConfigurationPanel
{
public:

    cbmakefilecfg(wxWindow* parent,wxWindowID id = -1);
    virtual ~cbmakefilecfg();

    //(*Identifiers(cbmakefilecfg)
    static const long ID_STATICTEXT1;
    static const long ID_TEXTCTRL1;
    static const long ID_CHECKBOX1;
    static const long ID_CHECKBOX2;
    static const long ID_CHECKBOX3;
    static const long ID_STATICTEXT2;
    //*)

    /// @return the panel's title.
    virtual wxString GetTitle() const
    {
        return _T("C::B MakefileGen");
    };
    /// @return the panel's bitmap base name. You must supply two bitmaps: <basename>.png and <basename>-off.png...
    virtual wxString GetBitmapBaseName() const
    {
        return _T("generic-plugin");
    }
    /// Called when the user chooses to apply the configuration.
    virtual void OnApply();
    /// Called when the user chooses to cancel the configuration.
    virtual void OnCancel()
    {};
protected:

    //(*Handlers(cbmakefilecfg)
    //*)

    //(*Declarations(cbmakefilecfg)
    wxStaticText* m_pStaticText1;
    wxStaticText* StaticText1;
    wxTextCtrl* m_pTCFilename;
    wxCheckBox* m_pCBSilence;
    wxCheckBox* m_pCBOverwrite;
    wxCheckBox* m_pCBAllTargets;
    //*)

private:

    DECLARE_EVENT_TABLE()
};

#endif
