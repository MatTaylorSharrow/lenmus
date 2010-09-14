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
#include <iostream>
#include "lenmus_config.h"

//classes related to these tests
#include "lenmus_injectors.h"
#include "lenmus_parser.h"
#include "lenmus_analyser.h"
#include "lenmus_internal_model.h"
#include "lenmus_im_note.h"
#include "lenmus_basic_model.h"
#include "lenmus_basic_objects.h"

using namespace UnitTest;
using namespace std;
using namespace lenmus;


class AnalyserTestFixture
{
public:

    AnalyserTestFixture()     //SetUp fixture
    {
        m_pLibraryScope = new LibraryScope(cout);
    }

    ~AnalyserTestFixture()    //TearDown fixture
    {
        delete m_pLibraryScope;
    }

    LibraryScope* m_pLibraryScope;
};

SUITE(AnalyserTest)
{

    TEST_FIXTURE(AnalyserTestFixture, AnalyserMissingMandatoryElementNoElements)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        expected << "Line 0. score: missing mandatory element 'vers'." << endl
                 << "Line 0. score: missing mandatory element 'instrument'." << endl;
        SpLdpTree tree = parser.parse_text("(score )");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
//        cout << "[" << errormsg.str() << "]" << endl;
//        cout << "[" << expected.str() << "]" << endl;
        CHECK( pBasicModel->get_root() != NULL );
        CHECK( errormsg.str() == expected.str() );
        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, AnalyserMissingMandatoryElementMoreElements)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        expected << "Line 0. score: missing mandatory element 'vers'." << endl
                 << "Line 0. score: missing mandatory element 'instrument'." << endl;
        SpLdpTree tree = parser.parse_text("(score)");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        //cout << score->get_root()->to_string() << endl;
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( pBasicModel->get_root() != NULL );
        CHECK( errormsg.str() == expected.str() );
        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, AnalyserLanguageRemoved)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        expected << "Line 0. score: missing mandatory element 'vers'." << endl
                 << "Line 0. score: missing mandatory element 'instrument'." << endl;
        SpLdpTree tree = parser.parse_text("(score (language en utf-8))");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        //cout << tree->get_root()->to_string() << endl;
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( pBasicModel->get_root() != NULL );
        CHECK( errormsg.str() == expected.str() );
        CHECK( tree->get_root()->to_string() == "(score )" );
        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, AnalyserHasMandatoryElement)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        expected << "Line 0. score: missing mandatory element 'instrument'." << endl;
        SpLdpTree tree = parser.parse_text("(score (vers 1.6))");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        ImoScore* pScore = dynamic_cast<ImoScore*>( pBasicModel->get_root() );
        CHECK( pScore != NULL );
        CHECK( pScore->get_version() == "1.6" );
        CHECK( pScore->get_num_instruments() == 0 );
        delete tree->get_root();
        delete pBasicModel;
    }

    // barline -------------------------------------------------------------------

    TEST_FIXTURE(AnalyserTestFixture, Analyser_Barline_OptionalElementMissing)
    {
        LdpParser parser(cout, m_pLibraryScope->ldp_factory());
        SpLdpTree tree = parser.parse_text("(barline)");
        Analyser a(cout, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        CHECK( pBasicModel->get_root()->is_barline() == true );
        ImoBarline* pBarline = dynamic_cast<ImoBarline*>( pBasicModel->get_root() );
        CHECK( pBarline != NULL );
        CHECK( pBarline->get_type() == ImoBarline::k_simple );
        CHECK( pBarline->is_visible() );
        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_Barline_OptionalElementPresent)
    {
        LdpParser parser(cout, m_pLibraryScope->ldp_factory());
        SpLdpTree tree = parser.parse_text("(barline double)");
        Analyser a(cout, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        CHECK( pBasicModel->get_root()->is_barline() == true );
        ImoBarline* pBarline = dynamic_cast<ImoBarline*>( pBasicModel->get_root() );
        CHECK( pBarline != NULL );
        CHECK( pBarline->get_type() == ImoBarline::k_double );
        CHECK( pBarline->is_visible() );
        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_Barline_InvalidType)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        expected << "Line 0. Unknown barline type 'invalid'. 'simple' barline assumed." << endl;
        SpLdpTree tree = parser.parse_text("(barline invalid)");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        CHECK( pBasicModel->get_root()->is_barline() == true );
        ImoBarline* pBarline = dynamic_cast<ImoBarline*>( pBasicModel->get_root() );
        CHECK( pBarline != NULL );
        CHECK( pBarline->get_type() == ImoBarline::k_simple );
        CHECK( pBarline->is_visible() );
        CHECK( tree->get_root()->to_string() == "(barline simple)" );
        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_Barline_Visible)
    {
        LdpParser parser(cout, m_pLibraryScope->ldp_factory());
        SpLdpTree tree = parser.parse_text("(barline double (visible yes))");
        Analyser a(cout, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        ImoBarline* pBarline = dynamic_cast<ImoBarline*>( pBasicModel->get_root() );
        CHECK( pBarline != NULL );
        CHECK( pBarline->get_type() == ImoBarline::k_double );
        CHECK( pBarline->is_visible() );
        CHECK( tree->get_root()->to_string() == "(barline double (visible yes))" );
        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_Barline_NoVisible)
    {
        LdpParser parser(cout, m_pLibraryScope->ldp_factory());
        SpLdpTree tree = parser.parse_text("(barline double noVisible)");
        Analyser a(cout, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        ImoBarline* pBarline = dynamic_cast<ImoBarline*>( pBasicModel->get_root() );
        CHECK( pBarline != NULL );
        CHECK( pBarline->get_type() == ImoBarline::k_double );
        CHECK( pBarline->is_visible() == false );
        CHECK( tree->get_root()->to_string() == "(barline double (visible no))" );
        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_Barline_BadVisible)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        expected << "Line 0. Element 'label:invisible' unknown or not possible here. Removed." << endl;
        SpLdpTree tree = parser.parse_text("(barline double invisible)");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        ImoBarline* pBarline = dynamic_cast<ImoBarline*>( pBasicModel->get_root() );
        CHECK( pBarline != NULL );
        CHECK( pBarline->get_type() == ImoBarline::k_double );
        CHECK( pBarline->is_visible() );
        CHECK( tree->get_root()->to_string() == "(barline double)" );
        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_Barline_LocationX)
    {
        LdpParser parser(cout, m_pLibraryScope->ldp_factory());
        SpLdpTree tree = parser.parse_text("(barline double (dx 70))");
        Analyser a(cout, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        ImoBarline* pBarline = dynamic_cast<ImoBarline*>( pBasicModel->get_root() );
        CHECK( pBarline != NULL );
        CHECK( pBarline->get_type() == ImoBarline::k_double );
        CHECK( pBarline->is_visible() );
        CHECK( pBarline->get_user_location_x() == 70.0f );
        CHECK( pBarline->get_user_location_y() == 0.0f );
        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_Barline_LocationY)
    {
        LdpParser parser(cout, m_pLibraryScope->ldp_factory());
        SpLdpTree tree = parser.parse_text("(barline double (dy 60.5))");
        Analyser a(cout, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        ImoBarline* pBarline = dynamic_cast<ImoBarline*>( pBasicModel->get_root() );
        CHECK( pBarline != NULL );
        CHECK( pBarline->get_type() == ImoBarline::k_double );
        CHECK( pBarline->is_visible() );
        CHECK( pBarline->get_user_location_x() == 0.0f );
        CHECK( pBarline->get_user_location_y() == 60.5f );
        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_Barline_LocationXY)
    {
        LdpParser parser(cout, m_pLibraryScope->ldp_factory());
        SpLdpTree tree = parser.parse_text("(barline double (dx 70)(dy 20.3))");
        Analyser a(cout, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        ImoBarline* pBarline = dynamic_cast<ImoBarline*>( pBasicModel->get_root() );
        CHECK( pBarline != NULL );
        CHECK( pBarline->get_type() == ImoBarline::k_double );
        CHECK( pBarline->is_visible() );
        CHECK( pBarline->get_user_location_x() == 70.0f );
        CHECK( pBarline->get_user_location_y() == 20.3f );
        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_Barline_BadLocationX)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        expected << "Line 0. Invalid real number 'seven'. Replaced by '0'." << endl;
        SpLdpTree tree = parser.parse_text("(barline double (dx seven))");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        ImoBarline* pBarline = dynamic_cast<ImoBarline*>( pBasicModel->get_root() );
        CHECK( pBarline != NULL );
        CHECK( pBarline->get_type() == ImoBarline::k_double );
        CHECK( pBarline->is_visible() );
        CHECK( pBarline->get_user_location_x() == 0.0f );
        CHECK( pBarline->get_user_location_y() == 0.0f );
        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_Barline_BadLocationY)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        expected << "Line 0. Invalid real number 'six'. Replaced by '0'." << endl;
        SpLdpTree tree = parser.parse_text("(barline double (dy six))");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        ImoBarline* pBarline = dynamic_cast<ImoBarline*>( pBasicModel->get_root() );
        CHECK( pBarline != NULL );
        CHECK( pBarline->get_type() == ImoBarline::k_double );
        CHECK( pBarline->is_visible() );
        CHECK( pBarline->get_user_location_x() == 0.0f );
        CHECK( pBarline->get_user_location_y() == 0.0f );
        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_Barline_LocationOrder)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        //expected << "Line 0. Element 'barline': too many parameters. Extra parameters from 'number' have been removed." << endl;
        SpLdpTree tree = parser.parse_text("(barline double (dy 70)(dx 20.3))");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        ImoBarline* pBarline = dynamic_cast<ImoBarline*>( pBasicModel->get_root() );
        CHECK( pBarline != NULL );
        CHECK( pBarline->get_type() == ImoBarline::k_double );
        CHECK( pBarline->is_visible() );
        CHECK( pBarline->get_user_location_x() == 20.3f );
        CHECK( pBarline->get_user_location_y() == 70.0f );
        delete tree->get_root();
        delete pBasicModel;
    }

    //----------------------------------------------------------------------

    TEST_FIXTURE(AnalyserTestFixture, AnalyserOneOrMoreMissingFirst)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        expected << "Line 0. score: missing mandatory element 'instrument'." << endl;
        SpLdpTree tree = parser.parse_text("(score (vers 1.6))");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        //cout << score->get_root()->to_string() << endl;
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        ImoScore* pScore = dynamic_cast<ImoScore*>( pBasicModel->get_root() );
        CHECK( pScore != NULL );
        CHECK( pScore->get_num_instruments() == 0 );
        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, AnalyserOneOrMorePresentOne)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        //expected << "" << endl;
        SpLdpTree tree = parser.parse_text("(score (vers 1.6)(instrument (musicData)))");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        ImoScore* pScore = dynamic_cast<ImoScore*>( pBasicModel->get_root() );
        CHECK( pScore != NULL );
        //CHECK( pScore->get_num_instruments() == 1 );
        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, AnalyserOneOrMorePresentMore)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        //expected << "" << endl;
        SpLdpTree tree = parser.parse_text("(score (vers 1.6)(instrument (musicData))(instrument (musicData)))");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        ImoScore* pScore = dynamic_cast<ImoScore*>( pBasicModel->get_root() );
        CHECK( pScore != NULL );
        //CHECK( pScore->get_num_instruments() == 2 );
        delete tree->get_root();
        delete pBasicModel;
    }

    // musicData -----------------------------------------------------------------

    TEST_FIXTURE(AnalyserTestFixture, AnalyserOneOrMoreOptionalAlternativesError)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        expected << "Line 0. Element 'instrument' unknown or not possible here. Removed." << endl;
        SpLdpTree tree = parser.parse_text("(musicData (n c4 q)(instrument 3))");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        CHECK( pBasicModel->get_root()->is_music_data() == true );
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        CHECK( tree->get_root()->to_string() == "(musicData (n c4 q))" );
        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, AnalyserOneOrMoreOptionalAlternativesErrorRemoved)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        expected << "Line 0. Element 'instrument' unknown or not possible here. Removed." << endl;
        SpLdpTree tree = parser.parse_text("(musicData (n c4 q)(instrument 3)(n d4 e))");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        CHECK( tree->get_root()->to_string() == "(musicData (n c4 q) (n d4 e))" );
        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, AnalyserOneOrMoreOptionalAlternativesTwo)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        //expected << "" << endl;
        SpLdpTree tree = parser.parse_text("(musicData (n c4 q)(n d4 e.))");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        CHECK( tree->get_root()->to_string() == "(musicData (n c4 q) (n d4 e.))" );
        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_MusicData_AuxobjIsAnchored)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        //expected << "" << endl;
        SpLdpTree tree = parser.parse_text("(musicData (n c4 q)(text \"Hello world\"))");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        CHECK( tree->get_root()->to_string() == "(musicData (n c4 q) (text \"Hello world\"))" );
        delete tree->get_root();
        delete pBasicModel;
    }

    // note ---------------------------------------------------------------------

    TEST_FIXTURE(AnalyserTestFixture, Analyser_Note)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        //expected << "" << endl;
        SpLdpTree tree = parser.parse_text("(n +d3 e.)");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        CHECK( pBasicModel->get_root()->is_note() == true );
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        ImoNote* pNote = dynamic_cast<ImoNote*>( pBasicModel->get_root() );
        CHECK( pNote != NULL );
        CHECK( pNote->get_accidentals() == ImoNote::k_sharp );
        CHECK( pNote->get_dots() == 1 );
        CHECK( pNote->get_note_type() == ImoNote::k_eighth );
        CHECK( pNote->get_octave() == 3 );
        CHECK( pNote->get_step() == ImoNote::D );
        CHECK( pNote->get_duration() == 48.0f );
        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_Note_PitchError)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        expected << "Line 0. Unknown note pitch 'j17'. Replaced by 'c4'." << endl;
        SpLdpTree tree = parser.parse_text("(n j17 q)");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        ImoNote* pNote = dynamic_cast<ImoNote*>( pBasicModel->get_root() );
        CHECK( pNote != NULL );
        CHECK( pNote->get_accidentals() == ImoNote::k_no_accidentals );
        CHECK( pNote->get_dots() == 0 );
        CHECK( pNote->get_note_type() == ImoNote::k_quarter );
        CHECK( pNote->get_octave() == 4 );
        CHECK( pNote->get_step() == ImoNote::C );
        CHECK( tree->get_root()->to_string() == "(n c4 q)" );
        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_Note_DurationErrorLetter)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        expected << "Line 0. Unknown note/rest duration 'j.'. Replaced by 'q'." << endl;
        SpLdpTree tree = parser.parse_text("(n c4 j.)");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        ImoNote* pNote = dynamic_cast<ImoNote*>( pBasicModel->get_root() );
        CHECK( pNote != NULL );
        CHECK( pNote->get_accidentals() == ImoNote::k_no_accidentals );
        CHECK( pNote->get_dots() == 0 );
        CHECK( pNote->get_note_type() == ImoNote::k_quarter );
        CHECK( pNote->get_octave() == 4 );
        CHECK( pNote->get_step() == ImoNote::C );
        CHECK( tree->get_root()->to_string() == "(n c4 q)" );
        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_Note_DurationErrorDots)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        expected << "Line 0. Unknown note/rest duration 'e.1'. Replaced by 'q'." << endl;
        SpLdpTree tree = parser.parse_text("(n c4 e.1)");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        ImoNote* pNote = dynamic_cast<ImoNote*>( pBasicModel->get_root() );
        CHECK( pNote != NULL );
        CHECK( pNote->get_accidentals() == ImoNote::k_no_accidentals );
        CHECK( pNote->get_dots() == 0 );
        CHECK( pNote->get_note_type() == ImoNote::k_quarter );
        CHECK( pNote->get_octave() == 4 );
        CHECK( pNote->get_step() == ImoNote::C );
        CHECK( tree->get_root()->to_string() == "(n c4 q)" );
        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_Note_Staff)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        //expected << "Line 0. Unknown note/rest duration 'e.1'. Replaced by 'q'." << endl;
        SpLdpTree tree = parser.parse_text("(n c4 e p7)");
        //cout << tree->get_root()->to_string() << endl;
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        ImoNote* pNote = dynamic_cast<ImoNote*>( pBasicModel->get_root() );
        CHECK( pNote != NULL );
        CHECK( pNote->get_accidentals() == ImoNote::k_no_accidentals );
        CHECK( pNote->get_dots() == 0 );
        CHECK( pNote->get_note_type() == ImoNote::k_eighth );
        CHECK( pNote->get_octave() == 4 );
        CHECK( pNote->get_step() == ImoNote::C );
        CHECK( pNote->get_staff() == 6 );
        CHECK( tree->get_root()->to_string() == "(n c4 e p7)" );
        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_Note_StaffError)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        expected << "Line 0. Invalid staff 'pz'. Replaced by 'p1'." << endl;
        SpLdpTree tree = parser.parse_text("(n c4 e pz)");
        //cout << tree->get_root()->to_string() << endl;
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        ImoNote* pNote = dynamic_cast<ImoNote*>( pBasicModel->get_root() );
        CHECK( pNote != NULL );
        CHECK( pNote->get_staff() == 0 );
        CHECK( tree->get_root()->to_string() == "(n c4 e p1)" );
        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_Note_Voice)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        //expected << "Line 0. Unknown note/rest duration 'e.1'. Replaced by 'q'." << endl;
        SpLdpTree tree = parser.parse_text("(n c4 e v3)");
        //cout << tree->get_root()->to_string() << endl;
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        ImoNote* pNote = dynamic_cast<ImoNote*>( pBasicModel->get_root() );
        CHECK( pNote != NULL );
        CHECK( pNote->get_accidentals() == ImoNote::k_no_accidentals );
        CHECK( pNote->get_dots() == 0 );
        CHECK( pNote->get_note_type() == ImoNote::k_eighth );
        CHECK( pNote->get_octave() == 4 );
        CHECK( pNote->get_step() == ImoNote::C );
        CHECK( pNote->get_voice() == 3 );
        CHECK( tree->get_root()->to_string() == "(n c4 e v3)" );
        CHECK( pNote->is_tied_next() == false );
        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_Note_VoiceError)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        expected << "Line 0. Invalid voice 'vx'. Replaced by 'v1'." << endl;
        SpLdpTree tree = parser.parse_text("(n c4 e vx)");
        //cout << tree->get_root()->to_string() << endl;
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        ImoNote* pNote = dynamic_cast<ImoNote*>( pBasicModel->get_root() );
        CHECK( pNote != NULL );
        CHECK( pNote->get_voice() == 1 );
        CHECK( tree->get_root()->to_string() == "(n c4 e v1)" );
        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_Note_Attachment)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        //expected << "" << endl;
        SpLdpTree tree = parser.parse_text("(n c4 e v3 (text \"andante\"))");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        ImoNote* pNote = dynamic_cast<ImoNote*>( pBasicModel->get_root() );
        CHECK( pNote != NULL );
        CHECK( pNote->get_accidentals() == ImoNote::k_no_accidentals );
        CHECK( pNote->get_dots() == 0 );
        CHECK( pNote->get_note_type() == ImoNote::k_eighth );
        CHECK( pNote->get_octave() == 4 );
        CHECK( pNote->get_step() == ImoNote::C );
        CHECK( pNote->get_voice() == 3 );
        CHECK( pNote->has_attachments() == true );
        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_Note_TieStart)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        expected << "Line 0. No 'stop' element for tie number 12. Tie removed." << endl;
        SpLdpTree tree = parser.parse_text("(n c4 e (tie 12 start))");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        ImoNote* pNote = dynamic_cast<ImoNote*>( pBasicModel->get_root() );
        CHECK( pNote != NULL );
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        //cout << tree->get_root()->to_string() << endl;
        CHECK( pNote->is_tied_next() == false );
        CHECK( pNote->is_tied_prev() == false );
        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_Note_TieStop)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        expected << "Line 0. No 'start' element for tie number 12. Tie removed." << endl;
        SpLdpTree tree = parser.parse_text("(n c4 e (tie 12 stop))");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        ImoNote* pNote = dynamic_cast<ImoNote*>( pBasicModel->get_root() );
        CHECK( pNote != NULL );
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        //cout << tree->get_root()->to_string() << endl;
        CHECK( pNote->is_tied_next() == false );
        CHECK( pNote->is_tied_prev() == false );
        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_Notes_Tied)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        //expected << "Line 0. No 'start' element for tie number 12. Tie removed." << endl;
        SpLdpTree tree = parser.parse_text("(musicData (n c4 q (tie 12 start)) (n c4 e (tie 12 stop)))");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);

        std::list<ImoTie*>& ties = pBasicModel->get_ties();
        CHECK (ties.size() == 1 );
        ImoTie* pTie = ties.front();

        ImoMusicData* pMusic = dynamic_cast<ImoMusicData*>( pBasicModel->get_root() );
        CHECK( pMusic != NULL );
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        //cout << tree->get_root()->to_string() << endl;
        CHECK( tree->get_root()->to_string() == "(musicData (n c4 q (tie 12 start)) (n c4 e (tie 12 stop)))" );

        ImoObj::children_iterator it = pMusic->begin();

        ImoNote* pNote1 = dynamic_cast<ImoNote*>( *it );
        CHECK( pNote1 != NULL );
        CHECK( pNote1->is_tied_next() == true );
        CHECK( pNote1->is_tied_prev() == false );
        CHECK( pTie->get_start_note() == pNote1 );

        ++it;
        ImoNote* pNote2 = dynamic_cast<ImoNote*>( *it );
        CHECK( pNote2 != NULL );
        CHECK( pNote2->is_tied_next() == false );
        CHECK( pNote2->is_tied_prev() == true );
        CHECK( pTie->get_end_note() == pNote2 );

        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_Notes_Tied_Impossible)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        expected << "Line 0. Requesting to tie notes of different voice or pitch. Tie number 12 will be removed." << endl;
        SpLdpTree tree = parser.parse_text("(musicData (n c4 q (tie 12 start)) (n c3 e (tie 12 stop)))");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);

        std::list<ImoTie*>& ties = pBasicModel->get_ties();
        CHECK (ties.size() == 0 );

        ImoMusicData* pMusic = dynamic_cast<ImoMusicData*>( pBasicModel->get_root() );
        CHECK( pMusic != NULL );

        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        //cout << tree->get_root()->to_string() << endl;

        ImoObj::children_iterator it = pMusic->begin();

        ImoNote* pNote1 = dynamic_cast<ImoNote*>( *it );
        CHECK( pNote1 != NULL );
        CHECK( pNote1->is_tied_next() == false );
        CHECK( pNote1->is_tied_prev() == false );

        ++it;
        ImoNote* pNote2 = dynamic_cast<ImoNote*>( *it );
        CHECK( pNote2 != NULL );
        CHECK( pNote2->is_tied_next() == false );
        CHECK( pNote2->is_tied_prev() == false );

        delete tree->get_root();
        delete pBasicModel;
    }

    // stem

    TEST_FIXTURE(AnalyserTestFixture, Analyser_Note_StemUp)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        //expected << "Line 0. ?" << endl;
        SpLdpTree tree = parser.parse_text("(n c4 e (stem up))");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        ImoNote* pNote = dynamic_cast<ImoNote*>( pBasicModel->get_root() );
        CHECK( pNote != NULL );
        CHECK( pNote->is_stem_up() == true );
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        //cout << tree->get_root()->to_string() << endl;
        CHECK( errormsg.str() == expected.str() );
        CHECK( tree->get_root()->to_string() == "(n c4 e (stem up))" );
        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_Note_StemDown)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        //expected << "Line 0. ?" << endl;
        SpLdpTree tree = parser.parse_text("(n c4 e (stem down))");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        ImoNote* pNote = dynamic_cast<ImoNote*>( pBasicModel->get_root() );
        CHECK( pNote != NULL );
        CHECK( pNote->is_stem_down() == true );
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        //cout << tree->get_root()->to_string() << endl;
        CHECK( errormsg.str() == expected.str() );
        CHECK( tree->get_root()->to_string() == "(n c4 e (stem down))" );
        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_Note_StemError)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        expected << "Line 0. Invalid value 'no' for stem type. Default stem asigned." << endl;
        SpLdpTree tree = parser.parse_text("(n c4 e (stem no))");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        ImoNote* pNote = dynamic_cast<ImoNote*>( pBasicModel->get_root() );
        CHECK( pNote != NULL );
        CHECK( pNote->is_stem_default() == true );
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        //cout << tree->get_root()->to_string() << endl;
        CHECK( errormsg.str() == expected.str() );
        CHECK( tree->get_root()->to_string() == "(n c4 e)" );
        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_Note_StemTie)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        //expected << "Line 0. " << endl;
        SpLdpTree tree = parser.parse_text("(musicData (n c4 q (tie 12 start)(stem down)) (n c4 e (stem up)(tie 12 stop)))");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);

        std::list<ImoTie*>& ties = pBasicModel->get_ties();
        CHECK (ties.size() == 1 );
        ImoTie* pTie = ties.front();

        ImoMusicData* pMusic = dynamic_cast<ImoMusicData*>( pBasicModel->get_root() );
        CHECK( pMusic != NULL );
        //cout << tree->get_root()->to_string() << endl;
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );

        ImoObj::children_iterator it = pMusic->begin();

        ImoNote* pNote1 = dynamic_cast<ImoNote*>( *it );
        CHECK( pNote1 != NULL );
        CHECK( pNote1->is_tied_next() == true );
        CHECK( pNote1->is_tied_prev() == false );
        CHECK( pTie->get_start_note() == pNote1 );
        CHECK( pNote1->is_stem_down() == true );

        ++it;
        ImoNote* pNote2 = dynamic_cast<ImoNote*>( *it );
        CHECK( pNote2 != NULL );
        CHECK( pNote2->is_tied_next() == false );
        CHECK( pNote2->is_tied_prev() == true );
        CHECK( pTie->get_end_note() == pNote2 );
        CHECK( pNote2->is_stem_up() == true );

        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_Note_SeveralOldParams)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        //expected << "" << endl;
        SpLdpTree tree = parser.parse_text("(n +d3 e. g+ p2)");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        CHECK( pBasicModel->get_root()->is_note() == true );
//        cout << "[" << errormsg.str() << "]" << endl;
//        cout << "[" << expected.str() << "]" << endl;
//        cout << tree->get_root()->to_string() << endl;
        CHECK( errormsg.str() == expected.str() );
        CHECK( tree->get_root()->to_string() == "(n +d3 e. g+ p2)" );
        ImoNote* pNote = dynamic_cast<ImoNote*>( pBasicModel->get_root() );
        CHECK( pNote != NULL );
        CHECK( pNote->get_accidentals() == ImoNote::k_sharp );
        CHECK( pNote->get_dots() == 1 );
        CHECK( pNote->get_note_type() == ImoNote::k_eighth );
        CHECK( pNote->get_octave() == 3 );
        CHECK( pNote->get_step() == ImoNote::D );
        CHECK( pNote->get_duration() == 48.0f );
        CHECK( pNote->get_staff() == 1 );
        delete tree->get_root();
        delete pBasicModel;
    }

    // tie (old syntax) --------------------------------------------------------

    TEST_FIXTURE(AnalyserTestFixture, Analyser_TieOld)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        //expected << "Line 0. " << endl;
        SpLdpTree tree = parser.parse_text("(musicData (n c4 e l)(n c4 q))");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);

        std::list<ImoTie*>& ties = pBasicModel->get_ties();
        CHECK (ties.size() == 1 );
        ImoTie* pTie = ties.front();

        ImoMusicData* pMusic = dynamic_cast<ImoMusicData*>( pBasicModel->get_root() );
        CHECK( pMusic != NULL );
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        //cout << tree->get_root()->to_string() << endl;
        CHECK( errormsg.str() == expected.str() );
        CHECK( tree->get_root()->to_string() == "(musicData (n c4 e l) (n c4 q))" );

        ImoObj::children_iterator it = pMusic->begin();

        ImoNote* pNote1 = dynamic_cast<ImoNote*>( *it );
        CHECK( pNote1 != NULL );
        CHECK( pNote1->is_tied_next() == true );
        CHECK( pNote1->is_tied_prev() == false );
        CHECK( pTie->get_start_note() == pNote1 );

        ++it;
        ImoNote* pNote2 = dynamic_cast<ImoNote*>( *it );
        CHECK( pNote2 != NULL );
        CHECK( pNote2->is_tied_next() == false );
        CHECK( pNote2->is_tied_prev() == true );
        CHECK( pTie->get_end_note() == pNote2 );

        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_TieOld_Error1)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        expected << "Line 0. No note found to match old syntax tie. Tie removed." << endl;
        SpLdpTree tree = parser.parse_text("(musicData (n c4 e l)(n d4 q))");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);

        std::list<ImoTie*>& ties = pBasicModel->get_ties();
        CHECK (ties.size() == 0 );

        ImoMusicData* pMusic = dynamic_cast<ImoMusicData*>( pBasicModel->get_root() );
        CHECK( pMusic != NULL );
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        //cout << tree->get_root()->to_string() << endl;
        CHECK( errormsg.str() == expected.str() );

        ImoObj::children_iterator it = pMusic->begin();

        ImoNote* pNote1 = dynamic_cast<ImoNote*>( *it );
        CHECK( pNote1 != NULL );
        CHECK( pNote1->is_tied_next() == false );
        CHECK( pNote1->is_tied_prev() == false );

        ++it;
        ImoNote* pNote2 = dynamic_cast<ImoNote*>( *it );
        CHECK( pNote2 != NULL );
        CHECK( pNote2->is_tied_next() == false );
        CHECK( pNote2->is_tied_prev() == false );

        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_TieOld_IntermediateNote)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        //expected << "Line 0. No 'start' element for tie number 12. Tie removed." << endl;
        SpLdpTree tree = parser.parse_text("(musicData (n c4 q v1 l)(n e4 q v2)(n c4 e v1))");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);

        std::list<ImoTie*>& ties = pBasicModel->get_ties();
        CHECK (ties.size() == 1 );
        ImoTie* pTie = ties.front();

        ImoMusicData* pMusic = dynamic_cast<ImoMusicData*>( pBasicModel->get_root() );
        CHECK( pMusic != NULL );
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        //cout << tree->get_root()->to_string() << endl;
        CHECK( errormsg.str() == expected.str() );
        CHECK( tree->get_root()->to_string() == "(musicData (n c4 q v1 l) (n e4 q v2) (n c4 e v1))" );

        ImoObj::children_iterator it = pMusic->begin();

        ImoNote* pNote1 = dynamic_cast<ImoNote*>( *it );
        CHECK( pNote1 != NULL );
        CHECK( pNote1->is_tied_next() == true );
        CHECK( pNote1->is_tied_prev() == false );
        CHECK( pTie->get_start_note() == pNote1 );

        ++it;
        ++it;
        ImoNote* pNote2 = dynamic_cast<ImoNote*>( *it );
        CHECK( pNote2 != NULL );
        CHECK( pNote2->is_tied_next() == false );
        CHECK( pNote2->is_tied_prev() == true );
        CHECK( pTie->get_end_note() == pNote2 );

        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_TieOld_IntermediateBarline)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        //expected << "Line 0. No 'start' element for tie number 12. Tie removed." << endl;
        SpLdpTree tree = parser.parse_text("(musicData (n c4 q v1 l)(barline simple)(n c4 e v1))");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);

        std::list<ImoTie*>& ties = pBasicModel->get_ties();
        CHECK (ties.size() == 1 );
        ImoTie* pTie = ties.front();

        ImoMusicData* pMusic = dynamic_cast<ImoMusicData*>( pBasicModel->get_root() );
        CHECK( pMusic != NULL );
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        //cout << tree->get_root()->to_string() << endl;
        CHECK( errormsg.str() == expected.str() );
        CHECK( tree->get_root()->to_string() == "(musicData (n c4 q v1 l) (barline simple) (n c4 e v1))" );

        ImoObj::children_iterator it = pMusic->begin();

        ImoNote* pNote1 = dynamic_cast<ImoNote*>( *it );
        CHECK( pNote1 != NULL );
        CHECK( pNote1->is_tied_next() == true );
        CHECK( pNote1->is_tied_prev() == false );
        CHECK( pTie->get_start_note() == pNote1 );
        CHECK( pTie->is_tie() == true );

        ++it;
        ++it;
        ImoNote* pNote2 = dynamic_cast<ImoNote*>( *it );
        CHECK( pNote2 != NULL );
        CHECK( pNote2->is_tied_next() == false );
        CHECK( pNote2->is_tied_prev() == true );
        CHECK( pTie->get_end_note() == pNote2 );

        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_TieOld_Several)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        //expected << "Line 0. No 'start' element for tie number 12. Tie removed." << endl;
        SpLdpTree tree = parser.parse_text("(musicData (n c4 q l)(n c4 e l)(n c4 e))");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);

        std::list<ImoTie*>& ties = pBasicModel->get_ties();
        CHECK (ties.size() == 2 );
        ImoTie* pTie1 = ties.front();
        ImoTie* pTie2 = ties.back();

        ImoMusicData* pMusic = dynamic_cast<ImoMusicData*>( pBasicModel->get_root() );
        CHECK( pMusic != NULL );
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        //cout << tree->get_root()->to_string() << endl;
        CHECK( errormsg.str() == expected.str() );
        CHECK( tree->get_root()->to_string() == "(musicData (n c4 q l) (n c4 e l) (n c4 e))" );

        ImoObj::children_iterator it = pMusic->begin();

        ImoNote* pNote1 = dynamic_cast<ImoNote*>( *it );
        CHECK( pNote1 != NULL );
        CHECK( pNote1->is_tied_next() == true );
        CHECK( pNote1->is_tied_prev() == false );
        CHECK( pTie1->get_start_note() == pNote1 );

        ++it;
        ImoNote* pNote2 = dynamic_cast<ImoNote*>( *it );
        CHECK( pNote2 != NULL );
        CHECK( pNote2->is_tied_next() == true );
        CHECK( pNote2->is_tied_prev() == true );
        CHECK( pTie1->get_end_note() == pNote2 );
        CHECK( pTie2->get_start_note() == pNote2 );

        ++it;
        ImoNote* pNote3 = dynamic_cast<ImoNote*>( *it );
        CHECK( pNote3 != NULL );
        CHECK( pNote3->is_tied_next() == false );
        CHECK( pNote3->is_tied_prev() == true );
        CHECK( pTie2->get_end_note() == pNote3 );

        delete tree->get_root();
        delete pBasicModel;
    }

    // tie --------------------------------------------------------------------------

    TEST_FIXTURE(AnalyserTestFixture, Analyser_Tie_ParsedStop)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        //expected << "Line 0. Unknown note/rest duration 'e.1'. Replaced by 'q'." << endl;
        SpLdpTree tree = parser.parse_text("(tie 12 stop)");
        //cout << tree->get_root()->to_string() << endl;
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        CHECK( pBasicModel->get_root()->is_tie_info() == true );
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        ImoTieInfo* pInfo = dynamic_cast<ImoTieInfo*>( pBasicModel->get_root() );
        CHECK( pInfo != NULL );
        CHECK( pInfo->is_start() == false );
        CHECK( pInfo->get_tie_number() == 12 );
        CHECK( pInfo->get_note() == NULL );
        CHECK( pInfo->get_bezier() == NULL );
        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_Tie_ParsedStart)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        //expected << "Line 0. Unknown note/rest duration 'e.1'. Replaced by 'q'." << endl;
        SpLdpTree tree = parser.parse_text("(tie 15 start)");
        //cout << tree->get_root()->to_string() << endl;
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        ImoTieInfo* pInfo = dynamic_cast<ImoTieInfo*>( pBasicModel->get_root() );
        CHECK( pInfo != NULL );
        CHECK( pInfo->is_start() == true );
        CHECK( pInfo->get_tie_number() == 15 );
        CHECK( pInfo->get_note() == NULL );
        CHECK( pInfo->get_bezier() == NULL );
        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_Tie_Bezier)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        //expected << "Line 0. Unknown note/rest duration 'e.1'. Replaced by 'q'." << endl;
        SpLdpTree tree = parser.parse_text("(tie 15 start (bezier (ctrol2-x -25)(start-y 36.765)) )");
        //cout << tree->get_root()->to_string() << endl;
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        ImoTieInfo* pInfo = dynamic_cast<ImoTieInfo*>( pBasicModel->get_root() );
        CHECK( pInfo != NULL );
        CHECK( pInfo->is_start() == true );
        CHECK( pInfo->get_tie_number() == 15 );
        CHECK( pInfo->get_note() == NULL );
        ImoBezier* pBezier = pInfo->get_bezier();
        CHECK( pBezier != NULL );
        CHECK( pBezier->get_point(ImoBezier::k_start).x == 0.0f );
        CHECK( pBezier->get_point(ImoBezier::k_start).y == 36.765f );
        CHECK( pBezier->get_point(ImoBezier::k_end).x == 0.0f );
        CHECK( pBezier->get_point(ImoBezier::k_end).y == 0.0f );
        CHECK( pBezier->get_point(ImoBezier::k_ctrol1).x == 0.0f );
        CHECK( pBezier->get_point(ImoBezier::k_ctrol1).y == 0.0f );
        CHECK( pBezier->get_point(ImoBezier::k_ctrol2).x == -25.0f );
        CHECK( pBezier->get_point(ImoBezier::k_ctrol2).y == 0.0f );
        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_Tie_ParsedError)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        expected << "Line 0. Missing or invalid tie type. Tie ignored." << endl;
        SpLdpTree tree = parser.parse_text("(tie 15 end)");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        CHECK( tree->get_root() == NULL );
        delete pBasicModel;
    }

    // bezier ----------------------------------------------------------------------

    TEST_FIXTURE(AnalyserTestFixture, Analyser_Bezier_Ok)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        //expected << "Line 0. Unknown note/rest duration 'e.1'. Replaced by 'q'." << endl;
        SpLdpTree tree = parser.parse_text("(bezier ctrol1-x:-25 (start-x 36.765) ctrol1-y:55)");
        //cout << tree->get_root()->to_string() << endl;
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        CHECK( pBasicModel->get_root()->is_bezier() == true );
        ImoBezier* pBezier = dynamic_cast<ImoBezier*>( pBasicModel->get_root() );
        CHECK( pBezier != NULL );
        //cout << "start.x = " << pBezier->get_point(ImoBezier::k_start).x << endl;
        //cout << "start.y = " << pBezier->get_point(ImoBezier::k_start).y << endl;
        //cout << "end.x = " << pBezier->get_point(ImoBezier::k_end).x << endl;
        //cout << "end.y = " << pBezier->get_point(ImoBezier::k_end).y << endl;
        //cout << "ctrol1.x = " << pBezier->get_point(ImoBezier::k_ctrol1).x << endl;
        //cout << "ctrol1.y = " << pBezier->get_point(ImoBezier::k_ctrol1).y << endl;
        //cout << "ctrol2.x = " << pBezier->get_point(ImoBezier::k_ctrol2).x << endl;
        //cout << "ctrol2.y = " << pBezier->get_point(ImoBezier::k_ctrol2).y << endl;
        CHECK( pBezier->get_point(ImoBezier::k_start).x == 36.765f );
        CHECK( pBezier->get_point(ImoBezier::k_start).y == 0.0f );
        CHECK( pBezier->get_point(ImoBezier::k_end).x == 0.0f );
        CHECK( pBezier->get_point(ImoBezier::k_end).y == 0.0f );
        CHECK( pBezier->get_point(ImoBezier::k_ctrol1).x == -25.0f );
        CHECK( pBezier->get_point(ImoBezier::k_ctrol1).y == 55.0f );
        CHECK( pBezier->get_point(ImoBezier::k_ctrol2).x == 0.0f );
        CHECK( pBezier->get_point(ImoBezier::k_ctrol2).y == 0.0f );
        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_Bezier_Error)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        expected << "Line 0. Unknown tag 'startx'." << endl <<
            "Line 0. Element 'undefined' unknown or not possible here. Removed." << endl;
        SpLdpTree tree = parser.parse_text("(bezier (startx 36.765) ctrol1-x:-25 ctrol1-y:55)");
        //cout << tree->get_root()->to_string() << endl;
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        ImoBezier* pBezier = dynamic_cast<ImoBezier*>( pBasicModel->get_root() );
        CHECK( pBezier != NULL );
        //cout << "start.x = " << pBezier->get_point(ImoBezier::k_start).x << endl;
        //cout << "start.y = " << pBezier->get_point(ImoBezier::k_start).y << endl;
        //cout << "end.x = " << pBezier->get_point(ImoBezier::k_end).x << endl;
        //cout << "end.y = " << pBezier->get_point(ImoBezier::k_end).y << endl;
        //cout << "ctrol1.x = " << pBezier->get_point(ImoBezier::k_ctrol1).x << endl;
        //cout << "ctrol1.y = " << pBezier->get_point(ImoBezier::k_ctrol1).y << endl;
        //cout << "ctrol2.x = " << pBezier->get_point(ImoBezier::k_ctrol2).x << endl;
        //cout << "ctrol2.y = " << pBezier->get_point(ImoBezier::k_ctrol2).y << endl;
        CHECK( pBezier->get_point(ImoBezier::k_start).x == 0.0f );
        CHECK( pBezier->get_point(ImoBezier::k_start).y == 0.0f );
        CHECK( pBezier->get_point(ImoBezier::k_end).x == 0.0f );
        CHECK( pBezier->get_point(ImoBezier::k_end).y == 0.0f );
        CHECK( pBezier->get_point(ImoBezier::k_ctrol1).x == -25.0f );
        CHECK( pBezier->get_point(ImoBezier::k_ctrol1).y == 55.0f );
        CHECK( pBezier->get_point(ImoBezier::k_ctrol2).x == 0.0f );
        CHECK( pBezier->get_point(ImoBezier::k_ctrol2).y == 0.0f );
        delete tree->get_root();
        delete pBasicModel;
    }

    // rest ------------------------------------------------------------------------

    TEST_FIXTURE(AnalyserTestFixture, Analyser_Rest)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        //expected << "" << endl;
        SpLdpTree tree = parser.parse_text("(r e.)");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        CHECK( pBasicModel->get_root()->is_rest() == true );
        ImoRest* pRest = dynamic_cast<ImoRest*>( pBasicModel->get_root() );
        CHECK( pRest != NULL );
        CHECK( pRest->get_dots() == 1 );
        CHECK( pRest->get_note_type() == ImoNoteRest::k_eighth );
        CHECK( pRest->has_attachments() == false );
        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_Rest_StaffNum)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        //expected << "" << endl;
        SpLdpTree tree = parser.parse_text("(r e. p2)");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        ImoRest* pRest = dynamic_cast<ImoRest*>( pBasicModel->get_root() );
        CHECK( pRest != NULL );
        CHECK( pRest->get_dots() == 1 );
        CHECK( pRest->get_note_type() == ImoNoteRest::k_eighth );
        CHECK( pRest->get_staff() == 1 );
        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_Rest_DefaultStaffNum)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        //expected << "" << endl;
        SpLdpTree tree = parser.parse_text("(r e.)");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        ImoRest* pRest = dynamic_cast<ImoRest*>( pBasicModel->get_root() );
        CHECK( pRest != NULL );
        CHECK( pRest->get_dots() == 1 );
        CHECK( pRest->get_note_type() == ImoNoteRest::k_eighth );
        CHECK( pRest->get_staff() == 0 );
        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_Rest_StaffNumInherited)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        //expected << "" << endl;
        SpLdpTree tree = parser.parse_text("(musicData (r e. p2)(n c4 q)(n d4 e p3)(r q))");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        LdpTree::iterator it = tree->begin();
        ++it;
        ImoRest* pRest = dynamic_cast<ImoRest*>( (*it)->get_imo() );
        CHECK( pRest != NULL );
        CHECK( pRest->get_staff() == 1 );
        ++it;
        ++it;
        ++it;
        ImoNote* pNote = dynamic_cast<ImoNote*>( (*it)->get_imo() );
        CHECK( pNote != NULL );
        CHECK( pNote->get_staff() == 1 );
        ++it;
        ++it;
        ++it;
        pNote = dynamic_cast<ImoNote*>( (*it)->get_imo() );
        CHECK( pNote != NULL );
        CHECK( pNote->get_staff() == 2 );
        ++it;
        ++it;
        ++it;
        ++it;
        pRest = dynamic_cast<ImoRest*>( (*it)->get_imo() );
        CHECK( pRest != NULL );
        CHECK( pRest->get_staff() == 2 );
        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_Rest_Attachment)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        //expected << "" << endl;
        SpLdpTree tree = parser.parse_text("(r e. (text \"andante\"))");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        ImoRest* pRest = dynamic_cast<ImoRest*>( pBasicModel->get_root() );
        CHECK( pRest != NULL );
        CHECK( pRest->get_dots() == 1 );
        CHECK( pRest->get_note_type() == ImoNoteRest::k_eighth );
        CHECK( pRest->has_attachments() == true );
        delete tree->get_root();
        delete pBasicModel;
    }

    // fermata ---------------------------------------------------------------

    TEST_FIXTURE(AnalyserTestFixture, Analyser_Fermata)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        //expected << "Line 0. " << endl;
        SpLdpTree tree = parser.parse_text("(fermata below)");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        CHECK( pBasicModel->get_root()->is_fermata() == true );
        ImoFermata* pFerm = dynamic_cast<ImoFermata*>( pBasicModel->get_root() );
        CHECK( pFerm != NULL );
        CHECK( pFerm->get_placement() == ImoFermata::k_below );
        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_Fermata_ErrorPlacement)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        expected << "Line 0. Unknown fermata placement 'under'. Replaced by 'above'." << endl;
        SpLdpTree tree = parser.parse_text("(fermata under)");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        ImoFermata* pFerm = dynamic_cast<ImoFermata*>( pBasicModel->get_root() );
        CHECK( pFerm != NULL );
        CHECK( pFerm->get_placement() == ImoFermata::k_above );
        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_Fermata_Location)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        //expected << "Line 0. Invalid voice 'vx'. Replaced by 'v1'." << endl;
        SpLdpTree tree = parser.parse_text("(fermata above (dx 70))");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        ImoFermata* pFerm = dynamic_cast<ImoFermata*>( pBasicModel->get_root() );
        CHECK( pFerm != NULL );
        CHECK( pFerm->get_placement() == ImoFermata::k_above );
        CHECK( pFerm->get_user_location_x() == 70.0f );
        CHECK( pFerm->get_user_location_y() == 0.0f );
        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_Fermata_ErrorMore)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        expected << "Line 0. Element 'fermata': too many parameters. Extra parameters from 'fermata' have been removed." << endl;
        SpLdpTree tree = parser.parse_text("(fermata above (dx 70)(fermata below))");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        ImoFermata* pFerm = dynamic_cast<ImoFermata*>( pBasicModel->get_root() );
        CHECK( pFerm != NULL );
        CHECK( pFerm->get_placement() == ImoFermata::k_above );
        CHECK( pFerm->get_user_location_x() == 70.0f );
        CHECK( pFerm->get_user_location_y() == 0.0f );
        CHECK( tree->get_root()->to_string() == "(fermata above (dx 70))" );
        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_Note_Fermata)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        //expected << "Line 0. ?" << endl;
        SpLdpTree tree = parser.parse_text("(n c4 e (stem up)(fermata above (dx 70)))");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        ImoNote* pNote = dynamic_cast<ImoNote*>( pBasicModel->get_root() );
        CHECK( pNote != NULL );
        CHECK( pNote->is_stem_up() == true );
        CHECK( pNote->get_num_attachments() == 1 );
        ImoFermata* pFerm = dynamic_cast<ImoFermata*>( pNote->get_attachment(0) );
        CHECK( pFerm != NULL );
        CHECK( pFerm->get_placement() == ImoFermata::k_above );
        CHECK( pFerm->get_user_location_x() == 70.0f );
        CHECK( pFerm->get_user_location_y() == 0.0f );
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        //cout << tree->get_root()->to_string() << endl;
        CHECK( errormsg.str() == expected.str() );
        CHECK( tree->get_root()->to_string() == "(n c4 e (stem up) (fermata above (dx 70)))" );
        delete tree->get_root();
        delete pBasicModel;
    }

    // goFwd -----------------------------------------------------------------
    // goBack ----------------------------------------------------------------

    TEST_FIXTURE(AnalyserTestFixture, AnalyserGoBackStart)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        //expected << "Line 0. Invalid voice 'vx'. Replaced by 'v1'." << endl;
        SpLdpTree tree = parser.parse_text("(goBack start)");
        //cout << tree->get_root()->to_string() << endl;
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        CHECK( pBasicModel->get_root()->is_go_back_fwd() == true );
        ImoGoBackFwd* pGBF = dynamic_cast<ImoGoBackFwd*>( pBasicModel->get_root() );
        CHECK( pGBF != NULL );
        CHECK( pGBF->is_to_start() );
        CHECK( tree->get_root()->to_string() == "(goBack start)" );
        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, AnalyserGoBackEnd)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        expected << "Line 0. Element 'goBack' has an incoherent value: go backwards to end?. Element ignored." << endl;
        SpLdpTree tree = parser.parse_text("(goBack end)");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        CHECK( tree->get_root() == NULL );
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, AnalyserGoBackQ)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        //expected << "Line 0. Element 'goBack' has an incoherent value: go backwards to end?. Element ignored." << endl;
        SpLdpTree tree = parser.parse_text("(goBack q)");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        ImoGoBackFwd* pGBF = dynamic_cast<ImoGoBackFwd*>( pBasicModel->get_root() );
        CHECK( pGBF != NULL );
        CHECK( !pGBF->is_to_start() );
        CHECK( !pGBF->is_to_end() );
        CHECK( pGBF->get_time_shift() == -64.0f );
        CHECK( tree->get_root()->to_string() == "(goBack q)" );
        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, AnalyserGoFwdEnd)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        //expected << "Line 0. Invalid voice 'vx'. Replaced by 'v1'." << endl;
        SpLdpTree tree = parser.parse_text("(goFwd end)");
        //cout << tree->get_root()->to_string() << endl;
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        ImoGoBackFwd* pGBF = dynamic_cast<ImoGoBackFwd*>( pBasicModel->get_root() );
        CHECK( pGBF != NULL );
        CHECK( pGBF->is_to_end() );
        CHECK( tree->get_root()->to_string() == "(goFwd end)" );
        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, AnalyserGoFwdStart)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        expected << "Line 0. Element 'goFwd' has an incoherent value: go forward to start?. Element ignored." << endl;
        SpLdpTree tree = parser.parse_text("(goFwd start)");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        CHECK( tree->get_root() == NULL );
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, AnalyserGoFwdH)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        //expected << "Line 0. Element 'goBack' has an incoherent value: go backwards to end?. Element ignored." << endl;
        SpLdpTree tree = parser.parse_text("(goFwd h)");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        ImoGoBackFwd* pGBF = dynamic_cast<ImoGoBackFwd*>( pBasicModel->get_root() );
        CHECK( pGBF != NULL );
        CHECK( !pGBF->is_to_start() );
        CHECK( !pGBF->is_to_end() );
        CHECK( pGBF->get_time_shift() == 128.0f );
        CHECK( tree->get_root()->to_string() == "(goFwd h)" );
        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, AnalyserGoFwdNum)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        //expected << "Line 0. Element 'goBack' has an incoherent value: go backwards to end?. Element ignored." << endl;
        SpLdpTree tree = parser.parse_text("(goFwd 128)");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        ImoGoBackFwd* pGBF = dynamic_cast<ImoGoBackFwd*>( pBasicModel->get_root() );
        CHECK( pGBF != NULL );
        CHECK( !pGBF->is_to_start() );
        CHECK( !pGBF->is_to_end() );
        CHECK( pGBF->get_time_shift() == 128.0f );
        CHECK( tree->get_root()->to_string() == "(goFwd 128)" );
        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, AnalyserGoFwdBadNumber)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        expected << "Line 0. Negative value for element 'goFwd/goBack'. Element ignored." << endl;
        SpLdpTree tree = parser.parse_text("(goFwd -128.3)");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        CHECK( tree->get_root() == NULL );
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, AnalyserGoBackNum)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        //expected << "Line 0. Element 'goBack' has an incoherent value: go backwards to end?. Element ignored." << endl;
        SpLdpTree tree = parser.parse_text("(goBack 21.3)");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        ImoGoBackFwd* pGBF = dynamic_cast<ImoGoBackFwd*>( pBasicModel->get_root() );
        CHECK( pGBF != NULL );
        CHECK( !pGBF->is_to_start() );
        CHECK( !pGBF->is_to_end() );
        CHECK( pGBF->get_time_shift() == -21.3f );
        CHECK( tree->get_root()->to_string() == "(goBack 21.3)" );
        delete tree->get_root();
        delete pBasicModel;
    }

    // clef ----------------------------------------------------------------------

    TEST_FIXTURE(AnalyserTestFixture, Analyser_Clef)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        //expected << "" << endl;
        SpLdpTree tree = parser.parse_text("(clef G)");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        CHECK( pBasicModel->get_root()->is_clef() == true );
        ImoClef* pClef = dynamic_cast<ImoClef*>( pBasicModel->get_root() );
        CHECK( pClef != NULL );
        CHECK( pClef->get_clef_type() == ImoClef::k_G3 );
        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_Clef_Error)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        expected << "Line 0. Unknown clef type 'Fa4'. Assumed 'G'." << endl;
        SpLdpTree tree = parser.parse_text("(clef Fa4)");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        ImoClef* pClef = dynamic_cast<ImoClef*>( pBasicModel->get_root() );
        CHECK( pClef != NULL );
        CHECK( pClef->get_clef_type() == ImoClef::k_G3 );
        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_Clef_LocationX)
    {
        LdpParser parser(cout, m_pLibraryScope->ldp_factory());
        SpLdpTree tree = parser.parse_text("(clef G (dx 70))");
        Analyser a(cout, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        ImoClef* pClef = dynamic_cast<ImoClef*>( pBasicModel->get_root() );
        CHECK( pClef != NULL );
        CHECK( pClef->get_clef_type() == ImoClef::k_G3 );
        CHECK( pClef->is_visible() );
        CHECK( pClef->get_user_location_x() == 70.0f );
        CHECK( pClef->get_user_location_y() == 0.0f );
        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_Clef_NoVisible)
    {
        LdpParser parser(cout, m_pLibraryScope->ldp_factory());
        SpLdpTree tree = parser.parse_text("(clef C2 noVisible)");
        Analyser a(cout, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        ImoClef* pClef = dynamic_cast<ImoClef*>( pBasicModel->get_root() );
        CHECK( pClef != NULL );
        CHECK( pClef->get_clef_type() == ImoClef::k_C2 );
        CHECK( pClef->get_user_location_x() == 0.0f );
        CHECK( pClef->get_user_location_y() == 0.0f );
        CHECK( pClef->is_visible() == false );
        CHECK( pClef->get_staff() == 0 );
        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_Clef_NoVisible_Staff2)
    {
        LdpParser parser(cout, m_pLibraryScope->ldp_factory());
        SpLdpTree tree = parser.parse_text("(clef C2 p2 noVisible)");
        Analyser a(cout, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        ImoClef* pClef = dynamic_cast<ImoClef*>( pBasicModel->get_root() );
        CHECK( pClef != NULL );
        CHECK( pClef->get_clef_type() == ImoClef::k_C2 );
        CHECK( pClef->get_user_location_x() == 0.0f );
        CHECK( pClef->get_user_location_y() == 0.0f );
        CHECK( pClef->is_visible() == false );
        CHECK( pClef->get_staff() == 1 );
        delete tree->get_root();
        delete pBasicModel;
    }

    // key -----------------------------------------------------------------------

    TEST_FIXTURE(AnalyserTestFixture, AnalyserKey)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        //expected << "" << endl;
        SpLdpTree tree = parser.parse_text("(key G)");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        CHECK( pBasicModel->get_root()->is_key_signature() == true );
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        ImoKeySignature* pKeySignature = dynamic_cast<ImoKeySignature*>( pBasicModel->get_root() );
        CHECK( pKeySignature != NULL );
        CHECK( pKeySignature->get_key_type() == ImoKeySignature::G );
        CHECK( pKeySignature->get_staff() == 0 );
        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, AnalyserKeyError)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        expected << "Line 0. Unknown key 'Sol'. Assumed 'C'." << endl;
        SpLdpTree tree = parser.parse_text("(key Sol)");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        ImoKeySignature* pKeySignature = dynamic_cast<ImoKeySignature*>( pBasicModel->get_root() );
        CHECK( pKeySignature != NULL );
        CHECK( pKeySignature->get_key_type() == ImoKeySignature::C );
        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_Key_LocationX)
    {
        LdpParser parser(cout, m_pLibraryScope->ldp_factory());
        SpLdpTree tree = parser.parse_text("(key d (dx 70))");
        Analyser a(cout, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        ImoKeySignature* pKeySignature = dynamic_cast<ImoKeySignature*>( pBasicModel->get_root() );
        CHECK( pKeySignature != NULL );
        CHECK( pKeySignature->get_key_type() == ImoKeySignature::d );
        CHECK( pKeySignature->get_staff() == 0 );
        CHECK( pKeySignature->is_visible() );
        CHECK( pKeySignature->get_user_location_x() == 70.0f );
        CHECK( pKeySignature->get_user_location_y() == 0.0f );
        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_Key_NoVisible)
    {
        LdpParser parser(cout, m_pLibraryScope->ldp_factory());
        SpLdpTree tree = parser.parse_text("(key E- noVisible)");
        Analyser a(cout, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        ImoKeySignature* pKeySignature = dynamic_cast<ImoKeySignature*>( pBasicModel->get_root() );
        CHECK( pKeySignature != NULL );
        CHECK( pKeySignature->get_key_type() == ImoKeySignature::Ef );
        CHECK( pKeySignature->get_user_location_x() == 0.0f );
        CHECK( pKeySignature->get_user_location_y() == 0.0f );
        CHECK( pKeySignature->is_visible() == false );
        delete tree->get_root();
        delete pBasicModel;
    }

    // instrument --------------------------------------------------------------------

    TEST_FIXTURE(AnalyserTestFixture, Analyser_Instrument_Staves)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        //expected << "" << endl;
        SpLdpTree tree = parser.parse_text("(instrument (staves 2)(musicData))");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        CHECK( pBasicModel->get_root()->is_instrument() == true );
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        ImoInstrument* pInstrument = dynamic_cast<ImoInstrument*>( pBasicModel->get_root() );
        CHECK( pInstrument != NULL );
        //cout << "num.staves=" << pInstrument->get_num_staves() << endl;
        CHECK( pInstrument->get_num_staves() == 2 );
        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_Instrument_StavesError)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        expected << "Line 0. Invalid value 'two' for staves. Replaced by 1." << endl;
        SpLdpTree tree = parser.parse_text("(instrument (staves two)(musicData))");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        ImoInstrument* pInstrument = dynamic_cast<ImoInstrument*>( pBasicModel->get_root() );
        CHECK( pInstrument != NULL );
        //cout << "num.staves=" << pInstrument->get_num_staves() << endl;
        CHECK( pInstrument->get_num_staves() == 1 );
        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_Instrument_AddedToScore)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        //expected << "" << endl;
        SpLdpTree tree = parser.parse_text("(score (vers 1.6)(instrument (musicData)))");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        CHECK( pBasicModel->get_root()->is_score() == true );
        ImoScore* pScore = dynamic_cast<ImoScore*>( pBasicModel->get_root() );
        CHECK( pScore != NULL );
        CHECK( pScore->get_num_instruments() == 1 );
        ImoInstrument* pInstr = pScore->get_instrument(0);
        CHECK( pInstr != NULL );
        CHECK( pInstr->get_num_staves() == 1 );
        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_Instrument_Name)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        //expected << "" << endl;
        SpLdpTree tree = parser.parse_text("(instrument (name \"Guitar\")(musicData))");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);

        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );

        ImoInstrument* pInstr = dynamic_cast<ImoInstrument*>( pBasicModel->get_root() );
        CHECK( pInstr != NULL );
        CHECK( pInstr->get_num_staves() == 1 );
        CHECK( pInstr->get_name() == "Guitar" );
        CHECK( pInstr->get_abbrev() == "" );

        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_Instrument_Abbrev)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        //expected << "" << endl;
        SpLdpTree tree = parser.parse_text("(instrument (abbrev \"G.\")(musicData))");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);

        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );

        ImoInstrument* pInstr = dynamic_cast<ImoInstrument*>( pBasicModel->get_root() );
        CHECK( pInstr != NULL );
        CHECK( pInstr->get_num_staves() == 1 );
        CHECK( pInstr->get_name() == "" );
        CHECK( pInstr->get_abbrev() == "G." );

        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_Instrument_NameAbbrev)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        //expected << "" << endl;
        SpLdpTree tree = parser.parse_text("(instrument (name \"Guitar\")(abbrev \"G.\")(musicData))");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);

        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );

        ImoInstrument* pInstr = dynamic_cast<ImoInstrument*>( pBasicModel->get_root() );
        CHECK( pInstr != NULL );
        CHECK( pInstr->get_num_staves() == 1 );
        CHECK( pInstr->get_name() == "Guitar" );
        CHECK( pInstr->get_abbrev() == "G." );

        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_MidiInfo_InstrErrorValue)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        expected << "Line 0. Missing or invalid MIDI instrument (1..256). MIDI info ignored." << endl;
        SpLdpTree tree = parser.parse_text("(infoMIDI piano 1)");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);

        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );

        ImoMidiInfo* pInfo = dynamic_cast<ImoMidiInfo*>( pBasicModel->get_root() );
        CHECK( pInfo == NULL );

        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_MidiInfo_InstrErrorRange)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        expected << "Line 0. Missing or invalid MIDI instrument (1..256). MIDI info ignored." << endl;
        SpLdpTree tree = parser.parse_text("(infoMIDI 315 1)");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);

        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );

        ImoMidiInfo* pInfo = dynamic_cast<ImoMidiInfo*>( pBasicModel->get_root() );
        CHECK( pInfo == NULL );

        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_MidiInfo_InstrumentOk)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        //expected << "Line 0. " << endl;
        SpLdpTree tree = parser.parse_text("(infoMIDI 56)");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);

        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );

        CHECK( pBasicModel->get_root()->is_midi_info() == true );
        ImoMidiInfo* pInfo = dynamic_cast<ImoMidiInfo*>( pBasicModel->get_root() );
        CHECK( pInfo != NULL );
        CHECK( pInfo->get_channel() == 0 );
        CHECK( pInfo->get_instrument() == 55 );

        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_MidiInfo_ChannelErrorValue)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        expected << "Line 0. Invalid MIDI channel (1..16). Channel info ignored." << endl;
        SpLdpTree tree = parser.parse_text("(infoMIDI 56 25)");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);

        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        //cout << tree->get_root()->to_string() << endl;
        CHECK( errormsg.str() == expected.str() );
        CHECK( tree->get_root()->to_string() == "(infoMIDI 56)" );

        ImoMidiInfo* pInfo = dynamic_cast<ImoMidiInfo*>( pBasicModel->get_root() );
        CHECK( pInfo != NULL );
        CHECK( pInfo->get_channel() == 0 );
        CHECK( pInfo->get_instrument() == 55 );

        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_MidiInfo_InstrumentChannelOk)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        //expected << "Line 0. " << endl;
        SpLdpTree tree = parser.parse_text("(infoMIDI 56 10)");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);

        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );

        ImoMidiInfo* pInfo = dynamic_cast<ImoMidiInfo*>( pBasicModel->get_root() );
        CHECK( pInfo != NULL );
        CHECK( pInfo->get_channel() == 9 );
        CHECK( pInfo->get_instrument() == 55 );

        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_Instrument_MidiInfo)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        //expected << "" << endl;
        SpLdpTree tree = parser.parse_text("(instrument (infoMIDI 56 12)(musicData))");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);

        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );

        ImoInstrument* pInstr = dynamic_cast<ImoInstrument*>( pBasicModel->get_root() );
        CHECK( pInstr != NULL );
        CHECK( pInstr->get_num_staves() == 1 );
        CHECK( pInstr->get_name() == "" );
        CHECK( pInstr->get_abbrev() == "" );
        CHECK( pInstr->get_channel() == 11 );
        CHECK( pInstr->get_instrument() == 55 );

        delete tree->get_root();
        delete pBasicModel;
    }

    // time signature -------------------------------------------------------------

    TEST_FIXTURE(AnalyserTestFixture, Analyser_TimeSignature)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        //expected << "" << endl;
        SpLdpTree tree = parser.parse_text("(time 6 8)");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        CHECK( pBasicModel->get_root()->is_time_signature() == true );
        ImoTimeSignature* pTimeSignature = dynamic_cast<ImoTimeSignature*>( pBasicModel->get_root() );
        CHECK( pTimeSignature != NULL );
        CHECK( pTimeSignature->get_beats() == 6 );
        CHECK( pTimeSignature->get_beat_type() == 8 );
        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_TimeSignature_Error)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        expected << "Line 0. time: missing mandatory element 'number'." << endl;
        SpLdpTree tree = parser.parse_text("(time 2)");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        ImoTimeSignature* pTimeSignature = dynamic_cast<ImoTimeSignature*>( pBasicModel->get_root() );
        CHECK( pTimeSignature != NULL );
        CHECK( pTimeSignature->get_beats() == 2 );
        CHECK( pTimeSignature->get_beat_type() == 4 );
        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_TimeSignature_LocationX)
    {
        LdpParser parser(cout, m_pLibraryScope->ldp_factory());
        SpLdpTree tree = parser.parse_text("(time 3 4 (dx 70))");
        Analyser a(cout, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        ImoTimeSignature* pTimeSignature = dynamic_cast<ImoTimeSignature*>( pBasicModel->get_root() );
        CHECK( pTimeSignature != NULL );
        CHECK( pTimeSignature->get_beats() == 3 );
        CHECK( pTimeSignature->get_beat_type() == 4 );
        CHECK( pTimeSignature->is_visible() );
        CHECK( pTimeSignature->get_user_location_x() == 70.0f );
        CHECK( pTimeSignature->get_user_location_y() == 0.0f );
        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_TimeSignature_NoVisible)
    {
        LdpParser parser(cout, m_pLibraryScope->ldp_factory());
        SpLdpTree tree = parser.parse_text("(time 6 8 noVisible)");
        Analyser a(cout, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        ImoTimeSignature* pTimeSignature = dynamic_cast<ImoTimeSignature*>( pBasicModel->get_root() );
        CHECK( pTimeSignature != NULL );
        CHECK( pTimeSignature->get_beats() == 6 );
        CHECK( pTimeSignature->get_beat_type() == 8 );
        CHECK( pTimeSignature->get_user_location_x() == 0.0f );
        CHECK( pTimeSignature->get_user_location_y() == 0.0f );
        CHECK( pTimeSignature->is_visible() == false );
        delete tree->get_root();
        delete pBasicModel;
    }

    // systemLayout -------------------------------------------------------------

    TEST_FIXTURE(AnalyserTestFixture, AnalyserSystemLayoutBadType)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        expected << "Line 0. Expected 'first' or 'other' value but found 'third'. 'first' assumed." << endl;
        SpLdpTree tree = parser.parse_text("(systemLayout third (systemMargins 0 0 0 2000))");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        CHECK( pBasicModel->get_root()->is_system_layout() == true );
        ImoSystemLayout* pSL = dynamic_cast<ImoSystemLayout*>( pBasicModel->get_root() );
        CHECK( pSL != NULL );
        CHECK( pSL->is_first() );
        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, AnalyserSystemLayoutMissingMargins)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        expected << "Line 0. systemLayout: missing mandatory element 'systemMargins'." << endl;
        SpLdpTree tree = parser.parse_text("(systemLayout other)");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        ImoSystemLayout* pSL = dynamic_cast<ImoSystemLayout*>( pBasicModel->get_root() );
        CHECK( pSL != NULL );
        CHECK( !pSL->is_first() );
        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, AnalyserSystemMargins)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        //expected << "Line 0. systemLayout: missing mandatory element 'systemMargins'." << endl;
        SpLdpTree tree = parser.parse_text("(systemMargins 0 100 0 2000)");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        CHECK( pBasicModel->get_root()->is_system_margins() == true );
        ImoSystemMargins* pSM = dynamic_cast<ImoSystemMargins*>( pBasicModel->get_root() );
        CHECK( pSM != NULL );
        CHECK( pSM->get_left_margin() == 0.0f );
        CHECK( pSM->get_right_margin() == 100.0f );
        CHECK( pSM->get_system_distance() == 0.0f );
        CHECK( pSM->get_top_system_distance() == 2000.0f );
        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, AnalyserSystemLayoutOk)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        //expected << "Line 0. systemLayout: missing mandatory element 'systemMargins'." << endl;
        SpLdpTree tree = parser.parse_text("(systemLayout other (systemMargins 0 100 0 2000))");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        ImoSystemLayout* pSL = dynamic_cast<ImoSystemLayout*>( pBasicModel->get_root() );
        CHECK( pSL != NULL );
        CHECK( !pSL->is_first() );
        delete tree->get_root();
        delete pBasicModel;
    }

    // text ---------------------------------------------------------------------

    TEST_FIXTURE(AnalyserTestFixture, AnalyserText)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        //expected << "Line 0. systemLayout: missing mandatory element 'systemMargins'." << endl;
        SpLdpTree tree = parser.parse_text("(text \"This is a text\")");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        CHECK( pBasicModel->get_root()->is_text_string() == true );
        ImoTextString* pText = dynamic_cast<ImoTextString*>( pBasicModel->get_root() );
        CHECK( pText != NULL );
        CHECK( pText->get_text() == "This is a text" );
        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, AnalyserTextMissingText)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        expected << "Line 0. text: missing mandatory element 'string'." << endl;
        SpLdpTree tree = parser.parse_text("(text)");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        ImoTextString* pText = dynamic_cast<ImoTextString*>( pBasicModel->get_root() );
        CHECK( pText == NULL );
        delete tree->get_root();
        delete pBasicModel;
    }

    // metronome ------------------------------------------------------------------

    TEST_FIXTURE(AnalyserTestFixture, Analyser_Metronome_Value)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        //expected << "" << endl;
        SpLdpTree tree = parser.parse_text("(metronome 88)");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        CHECK( pBasicModel->get_root()->is_metronome_mark() == true );
        ImoMetronomeMark* pMM = dynamic_cast<ImoMetronomeMark*>( pBasicModel->get_root() );
        CHECK( pMM != NULL );
        CHECK( pMM->get_mark_type() == ImoMetronomeMark::k_value );
        CHECK( pMM->get_ticks_per_minute() == 88 );
        CHECK( pMM->is_visible() == true );
        CHECK( pMM->has_parenthesis() == false );
        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_Metronome_Error)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        expected << "Line 0. Missing metronome parameters. Replaced by '(metronome 60)'." << endl;
        SpLdpTree tree = parser.parse_text("(metronome)");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        ImoMetronomeMark* pMM = dynamic_cast<ImoMetronomeMark*>( pBasicModel->get_root() );
        CHECK( pMM != NULL );
        CHECK( pMM->get_mark_type() == ImoMetronomeMark::k_value );
        CHECK( pMM->get_ticks_per_minute() == 60 );
        CHECK( pMM->is_visible() == true );
        CHECK( pMM->has_parenthesis() == false );
        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_Metronome_NoteValue)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        //expected << "" << endl;
        SpLdpTree tree = parser.parse_text("(metronome e. 77)");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        ImoMetronomeMark* pMM = dynamic_cast<ImoMetronomeMark*>( pBasicModel->get_root() );
        CHECK( pMM != NULL );
        CHECK( pMM->get_mark_type() == ImoMetronomeMark::k_note_value );
        CHECK( pMM->get_ticks_per_minute() == 77 );
        CHECK( pMM->get_left_note_type() == ImoNote::k_eighth );
        CHECK( pMM->get_left_dots() == 1 );
        CHECK( pMM->is_visible() == true );
        CHECK( pMM->has_parenthesis() == false );
        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_Metronome_NoteNote)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        //expected << "" << endl;
        SpLdpTree tree = parser.parse_text("(metronome e. s)");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        ImoMetronomeMark* pMM = dynamic_cast<ImoMetronomeMark*>( pBasicModel->get_root() );
        CHECK( pMM != NULL );
        CHECK( pMM->get_mark_type() == ImoMetronomeMark::k_note_note );
        CHECK( pMM->get_left_note_type() == ImoNote::k_eighth );
        CHECK( pMM->get_left_dots() == 1 );
        CHECK( pMM->get_right_note_type() == ImoNote::k_16th );
        CHECK( pMM->get_right_dots() == 0 );
        CHECK( pMM->is_visible() == true );
        CHECK( pMM->has_parenthesis() == false );
        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_Metronome_Error2)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        expected << "Line 0. Error in metronome parameters. Replaced by '(metronome 60)'." << endl;
        SpLdpTree tree = parser.parse_text("(metronome e. \"s\")");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        ImoMetronomeMark* pMM = dynamic_cast<ImoMetronomeMark*>( pBasicModel->get_root() );
        CHECK( pMM != NULL );
        CHECK( pMM->get_mark_type() == ImoMetronomeMark::k_value );
        CHECK( pMM->get_ticks_per_minute() == 60 );
        CHECK( pMM->is_visible() == true );
        CHECK( pMM->has_parenthesis() == false );
        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_Metronome_LocationX)
    {
        LdpParser parser(cout, m_pLibraryScope->ldp_factory());
        SpLdpTree tree = parser.parse_text("(metronome 88 (dx 70))");
        Analyser a(cout, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        ImoMetronomeMark* pMM = dynamic_cast<ImoMetronomeMark*>( pBasicModel->get_root() );
        CHECK( pMM != NULL );
        CHECK( pMM->get_mark_type() == ImoMetronomeMark::k_value );
        CHECK( pMM->get_ticks_per_minute() == 88 );
        CHECK( pMM->is_visible() );
        CHECK( pMM->get_user_location_x() == 70.0f );
        CHECK( pMM->get_user_location_y() == 0.0f );
        CHECK( pMM->is_visible() == true );
        CHECK( pMM->has_parenthesis() == false );
        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_Metronome_NoVisible)
    {
        LdpParser parser(cout, m_pLibraryScope->ldp_factory());
        SpLdpTree tree = parser.parse_text("(metronome 88 noVisible)");
        Analyser a(cout, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        ImoMetronomeMark* pMM = dynamic_cast<ImoMetronomeMark*>( pBasicModel->get_root() );
        CHECK( pMM != NULL );
        CHECK( pMM->get_mark_type() == ImoMetronomeMark::k_value );
        CHECK( pMM->get_ticks_per_minute() == 88 );
        CHECK( pMM->get_user_location_x() == 0.0f );
        CHECK( pMM->get_user_location_y() == 0.0f );
        CHECK( pMM->is_visible() == false );
        CHECK( pMM->has_parenthesis() == false );
        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_Metronome_Parenthesis)
    {
        LdpParser parser(cout, m_pLibraryScope->ldp_factory());
        SpLdpTree tree = parser.parse_text("(metronome 88 parenthesis (visible no))");
        Analyser a(cout, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        ImoMetronomeMark* pMM = dynamic_cast<ImoMetronomeMark*>( pBasicModel->get_root() );
        CHECK( pMM != NULL );
        CHECK( pMM->get_mark_type() == ImoMetronomeMark::k_value );
        CHECK( pMM->get_ticks_per_minute() == 88 );
        CHECK( pMM->get_user_location_x() == 0.0f );
        CHECK( pMM->get_user_location_y() == 0.0f );
        CHECK( pMM->is_visible() == false );
        CHECK( pMM->has_parenthesis() == true );
        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_Metronome_Ordering)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        //expected << "Line 0. ?" << endl;
        SpLdpTree tree = parser.parse_text("(metronome 88 parenthesis (dx 7) noVisible)");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        ImoMetronomeMark* pMM = dynamic_cast<ImoMetronomeMark*>( pBasicModel->get_root() );
        CHECK( pMM != NULL );
        CHECK( pMM->get_mark_type() == ImoMetronomeMark::k_value );
        CHECK( pMM->get_ticks_per_minute() == 88 );
        CHECK( pMM->is_visible() == false );
        CHECK( pMM->has_parenthesis() == true );
        CHECK( pMM->get_user_location_x() == 7.0f );
        CHECK( pMM->get_user_location_y() == 0.0f );
        //cout << tree->get_root()->to_string() << endl;
        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_Metronome_Error3)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        expected << "Line 0. Element 'label:parentesis' unknown or not possible here. Removed." << endl;
        SpLdpTree tree = parser.parse_text("(metronome 88 parentesis (dx 7))");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        ImoMetronomeMark* pMM = dynamic_cast<ImoMetronomeMark*>( pBasicModel->get_root() );
        CHECK( pMM != NULL );
        CHECK( pMM->get_mark_type() == ImoMetronomeMark::k_value );
        CHECK( pMM->get_ticks_per_minute() == 88 );
        CHECK( pMM->is_visible() == true );
        CHECK( pMM->has_parenthesis() == false );
        CHECK( pMM->get_user_location_x() == 7.0f );
        CHECK( pMM->get_user_location_y() == 0.0f );
        CHECK( tree->get_root()->to_string() == "(metronome 88 (dx 7))" );
        //cout << tree->get_root()->to_string() << endl;
        delete tree->get_root();
        delete pBasicModel;
    }

    // opt ----------------------------------------------------------------------

    TEST_FIXTURE(AnalyserTestFixture, Analyser_Opt_BoolTrue)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        //expected << "" << endl;
        SpLdpTree tree = parser.parse_text("(opt StaffLines.StopAtFinalBarline true)");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        CHECK( pBasicModel->get_root()->is_option() == true );
        ImoOption* pOpt = dynamic_cast<ImoOption*>( pBasicModel->get_root() );
        CHECK( pOpt != NULL );
        CHECK( pOpt->get_name() == "StaffLines.StopAtFinalBarline" );
        CHECK( pOpt->get_type() == ImoOption::k_boolean );
        CHECK( pOpt->get_bool_value() == true );
        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_Opt_BoolErrorValue)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        expected << "Line 0. Invalid value for option 'StaffLines.StopAtFinalBarline'. Option ignored." << endl;
        SpLdpTree tree = parser.parse_text("(opt StaffLines.StopAtFinalBarline perhaps)");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        CHECK( tree->get_root() == NULL );
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_Opt_ErrorName)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        expected << "Line 0. Invalid option 'StaffLines.Funny'. Option ignored." << endl;
        SpLdpTree tree = parser.parse_text("(opt StaffLines.Funny funny thing)");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        CHECK( tree->get_root() == NULL );
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_Opt_LongOk)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        //expected << "" << endl;
        SpLdpTree tree = parser.parse_text("(opt Render.SpacingValue 40)");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        ImoOption* pOpt = dynamic_cast<ImoOption*>( pBasicModel->get_root() );
        CHECK( pOpt != NULL );
        CHECK( pOpt->get_name() == "Render.SpacingValue" );
        CHECK( pOpt->get_type() == ImoOption::k_number_long );
        CHECK( pOpt->get_long_value() == 40 );
        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_Opt_LongErrorValue)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        expected << "Line 0. Invalid value for option 'Render.SpacingValue'. Option ignored." << endl;
        SpLdpTree tree = parser.parse_text("(opt Render.SpacingValue perhaps)");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        CHECK( tree->get_root() == NULL );
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_Opt_FloatOk)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        //expected << "" << endl;
        SpLdpTree tree = parser.parse_text("(opt Render.SpacingFactor 0.536)");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        ImoOption* pOpt = dynamic_cast<ImoOption*>( pBasicModel->get_root() );
        CHECK( pOpt != NULL );
        CHECK( pOpt->get_name() == "Render.SpacingFactor" );
        CHECK( pOpt->get_type() == ImoOption::k_number_float );
        CHECK( pOpt->get_float_value() == 0.536f );
        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_Opt_FloatErrorValue)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        expected << "Line 0. Invalid value for option 'Render.SpacingFactor'. Option ignored." << endl;
        SpLdpTree tree = parser.parse_text("(opt Render.SpacingFactor perhaps)");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        CHECK( tree->get_root() == NULL );
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_Opt_ErrorMissingValue)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        expected << "Line 0. Missing value for option 'Render.SpacingFactor'. Option ignored." << endl;
        SpLdpTree tree = parser.parse_text("(opt Render.SpacingFactor)");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        CHECK( tree->get_root() == NULL );
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_Opt_AddedToScore)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        //expected << "" << endl;
        SpLdpTree tree = parser.parse_text("(score (vers 1.6)(opt StaffLines.StopAtFinalBarline true)(instrument (musicData)))");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        ImoScore* pScore = dynamic_cast<ImoScore*>( pBasicModel->get_root() );
        CHECK( pScore != NULL );
        CHECK( pScore->has_options() == true );
        ImoOption* pOpt = pScore->get_option("StaffLines.StopAtFinalBarline");
        CHECK( pOpt != NULL );
        CHECK( pOpt->get_name() == "StaffLines.StopAtFinalBarline" );
        CHECK( pOpt->get_type() == ImoOption::k_boolean );
        CHECK( pOpt->get_bool_value() == true );
        delete tree->get_root();
        delete pBasicModel;
    }


    // nodes ----------------------------------------------------------------------

    TEST_FIXTURE(AnalyserTestFixture, Analyser_DeleteParameter)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        expected << "Line 0. Element 'label:instrument' unknown or not possible here. Removed." << endl;
        SpLdpTree tree = parser.parse_text("(n c4 q instrument)");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        ImoNote* pNote = dynamic_cast<ImoNote*>( pBasicModel->get_root() );
        CHECK( pNote != NULL );
        CHECK( tree->get_root()->to_string() == "(n c4 q)" );
        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_DeleteNode)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        expected << "Line 0. Invalid option 'StaffLines.Funny'. Option ignored." << endl;
        SpLdpTree tree = parser.parse_text("(opt StaffLines.Funny true)");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        CHECK( errormsg.str() == expected.str() );
        CHECK( tree->get_root() == NULL );
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_DeleteNodeNoProblems)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        expected << "Line 0. Invalid option 'StaffLines.Funny'. Option ignored." << endl;
        SpLdpTree tree = parser.parse_text("(score (vers 1.6) (opt StaffLines.Funny true) (instrument (musicData (r q))))");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        CHECK( errormsg.str() == expected.str() );
        CHECK( tree->get_root()->to_string() == "(score (vers 1.6) (instrument (musicData (r q))))" );
        delete tree->get_root();
        delete pBasicModel;
    }

    // spacer --------------------------------------------------------------------

    TEST_FIXTURE(AnalyserTestFixture, Analyser_Spacer_Ok)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        //expected << "" << endl;
        SpLdpTree tree = parser.parse_text("(spacer 70.5)");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        CHECK( pBasicModel->get_root()->is_spacer() == true );
        ImoSpacer* pSp = dynamic_cast<ImoSpacer*>( pBasicModel->get_root() );
        CHECK( pSp != NULL );
        CHECK( pSp->get_width() == 70.5f );
        CHECK( pSp->get_staff() == 0 );
        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_Spacer_MissingWidth)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        expected << "Line 0. Missing width for spacer. Spacer removed." << endl;
        SpLdpTree tree = parser.parse_text("(spacer)");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        CHECK( tree->get_root() == NULL );
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_Spacer_Staff)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        //expected << "" << endl;
        SpLdpTree tree = parser.parse_text("(spacer 70.5 p3)");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        ImoSpacer* pSp = dynamic_cast<ImoSpacer*>( pBasicModel->get_root() );
        CHECK( pSp != NULL );
        CHECK( pSp->get_width() == 70.5f );
        CHECK( pSp->get_staff() == 2 );
        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_Spacer_ErrorStaff)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        expected << "Line 0. Invalid staff 'pan'. Replaced by 'p1'." << endl;
        SpLdpTree tree = parser.parse_text("(spacer 70.5 pan)");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        ImoSpacer* pSp = dynamic_cast<ImoSpacer*>( pBasicModel->get_root() );
        CHECK( pSp != NULL );
        CHECK( pSp->get_width() == 70.5f );
        CHECK( pSp->get_staff() == 0 );
        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_Spacer_ErrorMoreParams)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        expected << "Line 0. Element 'label:more' unknown or not possible here. Removed." << endl;
        SpLdpTree tree = parser.parse_text("(spacer 70.5 more)");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        ImoSpacer* pSp = dynamic_cast<ImoSpacer*>( pBasicModel->get_root() );
        CHECK( pSp != NULL );
        CHECK( pSp->get_width() == 70.5f );
        CHECK( pSp->get_staff() == 0 );
        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_Spacer_Attachment)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        //expected << "" << endl;
        SpLdpTree tree = parser.parse_text("(spacer 70 (text \"andante\"))");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        ImoSpacer* pSp = dynamic_cast<ImoSpacer*>( pBasicModel->get_root() );
        CHECK( pSp != NULL );
        CHECK( pSp->get_width() == 70.0f );
        CHECK( pSp->get_staff() == 0 );
        CHECK( pSp->has_attachments() == true );
        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_Spacer_ErrorAttachment)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        expected << "Line 0. Element 'r' unknown or not possible here. Removed." << endl;
        SpLdpTree tree = parser.parse_text("(spacer 70 (r q))");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        ImoSpacer* pSp = dynamic_cast<ImoSpacer*>( pBasicModel->get_root() );
        CHECK( pSp != NULL );
        CHECK( pSp->get_width() == 70.0f );
        CHECK( pSp->get_staff() == 0 );
        CHECK( pSp->has_attachments() == false );
        CHECK( tree->get_root()->to_string() == "(spacer 70)" );
        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_Spacer_ErrorAttachment2)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        expected << "Line 0. Element 'r' unknown or not possible here. Removed." << endl;
        SpLdpTree tree = parser.parse_text("(spacer 70 (r q)(text \"andante\"))");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        ImoSpacer* pSp = dynamic_cast<ImoSpacer*>( pBasicModel->get_root() );
        CHECK( pSp != NULL );
        CHECK( pSp->get_width() == 70.0f );
        CHECK( pSp->get_staff() == 0 );
        CHECK( pSp->has_attachments() == true );
        CHECK( tree->get_root()->to_string() == "(spacer 70 (text \"andante\"))" );
        delete tree->get_root();
        delete pBasicModel;
    }

    // lenmusdoc --------------------------------------------------------------------

    TEST_FIXTURE(AnalyserTestFixture, Analyser_Lenmusdoc_ReturnsImobj)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        //expected << "" << endl;
        SpLdpTree tree = parser.parse_text("(lenmusdoc (vers 0.0)(content ))");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        CHECK( pBasicModel->get_root() != NULL);
        CHECK( pBasicModel->get_root()->is_document() == true );
        ImoDocument* pDoc = dynamic_cast<ImoDocument*>( pBasicModel->get_root() );
        CHECK( pDoc != NULL );
        CHECK( pDoc->get_num_content_items() == 0 );
        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_Lenmusdoc_HasContent)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        //expected << "" << endl;
        SpLdpTree tree = parser.parse_text("(lenmusdoc (vers 0.0)(content (text \"hello world\")))");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        ImoDocument* pDoc = dynamic_cast<ImoDocument*>( pBasicModel->get_root() );
        CHECK( pDoc != NULL );
        CHECK( pDoc->get_num_content_items() == 1 );
        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_Lenmusdoc_GetContentItemText)
    {
        LdpParser parser(cout, m_pLibraryScope->ldp_factory());
        SpLdpTree tree = parser.parse_text("(lenmusdoc (vers 0.0)(content (text \"hello world\")))");
        Analyser a(cout, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        ImoDocument* pDoc = dynamic_cast<ImoDocument*>( pBasicModel->get_root() );
        ImoTextString* pText = dynamic_cast<ImoTextString*>( pDoc->get_content_item(0) );
        CHECK( pText != NULL );
        CHECK( pText->get_text() == "hello world" );
        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_Lenmusdoc_GetContentItemScore)
    {
        LdpParser parser(cout, m_pLibraryScope->ldp_factory());
        SpLdpTree tree = parser.parse_text("(lenmusdoc (vers 0.0)(content (score (vers 1.6)(instrument (musicData))) (text \"hello world\")))");
        Analyser a(cout, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        ImoDocument* pDoc = dynamic_cast<ImoDocument*>( pBasicModel->get_root() );
        CHECK( pDoc->get_num_content_items() == 2 );
        ImoScore* pScore = dynamic_cast<ImoScore*>( pDoc->get_content_item(0) );
        CHECK( pScore != NULL );
        CHECK( pScore->get_num_instruments() == 1 );
        ImoTextString* pText = dynamic_cast<ImoTextString*>( pDoc->get_content_item(1) );
        CHECK( pText != NULL );
        CHECK( pText->get_text() == "hello world" );
        delete tree->get_root();
        delete pBasicModel;
    }

    // TiesBuilder ---------------------------------------------------------------

    TEST_FIXTURE(AnalyserTestFixture, Analyser_TiesBuilder_EndTieOk)
    {
        DtoNote startDto;
        startDto.set_octave(4);
        startDto.set_step(2);
        startDto.set_accidentals(ImoNote::k_sharp);
        ImoNote* pStartNote = new ImoNote(startDto);

        DtoNote endDto;
        endDto.set_octave(4);
        endDto.set_step(2);
        endDto.set_accidentals(ImoNote::k_sharp);
        ImoNote* pEndNote = new ImoNote(endDto);

        ImoTieInfo* pStartInfo = new ImoTieInfo();
        pStartInfo->set_start(true);
        pStartInfo->set_tie_number(12);
        pStartInfo->set_note(pStartNote);

        ImoTieInfo* pEndInfo = new ImoTieInfo();
        pEndInfo->set_start(false);
        pEndInfo->set_tie_number(12);
        pEndInfo->set_note(pEndNote);

        Analyser a(cout, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = new BasicModel();
        TiesBuilder builder(cout, pBasicModel, &a);
        builder.start_tie(pStartInfo);
        builder.end_tie(pEndInfo);

        std::list<ImoTie*>& ties = pBasicModel->get_ties();
        CHECK (ties.size() == 1 );

        CHECK( pStartNote->is_tied_next() == true);
        CHECK( pStartNote->is_tied_prev() == false);
        CHECK( pEndNote->is_tied_next() == false);
        CHECK( pEndNote->is_tied_prev() == true);

        delete pStartNote;
        delete pEndNote;
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_TiesBuilder_StartTieDuplicated)
    {
        stringstream errormsg;
        stringstream expected;
        expected << "Line 0. This tie has the same number than that defined in line 0. This tie will be ignored." << endl;

        ImoTieInfo* pStartInfo = new ImoTieInfo();
        pStartInfo->set_start(true);
        pStartInfo->set_tie_number(12);

        ImoTieInfo* pOtherInfo = new ImoTieInfo();
        pOtherInfo->set_start(true);
        pOtherInfo->set_tie_number(12);

        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = new BasicModel();
        TiesBuilder builder(errormsg, pBasicModel, &a);
        builder.start_tie(pStartInfo);
        builder.start_tie(pOtherInfo);

        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_TiesBuilder_ErrorNotesCanNotBeTied)
    {
        stringstream errormsg;
        stringstream expected;
        expected << "Line 0. Requesting to tie notes of different voice or pitch. Tie number 12 will be removed." << endl;

        DtoNote startDto;
        startDto.set_octave(4);
        startDto.set_step(2);
        startDto.set_accidentals(ImoNote::k_sharp);
        ImoNote startNote(startDto);

        DtoNote endDto;
        endDto.set_octave(3);
        endDto.set_step(2);
        endDto.set_accidentals(ImoNote::k_sharp);
        ImoNote endNote(endDto);

        ImoTieInfo* pStartInfo = new ImoTieInfo();
        pStartInfo->set_start(true);
        pStartInfo->set_tie_number(12);
        pStartInfo->set_note(&startNote);

        ImoTieInfo* pEndInfo = new ImoTieInfo();
        pEndInfo->set_start(false);
        pEndInfo->set_tie_number(12);
        pEndInfo->set_note(&endNote);

        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = new BasicModel();
        TiesBuilder builder(errormsg, pBasicModel, &a);
        builder.start_tie(pStartInfo);
        builder.end_tie(pEndInfo);

        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );

        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_TiesBuilder_ErrorNoStartInfo)
    {
        stringstream errormsg;
        stringstream expected;
        expected << "Line 0. No 'start' element for tie number 12. Tie removed." << endl;

        ImoTieInfo* pEndInfo = new ImoTieInfo();
        pEndInfo->set_start(false);
        pEndInfo->set_tie_number(12);

        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = new BasicModel();
        TiesBuilder builder(errormsg, pBasicModel, &a);
        builder.end_tie(pEndInfo);

        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_TiesBuilder_PendingTiesAtDeletion)
    {
        stringstream errormsg;
        stringstream expected;
        expected << "Line 0. No 'stop' element for tie number 12. Tie removed." << endl
                 << "Line 0. No 'stop' element for tie number 14. Tie removed." << endl;

        ImoTieInfo* pStartInfo = new ImoTieInfo();
        pStartInfo->set_start(true);
        pStartInfo->set_tie_number(12);

        ImoTieInfo* pOtherInfo = new ImoTieInfo();
        pOtherInfo->set_start(true);
        pOtherInfo->set_tie_number(14);

        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = new BasicModel();
        TiesBuilder* pBuilder = new TiesBuilder(errormsg, pBasicModel, &a);
        pBuilder->start_tie(pStartInfo);
        pBuilder->start_tie(pOtherInfo);

        delete pBuilder;

        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_TiesBuilder_InstrumentChangeError)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        expected << "Line 0. No 'stop' element for tie number 12. Tie removed." << endl
                 << "Line 0. No 'start' element for tie number 12. Tie removed." << endl;
        SpLdpTree tree = parser.parse_text("(score (vers 1.6)(instrument (musicData (n c4 q (tie 12 start))))(instrument (musicData (n d4 e (tie 12 stop)))))");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);

        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        //cout << tree->get_root()->to_string() << endl;
        CHECK( errormsg.str() == expected.str() );

        ImoScore* pScore = dynamic_cast<ImoScore*>( pBasicModel->get_root() );
        ImoInstrument* pInstr = pScore->get_instrument(0);
        ImoMusicData* pMusic = pInstr->get_musicdata();
        CHECK( pMusic != NULL );

        ImoObj::children_iterator it = pMusic->begin();

        ImoNote* pNote = dynamic_cast<ImoNote*>(*it);
        CHECK( pNote->get_octave() == 4 );
        CHECK( pNote->get_step() == 0 );
        CHECK( pNote->is_tied_next() == false );
        CHECK( pNote->is_tied_prev() == false );

        pInstr = pScore->get_instrument(1);
        pMusic = pInstr->get_musicdata();
        CHECK( pMusic != NULL );

        it = pMusic->begin();

        pNote = dynamic_cast<ImoNote*>(*it);
        CHECK( pNote->get_octave() == 4 );
        CHECK( pNote->get_step() == 1 );
        CHECK( pNote->is_tied_next() == false );
        CHECK( pNote->is_tied_prev() == false );

        delete tree->get_root();
        delete pBasicModel;
    }

    // beam -----------------------------------------------------------------------

    TEST_FIXTURE(AnalyserTestFixture, Analyser_Beam_Start)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        //expected << "Line 0. " << endl;
        SpLdpTree tree = parser.parse_text("(beam 12 begin)");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        CHECK( pBasicModel->get_root()->is_beam_info() == true );
        ImoBeamInfo* pInfo = dynamic_cast<ImoBeamInfo*>( pBasicModel->get_root() );
        CHECK( pInfo != NULL );
        CHECK( pInfo->get_beam_number() == 12 );
        CHECK( pInfo->get_beam_type(0) == ImoBeam::k_begin );
//        CHECK( pInfo->get_beam_element() == tree->get_root() );
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        //cout << tree->get_root()->to_string() << endl;
        CHECK( errormsg.str() == expected.str() );
        CHECK( tree->get_root()->to_string() == "(beam 12 begin)" );
        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_Beam_TreeLevels)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        //expected << "Line 0. " << endl;
        SpLdpTree tree = parser.parse_text("(beam 12 begin begin forward)");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        ImoBeamInfo* pInfo = dynamic_cast<ImoBeamInfo*>( pBasicModel->get_root() );
        CHECK( pInfo != NULL );
        CHECK( pInfo->get_beam_number() == 12 );
        CHECK( pInfo->get_beam_type(0) == ImoBeam::k_begin );
        CHECK( pInfo->get_beam_type(1) == ImoBeam::k_begin );
        CHECK( pInfo->get_beam_type(2) == ImoBeam::k_forward );
        CHECK( pInfo->get_beam_type(3) == ImoBeam::k_none );
        CHECK( pInfo->get_beam_type(4) == ImoBeam::k_none );
        CHECK( pInfo->get_beam_type(5) == ImoBeam::k_none );
//        CHECK( pInfo->get_beam_element() == tree->get_root() );
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        //cout << tree->get_root()->to_string() << endl;
        CHECK( errormsg.str() == expected.str() );
        CHECK( tree->get_root()->to_string() == "(beam 12 begin begin forward)" );
        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_Beam_ErrorNum)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        expected << "Line 0. Missing or invalid beam number. Beam ignored." << endl;
        SpLdpTree tree = parser.parse_text("(beam begin)");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        CHECK( pBasicModel->get_root() == NULL );
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_Beam_ErrorType)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        expected << "Line 0. Missing or invalid beam type. Beam ignored." << endl;
        SpLdpTree tree = parser.parse_text("(beam 34 empieza)");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        CHECK( pBasicModel->get_root() == NULL );
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_BeamsBuilder_Destructor)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        expected << "Line 0. No 'end' element for beam number 14. Beam removed." << endl;
        SpLdpTree tree = parser.parse_text("(n c4 e (beam 14 begin))");
        Analyser* pA = new Analyser(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = pA->analyse_tree(tree);
        delete pA;
        ImoNote* pNote = dynamic_cast<ImoNote*>( pBasicModel->get_root() );
        CHECK( pNote != NULL );
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        //cout << tree->get_root()->to_string() << endl;
        CHECK( errormsg.str() == expected.str() );
        CHECK( pNote != NULL );
        CHECK( pNote->get_beam_type(0) == ImoBeam::k_none );

        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_BeamsBuilder_BeamError)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        expected << "Line 0. No 'begin/continue' elements for beam number 13. Beam removed." << endl
                 << "Line 0. No 'end' element for beam number 14. Beam removed." << endl;
        SpLdpTree tree = parser.parse_text("(musicData (n c4 q. (beam 14 begin)) (n d4 s (beam 13 end)))");
        Analyser* pA = new Analyser(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = pA->analyse_tree(tree);
        delete pA;
        ImoMusicData* pMusic = dynamic_cast<ImoMusicData*>( pBasicModel->get_root() );
        CHECK( pMusic != NULL );
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        //cout << tree->get_root()->to_string() << endl;
        CHECK( errormsg.str() == expected.str() );

        ImoObj::children_iterator it = pMusic->begin();

        ImoNote* pNote1 = dynamic_cast<ImoNote*>( *it );
        CHECK( pNote1 != NULL );
        CHECK( pNote1->get_beam_type(0) == ImoBeam::k_none );

        ++it;
        ImoNote* pNote2 = dynamic_cast<ImoNote*>( *it );
        CHECK( pNote2 != NULL );
        CHECK( pNote2->get_beam_type(0) == ImoBeam::k_none );

        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_BeamsBuilder_BeamOk)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        //expected << "Line 0. No 'end' element for beam number 14. Beam removed." << endl;
        SpLdpTree tree = parser.parse_text("(musicData (n c4 q. (beam 14 begin)) (n d4 s (beam 14 end backward)))");
        Analyser* pA = new Analyser(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = pA->analyse_tree(tree);
        delete pA;
        ImoMusicData* pMusic = dynamic_cast<ImoMusicData*>( pBasicModel->get_root() );
        CHECK( pMusic != NULL );
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        //cout << tree->get_root()->to_string() << endl;
        CHECK( errormsg.str() == expected.str() );
        CHECK( tree->get_root()->to_string() == "(musicData (n c4 q. (beam 14 begin)) (n d4 s (beam 14 end backward)))" );

        ImoObj::children_iterator it = pMusic->begin();

        ImoNote* pNote1 = dynamic_cast<ImoNote*>( *it );
        CHECK( pNote1 != NULL );
        CHECK( pNote1->get_beam_type(0) == ImoBeam::k_begin );
        CHECK( pNote1->get_beam_type(1) == ImoBeam::k_none );
        CHECK( pNote1->get_beam_type(2) == ImoBeam::k_none );

        ++it;
        ImoNote* pNote2 = dynamic_cast<ImoNote*>( *it );
        CHECK( pNote2 != NULL );
        CHECK( pNote2->get_beam_type(0) == ImoBeam::k_end );
        CHECK( pNote2->get_beam_type(1) == ImoBeam::k_backward );
        CHECK( pNote2->get_beam_type(2) == ImoBeam::k_none );

        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_BeamsBuilder_InstrumentChangeError)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        expected << "Line 0. No 'end' element for beam number 14. Beam removed." << endl;
        SpLdpTree tree = parser.parse_text("(score (vers 1.6) (instrument (musicData (n c4 q. (beam 14 begin)))) (instrument (musicData (n c4 e))))" );
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        //cout << tree->get_root()->to_string() << endl;
        CHECK( errormsg.str() == expected.str() );

        ImoScore* pScore = dynamic_cast<ImoScore*>( pBasicModel->get_root() );
        ImoInstrument* pInstr = pScore->get_instrument(0);
        ImoMusicData* pMusic = pInstr->get_musicdata();
        CHECK( pMusic != NULL );

        ImoObj::children_iterator it = pMusic->begin();
        ImoNote* pNote = dynamic_cast<ImoNote*>(*it);
        CHECK( pNote->get_octave() == 4 );
        CHECK( pNote->get_step() == 0 );
        CHECK( pNote->get_beam_type(0) == ImoBeam::k_none );

        delete tree->get_root();
        delete pBasicModel;
    }

    // beam (old syntax) --------------------------------------------------------

    TEST_FIXTURE(AnalyserTestFixture, Analyser_BeamOld_ErrorInvalidG)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        expected << "Line 0. Invalid parameter 'g+7'. Removed." << endl;
        SpLdpTree tree = parser.parse_text("(n c4 e g+7)");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        //cout << tree->get_root()->to_string() << endl;
        CHECK( errormsg.str() == expected.str() );
        CHECK( tree->get_root()->to_string() == "(n c4 e)" );

        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_BeamOld_ErrorInvalidNote)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        expected << "Line 0. Requesting beaming a note longer than eighth. Beam ignored." << endl;
        SpLdpTree tree = parser.parse_text("(n c4 w g+)");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        //cout << tree->get_root()->to_string() << endl;
        CHECK( errormsg.str() == expected.str() );
        CHECK( tree->get_root()->to_string() == "(n c4 w)" );

        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_BeamOld_ErrorAlreadyOpen)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        expected << "Line 0. Requesting to start a beam (g+) but there is already an open beam. Beam ignored." << endl;
        SpLdpTree tree = parser.parse_text("(musicData (n c4 s g+) (n e4 e g+))");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        std::list<ImoBeam*>& beams = pBasicModel->get_beams();
        CHECK( beams.size() == 0 );
        //cout << "beams size = " << static_cast<int>(beams.size()) << endl;
        ImoMusicData* pMusic = dynamic_cast<ImoMusicData*>( pBasicModel->get_root() );
        CHECK( pMusic != NULL );
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        //cout << tree->get_root()->to_string() << endl;
        CHECK( errormsg.str() == expected.str() );
        CHECK( tree->get_root()->to_string() == "(musicData (n c4 s g+) (n e4 e))" );

        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_BeamOld_SES)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        //expected << "Line 0. No 'start' element for beam number 12. Tie removed." << endl;
        SpLdpTree tree = parser.parse_text("(musicData (n c4 s g+)(n e4 e)(n c4 s g-))");
        Analyser* pA = new Analyser(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = pA->analyse_tree(tree);
        std::list<ImoBeam*>& beams = pBasicModel->get_beams();
        CHECK( beams.size() == 1 );
        ImoBeam* pBeam = beams.front();
        std::list<ImoStaffObj*>& notes = pBeam->get_related_objects();
        CHECK( notes.size() == 3 );
        //cout << "notes size = " << static_cast<int>(notes.size()) << endl;

        delete pA;
        ImoMusicData* pMusic = dynamic_cast<ImoMusicData*>( pBasicModel->get_root() );
        CHECK( pMusic != NULL );
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        //cout << tree->get_root()->to_string() << endl;
        CHECK( errormsg.str() == expected.str() );
        CHECK( tree->get_root()->to_string() == "(musicData (n c4 s g+) (n e4 e) (n c4 s g-))" );

        ImoObj::children_iterator it = pMusic->begin();

        ImoNote* pNote1 = dynamic_cast<ImoNote*>( *it );
        CHECK( pNote1 != NULL );
        CHECK( pNote1->get_beam_type(0) == ImoBeam::k_begin );
        CHECK( pNote1->get_beam_type(1) == ImoBeam::k_forward );
        CHECK( pNote1->get_beam_type(2) == ImoBeam::k_none );
        CHECK( pNote1->get_beam_type(3) == ImoBeam::k_none );
        CHECK( pNote1->get_beam_type(4) == ImoBeam::k_none );
        CHECK( pNote1->get_beam_type(5) == ImoBeam::k_none );

        ++it;
        ImoNote* pNote2 = dynamic_cast<ImoNote*>( *it );
        CHECK( pNote2 != NULL );
        CHECK( pNote2->get_beam_type(0) == ImoBeam::k_continue );
        CHECK( pNote2->get_beam_type(1) == ImoBeam::k_none );
        CHECK( pNote2->get_beam_type(2) == ImoBeam::k_none );
        CHECK( pNote2->get_beam_type(3) == ImoBeam::k_none );
        CHECK( pNote2->get_beam_type(4) == ImoBeam::k_none );
        CHECK( pNote2->get_beam_type(5) == ImoBeam::k_none );

        ++it;
        ImoNote* pNote3 = dynamic_cast<ImoNote*>( *it );
        CHECK( pNote3 != NULL );
        CHECK( pNote3->get_beam_type(0) == ImoBeam::k_end );
        CHECK( pNote3->get_beam_type(1) == ImoBeam::k_backward );
        CHECK( pNote3->get_beam_type(2) == ImoBeam::k_none );
        CHECK( pNote3->get_beam_type(3) == ImoBeam::k_none );
        CHECK( pNote3->get_beam_type(4) == ImoBeam::k_none );
        CHECK( pNote3->get_beam_type(5) == ImoBeam::k_none );

        delete tree->get_root();
        delete pBasicModel;
    }

    // AutoBeamer ----------------------------------------------------------------

    TEST_FIXTURE(AnalyserTestFixture, Analyser_AutoBeamer_SE)
    {
        ImoNote* pNote1 = new ImoNote();
        pNote1->set_note_type(ImoNote::k_16th);

        ImoNote* pNote2 = new ImoNote();
        pNote2->set_note_type(ImoNote::k_eighth);

        ImoBeam beam;
        beam.push_back(pNote1);
        beam.push_back(pNote2);
        CHECK( beam.is_beam() == true );

        AutoBeamer autobeamer(&beam);
        autobeamer.do_autobeam();

        CHECK( pNote1->get_beam_type(0) == ImoBeam::k_begin );
        CHECK( pNote1->get_beam_type(1) == ImoBeam::k_forward );
        CHECK( pNote1->get_beam_type(2) == ImoBeam::k_none );
        CHECK( pNote1->get_beam_type(3) == ImoBeam::k_none );
        CHECK( pNote1->get_beam_type(4) == ImoBeam::k_none );
        CHECK( pNote1->get_beam_type(5) == ImoBeam::k_none );

        CHECK( pNote2->get_beam_type(0) == ImoBeam::k_end );
        CHECK( pNote2->get_beam_type(1) == ImoBeam::k_none );
        CHECK( pNote2->get_beam_type(2) == ImoBeam::k_none );
        CHECK( pNote2->get_beam_type(3) == ImoBeam::k_none );
        CHECK( pNote2->get_beam_type(4) == ImoBeam::k_none );
        CHECK( pNote2->get_beam_type(5) == ImoBeam::k_none );

        delete pNote1;
        delete pNote2;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_AutoBeamer_EE)
    {
        ImoNote* pNote1 = new ImoNote();
        pNote1->set_note_type(ImoNote::k_eighth);

        ImoNote* pNote2 = new ImoNote();
        pNote2->set_note_type(ImoNote::k_eighth);

        ImoBeam beam;
        beam.push_back(pNote1);
        beam.push_back(pNote2);

        AutoBeamer autobeamer(&beam);
        autobeamer.do_autobeam();

        CHECK( pNote1->get_beam_type(0) == ImoBeam::k_begin );
        CHECK( pNote1->get_beam_type(1) == ImoBeam::k_none );
        CHECK( pNote1->get_beam_type(2) == ImoBeam::k_none );
        CHECK( pNote1->get_beam_type(3) == ImoBeam::k_none );
        CHECK( pNote1->get_beam_type(4) == ImoBeam::k_none );
        CHECK( pNote1->get_beam_type(5) == ImoBeam::k_none );

        CHECK( pNote2->get_beam_type(0) == ImoBeam::k_end );
        CHECK( pNote2->get_beam_type(1) == ImoBeam::k_none );
        CHECK( pNote2->get_beam_type(2) == ImoBeam::k_none );
        CHECK( pNote2->get_beam_type(3) == ImoBeam::k_none );
        CHECK( pNote2->get_beam_type(4) == ImoBeam::k_none );
        CHECK( pNote2->get_beam_type(5) == ImoBeam::k_none );

        delete pNote1;
        delete pNote2;
    }

    // tuplet ------------------------------------------------------------------

    TEST_FIXTURE(AnalyserTestFixture, Analyser_Tuplet_TypeError)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        expected << "Line 0. Missing or invalid tuplet type. Tuplet ignored." << endl;
        SpLdpTree tree = parser.parse_text("(t start 3)");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);

        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        CHECK( pBasicModel->get_root() == NULL );
        CHECK( tree->get_root() == NULL );
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_Tuplet_ActualNotes)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        //expected << "Line 0. " << endl;
        SpLdpTree tree = parser.parse_text("(t + 3)");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);

        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        //cout << tree->get_root()->to_string() << endl;
        CHECK( errormsg.str() == expected.str() );
        CHECK( tree->get_root()->to_string() == "(t + 3)" );
        CHECK( pBasicModel->get_root()->is_tuplet_info() == true );
        ImoTupletInfo* pInfo = dynamic_cast<ImoTupletInfo*>( pBasicModel->get_root() );
        CHECK( pInfo != NULL );
        CHECK( pInfo->is_start_of_tuplet() == true );
        CHECK( pInfo->get_actual_number() == 3 );
        CHECK( pInfo->get_normal_number() == 2 );
        CHECK( pInfo->get_show_bracket() == true );
        CHECK( pInfo->get_show_number() == true );

        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_Tuplet_ErrorNormalNumRequired)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        expected << "Line 0. Tuplet: Missing or invalid normal notes number. Tuplet ignored." << endl;
        SpLdpTree tree = parser.parse_text("(t + 7)");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);

        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        CHECK( pBasicModel->get_root() == NULL );
        CHECK( tree->get_root() == NULL );
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_Tuplet_NormalNotes)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        //expected << "Line 0. " << endl;
        SpLdpTree tree = parser.parse_text("(t + 7 4)");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);

        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        //cout << tree->get_root()->to_string() << endl;
        CHECK( errormsg.str() == expected.str() );
        CHECK( tree->get_root()->to_string() == "(t + 7 4)" );
        ImoTupletInfo* pInfo = dynamic_cast<ImoTupletInfo*>( pBasicModel->get_root() );
        CHECK( pInfo != NULL );
        CHECK( pInfo->is_start_of_tuplet() == true );
        CHECK( pInfo->get_actual_number() == 7 );
        CHECK( pInfo->get_normal_number() == 4 );
        CHECK( pInfo->get_show_bracket() == true );
        CHECK( pInfo->get_show_number() == true );

        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_Tuplet_NoBracket)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        //expected << "Line 0. " << endl;
        SpLdpTree tree = parser.parse_text("(t + 3 noBracket)");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);

        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        //cout << tree->get_root()->to_string() << endl;
        CHECK( errormsg.str() == expected.str() );
        CHECK( tree->get_root()->to_string() == "(t + 3 noBracket)" );
        ImoTupletInfo* pInfo = dynamic_cast<ImoTupletInfo*>( pBasicModel->get_root() );
        CHECK( pInfo != NULL );
        CHECK( pInfo->is_start_of_tuplet() == true );
        CHECK( pInfo->get_actual_number() == 3 );
        CHECK( pInfo->get_normal_number() == 2 );
        CHECK( pInfo->get_show_bracket() == false );
        CHECK( pInfo->get_show_number() == true );

        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_Tuplet_ErrorLabelParameter)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        expected << "Line 0. Element 'label:blue' unknown or not possible here. Removed." << endl;
        SpLdpTree tree = parser.parse_text("(t + 3 noBracket blue)");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);

        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        //cout << tree->get_root()->to_string() << endl;
        CHECK( errormsg.str() == expected.str() );
        CHECK( tree->get_root()->to_string() == "(t + 3 noBracket)" );
        ImoTupletInfo* pInfo = dynamic_cast<ImoTupletInfo*>( pBasicModel->get_root() );
        CHECK( pInfo != NULL );
        CHECK( pInfo->is_start_of_tuplet() == true );
        CHECK( pInfo->get_actual_number() == 3 );
        CHECK( pInfo->get_normal_number() == 2 );
        CHECK( pInfo->get_show_bracket() == false );
        CHECK( pInfo->get_show_number() == true );

        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_Tuplet_ErrorCompoundParameter)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        expected << "Line 0. Element 'color' unknown or not possible here. Removed." << endl;
        SpLdpTree tree = parser.parse_text("(t + 3 (color blue) noBracket)");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);

        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        //cout << tree->get_root()->to_string() << endl;
        CHECK( errormsg.str() == expected.str() );
        CHECK( tree->get_root()->to_string() == "(t + 3 noBracket)" );
        ImoTupletInfo* pInfo = dynamic_cast<ImoTupletInfo*>( pBasicModel->get_root() );
        CHECK( pInfo != NULL );
        CHECK( pInfo->is_start_of_tuplet() == true );
        CHECK( pInfo->get_actual_number() == 3 );
        CHECK( pInfo->get_normal_number() == 2 );
        CHECK( pInfo->get_show_bracket() == false );
        CHECK( pInfo->get_show_number() == true );

        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_TupletsBuilder_Destructor)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        expected << "Line 0. No 'end' element for tuplet. Tuplet removed." << endl;
        SpLdpTree tree = parser.parse_text("(n c4 e (t + 3))");
        Analyser* pA = new Analyser(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = pA->analyse_tree(tree);
        delete pA;
        ImoNote* pNote = dynamic_cast<ImoNote*>( pBasicModel->get_root() );
        CHECK( pNote != NULL );
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        //cout << tree->get_root()->to_string() << endl;
        CHECK( errormsg.str() == expected.str() );
        CHECK( pNote->is_in_tuplet() == false );
        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_TupletsBuilder_TupletOk)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        //expected << "Line 0. No 'end' element for beam number 14. Tuplet removed." << endl;
        SpLdpTree tree = parser.parse_text("(musicData (n c4 e (t + 3)) (n e4 e) (n d4 e (t -)))");
        Analyser* pA = new Analyser(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = pA->analyse_tree(tree);

        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        //cout << tree->get_root()->to_string() << endl;
        CHECK( errormsg.str() == expected.str() );
        CHECK( tree->get_root()->to_string() == "(musicData (n c4 e (t + 3)) (n e4 e) (n d4 e (t -)))" );

        std::list<ImoTuplet*>& tuplets = pBasicModel->get_tuplets();
        CHECK( tuplets.size() == 1 );
        ImoTuplet* pTuplet = tuplets.front();
        CHECK( pTuplet->is_tuplet() == true );
        std::list<ImoStaffObj*>& notes = pTuplet->get_related_objects();
        CHECK( notes.size() == 3 );

        delete pA;

        ImoMusicData* pMusic = dynamic_cast<ImoMusicData*>( pBasicModel->get_root() );
        CHECK( pMusic != NULL );

        ImoObj::children_iterator it = pMusic->begin();

        ImoNote* pNote1 = dynamic_cast<ImoNote*>( *it );
        CHECK( pNote1 != NULL );
        CHECK( pNote1->is_in_tuplet() == true );

        ++it;
        ImoNote* pNote2 = dynamic_cast<ImoNote*>( *it );
        CHECK( pNote2 != NULL );
        CHECK( pNote2->is_in_tuplet() == true );

        ++it;
        ImoNote* pNote3 = dynamic_cast<ImoNote*>( *it );
        CHECK( pNote3 != NULL );
        CHECK( pNote3->is_in_tuplet() == true );

        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_TupletsBuilder_InstrumentChangeError)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        expected << "Line 0. No 'end' element for tuplet. Tuplet removed." << endl;
        SpLdpTree tree = parser.parse_text("(score (vers 1.6) (instrument (musicData (n c4 e (t + 3)))) (instrument (musicData (n c4 e))))" );
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        //cout << tree->get_root()->to_string() << endl;
        CHECK( errormsg.str() == expected.str() );

        ImoScore* pScore = dynamic_cast<ImoScore*>( pBasicModel->get_root() );
        ImoInstrument* pInstr = pScore->get_instrument(0);
        ImoMusicData* pMusic = pInstr->get_musicdata();
        CHECK( pMusic != NULL );

        ImoObj::children_iterator it = pMusic->begin();
        ImoNote* pNote = dynamic_cast<ImoNote*>(*it);
        CHECK( pNote->get_octave() == 4 );
        CHECK( pNote->get_step() == 0 );
        CHECK( pNote->is_in_tuplet() == false );

        delete tree->get_root();
        delete pBasicModel;
    }


    // tuplet (old tn/t- syntax) -----------------------------------------------

    TEST_FIXTURE(AnalyserTestFixture, Analyser_TupletOld_ErrorInvalidParameter)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        expected << "Line 0. Invalid parameter 't-7'. Removed." << endl;
        SpLdpTree tree = parser.parse_text("(n c4 e t-7)");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        //cout << tree->get_root()->to_string() << endl;
        CHECK( errormsg.str() == expected.str() );
        CHECK( tree->get_root()->to_string() == "(n c4 e)" );

        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_TupletOld_ok)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        //expected << "Line 0. " << endl;
        SpLdpTree tree = parser.parse_text("(n c4 e t3)");
        Analyser* pA = new Analyser(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = pA->analyse_tree(tree);
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        //cout << tree->get_root()->to_string() << endl;
        CHECK( errormsg.str() == expected.str() );
        CHECK( tree->get_root()->to_string() == "(n c4 e t3)" );

        delete pA;
        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_TupletOld_ok2)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        //expected << "Line 0. " << endl;
        SpLdpTree tree = parser.parse_text("(n c4 e t7/6)");
        Analyser* pA = new Analyser(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = pA->analyse_tree(tree);
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        //cout << tree->get_root()->to_string() << endl;
        CHECK( errormsg.str() == expected.str() );
        CHECK( tree->get_root()->to_string() == "(n c4 e t7/6)" );

        delete pA;
        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_TupletOld_ErrorAlreadyOpen)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        expected << "Line 0. Requesting to start a tuplet but there is already an open tuplet. Tuplet ignored." << endl;
        SpLdpTree tree = parser.parse_text("(musicData (n c4 s t3) (n d4 e) (n e4 e t3))");
        Analyser* pA = new Analyser(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = pA->analyse_tree(tree);

        std::list<ImoTuplet*>& tuplets = pBasicModel->get_tuplets();
        CHECK( tuplets.size() == 0 );
        //cout << "tuplets size = " << static_cast<int>(ts.size()) << endl;
        ImoMusicData* pMusic = dynamic_cast<ImoMusicData*>( pBasicModel->get_root() );
        CHECK( pMusic != NULL );
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        //cout << tree->get_root()->to_string() << endl;
        CHECK( errormsg.str() == expected.str() );
        CHECK( tree->get_root()->to_string() == "(musicData (n c4 s t3) (n d4 e) (n e4 e))" );

        delete pA;
        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_TupletOld_TupletOk)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        //expected << "Line 0. No 'end' element for beam number 14. Tuplet removed." << endl;
        SpLdpTree tree = parser.parse_text("(musicData (n c4 e t3) (n e4 e) (n d4 e t-))");
        Analyser* pA = new Analyser(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = pA->analyse_tree(tree);

        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        //cout << tree->get_root()->to_string() << endl;
        CHECK( errormsg.str() == expected.str() );
        CHECK( tree->get_root()->to_string() == "(musicData (n c4 e t3) (n e4 e) (n d4 e t-))" );

        std::list<ImoTuplet*>& tuplets = pBasicModel->get_tuplets();
        CHECK( tuplets.size() == 1 );
        ImoTuplet* pTuplet = tuplets.front();
        std::list<ImoStaffObj*>& notes = pTuplet->get_related_objects();
        CHECK( notes.size() == 3 );

        delete pA;

        ImoMusicData* pMusic = dynamic_cast<ImoMusicData*>( pBasicModel->get_root() );
        CHECK( pMusic != NULL );

        ImoObj::children_iterator it = pMusic->begin();

        ImoNote* pNote1 = dynamic_cast<ImoNote*>( *it );
        CHECK( pNote1 != NULL );
        CHECK( pNote1->is_in_tuplet() == true );

        ++it;
        ImoNote* pNote2 = dynamic_cast<ImoNote*>( *it );
        CHECK( pNote2 != NULL );
        CHECK( pNote2->is_in_tuplet() == true );

        ++it;
        ImoNote* pNote3 = dynamic_cast<ImoNote*>( *it );
        CHECK( pNote3 != NULL );
        CHECK( pNote3->is_in_tuplet() == true );

        delete tree->get_root();
        delete pBasicModel;
    }

    // voice (element) ----------------------------------------------------------

    TEST_FIXTURE(AnalyserTestFixture, Analyser_Note_Voice_Ok)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        //expected << "Line 0. ?" << endl;
        SpLdpTree tree = parser.parse_text("(n c4 e (voice 7))");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        ImoNote* pNote = dynamic_cast<ImoNote*>( pBasicModel->get_root() );
        CHECK( pNote != NULL );
        CHECK( pNote->get_voice() == 7 );
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        //cout << tree->get_root()->to_string() << endl;
        CHECK( errormsg.str() == expected.str() );
        CHECK( tree->get_root()->to_string() == "(n c4 e (voice 7))" );
        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_Note_Voice_Error)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        expected << "Line 0. Invalid integer number 'no'. Replaced by '1'." << endl;
        SpLdpTree tree = parser.parse_text("(n c4 e (voice no))");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        ImoNote* pNote = dynamic_cast<ImoNote*>( pBasicModel->get_root() );
        CHECK( pNote != NULL );
        CHECK( pNote->get_voice() == 1 );
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        //cout << tree->get_root()->to_string() << endl;
        CHECK( errormsg.str() == expected.str() );
        CHECK( tree->get_root()->to_string() == "(n c4 e (voice 1))" );
        delete tree->get_root();
        delete pBasicModel;
    }

    // staffNum (element) -------------------------------------------------------

    TEST_FIXTURE(AnalyserTestFixture, Analyser_Note_StaffNum_Ok)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        //expected << "Line 0. ?" << endl;
        SpLdpTree tree = parser.parse_text("(n c4 e (staffNum 2))");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        ImoNote* pNote = dynamic_cast<ImoNote*>( pBasicModel->get_root() );
        CHECK( pNote != NULL );
        CHECK( pNote->get_staff() == 1 );
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        //cout << tree->get_root()->to_string() << endl;
        CHECK( errormsg.str() == expected.str() );
        CHECK( tree->get_root()->to_string() == "(n c4 e (staffNum 2))" );
        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_Note_StaffNum_Error)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        expected << "Line 0. Invalid integer number 'alpha'. Replaced by '1'." << endl;
        SpLdpTree tree = parser.parse_text("(n c4 e (staffNum alpha))");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        ImoNote* pNote = dynamic_cast<ImoNote*>( pBasicModel->get_root() );
        CHECK( pNote != NULL );
        CHECK( pNote->get_staff() == 0 );
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        //cout << tree->get_root()->to_string() << endl;
        CHECK( errormsg.str() == expected.str() );
        CHECK( tree->get_root()->to_string() == "(n c4 e (staffNum 1))" );
        delete tree->get_root();
        delete pBasicModel;
    }

    // rest (full) -------------------------------------------------------------

    TEST_FIXTURE(AnalyserTestFixture, Analyser_Rest_Full)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        //expected << "Line 0. No 'end' element for beam number 14. Tuplet removed." << endl;
        SpLdpTree tree = parser.parse_text("(musicData (r e (t + 3)(voice 3)(staffNum 2)) (r e (text \"Hello\")) (r e (t -)))");
        Analyser* pA = new Analyser(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = pA->analyse_tree(tree);

        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        //cout << tree->get_root()->to_string() << endl;
        CHECK( errormsg.str() == expected.str() );
        CHECK( tree->get_root()->to_string() == "(musicData (r e (t + 3) (voice 3) (staffNum 2)) (r e (text \"Hello\")) (r e (t -)))" );

        std::list<ImoTuplet*>& tuplets = pBasicModel->get_tuplets();
        CHECK( tuplets.size() == 1 );
        ImoTuplet* pTuplet = tuplets.front();
        std::list<ImoStaffObj*>& notes = pTuplet->get_related_objects();
        CHECK( notes.size() == 3 );

        delete pA;

        ImoMusicData* pMusic = dynamic_cast<ImoMusicData*>( pBasicModel->get_root() );
        CHECK( pMusic != NULL );

        ImoObj::children_iterator it = pMusic->begin();

        CHECK( (*it)->is_rest() == true );
        ImoRest* pRest1 = dynamic_cast<ImoRest*>( *it );
        CHECK( pRest1 != NULL );
        CHECK( pRest1->is_in_tuplet() == true );
        CHECK( pRest1->get_voice() == 3 );
        CHECK( pRest1->get_staff() == 1 );

        ++it;
        ImoRest* pRest2 = dynamic_cast<ImoRest*>( *it );
        CHECK( pRest2 != NULL );
        CHECK( pRest2->is_in_tuplet() == true );
        CHECK( pRest2->get_voice() == 3 );
        CHECK( pRest2->get_staff() == 1 );
        CHECK( pRest2->has_attachments() == true );

        ++it;
        ImoRest* pRest3 = dynamic_cast<ImoRest*>( *it );
        CHECK( pRest3 != NULL );
        CHECK( pRest3->is_in_tuplet() == true );
        CHECK( pRest3->get_voice() == 3 );
        CHECK( pRest3->get_staff() == 1 );

        delete tree->get_root();
        delete pBasicModel;
    }

    // color -------------------------------------------------------------------

    TEST_FIXTURE(AnalyserTestFixture, ImoColorInfo)
    {
        ImoColorInfo color;
        CHECK( color.red() == 0 );
        CHECK( color.green() == 0 );
        CHECK( color.blue() == 0 );
        CHECK( color.alpha() == 255 );
        color.get_from_rgb_string("#ff3217");
        CHECK( color.red() == 255 );
        CHECK( color.green() == 50 );
        CHECK( color.blue() == 23 );
        CHECK( color.alpha() == 255 );
        color.get_from_rgba_string("#fffe4580");
        CHECK( color.red() == 255 );
        CHECK( color.green() == 254 );
        CHECK( color.blue() == 69 );
        CHECK( color.alpha() == 128 );
        CHECK( color.is_color_info() == true );
    }

    TEST_FIXTURE(AnalyserTestFixture, ImoColor_Constructor)
    {
        ImoColorInfo color(12, 32, 255, 180);
        CHECK( color.red() == 12 );
        CHECK( color.green() == 32 );
        CHECK( color.blue() == 255 );
        CHECK( color.alpha() == 180 );
    }

    TEST_FIXTURE(AnalyserTestFixture, ImoColor_Error)
    {
        ImoColorInfo color;
        color.get_from_string("#ff3g17");
        CHECK( color.red() == 0 );
        CHECK( color.green() == 0 );
        CHECK( color.blue() == 0 );
        CHECK( color.alpha() == 255 );
        color.get_from_string("#fff");
        CHECK( color.red() == 0 );
        CHECK( color.green() == 0 );
        CHECK( color.blue() == 0 );
        CHECK( color.alpha() == 255 );
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_Color_ErrorInvalidData)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        expected << "Line 0. Missing or invalid color value. Must be #rrggbbaa. Color ignored." << endl;
        SpLdpTree tree = parser.parse_text("(color 321700)");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        CHECK( pBasicModel->get_root() == NULL );
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_Color_Ok)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        //expected << "Line 0. " << endl;
        SpLdpTree tree = parser.parse_text("(color #f0457f)");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        ImoColorInfo* pColor = dynamic_cast<ImoColorInfo*>( pBasicModel->get_root() );
        CHECK( pColor != NULL );
        CHECK( pColor->red() == 240 );
        CHECK( pColor->green() == 69 );
        CHECK( pColor->blue() == 127 );
        CHECK( pColor->alpha() == 255 );
        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        //cout << tree->get_root()->to_string() << endl;
        CHECK( errormsg.str() == expected.str() );
        CHECK( tree->get_root()->to_string() == "(color #f0457f)" );
        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_Color_SetInParent)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        //expected << "Line 0. " << endl;
        SpLdpTree tree = parser.parse_text("(n c4 e (color #f0457f))");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);

        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        //cout << tree->get_root()->to_string() << endl;
        CHECK( errormsg.str() == expected.str() );
        CHECK( tree->get_root()->to_string() == "(n c4 e (color #f0457f))" );

        ImoNote* pNote = dynamic_cast<ImoNote*>( pBasicModel->get_root() );
        CHECK( pNote != NULL );
        rgba16& color = pNote->get_color();
        CHECK( color.r == 240 );
        CHECK( color.g == 69 );
        CHECK( color.b == 127 );
        CHECK( color.a == 255 );

        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_Barline_Color)
    {
        LdpParser parser(cout, m_pLibraryScope->ldp_factory());
        SpLdpTree tree = parser.parse_text("(barline double (color #ff0000))");
        Analyser a(cout, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        ImoBarline* pBarline = dynamic_cast<ImoBarline*>( pBasicModel->get_root() );
        CHECK( pBarline != NULL );
        CHECK( pBarline->get_type() == ImoBarline::k_double );
        CHECK( pBarline->is_visible() );
        //CHECK( pBarline->get_color() == rgba16(255,0,0) );
        rgba16& color = pBarline->get_color();
        CHECK( color.r == 255 );
        CHECK( color.g == 0 );
        CHECK( color.b == 0 );
        CHECK( color.a == 255 );
        delete tree->get_root();
        delete pBasicModel;
    }

//    //// group ----------------------------------------------------------------------
//
//    //TEST_FIXTURE(AnalyserTestFixture, Analyser_Group_All)
//    //{
//    //    stringstream errormsg;
//    //    LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
//    //    stringstream expected;
//    //    //expected << "" << endl;
//    //    SpLdpTree tree = parser.parse_text("(group (name \"Group\")(abbrev \"G.\")"
//    //            "(symbol bracket)(joinBarlines no)"
//    //            "(instrument (name \"Soprano\")(abbrev \"S\")(musicData))"
//    //            "(instrument (name \"Tenor\")(abbrev \"T\") (musicData))"
//    //            "(instrument (name \"Bass\")(abbrev \"B\")(musicData)))");
//    //    Analyser a(errormsg, m_pLibraryScope->ldp_factory());
//    //    BasicModel* pBasicModel = a.analyse_tree(tree);
//
//    //    //cout << "[" << errormsg.str() << "]" << endl;
//    //    //cout << "[" << expected.str() << "]" << endl;
//    //    CHECK( errormsg.str() == expected.str() );
//
//    //    CHECK( pBasicModel->get_root()->is_instr_group() == true );
//    //    ImoInstrGroup* pGrp = dynamic_cast<ImoInstrGroup*>( pBasicModel->get_root() );
//    //    CHECK( pGrp != NULL );
//    //    CHECK( pGrp->get_name() == "Group" );
//    //    CHECK( pGrp->get_abbrev() == "G." );
//    //    CHECK( pGrp->join_barlines() == false );
//    //    CHECK( pGrp->get_symbol() == ImoInstrGroup::k_bracket );
//    //    CHECK( pGrp->get_num_instruments() == 3 );
//
//    //    delete tree->get_root();
//    //    delete pBasicModel;
//    //}
//
//    //TEST_FIXTURE(AnalyserTestFixture, Analyser_Group_NoName)
//    //{
//    //    stringstream errormsg;
//    //    LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
//    //    stringstream expected;
//    //    //expected << "" << endl;
//    //    SpLdpTree tree = parser.parse_text("(group (abbrev \"G.\")"
//    //            "(symbol bracket)(joinBarlines no)"
//    //            "(instrument (name \"Soprano\")(abbrev \"S\")(musicData))"
//    //            "(instrument (name \"Tenor\")(abbrev \"T\") (musicData))"
//    //            "(instrument (name \"Bass\")(abbrev \"B\")(musicData)))");
//    //    Analyser a(errormsg, m_pLibraryScope->ldp_factory());
//    //    BasicModel* pBasicModel = a.analyse_tree(tree);
//
//    //    //cout << "[" << errormsg.str() << "]" << endl;
//    //    //cout << "[" << expected.str() << "]" << endl;
//    //    CHECK( errormsg.str() == expected.str() );
//
//    //    ImoInstrGroup* pGrp = dynamic_cast<ImoInstrGroup*>( pBasicModel->get_root() );
//    //    CHECK( pGrp != NULL );
//    //    CHECK( pGrp->get_name() == "" );
//    //    CHECK( pGrp->get_abbrev() == "G." );
//    //    CHECK( pGrp->join_barlines() == false );
//    //    CHECK( pGrp->get_symbol() == ImoInstrGroup::k_bracket );
//    //    CHECK( pGrp->get_num_instruments() == 3 );
//
//    //    delete tree->get_root();
//    //    delete pBasicModel;
//    //}
//
//    //TEST_FIXTURE(AnalyserTestFixture, Analyser_Group_NoAbbrev)
//    //{
//    //    stringstream errormsg;
//    //    LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
//    //    stringstream expected;
//    //    //expected << "" << endl;
//    //    SpLdpTree tree = parser.parse_text("(group (name \"Group\")"
//    //            "(symbol bracket)(joinBarlines no)"
//    //            "(instrument (name \"Soprano\")(abbrev \"S\")(musicData))"
//    //            "(instrument (name \"Tenor\")(abbrev \"T\") (musicData))"
//    //            "(instrument (name \"Bass\")(abbrev \"B\")(musicData)))");
//    //    Analyser a(errormsg, m_pLibraryScope->ldp_factory());
//    //    BasicModel* pBasicModel = a.analyse_tree(tree);
//
//    //    //cout << "[" << errormsg.str() << "]" << endl;
//    //    //cout << "[" << expected.str() << "]" << endl;
//    //    CHECK( errormsg.str() == expected.str() );
//
//    //    ImoInstrGroup* pGrp = dynamic_cast<ImoInstrGroup*>( pBasicModel->get_root() );
//    //    CHECK( pGrp != NULL );
//    //    CHECK( pGrp->get_name() == "Group" );
//    //    CHECK( pGrp->get_abbrev() == "" );
//    //    CHECK( pGrp->join_barlines() == false );
//    //    CHECK( pGrp->get_symbol() == ImoInstrGroup::k_bracket );
//    //    CHECK( pGrp->get_num_instruments() == 3 );
//
//    //    delete tree->get_root();
//    //    delete pBasicModel;
//    //}
//
//    //TEST_FIXTURE(AnalyserTestFixture, Analyser_Group_NoNameAbbrev)
//    //{
//    //    stringstream errormsg;
//    //    LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
//    //    stringstream expected;
//    //    //expected << "" << endl;
//    //    SpLdpTree tree = parser.parse_text("(group (symbol bracket)(joinBarlines no)"
//    //            "(instrument (name \"Soprano\")(abbrev \"S\")(musicData))"
//    //            "(instrument (name \"Tenor\")(abbrev \"T\") (musicData))"
//    //            "(instrument (name \"Bass\")(abbrev \"B\")(musicData)))");
//    //    Analyser a(errormsg, m_pLibraryScope->ldp_factory());
//    //    BasicModel* pBasicModel = a.analyse_tree(tree);
//
//    //    //cout << "[" << errormsg.str() << "]" << endl;
//    //    //cout << "[" << expected.str() << "]" << endl;
//    //    CHECK( errormsg.str() == expected.str() );
//
//    //    ImoInstrGroup* pGrp = dynamic_cast<ImoInstrGroup*>( pBasicModel->get_root() );
//    //    CHECK( pGrp != NULL );
//    //    CHECK( pGrp->get_name() == "" );
//    //    CHECK( pGrp->get_abbrev() == "" );
//    //    CHECK( pGrp->join_barlines() == false );
//    //    CHECK( pGrp->get_symbol() == ImoInstrGroup::k_bracket );
//    //    CHECK( pGrp->get_num_instruments() == 3 );
//
//    //    delete tree->get_root();
//    //    delete pBasicModel;
//    //}
//
//    //TEST_FIXTURE(AnalyserTestFixture, Analyser_Group_ErrorSymbol)
//    //{
//    //    stringstream errormsg;
//    //    LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
//    //    stringstream expected;
//    //    expected << "Line 0. Missing or invalid group symbol. Must be 'none', 'brace' or 'bracket'. Group removed." << endl;
//    //    SpLdpTree tree = parser.parse_text("(group (symbol good)(joinBarlines no)"
//    //            "(instrument (name \"Soprano\")(abbrev \"S\")(musicData))"
//    //            "(instrument (name \"Tenor\")(abbrev \"T\") (musicData))"
//    //            "(instrument (name \"Bass\")(abbrev \"B\")(musicData)))");
//    //    Analyser a(errormsg, m_pLibraryScope->ldp_factory());
//    //    BasicModel* pBasicModel = a.analyse_tree(tree);
//
//    //    //cout << "[" << errormsg.str() << "]" << endl;
//    //    //cout << "[" << expected.str() << "]" << endl;
//    //    CHECK( errormsg.str() == expected.str() );
//
//    //    CHECK( pBasicModel->get_root() == NULL );
//    //    delete pBasicModel;
//    //}
//
//    //TEST_FIXTURE(AnalyserTestFixture, Analyser_Group_ErrorJoin)
//    //{
//    //    stringstream errormsg;
//    //    LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
//    //    stringstream expected;
//    //    expected << "Line 0. Invalid boolean value 'perhaps'. Replaced by '1'." << endl;
//    //    SpLdpTree tree = parser.parse_text("(group (symbol brace)(joinBarlines perhaps)"
//    //            "(instrument (name \"Soprano\")(abbrev \"S\")(musicData))"
//    //            "(instrument (name \"Tenor\")(abbrev \"T\") (musicData))"
//    //            "(instrument (name \"Bass\")(abbrev \"B\")(musicData)))");
//    //    Analyser a(errormsg, m_pLibraryScope->ldp_factory());
//    //    BasicModel* pBasicModel = a.analyse_tree(tree);
//
//    //    //cout << "[" << errormsg.str() << "]" << endl;
//    //    //cout << "[" << expected.str() << "]" << endl;
//    //    CHECK( errormsg.str() == expected.str() );
//
//    //    ImoInstrGroup* pGrp = dynamic_cast<ImoInstrGroup*>( pBasicModel->get_root() );
//    //    CHECK( pGrp != NULL );
//    //    CHECK( pGrp->get_name() == "" );
//    //    CHECK( pGrp->get_abbrev() == "" );
//    //    CHECK( pGrp->join_barlines() == true );
//    //    CHECK( pGrp->get_symbol() == ImoInstrGroup::k_brace );
//    //    CHECK( pGrp->get_num_instruments() == 3 );
//
//    //    delete tree->get_root();
//    //    delete pBasicModel;
//    //}
//
//    //TEST_FIXTURE(AnalyserTestFixture, Analyser_Group_ErrorMissingInstruments)
//    //{
//    //    stringstream errormsg;
//    //    LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
//    //    stringstream expected;
//    //    expected << "Line 0. Missing instruments in group!. Group removed." << endl;
//    //    SpLdpTree tree = parser.parse_text("(group (symbol brace)(joinBarlines true))");
//    //    Analyser a(errormsg, m_pLibraryScope->ldp_factory());
//    //    BasicModel* pBasicModel = a.analyse_tree(tree);
//
//    //    //cout << "[" << errormsg.str() << "]" << endl;
//    //    //cout << "[" << expected.str() << "]" << endl;
//    //    CHECK( errormsg.str() == expected.str() );
//
//    //    CHECK( pBasicModel->get_root() == NULL );
//    //    delete pBasicModel;
//    //}
//
//    //TEST_FIXTURE(AnalyserTestFixture, Analyser_Group_ErrorInInstrument)
//    //{
//    //    stringstream errormsg;
//    //    LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
//    //    stringstream expected;
//    //    expected << "Line 0. Element 'n' unknown or not possible here. Removed." << endl;
//    //    SpLdpTree tree = parser.parse_text("(group (symbol brace)(joinBarlines no)"
//    //            "(instrument (name \"Soprano\")(abbrev \"S\")(musicData))"
//    //            "(n c4 q)"
//    //            "(instrument (name \"Bass\")(abbrev \"B\")(musicData)))");
//    //    Analyser a(errormsg, m_pLibraryScope->ldp_factory());
//    //    BasicModel* pBasicModel = a.analyse_tree(tree);
//
//    //    //cout << "[" << errormsg.str() << "]" << endl;
//    //    //cout << "[" << expected.str() << "]" << endl;
//    //    CHECK( errormsg.str() == expected.str() );
//
//    //    ImoInstrGroup* pGrp = dynamic_cast<ImoInstrGroup*>( pBasicModel->get_root() );
//    //    CHECK( pGrp != NULL );
//    //    CHECK( pGrp->get_name() == "" );
//    //    CHECK( pGrp->get_abbrev() == "" );
//    //    CHECK( pGrp->join_barlines() == false );
//    //    CHECK( pGrp->get_symbol() == ImoInstrGroup::k_brace );
//    //    CHECK( pGrp->get_num_instruments() == 2 );
//
//    //    delete tree->get_root();
//    //    delete pBasicModel;
//    //}
//
    // chord ----------------------------------------------------------------------

    TEST_FIXTURE(AnalyserTestFixture, Analyser_Chord_Ok)
    {
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLibraryScope->ldp_factory());
        stringstream expected;
        //expected << "" << endl;
        SpLdpTree tree = parser.parse_text("(chord (n c4 q)(n e4 q)(n g4 q))");
        Analyser a(errormsg, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);

        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );

        ImoMusicData* pMusic = dynamic_cast<ImoMusicData*>( pBasicModel->get_root() );
        CHECK( pMusic != NULL );

        ImoObj::children_iterator it = pMusic->begin();
        CHECK( pMusic->get_num_children() == 3 );

        ImoNote* pNote = dynamic_cast<ImoNote*>( *it );
        CHECK( pNote != NULL );
        CHECK( pNote->is_in_chord() == false );

        ++it;
        pNote = dynamic_cast<ImoNote*>( *it );
        CHECK( pNote != NULL );

        ++it;
        pNote = dynamic_cast<ImoNote*>( *it );
        CHECK( pNote != NULL );

        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(AnalyserTestFixture, Analyser_CreateNote)
    {
        LdpParser parser(cout, m_pLibraryScope->ldp_factory());
        SpLdpTree tree = parser.parse_text("(n e4 q.)");
        Analyser a(cout, m_pLibraryScope->ldp_factory());
        BasicModel* pBasicModel = a.analyse_tree(tree);
        ImoObj* pObj = pBasicModel->get_root();
        CHECK( pObj != NULL );
        CHECK( pObj->is_note() == true );
        delete tree->get_root();
        delete pBasicModel;
    }


}

#endif  // _LM_DEBUG_

