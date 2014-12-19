#include <sdk.h> // Code::Blocks SDK
#include <configurationpanel.h>
#include "cbMakefileGen.h"
#include <wx/menu.h>
#include "Makefile.hpp"
#include "cbmakefilecfg.h"
#include <cbproject.h>
#include <projectmanager.h>
#include <logmanager.h>
#include <configmanager.h>

int MakefileGenId = wxNewId();

// Register the plugin with Code::Blocks.
// We are using an anonymous namespace so we don't litter the global one.
namespace
{
    PluginRegistrant<cbMakefileGen> reg(_T("cbMakefileGen"));
}


// events handling
BEGIN_EVENT_TABLE(cbMakefileGen, cbPlugin)
    // add any events you want to handle here
    EVT_MENU(MakefileGenId, cbMakefileGen::OnExecute)
END_EVENT_TABLE()

// constructor
cbMakefileGen::cbMakefileGen()
{
    // Make sure our resources are available.
    // In the generated boilerplate code we have no resources but when
    // we add some, it will be nice that this code is in place already ;)
    if (!Manager::LoadResource(_T("cbMakefileGen.zip")))
    {
        NotifyMissingFile(_T("cbMakefileGen.zip"));
    }
}

// destructor
cbMakefileGen::~cbMakefileGen()
{}

void cbMakefileGen::OnAttach()
{
    // do whatever initialization you need for your plugin
    // NOTE: after this function, the inherited member variable
    // m_IsAttached will be TRUE...
    // You should check for it in other functions, because if it
    // is FALSE, it means that the application did *not* "load"
    // (see: does not need) this plugin...
}

void cbMakefileGen::OnRelease(bool appShutDown)
{
    // do de-initialization for your plugin
    // if appShutDown is true, the plugin is unloaded because Code::Blocks is being shut down,
    // which means you must not use any of the SDK Managers
    // NOTE: after this function, the inherited member variable
    // m_IsAttached will be FALSE...
}

int cbMakefileGen::Configure()
{
    //create and display the configuration dialog for your plugin
    cbConfigurationDialog dlg(Manager::Get()->GetAppWindow(), wxID_ANY, _("Makefile generator configure"));
    cbConfigurationPanel* panel = GetConfigurationPanel(&dlg);
    if (panel)
    {
        dlg.AttachConfigurationPanel(panel);
        PlaceWindow(&dlg);
        return dlg.ShowModal() == wxID_OK ? 0 : -1;
    }
    return -1;
}

void cbMakefileGen::BuildMenu(wxMenuBar* menuBar)
{
    //The application is offering its menubar for your plugin,
    //to add any menu items you want...
    //Append any items you need in the menu...
    //NOTE: Be careful in here... The application's menubar is at your disposal.
    if (!IsAttached())
    {
        return;
    }

    int idx = menuBar->FindMenu(_("Project"));
    if (idx != wxNOT_FOUND)
    {
        wxMenu* lMenu = menuBar->GetMenu(idx);
        if ( lMenu )
        {
            lMenu->AppendSeparator();
            lMenu->Append(MakefileGenId, _("Generate Makefile"));
        }
    }
}

void cbMakefileGen::BuildModuleMenu(const ModuleType type, wxMenu* menu, const FileTreeData* data)
{
    //Some library module is ready to display a pop-up menu.
    //Check the parameter \"type\" and see which module it is
    //and append any items you need in the menu...
    //TIP: for consistency, add a separator as the first item...
    // NotImplemented(_T("cbMakefileGen::BuildModuleMenu()"));
}

void cbMakefileGen::OnExecute(wxCommandEvent &event)
{
    // if not attached, exit
    if (!IsAttached())
        return;

    wxString msg;
    cbProject* project = Manager::Get()->GetProjectManager()->GetActiveProject();
    // if no project open, exit
    if (!project)
    {
        msg = _("You need to open a project\nbefore using the plugin!\n"
                "C::B MakefileGen could not complete the operation.");
        cbMessageBox(msg, _("Error"), wxICON_ERROR | wxOK, Manager::Get()->GetAppWindow());
        Manager::Get()->GetLogManager()->DebugLog(msg);
        return;
    }

    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("cbMakefileGen"));
    m_Filename = cfg->Read(_T("/filename"),_T("Makefile.gen"));
    m_Overwrite = cfg->ReadBool(_T("/overwrite"),false);
    m_Silence = cfg->ReadBool(_T("/silence"),true);
    m_AllTargets = cfg->ReadBool(_T("/alltargets"),false);
    /* fix by oBFusCATed */
    m_Filename.Replace(_T("$(proj_title)"), project->GetTitle());
    cbMGMakefile lMakefile(project,m_Filename,m_Overwrite,m_Silence,m_AllTargets);
    if ( lMakefile.SaveMakefile() )
    {
        msg = _("C::B MakefileGen save : ");
        msg += m_Filename; msg += _T("\n");
        msg += _("Targets: ");
        msg += lMakefile.GetProceedTargets();
        cbMessageBox(msg, _("Info"), wxICON_INFORMATION | wxOK, Manager::Get()->GetAppWindow());
    }
    else
    {
        msg = _("C::B MakefileGen found errors, saves aborted.");
        cbMessageBox(msg, _("Error"), wxICON_EXCLAMATION | wxOK, Manager::Get()->GetAppWindow());
    }
}

cbConfigurationPanel* cbMakefileGen::GetConfigurationPanel(wxWindow* parent)
{
    return new cbmakefilecfg(parent);
}
