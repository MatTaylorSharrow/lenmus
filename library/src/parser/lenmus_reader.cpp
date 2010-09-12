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

#include <iostream>
#include <sstream>
#include <stdexcept>

#include "lenmus_reader.h"

using namespace std;

namespace lenmus
{

LdpFileReader::LdpFileReader(const std::string& fullFilename)
    : LdpReader()
    , m_file(fullFilename.c_str())
    , m_filename(fullFilename)
    , m_numLine(1)
    , m_repeating_last_char(false)
{
    if(!m_file.is_open())
    {
        stringstream s;
        s << "File not found: \"" << fullFilename << "\"";
        throw std::invalid_argument(s.str());
    }
}

char LdpFileReader::get_next_char()
{
    char ch = m_file.get();
    if (!m_repeating_last_char && ch == 0x0a)
        m_numLine++;
    m_repeating_last_char = false;
    return ch;
}

void LdpFileReader::repeat_last_char()
{
    m_repeating_last_char = true;
    m_file.unget();
}

bool LdpFileReader::is_ready()
{
    return m_file.is_open();
}

bool LdpFileReader::end_of_data()
{
    return m_file.eof();
}



//---------------------------------------------------------------------------------

LdpTextReader::LdpTextReader(const std::string& sourceText)
    : LdpReader()
    , m_stream(sourceText)
{
}

char LdpTextReader::get_next_char()
{
    return m_stream.get();
}

void LdpTextReader::repeat_last_char()
{
    m_stream.unget();
}

bool LdpTextReader::is_ready()
{
    return true;
}

bool LdpTextReader::end_of_data()
{
    return m_stream.peek() == EOF;
}


}  //namespace lenmus
