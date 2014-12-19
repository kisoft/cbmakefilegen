#ifndef CBMGRULE_HPP
#define CBMGRULE_HPP

#include <wx/string.h>
#include <wx/arrstr.h>

/**
 *     TARGET ... : PREREQUISITES ...
 *            COMMAND
 *            ...
 *            ...
 *     or
 *     TARGETS : PREREQUISITES ; COMMAND
 *             COMMAND
 *             ...
 *
 *     The order of rules is not significant, except for determining the
 *  "default goal": the target for `make' to consider, if you do not
 *  otherwise specify one.  The default goal is the target of the first
 *  rule in the first makefile.  If the first rule has multiple targets,
 *  only the first target is taken as the default.  There are two
 *  exceptions: a target starting with a period is not a default unless it
 *  contains one or more slashes, `/', as well; and, a target that defines
 *  a pattern rule has no effect on the default goal.  (*Note Defining and
 *  Redefining Pattern Rules: Pattern Rules.)
 *
 *  Example:
 *    foo.o : foo.c defs.h       # module for twiddling the frobs
 *            cc -c -g foo.c
 **/

class cbMGRule
{
    wxString      m_Target;
    wxString      m_Prerequisites;
    wxArrayString m_Command;
    bool          m_IsDefault;
public:
    cbMGRule();
    cbMGRule(const cbMGRule& pRule);
    virtual ~cbMGRule();

    void setDefault( bool pIsDefault )
    {
        m_IsDefault = pIsDefault;
    }
    bool IsDefault() const
    {
        return m_IsDefault;
    }
    const wxString& GetTarget() const
    {
        return m_Target;
    }
    void SetTarget(const wxString& pTarget)
    {
        m_Target = pTarget;
    }
    const wxString& GetPrerequisites() const
    {
        return m_Prerequisites;
    }
    void SetPrerequisites(const wxString& pPrerequisites)
    {
        m_Prerequisites = pPrerequisites;
    }
    const wxArrayString& GetCommands() const
    {
        return m_Command;
    }
    long AddCommand( const wxString& pCommand )
    {
        m_Command.Add( pCommand );
        return m_Command.GetCount();
    }
    void Clear();
protected:
private:
};

#endif // RULE_HPP
