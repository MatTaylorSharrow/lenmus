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

#ifdef _LM_DEBUG_

#include <UnitTest++.h>
#include <sstream>

//classes related to these tests
#include "lenmus_injectors.h"
#include "lenmus_compiler.h"
#include "lenmus_internal_model.h"
#include "lenmus_basic_model.h"

using namespace UnitTest;
using namespace std;
using namespace lenmus;


class LdpCompilerTestFixture
{
public:

    LdpCompilerTestFixture()     //SetUp fixture
    {
        m_pLibraryScope = new LibraryScope(cout);
        m_scores_path = "../../../test-scores/";        //linux CodeBlobks
        //m_scores_path = "../../../../test-scores/";        //windows MS Visual studio .NET
    }

    ~LdpCompilerTestFixture()    //TearDown fixture
    {
        delete m_pLibraryScope;
    }

    LibraryScope* m_pLibraryScope;
    std::string m_scores_path;
};

SUITE(LdpCompilerTest)
{
    TEST_FIXTURE(LdpCompilerTestFixture, LdpCompilerCreateEmpty)
    {
        DocumentScope documentScope(cout);
        LdpCompiler compiler(*m_pLibraryScope, documentScope);
        ImoDocument* pDoc = compiler.create_empty();
        CHECK( pDoc->get_version() == "0.0" );
        CHECK( pDoc->get_num_content_items() == 0 );
    }

    TEST_FIXTURE(LdpCompilerTestFixture, LdpCompilerFromString)
    {
        DocumentScope documentScope(cout);
        LdpCompiler compiler(*m_pLibraryScope, documentScope);
        ImoDocument* pDoc = compiler.compile_string("(lenmusdoc (vers 0.0) (content (score (vers 1.6) (language en iso-8859-1) (opt Score.FillPageWithEmptyStaves true) (opt StaffLines.StopAtFinalBarline false) (instrument (musicData )))))" );
        CHECK( pDoc->get_version() == "0.0" );
        CHECK( pDoc->get_num_content_items() == 1 );
        ImoScore* pScore = dynamic_cast<ImoScore*>( pDoc->get_content_item(0) );
        CHECK( pScore != NULL );
        CHECK( pScore->get_num_instruments() == 1 );
        CHECK( pScore->get_staffobjs_table() != NULL );
        CHECK( pScore->get_version() == "1.6" );
    }

    TEST_FIXTURE(LdpCompilerTestFixture, LdpCompilerScoreIdsFixed)
    {
        DocumentScope documentScope(cout);
        LdpCompiler compiler(*m_pLibraryScope, documentScope);
        ImoDocument* pDoc = compiler.compile_string("(score (vers 1.6) (language en iso-8859-1) (systemLayout first (systemMargins 0 0 0 2000)) (systemLayout other (systemMargins 0 0 1200 2000)) (opt Score.FillPageWithEmptyStaves true) (opt StaffLines.StopAtFinalBarline false) (instrument (musicData )))" );
        CHECK( pDoc->get_version() == "0.0" );
        CHECK( pDoc->get_num_content_items() == 1 );
        ImoScore* pScore = dynamic_cast<ImoScore*>( pDoc->get_content_item(0) );
        CHECK( pScore != NULL );
        CHECK( pScore->get_id() == 3L );
        CHECK( pScore->get_num_instruments() == 1 );
        ImoInstrument* pInstr = pScore->get_instrument(0);
        CHECK( pInstr->get_id() == 12L );
    }

    TEST_FIXTURE(LdpCompilerTestFixture, LdpCompilerFromFile)
    {
        DocumentScope documentScope(cout);
        LdpCompiler compiler(*m_pLibraryScope, documentScope);
        ImoDocument* pDoc = compiler.compile_file(m_scores_path + "00011-empty-fill-page.lms");
        CHECK( pDoc->get_version() == "0.0" );
        CHECK( pDoc->get_num_content_items() == 1 );
        ImoScore* pScore = dynamic_cast<ImoScore*>( pDoc->get_content_item(0) );
        CHECK( pScore != NULL );
        CHECK( pScore->get_id() == 3L );
        CHECK( pScore->get_num_instruments() == 1 );
        ImoInstrument* pInstr = pScore->get_instrument(0);
        CHECK( pInstr->get_id() == 12L );
    }

//    TEST_FIXTURE(LdpCompilerTestFixture, LdpCompilerCreateElement)
//    {
//        DocumentScope documentScope(cout);
//        LdpCompiler compiler(*m_pLibraryScope, documentScope);
//        LdpElement* pElm = compiler.create_element("(systemLayout first (systemMargins 0 0 0 2000))" );
//        CHECK( pElm != NULL );
//        //cout << pElm->to_string() << endl;
//        CHECK( pElm->to_string() == "(systemLayout first (systemMargins 0 0 0 2000))" );
//        BasicModel* pBasicModel = compiler.get_outcome();
//        delete pBasicModel;
//        delete pElm;
//    }
//
//    TEST_FIXTURE(LdpCompilerTestFixture, LdpCompiler_CreateBasicModel)
//    {
//        DocumentScope documentScope(cout);
//        LdpCompiler compiler(*m_pLibraryScope, documentScope);
//        BasicModel* pBasicModel = compiler.create_basic_model("(n e4 q.)" );
//        DtoObj* pObj = pBasicModel->get_root();
//        CHECK( pObj != NULL );
//        CHECK( pObj->is_note() == true );
//        delete pBasicModel;
//    }

};

#endif  // _LM_DEBUG_

