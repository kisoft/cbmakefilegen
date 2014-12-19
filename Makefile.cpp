#include <sdk.h>
#include <wx/textfile.h>
#include "Makefile.hpp"
#include <globals.h>
#include <logmanager.h>
#include <compiler.h>
#include <compilerfactory.h>
#include <macrosmanager.h>
#include "version.h"
#include <annoyingdialog.h>

#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY( cbMGArrayOfRules );
WX_DEFINE_OBJARRAY( cbMGSortFilesArray ); // keep our own copy, to sort it by file weight (priority)

static wxString sHeader = _T( "# A simple makefile generator by KiSoft, 2010. mailto: kisoft@rambler.ru" );
static wxString sHeaderVersion = _T( "# version: $MAJOR.$MINOR.$BUILD.$REVISION" );

const wxString cmdbefore = _T( "commandsbeforebuild" );
const wxString cmdafter  = _T( "commandsafterbuild" );
const wxString cmdphony  = _T( ".PHONY" );
const wxString cmdclean  = _T("clean");

// #define USE_PRINTLOG 1
// #define USE_DEBUGSAVE 1

#ifdef USE_PRINTLOG
const wxString g_FileName = _T( "F:/log.txt" );

void printLog( const wxString &func, const wxString &str )
{
  wxTextFile l_File;
  if( l_File.Open( g_FileName, wxConvFile ) )
  {
    l_File.AddLine( func + _T( ": " ) + str );
    l_File.Write();
    l_File.Close();
  }
}

#endif

void ConvertToMakefileFriendly(wxString& str)
{
    if(str.IsEmpty())
        return;

    str.Replace(_T("\\"), _T("/"));
    for (unsigned int i = 0; i < str.Length(); ++i)
    {
        if (str[i] == _T(' ') && (i > 0 && str[i - 1] != _T('\\')))
            str.insert(i, _T('\\'));
    }
}

cbMGMakefile::cbMGMakefile( cbProject* ppProject, const wxString& pFileName, bool pOverwrite,bool pSilence,bool pAllTargets)
        : m_Objs()
        , m_CommandPrefix( _T( '\t' ) )
        , m_CommandPrefixRepeatCnt( 1 )
        , m_pProj( ppProject )
        , m_Filename( pFileName )
        , m_Path( _T("") )
        , m_IsSilence( pSilence )
        , m_Overwrite( pOverwrite )
        , m_AllTargets( pAllTargets )
        , m_VariablesIsSaved( false )
        , m_ProceedTargets( _T("") )
        , m_DependenciesIsNotExistsIsNoProblem( false )
{
    //ctor
    m_Path = m_pProj->GetBasePath();

    #ifdef USE_PRINTLOG
    wxTextFile l_File( g_FileName );
    if( l_File.Exists() )
    {
      if( l_File.Open() )
      {
        l_File.Clear();
        l_File.Write();
        l_File.Close();
      }
    }
    else if( l_File.Create() )
    {
      l_File.AddLine( _T( "# Start log" ) );
      l_File.Write();
      l_File.Close();
    }

    printLog( _T( "cbMGMakeFile::cbMGMakeFile" ), m_Path );
    #endif
}

cbMGMakefile::~cbMGMakefile()
{
    //dtor
}

void cbMGMakefile::SetCommandPrefix( const wxChar& p_CommandPrefix )
{
    m_CommandPrefix = p_CommandPrefix;
}

void cbMGMakefile::SetCommandPrefixRepeatCnt( long p_CommandPrefixRepeatCnt )
{
    m_CommandPrefixRepeatCnt = p_CommandPrefixRepeatCnt;
}

int cbMGSortProjectFilesByWeight( ProjectFile*** p_ppOne, ProjectFile*** p_ppTwo )
{
    return ( *( *p_ppOne ) )->weight - ( *( *p_ppTwo ) )->weight;
}

cbMGSortFilesArray cbMGMakefile::GetProjectFilesSortedByWeight( ProjectBuildTarget* p_pTarget, bool p_Compile, bool p_Link )
{
    cbMGSortFilesArray files;
    for ( int i = 0; i < m_pProj->GetFilesCount(); ++i )
    {
        ProjectFile* pf = m_pProj->GetFile( i );
        // require compile
        if ( p_Compile && !pf->compile )
            continue;
        // require link
        if ( p_Link && !pf->link )
            continue;
        // if the file does not belong in this target (if we have a target), skip it
        if ( p_pTarget && ( pf->buildTargets.Index( p_pTarget->GetTitle() ) == wxNOT_FOUND ) )
            continue;
        files.Add( pf );
    }
    files.Sort( cbMGSortProjectFilesByWeight );
    return files;
}

bool cbMGMakefile::reLoadDependecies(const wxString &p_DepsFileName,ProjectBuildTarget *p_pTarget,Compiler* p_pCompiler)
{
    #ifdef USE_PRINTLOG
    printLog( _T( "cbMGMakefile::reLoadDependecies, Start" ), p_pTarget->GetTitle() );
    #endif

    m_Deps.Clear();
    if ( !wxFileExists( p_DepsFileName ) )
    {
        wxString l_Msg = _( "Dependencies file (.depend) is absent!\n"
                            "C::B MakefileGen could not complete the operation." );
        cbMessageBox( l_Msg, _( "Error" ), wxICON_ERROR | wxOK, (wxWindow *)Manager::Get()->GetAppWindow() );
        Manager::Get()->GetLogManager()->DebugLog( l_Msg );
        #ifdef USE_PRINTLOG
        printLog( _T( "cbMGMakefile::reLoadDependecies" ), _T( "End false" ) );
        #endif
        return false;
    }

    wxString l_Buf;
    wxString l_VarName;
    wxTextFile l_DepFile;
    wxString l_TargetName = p_pTarget->GetTitle();
    bool IsSkipThisFile = true;
    if (l_DepFile.Open( p_DepsFileName, wxConvFile ))
    {
        for ( unsigned long i = 0; i < l_DepFile.GetLineCount(); i++ )
        {
            l_Buf = l_DepFile[i];
            l_Buf.Trim(true);
            // Wrong! Don't uncomment it! Being deleted '\t' symbol! l_Buf.Trim(false);
            if ( l_Buf.IsEmpty() )
            {
                l_VarName.Clear();
                IsSkipThisFile = true;
                continue;
            }
            if ( _T('#') == l_Buf[0] ) continue;
            if ( _T('\t') == l_Buf[0] )
            {
                if ( !IsSkipThisFile )
                {
                    if ( _T('"') == l_Buf[1] )
                    {
                        wxString l_TmpName = l_Buf.AfterFirst( _T('\t') );
                        if (!l_TmpName.IsEmpty() && l_TmpName.GetChar(0) == _T('"') && l_TmpName.Last() == _T('"'))
                            l_TmpName = l_TmpName.Mid(1, l_TmpName.Length() - 2);
                        QuoteStringIfNeeded( l_TmpName );
                        m_Deps.AppendValue( l_VarName, l_TmpName );
                    }
                }
            }
            else
            {
                l_VarName = l_Buf.AfterFirst(_T(' '));
                bool IsSource = l_VarName.Matches( _T("source:*") );
                if ( IsSource )
                {
                    l_VarName = l_VarName.AfterFirst( _T(':') );
                }
                /*
                 * You would MUST found source file and get his filename from project
                 * !!! .depend file content lowcase filenames
                 */
                wxFileName l_DepFileName = l_VarName;

                #ifdef USE_PRINTLOG
                printLog( _T( "cbMGMakefile::reLoadDependecies, depfilename" ), l_VarName.wc_str() );
                #endif

                ProjectFile *pf = m_pProj->GetFileByFilename( l_DepFileName.GetFullPath(), l_DepFileName.IsRelative(), false );
                if ( pf )
                {
                    #ifdef USE_PRINTLOG
                    printLog( _T( "cbMGMakefile::reLoadDependecies, pf readed" ), pf->GetObjName() );
                    #endif

                    if( pf->buildTargets.Index( l_TargetName ) != wxNOT_FOUND )
                    {

                        #ifdef USE_PRINTLOG
                        printLog( _T( "cbMGMakefile::reLoadDependecies, before pfd read, target" ), l_TargetName );
                        #endif

                        const pfDetails& pfd = pf->GetFileDetails( p_pTarget );

                        #ifdef USE_PRINTLOG
                        printLog( _T( "cbMGMakefile::reLoadDependecies, pfd readed" ), pfd.dep_dir );
                        #endif

                        if ( p_pCompiler->GetSwitches().UseFullSourcePaths )
                        {
                            l_VarName = UnixFilename( pfd.source_file_absolute_native );
                        }
                        else
                        {
                            l_VarName = pfd.source_file;
                        }
                        QuoteStringIfNeeded( l_VarName );
                        IsSkipThisFile = false;
                    }
                    else
                    {
                        #ifdef USE_PRINTLOG
                        printLog( _T( "cbMGMakefile::reLoadDependecies, pf skip" ), pf->GetObjName() );
                        #endif

                        IsSkipThisFile = true;
                    }
                }
                else
                {
                    IsSkipThisFile = true;
                }
            }
        }
    }
    /* FIXME (kisoftcb#1#): Next code for debug only!
    wxTextFile l_DebFile;
    l_DebFile.Create( _T("D:/DepsFile.log") );
    m_Deps.SaveAllVars( l_DebFile );
    l_DebFile.Write();
    l_DebFile.Close(); */
    /* return was absent here! */
    #ifdef USE_PRINTLOG
    printLog( _T( "cbMGMakefile::reLoadDependecies" ), _T( "End true" ) );
    #endif
    return true;
}

bool cbMGMakefile::getDependencies(ProjectBuildTarget *p_pTarget,Compiler* p_pCompiler)
{
    /* if Yes, dependenses is not exists, no work required */
    if ( m_DependenciesIsNotExistsIsNoProblem ) return true;

    wxFileName l_DepsFilename(m_pProj->GetFilename());
    l_DepsFilename.SetExt(_T("depend"));
    if ( !wxFileExists( l_DepsFilename.GetFullPath() ) )
    {
        wxString lMsg = _( "Dependencies file " ) + l_DepsFilename.GetFullPath() + _(" is not exists!\n"
                        "Dependencies must being created before use MakefileGen plugin.\n"
                        "Continue anyway?" );

	// if (wxID_YES == cbMessageBox(lMsg, _("Warning"), wxICON_EXCLAMATION | wxYES_NO, (wxWindow *)Manager::Get()->GetAppWindow()))
	AnnoyingDialog dlg(_("cbMakefileGen: Warning! Dependencies file is not exists"),lMsg,wxART_QUESTION,AnnoyingDialog::YES_NO,wxID_YES);
	if( wxID_YES == dlg.ShowModal() )
	{
            m_DependenciesIsNotExistsIsNoProblem = true;
            return true;
        }
        else
        {
            m_DependenciesIsNotExistsIsNoProblem = false;
            return false;
        }
    }
    /* l_DepsFilename content filename for <project>.depend */
    return reLoadDependecies(l_DepsFilename.GetFullPath(),p_pTarget,p_pCompiler);
}

bool cbMGMakefile::formFileForTarget( ProjectBuildTarget *p_BuildTarget, wxTextFile &p_File )
{
    bool l_Ret = false;
    if ( !p_BuildTarget )
    {
        wxString l_Msg = _( "Can't found an active target!\n"
                            "C::B MakefileGen could not complete the operation." );
        cbMessageBox( l_Msg, _( "Error" ), wxICON_ERROR | wxOK, (wxWindow *)Manager::Get()->GetAppWindow() );
        Manager::Get()->GetLogManager()->DebugLog( l_Msg );
        return l_Ret;
    }

    wxString l_TargetName = p_BuildTarget->GetTitle();
    l_TargetName.Replace( _T( " " ), _T( "_" ) );

    wxString l_ObjsName = _T("OBJS_") + l_TargetName.Upper();
    wxString l_CmdBefore = cmdbefore + _T('_') + l_TargetName;
    wxString l_CmdAfter = cmdafter + _T('_') + l_TargetName;
    wxString l_CmdClean = cmdclean + _T('_') + l_TargetName;

    m_Rules.Clear();

    const wxString& l_CompilerId = p_BuildTarget->GetCompilerID();
    Compiler* l_pCompiler = CompilerFactory::GetCompiler( l_CompilerId );

    if( !l_pCompiler )
    {
        wxString l_Msg = _( "Can't found an compiler settings!\n"
                            "C::B MakefileGen could not complete the operation." );
        cbMessageBox( l_Msg, _( "Error" ), wxICON_ERROR | wxOK, (wxWindow *)Manager::Get()->GetAppWindow() );
        Manager::Get()->GetLogManager()->DebugLog( l_Msg );
        return false;
    }

    if ( !getDependencies( p_BuildTarget, l_pCompiler ) ) return false;

    if ( !m_VariablesIsSaved )
    {
        m_Variables.AddVariable(_T("CPP"),l_pCompiler->GetPrograms().CPP);
        m_Variables.AddVariable(_T("CC"),l_pCompiler->GetPrograms().C);
        m_Variables.AddVariable(_T("LD"),l_pCompiler->GetPrograms().LD);
        m_Variables.AddVariable(_T("LIB"),l_pCompiler->GetPrograms().LIB);
        m_Variables.AddVariable(_T("WINDRES"),l_pCompiler->GetPrograms().WINDRES);
    }

    const wxArrayString& l_CommandsBeforeBuild = p_BuildTarget->GetCommandsBeforeBuild();
    const wxArrayString& l_CommandsAfterBuild = p_BuildTarget->GetCommandsAfterBuild();

    wxString l_TargetFileName = p_BuildTarget->GetOutputFilename();
    Manager::Get()->GetMacrosManager()->ReplaceMacros(l_TargetFileName, p_BuildTarget);
    #ifdef USE_PRINTLOG
    printLog( _T( "cbMGMakefile::formFileForTarget" ), l_TargetFileName );
    #endif
    wxFileName l_OutFileName = UnixFilename(l_TargetFileName);
    wxString l_OutFileNameFullPath = l_OutFileName.GetFullPath();
    QuoteStringIfNeeded( l_OutFileNameFullPath );
    wxString l_OutFileNameFullPathMakefileFriendly = l_OutFileNameFullPath;
    ConvertToMakefileFriendly( l_OutFileNameFullPathMakefileFriendly );
    cbMGRule l_Rule;
    if ( 0 == l_TargetName.CmpNoCase( _T( "default" ) ) )
    {
        l_Rule.SetTarget( _T( "all" ) );
    }
    else
    {
        l_Rule.SetTarget( l_TargetName );
    }
    wxString l_Pre;
    if ( l_CommandsBeforeBuild.GetCount() > 0 )
    {
        l_Pre = l_CmdBefore;
    }
    l_Pre += l_OutFileNameFullPathMakefileFriendly;
    if ( l_CommandsAfterBuild.GetCount() > 0 )
    {
        l_Pre += _T(" ") + l_CmdAfter;
    }

    l_Rule.SetPrerequisites( l_Pre );
    m_Rules.Add( l_Rule );

    if ( l_CommandsBeforeBuild.GetCount() > 0 )
    {
        l_Rule.Clear();
        l_Rule.SetTarget( cmdphony );
        l_Rule.SetPrerequisites( l_CmdBefore );
        m_Rules.Add( l_Rule );
        l_Rule.Clear();
        l_Rule.SetTarget( l_CmdBefore );
        for ( unsigned long idx = 0; idx < l_CommandsBeforeBuild.GetCount(); idx ++ )
        {
            l_Rule.AddCommand( l_CommandsBeforeBuild[ idx ] );
        }
        m_Rules.Add( l_Rule );
    }
    if ( l_CommandsAfterBuild.GetCount() > 0 )
    {
        l_Rule.Clear();
        l_Rule.SetTarget( cmdphony );
        l_Rule.SetPrerequisites( l_CmdAfter );
        m_Rules.Add( l_Rule );
        l_Rule.Clear();
        l_Rule.SetTarget( l_CmdAfter );
        for ( unsigned long idx = 0; idx < l_CommandsAfterBuild.GetCount(); idx ++ )
        {
            l_Rule.AddCommand( l_CommandsAfterBuild[ idx ] );
        }
        m_Rules.Add( l_Rule );
    }
    l_Rule.Clear();
    l_Rule.SetTarget( l_OutFileNameFullPathMakefileFriendly );
    l_Rule.SetPrerequisites( _T("$(") + l_ObjsName + _T(")") );

    wxString kind_of_output = _T( "unknown" );
    CommandType ct = ctCount;
    bool l_TargetTypeValid = true;
    switch ( p_BuildTarget->GetTargetType() )
    {
    case ttConsoleOnly:
        ct = ctLinkConsoleExeCmd;
        kind_of_output = _T( "console executable" );
        break;

    case ttExecutable:
        ct = ctLinkExeCmd;
        kind_of_output = _T( "executable" );
        break;

    case ttDynamicLib:
        ct = ctLinkDynamicCmd;
        kind_of_output = _T( "dynamic library" );
        break;

    case ttStaticLib:
        ct = ctLinkStaticCmd;
        kind_of_output = _T( "static library" );
        break;

    case ttNative:
        ct = ctLinkNativeCmd;
        kind_of_output = _T( "native" );
        break;
    case ttCommandsOnly:
        ct = ctLinkConsoleExeCmd;
        kind_of_output = _T( "commands only" );
        break;
    default:
        l_TargetTypeValid = false;
        break;

//      case ttCommandsOnly:
//          // add target post-build commands
//          ret.Clear();
//          AppendArray(GetPostBuildCommands(target), ret);
//          return ret;
//          break;
        break;
    }
    /*if(ttCommandsOnly == lTarget->GetTargetType())
    {
      GetPostBuildCommands(lTarget);
    }
    else*/
    {
        l_Rule.AddCommand( _T( "echo Building " ) + kind_of_output + _T( " " ) + l_OutFileNameFullPath );
        if( l_TargetTypeValid )
        {
            wxString l_LinkerCmd = l_pCompiler->GetCommand( ct );

        #if wxCHECK_VERSION(2, 9, 0)
            Manager::Get()->GetLogManager()->DebugLog(wxString::Format( _("LinkerCmd: %s"), l_LinkerCmd.wx_str()) );
        #else
            Manager::Get()->GetLogManager()->DebugLog(wxString::Format( _("LinkerCmd: %s"), l_LinkerCmd.c_str()) );
        #endif
            l_pCompiler->GenerateCommandLine( l_LinkerCmd,
                                              p_BuildTarget,
                                              NULL,
                                              l_OutFileNameFullPath,
//                                              _T("$$(") + l_TargetName + _T(")"),
                                              /* fix by oBFusCATed */
                                              _T("$$(") + l_ObjsName + _T(")"),
                                              wxEmptyString,
                                              wxEmptyString );
            l_Rule.AddCommand( l_LinkerCmd );
        }
    }
    m_Rules.Add( l_Rule );

    // Form rules
    wxString l_Tmp;

    unsigned long ii;

    wxString macro;
    wxString file;
    wxString object;
    wxString FlatObject;
    wxString deps;

    cbMGSortFilesArray files = GetProjectFilesSortedByWeight(p_BuildTarget,true,false);
    unsigned long lnb_files = files.GetCount();

    // fix by Sasquatch_47
    //need to clear the m_Objs string for cases where one is generating a makefile for more than one configuration
    m_Objs.clear();

    for ( ii = 0; ii < lnb_files; ii++ )
    {
        l_Rule.Clear();
        ProjectFile* pf = files[ ii ];
        const pfDetails& pfd = pf->GetFileDetails( p_BuildTarget );
        wxString l_Object = ( l_pCompiler->GetSwitches().UseFlatObjects )?pfd.object_file_flat:pfd.object_file;
        wxString l_CompilerCmd;
        // lookup file's type
        FileType ft = FileTypeOf( pf->relativeFilename );
        bool isResource = ft == ftResource;
        bool isHeader = ft == ftHeader;
        if ( !isHeader || l_pCompiler->GetSwitches().supportsPCH )
        {
            pfCustomBuild& pcfb = pf->customBuild[l_pCompiler->GetID()];
            l_CompilerCmd = pcfb.useCustomBuildCommand
                            ? pcfb.buildCommand
                            : l_pCompiler->GetCommand( isResource ? ctCompileResourceCmd : ctCompileObjectCmd );
            wxString l_SourceFile;
            wxString l_SourceFileMakefileFriendly;
            if ( l_pCompiler->GetSwitches().UseFullSourcePaths )
            {
                wxString l_fName = pfd.source_file_absolute_native;
                if( l_fName != wxEmptyString )
                {

                    #ifdef USE_DEBUGSAVE
                    p_File.AddLine( _T("# File: ") + pfd.source_file_absolute_native );
                    p_File.AddLine( _T("# File: ") + l_fName );
                    p_File.AddLine( wxEmptyString );
                    #endif

                    #ifdef USE_PRINTLOG
                    printLog( _T( "cbMGMakefile::formFileForTarget" ), l_fName );
                    #endif
                    l_SourceFile = UnixFilename( l_fName );
                    // for resource files, use short from if path because if windres bug with spaces-in-paths
                    if ( isResource )
                    {
                        l_SourceFile = pf->file.GetShortPath();
                    }
                }
            }
            else
            {
                l_SourceFile = pfd.source_file;
            }
            l_SourceFileMakefileFriendly = l_SourceFile;
            ConvertToMakefileFriendly( l_SourceFileMakefileFriendly );
            QuoteStringIfNeeded( l_SourceFile );
        #if wxCHECK_VERSION(2, 9, 0)
            Manager::Get()->GetLogManager()->DebugLog(wxString::Format( _("CompilerCmd: %s"), l_CompilerCmd.wx_str()) );
        #else
            Manager::Get()->GetLogManager()->DebugLog(wxString::Format( _("CompilerCmd: %s"), l_CompilerCmd.c_str()) );
        #endif
            /* FIXME: traps after next command */
            l_pCompiler->GenerateCommandLine( l_CompilerCmd,
                                              p_BuildTarget,
                                              pf,
                                              l_SourceFile,
                                              l_Object,
                                              pfd.object_file_flat,
                                              pfd.dep_file );
            if ( m_Objs.size() )
            {
                m_Objs += _T(' ');
            }
            m_Objs += l_Object;
            l_Rule.SetTarget( l_Object );
            l_Rule.SetPrerequisites( l_SourceFileMakefileFriendly );
            l_Rule.AddCommand( _T( "echo Compiling: " ) + l_SourceFile );
            l_Rule.AddCommand( l_CompilerCmd );
            m_Rules.Add( l_Rule );
        }
    }
    for ( unsigned long i=0; i < m_Deps.Count(); i++ )
    {
        l_Rule.Clear();
        l_Rule.SetTarget( m_Deps.GetVarName( i ) );
        l_Rule.SetPrerequisites( m_Deps.GetVariable( i ) );
        m_Rules.Add( l_Rule );
    }

    l_Rule.Clear();
    l_Rule.SetTarget( cmdphony );
    l_Rule.SetPrerequisites( l_CmdClean );
    m_Rules.Add( l_Rule );
    l_Rule.Clear();
    l_Rule.SetTarget( l_CmdClean );
    l_Rule.AddCommand( _T( "echo Delete $(" ) + l_ObjsName + _T( ") " ) + l_OutFileNameFullPath );
#ifdef __WXMSW__
    l_Rule.AddCommand( _T( "-del /f $(" ) + l_ObjsName + _T( ") " ) + l_OutFileNameFullPath );
#else
    l_Rule.AddCommand( _T( "-rm -f $(" ) + l_ObjsName + _T( ") " ) + l_OutFileNameFullPath );
#endif
    m_Rules.Add( l_Rule );

    if ( !m_VariablesIsSaved )
    {
        m_Variables.SaveAllVars( p_File );
        m_VariablesIsSaved = true;
    }
    p_File.AddLine( _T("# Target: ") + l_TargetName );
    p_File.AddLine( wxEmptyString );
    p_File.AddLine( l_ObjsName + _T("=") + m_Objs );
    p_File.AddLine( wxEmptyString );
    SaveAllRules( p_File );
    p_File.AddLine( wxEmptyString );
    p_File.AddLine( wxEmptyString );
    l_Ret = true;
    return l_Ret;
}

/*
 * Save result makefile
 */
bool cbMGMakefile::SaveMakefile()
{
    bool l_Ret = false;
    wxString l_FullFilename = m_Path + m_Filename;

    if ( m_Overwrite && wxFileExists( l_FullFilename ) ) wxRemoveFile( l_FullFilename );

    wxTextFile l_File;
    if ( !l_File.Create( l_FullFilename ) )
    {
        wxString lMsg = _( "File " ) + l_FullFilename + _(" is exists!\nOverwrite?" );
        if (wxID_YES == cbMessageBox(lMsg, _("Warning"), wxICON_EXCLAMATION | wxYES_NO, (wxWindow *)Manager::Get()->GetAppWindow()))
        {
            wxRemoveFile( l_FullFilename );
        }
        else
        {
            return l_Ret;
        }
    }

    l_File.AddLine( sHeader );
    {
        wxString lHeaderVersion = sHeaderVersion;
        /* FIXME (shl#1#): Add HeaderVersion replacing */
        wxString lTmp;
        lTmp = wxString::Format(_T("%d"),AutoVersion::MAJOR);
        lHeaderVersion.Replace(_T("$MAJOR"),lTmp,true);
        lTmp = wxString::Format(_T("%d"),AutoVersion::MINOR);
        lHeaderVersion.Replace(_T("$MINOR"),lTmp,true);
        lTmp = wxString::Format(_T("%d"),AutoVersion::BUILD);
        lHeaderVersion.Replace(_T("$BUILD"),lTmp,true);
        lTmp = wxString::Format(_T("%d"),AutoVersion::REVISION);
        lHeaderVersion.Replace(_T("$REVISION"),lTmp,true);
        l_File.AddLine( lHeaderVersion );
    }
    l_File.AddLine( wxEmptyString );

    long l_TargetsCount = m_pProj->GetBuildTargetsCount();
    if ( l_TargetsCount < 1 )
    {
        wxString l_Msg = _( "Can't found targets!\n"
                            "C::B MakefileGen could not complete the operation." );
        cbMessageBox( l_Msg, _( "Error" ), wxICON_ERROR | wxOK, (wxWindow *)Manager::Get()->GetAppWindow() );
        Manager::Get()->GetLogManager()->DebugLog( l_Msg );
        return l_Ret;
    }

    #ifdef USE_DEBUGSAVE
    /* FIXME (shl#1#): forDebug only */
    {
        wxString lTmp;
        lTmp = wxString::Format(_T("%d"),l_TargetsCount);
        l_File.AddLine( _( "# Header wrote, target count = " ) + lTmp );
        l_File.AddLine( wxEmptyString );
        l_File.Write();
    }
    #endif

    m_DependenciesIsNotExistsIsNoProblem = false;
    if ( m_AllTargets )
    {
        #ifdef USE_DEBUGSAVE
        /* FIXME (shl#1#): forDebug only */
        l_File.AddLine( _( "# All targets" ) );
        l_File.AddLine( wxEmptyString );
        l_File.Write();
        #endif

        for ( long i = 0; i < l_TargetsCount; i++ )
        {
            ProjectBuildTarget *l_BuildTarget = m_pProj->GetBuildTarget( i );

            if ( l_BuildTarget )
            {
                if ( !m_ProceedTargets.IsEmpty() )
                {
                    m_ProceedTargets += _T(", ");
                }
                m_ProceedTargets += l_BuildTarget->GetTitle();

                #ifdef USE_DEBUGSAVE
                /* FIXME (shl#1#): forDebug only */
                l_File.AddLine( _( "# " ) + l_BuildTarget->GetTitle() );
                l_File.AddLine( wxEmptyString );
                l_File.Write();
                #endif

                l_Ret = formFileForTarget( l_BuildTarget, l_File );

                if ( l_Ret )
                {
                    l_File.Write();
                }
                else
                {
                    break;
                }
            }
        }
    }
    else
    {
        #ifdef USE_DEBUGSAVE
        /* FIXME (shl#1#): forDebug only */
        l_File.AddLine( _( "# One target" ) );
        l_File.AddLine( wxEmptyString );
        l_File.Write();
        #endif

        /* FIXME (shl#1#): return NULL!!! Why?! */
        wxString l_ActiveBuildTarget = m_pProj->GetActiveBuildTarget();

        #ifdef USE_DEBUGSAVE
        l_File.AddLine( _( "# Proj Title = " ) + m_pProj->GetTitle() );
        l_File.AddLine( _( "# Active Build Target = " ) + l_ActiveBuildTarget );
        l_File.AddLine( wxEmptyString );
        l_File.Write();
        #endif

        if( m_pProj->HasVirtualBuildTarget( l_ActiveBuildTarget ) )
        {
          #ifdef USE_DEBUGSAVE
          /* Virtual target! Require to expand target list */
          l_File.AddLine( _( "# Active Build Target isVirtual!" ) );
          l_File.AddLine( wxEmptyString );
          l_File.Write();
          #endif

          wxArrayString l_GroupTargets = m_pProj->GetExpandedVirtualBuildTargetGroup( l_ActiveBuildTarget );

          #ifdef USE_DEBUGSAVE
          l_File.AddLine( _( "# l_GroupTargets ready" ) );
          l_File.AddLine( wxEmptyString );
          l_File.Write();
          #endif

          wxArrayString::iterator l_ci = l_GroupTargets.begin();
          wxArrayString::iterator l_end = l_GroupTargets.end();
          for( ; l_ci != l_end; l_ci++ )
          {


            wxString l_TargetName = *l_ci;

            #ifdef USE_DEBUGSAVE
            l_File.AddLine( _( "# l_BuildTarget is " ) + ( l_TargetName ) );
            l_File.AddLine( wxEmptyString );
            l_File.Write();
            #endif

            ProjectBuildTarget *l_BuildTarget = m_pProj->GetBuildTarget( l_TargetName );

            if ( l_BuildTarget )
            {
                if( l_BuildTarget->SupportsCurrentPlatform() )
                {
                    m_ProceedTargets += l_BuildTarget->GetTitle();

                    l_Ret = formFileForTarget( l_BuildTarget, l_File );
                    if ( l_Ret )
                    {
                        l_File.Write();
                    }
                }
                else
                {
                    #ifdef USE_DEBUGSAVE
                    /* FIXME (shl#1#): forDebug only */
                    l_File.AddLine( _( "# Not supported current platform" ) );
                    l_File.AddLine( wxEmptyString );
                    l_File.Write();
                    #endif
                }
            }
            else
            {
                #ifdef USE_DEBUGSAVE
                /* FIXME (shl#1#): forDebug only */
                l_File.AddLine( _( "# l_BuildTarget is NULL" ) );
                l_File.AddLine( wxEmptyString );
                l_File.Write();
                #endif
            }
          }
        }
        else
        {
          ProjectBuildTarget *l_BuildTarget = m_pProj->GetBuildTarget( l_ActiveBuildTarget );

          if ( l_BuildTarget )
          {
              m_ProceedTargets += l_BuildTarget->GetTitle();

              #ifdef USE_DEBUGSAVE
              /* FIXME (shl#1#): forDebug only */
              l_File.AddLine( _( "# " ) + l_BuildTarget->GetTitle() );
              l_File.AddLine( wxEmptyString );
              l_File.Write();
              #endif

              l_Ret = formFileForTarget( l_BuildTarget, l_File );
              if ( l_Ret )
              {
                  l_File.Write();
              }
          }
          else
          {
              #ifdef USE_DEBUGSAVE
              /* FIXME (shl#1#): forDebug only */
              l_File.AddLine( _( "# l_BuildTarget is NULL" ) );
              l_File.AddLine( wxEmptyString );
              l_File.Write();
              #endif
          }
        }
    }
    l_File.Close();
    return l_Ret;
}

bool cbMGMakefile::SaveAllRules( wxTextFile& pFile )
{
    bool lRet = false;

    size_t num = m_Rules.GetCount();
    unsigned long i;
    unsigned long k;
    wxString l_CommandPrefix = GetCommandPrefix();
    wxString l_Prefix;

    for ( i = 0; i < num; i++ )
    {
        cbMGRule& lRule = m_Rules.Item( i );
        pFile.AddLine( lRule.GetTarget() + _T( ": " ) + lRule.GetPrerequisites() );
        for ( k = 0; k < lRule.GetCommands().GetCount(); k++ )
        {
            l_Prefix = l_CommandPrefix;
            wxString l_Cmd = lRule.GetCommands()[ k ];
            if ( _T('-') == l_Cmd[ 0 ] )
            {
                l_Cmd = l_Cmd.SubString( 1, l_Cmd.size() );
                l_Prefix += _T('-');
            }
            if ( m_IsSilence )
            {
                pFile.AddLine( l_Prefix + _T( "@" ) + l_Cmd );
            }
            else
            {
                pFile.AddLine( l_Prefix + l_Cmd );
            }
        }
        pFile.AddLine( wxEmptyString );
    }
    lRet = true;
    return lRet;
}

