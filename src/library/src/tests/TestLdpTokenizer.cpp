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

#include <UnitTest++.h>
#include <iostream>

//classes related to these tests
#include "../parser/LdpReader.h"
#include "../parser/LdpTokenizer.h"


using namespace UnitTest;
using namespace std;
using namespace lenmus;


class LdpTokenizerTestFixture
{
public:

    LdpTokenizerTestFixture()     //SetUp fixture
    {
    }

    ~LdpTokenizerTestFixture()    //TearDown fixture
    {
    }
};

SUITE(LdpTokenizerTest)
{
    TEST_FIXTURE(LdpTokenizerTestFixture, TokenizerReadTokens)
    {
        LdpTextReader reader("(score blue)");
        LdpTokenizer tokenizer(reader, cout);
        LdpToken* token = tokenizer.read_token();
        CHECK( token != NULL );
    }

    TEST_FIXTURE(LdpTokenizerTestFixture, TokenizerReadRightTokens)
    {
        LdpTextReader reader("(score blue)");
        LdpTokenizer tokenizer(reader, cout);
        LdpToken* token = tokenizer.read_token();
        CHECK( token->get_type() == tkStartOfElement );
        token = tokenizer.read_token();
        CHECK( token->get_type() == tkLabel );
        CHECK( token->get_value() == "score" );
        token = tokenizer.read_token();
        CHECK( token->get_type() == tkLabel );
        CHECK( token->get_value() == "blue" );
        token = tokenizer.read_token();
        CHECK( token->get_type() == tkEndOfElement );
        token = tokenizer.read_token();
        CHECK( token->get_type() == tkEndOfFile );
    }

    TEST_FIXTURE(LdpTokenizerTestFixture, TokenizerCanReadFile)
    {
        LdpFileReader reader("../../test-scores/00011-empty-fill-page.lms");
        LdpTokenizer tokenizer(reader, cout);
        int numTokens = 0;
        for (LdpToken* token = tokenizer.read_token();
             token->get_type() != tkEndOfFile;
             token = tokenizer.read_token())
        {
            numTokens++;
            //cout << token->get_value() << endl;
        }
        CHECK( reader.end_of_data() );
        CHECK( numTokens == 50 );
    }

    TEST_FIXTURE(LdpTokenizerTestFixture, TokenizerCanReadUnicodeString)
    {
        //cout << "'" << "Текст на кирилица" << "'" << endl;
        LdpFileReader reader("../../test-scores/00002-unicode-text.lms");
        LdpTokenizer tokenizer(reader, cout);
        int numTokens = 0;
        LdpToken* token = tokenizer.read_token();
        for (; token->get_type() != tkEndOfFile; token = tokenizer.read_token())
        {
            numTokens++;
            if (token->get_type() == tkString)
                break;
        }
        //cout << "'" << token->get_value() << "'" << endl;
        CHECK( token->get_type() == tkString );
        CHECK( numTokens == 59 );
        CHECK( token->get_value() == "Текст на кирилица" );
    }

    TEST_FIXTURE(LdpTokenizerTestFixture, TokenizerReadIntegerNumber)
    {
        LdpTextReader reader("(dx 15)");
        LdpTokenizer tokenizer(reader, cout);
        LdpToken* token = tokenizer.read_token();
        CHECK( token->get_type() == tkStartOfElement );
        token = tokenizer.read_token();
        CHECK( token->get_type() == tkLabel );
        CHECK( token->get_value() == "dx" );
        token = tokenizer.read_token();
        CHECK( token->get_type() == tkIntegerNumber );
        CHECK( token->get_value() == "15" );
    }

    TEST_FIXTURE(LdpTokenizerTestFixture, TokenizerReadCompactNotation)
    {
        LdpTextReader reader("dx:15");
        LdpTokenizer tokenizer(reader, cout);
        LdpToken* token = tokenizer.read_token();
        CHECK( token->get_type() == tkStartOfElement );
        token = tokenizer.read_token();
        CHECK( token->get_type() == tkLabel );
        CHECK( token->get_value() == "dx" );
        token = tokenizer.read_token();
        //cout << "type='" << token->get_type() << "' value='" 
        //      << "' value='" << token->get_value() << "'" << endl;
        CHECK( token->get_type() == tkIntegerNumber );
        CHECK( token->get_value() == "15" );
    }

    TEST_FIXTURE(LdpTokenizerTestFixture, TokenizerReadCompactNotationTwo)
    {
        LdpTextReader reader("dx:15 dy:12.77");
        LdpTokenizer tokenizer(reader, cout);
        LdpToken* token = tokenizer.read_token();
        CHECK( token->get_type() == tkStartOfElement );
        token = tokenizer.read_token();
        CHECK( token->get_type() == tkLabel );
        CHECK( token->get_value() == "dx" );
        token = tokenizer.read_token();
        CHECK( token->get_type() == tkIntegerNumber );
        CHECK( token->get_value() == "15" );
        token = tokenizer.read_token();
        CHECK( token->get_type() == tkEndOfElement );
        token = tokenizer.read_token();
        CHECK( token->get_type() == tkStartOfElement );
        token = tokenizer.read_token();
        CHECK( token->get_type() == tkLabel );
        CHECK( token->get_value() == "dy" );
        token = tokenizer.read_token();
        //cout << "type='" << token->get_type() << "' value='" 
        //      << "' value='" << token->get_value() << "'" << endl;
        CHECK( token->get_type() == tkRealNumber );
        CHECK( token->get_value() == "12.77" );
    }

    TEST_FIXTURE(LdpTokenizerTestFixture, TokenizerReadStringSimpleQuotes)
    {
        LdpTextReader reader(" ''this is a string'' ");
        LdpTokenizer tokenizer(reader, cout);
        LdpToken* token = tokenizer.read_token();
        CHECK( token->get_type() == tkString );
        CHECK( token->get_value() == "this is a string" );
    }

    TEST_FIXTURE(LdpTokenizerTestFixture, TokenizerReadStringDoubleQuotes)
    {
        LdpTextReader reader(" \"this is a string\" ");
        LdpTokenizer tokenizer(reader, cout);
        LdpToken* token = tokenizer.read_token();
        CHECK( token->get_type() == tkString );
        CHECK( token->get_value() == "this is a string" );
    }

    TEST_FIXTURE(LdpTokenizerTestFixture, TokenizerReadStringSimpleQuotesTranslatable)
    {
        LdpTextReader reader(" _''this is a string'' ");
        LdpTokenizer tokenizer(reader, cout);
        LdpToken* token = tokenizer.read_token();
        CHECK( token->get_type() == tkString );
        CHECK( token->get_value() == "this is a string" );
    }

    TEST_FIXTURE(LdpTokenizerTestFixture, TokenizerReadStringDoubleQuotesTranslatable)
    {
        LdpTextReader reader(" _\"this is a string\" ");
        LdpTokenizer tokenizer(reader, cout);
        LdpToken* token = tokenizer.read_token();
        CHECK( token->get_type() == tkString );
        CHECK( token->get_value() == "this is a string" );
    }

    TEST_FIXTURE(LdpTokenizerTestFixture, TokenizerReadLabel_1)
    {
        LdpTextReader reader(" #00ff45 ");
        LdpTokenizer tokenizer(reader, cout);
        LdpToken* token = tokenizer.read_token();
        CHECK( token->get_type() == tkLabel );
        CHECK( token->get_value() == "#00ff45" );
    }

    TEST_FIXTURE(LdpTokenizerTestFixture, TokenizerReadLabel_2)
    {
        LdpTextReader reader(" score ");
        LdpTokenizer tokenizer(reader, cout);
        LdpToken* token = tokenizer.read_token();
        CHECK( token->get_type() == tkLabel );
        CHECK( token->get_value() == "score" );
    }

    TEST_FIXTURE(LdpTokenizerTestFixture, TokenizerReadLabel_3)
    {
        LdpTextReader reader(" 45a ");
        LdpTokenizer tokenizer(reader, cout);
        LdpToken* token = tokenizer.read_token();
        CHECK( token->get_type() == tkLabel );
        CHECK( token->get_value() == "45a" );
    }

    TEST_FIXTURE(LdpTokenizerTestFixture, TokenizerPositiveIntegerNumber)
    {
        LdpTextReader reader(" +45 ");
        LdpTokenizer tokenizer(reader, cout);
        LdpToken* token = tokenizer.read_token();
        CHECK( token->get_type() == tkIntegerNumber );
        CHECK( token->get_value() == "+45" );
    }

    TEST_FIXTURE(LdpTokenizerTestFixture, TokenizerNegativeIntegerNumber)
    {
        LdpTextReader reader(" -45 ");
        LdpTokenizer tokenizer(reader, cout);
        LdpToken* token = tokenizer.read_token();
        CHECK( token->get_type() == tkIntegerNumber );
        CHECK( token->get_value() == "-45" );
    }

    TEST_FIXTURE(LdpTokenizerTestFixture, TokenizerPositiveRealNumber)
    {
        LdpTextReader reader(" +45.98 ");
        LdpTokenizer tokenizer(reader, cout);
        LdpToken* token = tokenizer.read_token();
        CHECK( token->get_type() == tkRealNumber );
        CHECK( token->get_value() == "+45.98" );
    }

    TEST_FIXTURE(LdpTokenizerTestFixture, TokenizerNegativeRealNumber)
    {
        LdpTextReader reader(" -45.70 ");
        LdpTokenizer tokenizer(reader, cout);
        LdpToken* token = tokenizer.read_token();
        CHECK( token->get_type() == tkRealNumber );
        CHECK( token->get_value() == "-45.70" );
    }

}

