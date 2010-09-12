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

#include <algorithm>
#include <iostream>

#include "lenmus_parser.h"
#include "lenmus_reader.h"
#include "lenmus_ldp_factory.h"
#include "lenmus_exceptions.h"

using namespace std;

namespace lenmus
{


//========================================================================================
// LdpParser implementation
//========================================================================================

LdpParser::LdpParser(ostream& reporter, LdpFactory* pFactory)
    : m_reporter(reporter)
    , m_pLdpFactory(pFactory)
    //, m_fDebugMode(g_pLogger->IsAllowedTraceMask("LdpParser"))
    //, m_pIgnoreSet((std::set<long>*)NULL)
    , m_pTokenizer(NULL)
    , m_pTk(NULL)
    , m_curNode(NULL)
{
}

LdpParser::~LdpParser()
{
    clear_all();
}

void LdpParser::clear_all()
{
    if (m_pTokenizer)
    {
        delete m_pTokenizer;
        m_pTokenizer = NULL;
    }

    while (!m_stack.empty())
    {
        std::pair<EParsingState, LdpElement*> data = m_stack.top();
        delete data.second;
        m_stack.pop();
    }
    m_curNode = NULL;
    m_numErrors = 0;
}

LdpTree* LdpParser::parse_text(const std::string& sourceText)
{
    LdpTextReader reader(sourceText);
    return do_syntax_analysis(reader);
}

LdpTree* LdpParser::parse_file(const std::string& filename, bool fErrorMsg)
{
    LdpFileReader reader(filename);
    return do_syntax_analysis(reader);
}

LdpTree* LdpParser::do_syntax_analysis(LdpReader& reader)
{
    //This function analyzes source code. The result of the analysis is a tree
    //of nodes, each one representing an element. The root node is the parsed
    //elemnent, usually the whole score. Nevertheless, the parser can be used
    //to parse any sub-element, such as a note, or a measure.
    //
    //This method performs the lexical and syntactical analysis and,
    //as result, builds a tree of syntactically correct nodes: the source code
    //has the structure of an element with nested elements (data between parenthesis).
    //
    //The analyzer is implemented with a main loop to deal with current
    //automata state and as many functions as automata states, to perform the
    //tasks asociated to each state.

    clear_all();

    m_pTokenizer = new LdpTokenizer(reader, m_reporter);
    m_id = 0L;
    m_state = A0_WaitingForStartOfElement;
    PushNode(A0_WaitingForStartOfElement);      //start the tree with the root node
    bool fExitLoop = false;
    while(!fExitLoop)
    {
        m_pTk = m_pTokenizer->read_token();        //m_pTk->read_token();

        switch (m_state) {
            case A0_WaitingForStartOfElement:
                Do_WaitingForStartOfElement();
                break;
            case A1_WaitingForName:
                Do_WaitingForName();
                break;
            case A2_WaitingForParameter:
                Do_ProcessingParameter();
                break;
            case A3_ProcessingParameter:
                Do_ProcessingParameter();
                break;
            case A4_Exit:
            case A5_ExitError:
                fExitLoop = true;
                break;
            default:
                report_error(m_state, m_pTk);
                fExitLoop = true;
        }
        if (m_pTk->get_type() == tkEndOfFile)
            fExitLoop = true;
    }
    m_nMaxId = --m_id;

    // exit if error
    if (m_state == A5_ExitError)
        return new LdpTree();

    // at this point m_curNode is all the tree
    if (!m_curNode)
        throw ldp_format_error();

    return new LdpTree(m_curNode);
}

void LdpParser::Do_WaitingForStartOfElement()
{
    switch (m_pTk->get_type())
    {
        case tkStartOfElement:
            m_state = A1_WaitingForName;
            break;
        case tkEndOfFile:
            m_state = A4_Exit;
            break;
        default:
            report_error(m_state, m_pTk);
            m_state = A0_WaitingForStartOfElement;
    }
}

void LdpParser::Do_WaitingForName()
{
    switch (m_pTk->get_type())
    {
        case tkLabel:
        {
            //check if the name has an ID and extract it
            const std::string& tagname = m_pTk->get_value();
            std::string nodename = tagname;
            size_t i = tagname.find('#');
            if (i != string::npos)
            {
                long id;
                nodename = tagname.substr(0, i);
                std::istringstream sid( tagname.substr(i+1) );
                if (!(sid >> id))
                    m_reporter << "Line " << m_pTk->get_line_number()
                               << ". Bad id in name '" + tagname + "'." << endl;
                else
                {
                    if (id < m_id)
                        m_reporter << "Line " << m_pTk->get_line_number()
                                << ". In '" + tagname + "'. Value for id already exists. Ignored." << endl;
                    else
                        m_id = id;
                }
            }

            //create the node
            m_curNode = m_pLdpFactory->create(nodename, m_pTk->get_line_number());
            if (m_curNode->get_type() == k_undefined)
                m_reporter << "Line " << m_pTk->get_line_number()
                           << ". Unknown tag '" + nodename + "'." << endl;
            m_curNode->set_id(m_id++);
            m_state = A2_WaitingForParameter;
            break;
        }

        default:
            report_error(m_state, m_pTk);
            if (m_pTk->get_type() == tkEndOfFile)
                m_state = A4_Exit;
            else
                m_state = A1_WaitingForName;
    }

}

void LdpParser::Do_WaitingForParameter()
{
    //switch (m_pTk->get_type())
    //{
    //    case tkStartOfElement:
    //        PushNode(A3_ProcessingParameter);    // add current node (name of element or parameter) to the tree
    //        m_pTokenizer->repeat_token();
    //        m_state = A0_WaitingForStartOfElement;
    //        break;
    //    case tkLabel:
    //        //m_curNode->append_child( m_pLdpFactory->new_label(m_pTk->get_value(),
    //        //                                                  m_pTk->get_line_number()) );
    //        //m_state = A3_ProcessingParameter;
    //        //break;
    //        if ( must_replace_tag(m_pTk->get_value()) )
    //            replace_current_tag();
    //        else
    //        {
    //            m_curNode->append_child( m_pLdpFactory->new_label(m_pTk->get_value(),
    //                                                              m_pTk->get_line_number()) );
    //            m_state = A3_ProcessingParameter;
    //        }
    //        break;
    //    case tkIntegerNumber:
    //    case tkRealNumber:
    //        m_curNode->append_child( m_pLdpFactory->new_number(m_pTk->get_value(),
    //                                                           m_pTk->get_line_number()) );
    //        m_state = A3_ProcessingParameter;
    //        break;
    //    case tkString:
    //        m_curNode->append_child( m_pLdpFactory->new_string(m_pTk->get_value(),
    //                                                           m_pTk->get_line_number()) );
    //        m_state = A3_ProcessingParameter;
    //        break;
    //    default:
    //        report_error(m_state, m_pTk);
    //        if (m_pTk->get_type() == tkEndOfFile)
    //            m_state = A4_Exit;
    //        else
    //            m_state = A2_WaitingForParameter;
    //}
}

void LdpParser::Do_ProcessingParameter()
{
    switch (m_pTk->get_type())
    {
        case tkLabel:
            //m_curNode->append_child( m_pLdpFactory->new_label(m_pTk->get_value(),
            //                                                  m_pTk->get_line_number()) );
            //m_state = A3_ProcessingParameter;
            //break;
            if ( must_replace_tag(m_pTk->get_value()) )
                replace_current_tag();
            else
            {
                m_curNode->append_child( m_pLdpFactory->new_label(m_pTk->get_value(),
                                                                  m_pTk->get_line_number()) );
                m_state = A3_ProcessingParameter;
            }
            break;
        case tkIntegerNumber:
        case tkRealNumber:
            m_curNode->append_child( m_pLdpFactory->new_number(m_pTk->get_value(),
                                                               m_pTk->get_line_number()) );
            m_state = A3_ProcessingParameter;
            break;
        case tkString:
            m_curNode->append_child( m_pLdpFactory->new_string(m_pTk->get_value(),
                                                               m_pTk->get_line_number()) );
            m_state = A3_ProcessingParameter;
            break;
        case tkStartOfElement:
            PushNode(A3_ProcessingParameter);    // add current node (name of element or parameter) to the tree
            m_pTokenizer->repeat_token();
            m_state = A0_WaitingForStartOfElement;
            break;
        case tkEndOfElement:
            terminate_current_parameter();
            break;
        default:
            report_error(m_state, m_pTk);
            if (m_pTk->get_type() == tkEndOfFile)
                m_state = A4_Exit;
            else
                m_state = A3_ProcessingParameter;
    }
}

bool LdpParser::must_replace_tag(const std::string& nodename)
{
    return nodename == "noVisible";
}

void LdpParser::replace_current_tag()
{
    //TODO: refactor to deal with many replacements

    PushNode(A3_ProcessingParameter);    // add current node (name of element or parameter) to the tree

    //get new element name
    //const std::string& oldname = m_pTk->get_value();
    std::string newname;
    //if (oldname == "noVisible")
        newname = "visible";
    //else if (oldname == "l")
    //    newname = "tied";

    //create the replacement node
    m_curNode = m_pLdpFactory->create(newname, m_pTk->get_line_number());
    m_curNode->set_id(m_id++);

    //add parameter
    m_curNode->append_child( m_pLdpFactory->new_label("no",
                                                      m_pTk->get_line_number()) );

    //close node
    terminate_current_parameter();
}

void LdpParser::terminate_current_parameter()
{
    m_state = A3_ProcessingParameter;
    LdpElement* pParm = m_curNode;        //save ptr to node just created
    if (PopNode()) {                      //restore previous node (the owner of this parameter)
        //error
        m_state = A5_ExitError;
    }
    else
    {
        if (m_curNode)
            m_curNode->append_child(pParm);
        else
            m_curNode = pParm;

        ////Filter out this element if its ID is in the ignore list
        //long nId = GetNodeId(pParm);
        //if (!(m_pIgnoreSet
        //      && nId != lmNEW_ID
        //      && m_pIgnoreSet->find(nId) != m_pIgnoreSet->end() ))
        //    m_curNode->append_child(pParm);
        //else
        //    delete pParm;   //discard this node
    }
}

void LdpParser::PushNode(EParsingState state)
{
    std::pair<EParsingState, LdpElement*> data(state, m_curNode);
    m_stack.push(data);
}

//! returns true if error
bool LdpParser::PopNode()
{
    if (m_stack.size() == 0)
    {
        //more closing parenthesis than parenthesis opened
        report_error("Syntax error: more closing parenthesis than parenthesis opened. Analysis stopped.");
        return true;    //error
    }
    else
    {
        std::pair<EParsingState, LdpElement*> data = m_stack.top();
        m_state = data.first;
        m_curNode = data.second;
        m_stack.pop();
        return false;   //no error
    }
}

void LdpParser::report_error(EParsingState nState, LdpToken* pTk)
{
    m_numErrors++;
    m_reporter << "** LDP ERROR **: Syntax error. State " << nState
               << ", TkType " << pTk->get_type()
               << ", tkValue <" << pTk->get_value() << ">" << endl;
}

void LdpParser::report_error(const std::string& msg)
{
    m_numErrors++;
    m_reporter << msg << endl;
}

//========================================================================================
//========================================================================================
//========================================================================================
#if 0
long LdpParser::GetNodeId(LdpElement* pNode)
{
    long nId = pNode->get_id();
    return nId;
}

bool LdpParser::ParenthesisMatch(const std::string& sSource)
{
    int i = sSource.length();
    int nPar = 0;
    for(i=0; i < (int)sSource.length(); i++) {
        if (sSource.GetChar(i) == _T('('))
            nPar++;
        else if (sSource.GetChar(i) == _T(')'))
            nPar--;
    }
    return (nPar == 0);
}


#endif

} //namespace lenmus

