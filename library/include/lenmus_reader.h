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

#ifndef __LML_LDP_READER_H__ 
#define __LML_LDP_READER_H__

#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

namespace lenmus
{

    /*! \brief Base class for any provider of LDP source code to be parsed */
    //-----------------------------------------------------------------------
    class LdpReader
    {
	public:
		LdpReader() {}
		virtual ~LdpReader() {}
		
        //! Returns the next char from the source
		virtual char get_next_char()=0;
        //! Instruct reader to repeat last returned char at next invocation of 
        //! get_next_char()
        virtual void repeat_last_char()=0;
        //! The reader is ready for get_next_char(), unget() operations
        virtual bool is_ready()=0;
        //! End of data reached. No more data available
		virtual bool end_of_data()=0;
        //! Returns the current line number (the one for char returned in last get_next_char() )
		virtual int get_line_number()=0;

    };


    /*!
    \brief A LDP reader using a file as origin of source code
    */
    //------------------------------------------------------------------
    class LdpFileReader : public LdpReader
    {
    public:
        LdpFileReader(const std::string& fullFilename);
		virtual ~LdpFileReader() {}

		virtual char get_next_char();
        virtual void repeat_last_char();
		virtual bool is_ready();
		virtual bool end_of_data();
        virtual int get_line_number() { return m_numLine; }

    private:
        std::ifstream m_file;
        const std::string& m_filename;
        int m_numLine;
        bool m_repeating_last_char;
    };


    /*!
    \brief A LDP reader using a string as origin of source code
    */
    //------------------------------------------------------------------
    class LdpTextReader : public LdpReader
    {
    public:
        LdpTextReader(const std::string& sourceText);
		virtual ~LdpTextReader() {}

		virtual char get_next_char();
        virtual void repeat_last_char();
		virtual bool is_ready();
		virtual bool end_of_data();
        virtual int get_line_number() { return 0; }

    private:
        stringstream   m_stream;

    };


}   //namespace lenmus

#endif      //__LML_LDP_READER_H__
