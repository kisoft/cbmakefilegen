#include "Rule.hpp"

cbMGRule::cbMGRule()
{
    //ctor
}

cbMGRule::~cbMGRule()
{
    //dtor
}


cbMGRule::cbMGRule(const cbMGRule& pRule)
{
    m_Target = pRule.m_Target;
    m_Prerequisites = pRule.m_Prerequisites;
    m_Command = pRule.m_Command;
    m_IsDefault = pRule.m_IsDefault;
}

void cbMGRule::Clear()
{
    m_Target = wxEmptyString;
    m_Prerequisites = wxEmptyString;
    m_Command.Clear();
    m_IsDefault = false;
}
