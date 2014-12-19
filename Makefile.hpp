#ifndef __MAKEFILE_HPP
#define __MAKEFILE_HPP

#include <wx/hashmap.h>
#include <cbproject.h>
#include "Rule.hpp"
#include "Variable.hpp"

class cbProject;
class Compiler;

#include <wx/arrimpl.cpp>

WX_DECLARE_OBJARRAY(cbMGRule,cbMGArrayOfRules);
WX_DECLARE_OBJARRAY(ProjectFile*, cbMGSortFilesArray); // keep our own copy, to sort it by file weight (priority)

class cbMGMakefile
{
    cbMGArrayOfRules m_Rules;
    cbMGVariable m_Variables;
    wxString     m_Objs;
    cbMGVariable m_Deps;
    wxChar       m_CommandPrefix;
    long         m_CommandPrefixRepeatCnt;
    cbProject    *m_pProj;
    wxString     m_Filename;
    wxString     m_Path;
    bool         m_IsSilence;
    bool         m_Overwrite;
    bool         m_AllTargets;
    bool         m_VariablesIsSaved;
    wxString     m_ProceedTargets;
    bool         m_DependenciesIsNotExistsIsNoProblem;
public:
    cbMGMakefile(cbProject* ppProj, const wxString& pFileName,bool pOverwrite,bool pSilence,bool pAllTargets);
    virtual ~cbMGMakefile();

    void SetCommandPrefix( const wxChar& pCommandPrefix );
    wxString GetCommandPrefix() const
    {
        wxString lRes;
        return lRes.Pad( m_CommandPrefixRepeatCnt, m_CommandPrefix );
    }
    void SetCommandPrefixRepeatCnt( long pCommandPrefixRepeatCnt );
    long GetCommandPrefixRepeatCnt() const
    {
        return m_CommandPrefixRepeatCnt;
    }
    bool SaveMakefile();
    void SetIsSilence( bool pIsSilence )
    {
        m_IsSilence = pIsSilence;
    }
    wxString GetProceedTargets() const
    {
        return m_ProceedTargets;
    }
protected:
    bool SaveAllRules( wxTextFile& pFile );
    cbMGSortFilesArray GetProjectFilesSortedByWeight(ProjectBuildTarget* ppTarget, bool pCompile, bool pLink);
    bool getDependencies(ProjectBuildTarget *p_pTarget,Compiler* p_pCompiler);
    bool reLoadDependecies(const wxString &p_DepsFileName,ProjectBuildTarget *p_pTarget,Compiler* p_pCompiler);
    bool formFileForTarget( ProjectBuildTarget *p_BuildTarget, wxTextFile &p_File );
private:
};

#endif


