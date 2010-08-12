//--------------------------------------------------------------------------------------
//  LenMus Library
//  Copyright (c) 2010 LenMus project
//
//  This program is free software; you can redistribute it and/or modify it under the 
//  terms of the GNU General Public License as published by the Free Software Foundation,
//  either version 3 of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful, but WITHOUT ANY 
//  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A 
//  PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public License along
//  with this library; if not, see <http://www.gnu.org/licenses/> or write to the
//  Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, 
//  MA  02111-1307,  USA.
//
//  For any comment, suggestion or feature request, please contact the manager of
//  the project at cecilios@users.sourceforge.net
//
//-------------------------------------------------------------------------------------

#ifndef __LM_LDP_PARSER_H__ 
#define __LM_LDP_PARSER_H__

#include <vector>
#include <set>

#include "lenmus_factory.h"
#include "lenmus_tokenizer.h"
#include "lenmus_elements.h"

using namespace std;

namespace lenmus
{

//forward declarations
class LdpFactory;

// The LDP parser
class LdpParser
{
public:
    LdpParser(ostream& reporter, LdpFactory* pFactory);
    ~LdpParser();

//    //setings and options
//    inline void SetIgnoreList(std::set<long>* pSet) { m_pIgnoreSet = pSet; }
//
    LdpTree* parse_file(const std::string& filename, bool fErrorMsg = true);
    LdpTree* parse_text(const std::string& sourceText);

    inline int get_num_errors() { return m_numErrors; }
    inline long get_max_id() { return m_nMaxId; }

protected:
    enum EParsingState
    {
        A0_WaitingForStartOfElement = 0,
        A1_WaitingForName,
        A2_WaitingForParameter,
        A3_ProcessingParameter,
        A4_Exit,
        A5_ExitError
    };

    LdpTree* do_syntax_analysis(LdpReader& reader);

    void clear_all();
    void PushNode(EParsingState nPopState);
    bool PopNode();
    void Do_WaitingForStartOfElement();
    void Do_WaitingForName();
    void Do_WaitingForParameter();
    void Do_ProcessingParameter();

    void report_error(EParsingState nState, LdpToken* pTk);
    void report_error(const std::string& msg);

//    long GetNodeId(SpLdpElement pNode);
//

    ostream&        m_reporter;
    LdpFactory*     m_pLdpFactory;
    LdpTokenizer*   m_pTokenizer;
    LdpToken*       m_pTk;              // current token
    EParsingState   m_state;            // current automata state
    long            m_id;
    std::stack<pair<EParsingState, LdpElement*> >  m_stack;    // To save current automata state and node
    LdpElement*     m_curNode;             //node in process

    // parsing control, options and error variables
//    bool            m_fDebugMode;
    int            m_numErrors;     // number of errors found during parsing
    long           m_nMaxId;        //maximun ID found
//    std::set<long>*         m_pIgnoreSet;   //set with elements to ignore
};


} //namespace lenmus

#endif    //__LM_LDP_PARSER_H__
