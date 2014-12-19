#ifndef VARIABLE_HPP
#define VARIABLE_HPP

#include <wx/arrstr.h>
#include <wx/textfile.h>

class cbMGVariable
{
    wxArrayString    m_VariableName;
    wxArrayString    m_VariableValue;
public:
    cbMGVariable();
    virtual ~cbMGVariable();
    void AddVariable(const wxString& pVarName,const wxString& pVarValue);
    wxString GetVariable(const wxString& pVarName) const;
    wxString GetVariable( unsigned long Index ) const;
    wxString GetVarName( unsigned long Index ) const;
    void AppendValue(const wxString& pVarName,const wxString& pVarValue,const wxChar& pDelimiter = _T(' '));
    bool SaveAllVars( wxTextFile& pFile );
    void Clear();
    unsigned long Count() const;
protected:
    long GetVariableIndex(const wxString& pVarName) const;
private:
};



#endif // VARIABLE_HPP
