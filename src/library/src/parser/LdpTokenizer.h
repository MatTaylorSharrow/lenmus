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

#ifndef __LM_LDP_TOKEN_H__ 
#define __LM_LDP_TOKEN_H__

#include "../global/StringType.h"

using namespace std;

namespace lenmus
{

    class LdpReader;

enum ETokenType {
    tkStartOfElement = 0,
    tkEndOfElement,
    tkIntegerNumber,
    tkRealNumber,
    tkLabel,
    tkString,
    tkEndOfFile,
    //tokens for internal use
    tkSpaces,        //token separator
    tkComment        //to be filtered out in tokenizer routines
};


    /*!
    \brief The lexical analyzer decompose the input into tokens. Class LdpToken represents a token
    */
    //----------------------------------------------------------------------------------------------
    class LdpToken
    {
    private:
        string_type m_value;
        ETokenType m_type;

    public:
        LdpToken(ETokenType type, string_type value) 
            : m_type(type), m_value(value) {}
        LdpToken(ETokenType type, char_type value) 
            : m_type(type), m_value(_T("")) { m_value += value; }

        ~LdpToken() {}

        inline ETokenType get_type() { return m_type; }
        inline const string_type& get_value() { return m_value; }
    };

    /*!
    \brief implements the lexical analyzer
    */
    //----------------------------------------------------------------------------------------------
    class LdpTokenizer
    {
    public:
        LdpTokenizer(LdpReader& reader, tostream& reporter);
        ~LdpTokenizer();

        inline void repeat_token() { m_repeatToken = true; }
        LdpToken* read_token();

    private:
        LdpToken* parse_new_token();

        bool is_number(char_type ch);
        bool is_letter(char_type ch);

        LdpReader&  m_reader;
        tostream&    m_reporter;
        bool        m_repeatToken;
        LdpToken*   m_pToken;

        //to deal with compact notation [  name:value  -->  (name value)  ]
        bool        m_expectingEndOfElement;
        bool        m_expectingValuePart;
        bool        m_expectingNamePart;
        LdpToken*   m_pTokenNamePart;
    };


} //namespace lenmus

#endif      //__LM_LDP_TOKEN_H__