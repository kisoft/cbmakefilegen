#include "cbmakefilecfg.h"

//(*InternalHeaders(cbmakefilecfg)
#include <wx/intl.h>
#include <wx/string.h>
//*)

#include <configmanager.h>

//(*IdInit(cbmakefilecfg)
const long cbmakefilecfg::ID_STATICTEXT1 = wxNewId();
const long cbmakefilecfg::ID_TEXTCTRL1 = wxNewId();
const long cbmakefilecfg::ID_CHECKBOX1 = wxNewId();
const long cbmakefilecfg::ID_CHECKBOX2 = wxNewId();
const long cbmakefilecfg::ID_CHECKBOX3 = wxNewId();
const long cbmakefilecfg::ID_STATICTEXT2 = wxNewId();
//*)

BEGIN_EVENT_TABLE(cbmakefilecfg,cbConfigurationPanel)
    //(*EventTable(cbmakefilecfg)
    //*)
END_EVENT_TABLE()

cbmakefilecfg::cbmakefilecfg(wxWindow* parent,wxWindowID id)
{
    //(*Initialize(cbmakefilecfg)
    Create(parent, id, wxDefaultPosition, wxSize(573,123), wxTAB_TRAVERSAL, _T("id"));
    m_pStaticText1 = new wxStaticText(this, ID_STATICTEXT1, _("Filename :"), wxPoint(4,6), wxDefaultSize, 0, _T("ID_STATICTEXT1"));
    m_pTCFilename = new wxTextCtrl(this, ID_TEXTCTRL1, _("Makefile.gen"), wxPoint(70,3), wxSize(500,21), 0, wxDefaultValidator, _T("ID_TEXTCTRL1"));
    m_pCBOverwrite = new wxCheckBox(this, ID_CHECKBOX1, _("Overwrite if exists"), wxPoint(70,48), wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX1"));
    m_pCBOverwrite->SetValue(false);
    m_pCBSilence = new wxCheckBox(this, ID_CHECKBOX2, _("Add \'@\' prefix for commands"), wxPoint(70,68), wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX2"));
    m_pCBSilence->SetValue(false);
    m_pCBAllTargets = new wxCheckBox(this, ID_CHECKBOX3, _("Generate for target \'All\' (else being used active target only)"), wxPoint(70,88), wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX3"));
    m_pCBAllTargets->SetValue(false);
    StaticText1 = new wxStaticText(this, ID_STATICTEXT2, _("NOTE: Filename can content macro $(proj_title). Will replaced with a project title."), wxPoint(70,28), wxDefaultSize, 0, _T("ID_STATICTEXT2"));
    StaticText1->SetForegroundColour(wxColour(198,65,62));
    //*)
    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("cbMakefileGen"));
    m_pTCFilename->SetValue(cfg->Read(_T("/filename"),_T("Makefile.gen")));
    m_pCBOverwrite->SetValue(cfg->ReadBool(_T("/overwrite"),false));
    m_pCBSilence->SetValue(cfg->ReadBool(_T("/silence"),true));
    m_pCBAllTargets->SetValue(cfg->ReadBool(_T("/alltargets"),false));
}

cbmakefilecfg::~cbmakefilecfg()
{
    //(*Destroy(cbmakefilecfg)
    //*)
}

void cbmakefilecfg::OnApply()
{
    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("cbMakefileGen"));
    cfg->Write(_T("/filename"),(const wxString &)m_pTCFilename->GetValue());
    cfg->Write(_T("/overwrite"),(bool)m_pCBOverwrite->GetValue());
    cfg->Write(_T("/silence"),(bool)m_pCBSilence->GetValue());
    cfg->Write(_T("/alltargets"),(bool)m_pCBAllTargets->GetValue());
}

