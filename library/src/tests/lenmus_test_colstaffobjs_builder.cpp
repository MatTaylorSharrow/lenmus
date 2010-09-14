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
#include "lenmus_config.h"

//classes related to these tests
#include "lenmus_injectors.h"
#include "lenmus_staffobjs_table.h"
#include "lenmus_parser.h"
#include "lenmus_analyser.h"
#include "lenmus_internal_model.h"
#include "lenmus_im_note.h"
#include "lenmus_document.h"
#include "lenmus_document_iterator.h"
#include "lenmus_score_iterator.h"
#include "lenmus_basic_model.h"
#include "lenmus_model_builder.h"

using namespace UnitTest;
using namespace std;
using namespace lenmus;


class StaffVoiceLineTableTestFixture
{
public:

    StaffVoiceLineTableTestFixture()     //SetUp fixture
    {
    }

    ~StaffVoiceLineTableTestFixture()    //TearDown fixture
    {
    }
};

SUITE(StaffVoiceLineTableTest)
{

    TEST_FIXTURE(StaffVoiceLineTableTestFixture, StaffVoiceLineTable_Voice0FirstStaff)
    {
        StaffVoiceLineTable table;
        CHECK( table.get_line_assigned_to(0, 0) == 0 );
    }

    TEST_FIXTURE(StaffVoiceLineTableTestFixture, StaffVoiceLineTable_Voice0Staff1)
    {
        StaffVoiceLineTable table;
        //cout << table.get_line_assigned_to(0, 1) << endl;
        CHECK( table.get_line_assigned_to(0, 1) == 1 );
    }

    TEST_FIXTURE(StaffVoiceLineTableTestFixture, StaffVoiceLineTable_FirstVoiceAssignedToDefault)
    {
        StaffVoiceLineTable table;
        table.get_line_assigned_to(0, 1);
        CHECK( table.get_line_assigned_to(3, 1) == 1 );
    }

}

class ColStaffObjsTestFixture
{
public:

    ColStaffObjsTestFixture()     //SetUp fixture
    {
        m_scores_path = "../../../../test-scores/";
        m_pLibraryScope = new LibraryScope(cout);
        m_pLdpFactory = m_pLibraryScope->ldp_factory();
    }

    ~ColStaffObjsTestFixture()    //TearDown fixture
    {
        delete m_pLibraryScope;
    }

    LibraryScope* m_pLibraryScope;
    LdpFactory* m_pLdpFactory;
    std::string m_scores_path;
};

SUITE(ColStaffObjsTest)
{

    TEST_FIXTURE(ColStaffObjsTestFixture, ColStaffObjsAddEntries)
    {
        LdpParser parser(cout, m_pLdpFactory);
        SpLdpTree tree = parser.parse_text("(lenmusdoc (vers 0.0) (content (score (vers 1.6) (instrument (musicData (n c4 q) (barline simple))))))" );
        Analyser a(cout, m_pLdpFactory);
        BasicModel* pBasicModel = a.analyse_tree(tree);
        ImObjectsBuilder imb(cout);
        ImoDocument* pDoc = imb.create_objects(pBasicModel);
        ImoScore* pScore = dynamic_cast<ImoScore*>( pDoc->get_content_item(0) );
        ColStaffObjsBuilder builder(tree);
        ColStaffObjs* pColStaffObjs = builder.build(pScore, false);    //false: only creation, no sort
        CHECK( pColStaffObjs->num_entries() == 2 );
        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(ColStaffObjsTestFixture, ScoreIteratorPointsFirst)
    {
        LdpParser parser(cout, m_pLdpFactory);
        SpLdpTree tree = parser.parse_text("(lenmusdoc (vers 0.0) (content (score (vers 1.6) (instrument (musicData (n c4 q) (barline simple))))))" );
        Analyser a(cout, m_pLdpFactory);
        BasicModel* pBasicModel = a.analyse_tree(tree);
        ImoDocument* pDoc = dynamic_cast<ImoDocument*>( pBasicModel->get_root() );
        ImoScore* pScore = dynamic_cast<ImoScore*>( pDoc->get_content_item(0) );
        ColStaffObjsBuilder builder(tree);
        ColStaffObjs* pColStaffObjs = builder.build(pScore, false);    //false: only creation, no sort
        ColStaffObjs::iterator it = pColStaffObjs->begin();
        //(*it)->dump();
        //cout << (*it)->to_string() << endl;
        //CHECK( (*it)->to_string() == "(n c4 q)" );
        CHECK( (*it)->segment() == 0 );
        CHECK( (*it)->time() == 0.0f );
        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(ColStaffObjsTestFixture, ColStaffObjsChangeSegment)
    {
        LdpParser parser(cout, m_pLdpFactory);
        SpLdpTree tree = parser.parse_text("(lenmusdoc (vers 0.0) (content (score (vers 1.6) (instrument (musicData (n c4 q) (barline simple) (n d4 e) (barline simple) (n e4 w))))))" );
        Analyser a(cout, m_pLdpFactory);
        BasicModel* pBasicModel = a.analyse_tree(tree);
        ImoDocument* pDoc = dynamic_cast<ImoDocument*>( pBasicModel->get_root() );
        ImoScore* pScore = dynamic_cast<ImoScore*>( pDoc->get_content_item(0) );
        ColStaffObjsBuilder builder(tree);
        ColStaffObjs* pColStaffObjs = builder.build(pScore, false);    //false: only creation, no sort
        ColStaffObjs::iterator it = pColStaffObjs->begin();
        //(*it)->dump();
        CHECK( pColStaffObjs->num_entries() == 5 );
        //CHECK( (*it)->to_string() == "(n c4 q)" );
        CHECK( (*it)->segment() == 0 );
        ++it;
        //CHECK( (*it)->to_string() == "(barline simple)" );
        CHECK( (*it)->segment() == 0 );
        ++it;
        //CHECK( (*it)->to_string() == "(n d4 e)" );
        CHECK( (*it)->segment() == 1 );
        ++it;
        //CHECK( (*it)->to_string() == "(barline simple)" );
        CHECK( (*it)->segment() == 1 );
        ++it;
        //CHECK( (*it)->to_string() == "(n e4 w)" );
        CHECK( (*it)->segment() == 2 );
        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(ColStaffObjsTestFixture, ColStaffObjsTimeInSequence)
    {
        LdpParser parser(cout, m_pLdpFactory);
        SpLdpTree tree = parser.parse_text("(lenmusdoc (vers 0.0) (content (score (vers 1.6) (instrument (musicData (n c4 q)(n d4 e.)(n d4 s)(n e4 h)))) ))" );
        Analyser a(cout, m_pLdpFactory);
        BasicModel* pBasicModel = a.analyse_tree(tree);
        ImoDocument* pDoc = dynamic_cast<ImoDocument*>( pBasicModel->get_root() );
        ImoScore* pScore = dynamic_cast<ImoScore*>( pDoc->get_content_item(0) );
        ColStaffObjsBuilder builder(tree);
        ColStaffObjs* pColStaffObjs = builder.build(pScore, false);    //false: only creation, no sort
        ColStaffObjs::iterator it = pColStaffObjs->begin();
        //pColStaffObjs->dump();
        CHECK( pColStaffObjs->num_entries() == 4 );
        //CHECK( (*it)->to_string() == "(n c4 q)" );
        CHECK( (*it)->segment() == 0 );
        CHECK( (*it)->time() == 0.0f );
        ++it;
        //CHECK( (*it)->to_string() == "(n d4 e.)" );
        CHECK( (*it)->segment() == 0 );
        CHECK( (*it)->time() == 64.0f );
        ++it;
        //CHECK( (*it)->to_string() == "(n d4 s)" );
        CHECK( (*it)->segment() == 0 );
        CHECK( (*it)->time() == 112.0f );
        ++it;
        //CHECK( (*it)->to_string() == "(n e4 h)" );
        CHECK( (*it)->segment() == 0 );
        CHECK( (*it)->time() == 128.0f );
        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(ColStaffObjsTestFixture, ColStaffObjsTimeGoBack)
    {
        LdpParser parser(cout, m_pLdpFactory);
        SpLdpTree tree = parser.parse_text("(lenmusdoc (vers 0.0) (content (score (vers 1.6) (instrument (musicData (n c4 q)(n d4 e.)(n d4 s)(goBack start)(n e4 h)(n g4 q)))) ))" );
        Analyser a(cout, m_pLdpFactory);
        BasicModel* pBasicModel = a.analyse_tree(tree);
        ImoDocument* pDoc = dynamic_cast<ImoDocument*>( pBasicModel->get_root() );
        ImoScore* pScore = dynamic_cast<ImoScore*>( pDoc->get_content_item(0) );
        ColStaffObjsBuilder builder(tree);
        ColStaffObjs* pColStaffObjs = builder.build(pScore, false);    //false: only creation, no sort
        ColStaffObjs::iterator it = pColStaffObjs->begin();
        //pColStaffObjs->dump();
        CHECK( pColStaffObjs->num_entries() == 5 );
        //CHECK( (*it)->to_string() == "(n c4 q)" );
        CHECK( (*it)->segment() == 0 );
        CHECK( (*it)->time() == 0.0f );
        ++it;
        //CHECK( (*it)->to_string() == "(n d4 e.)" );
        CHECK( (*it)->segment() == 0 );
        CHECK( (*it)->time() == 64.0f );
        ++it;
        //CHECK( (*it)->to_string() == "(n d4 s)" );
        CHECK( (*it)->segment() == 0 );
        CHECK( (*it)->time() == 112.0f );
        ++it;
        //CHECK( (*it)->to_string() == "(n e4 h)" );
        CHECK( (*it)->segment() == 0 );
        CHECK( (*it)->time() == 0.0f );
        ++it;
        //CHECK( (*it)->to_string() == "(n g4 q)" );
        CHECK( (*it)->segment() == 0 );
        CHECK( (*it)->time() == 128.0f );
        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(ColStaffObjsTestFixture, ColStaffObjsTimeGoFwd)
    {
        LdpParser parser(cout, m_pLdpFactory);
        SpLdpTree tree = parser.parse_text("(lenmusdoc (vers 0.0) (content (score (vers 1.6) (instrument (musicData (n c4 q)(n d4 e.)(n d4 s)(goBack start)(n e4 q)(goFwd end)(barline)))) ))" );
        Analyser a(cout, m_pLdpFactory);
        BasicModel* pBasicModel = a.analyse_tree(tree);
        ImoDocument* pDoc = dynamic_cast<ImoDocument*>( pBasicModel->get_root() );
        ImoScore* pScore = dynamic_cast<ImoScore*>( pDoc->get_content_item(0) );
        ColStaffObjsBuilder builder(tree);
        ColStaffObjs* pColStaffObjs = builder.build(pScore, false);    //false: only creation, no sort
        ColStaffObjs::iterator it = pColStaffObjs->begin();
        //pColStaffObjs->dump();
        CHECK( pColStaffObjs->num_entries() == 5 );
        //CHECK( (*it)->to_string() == "(n c4 q)" );
        CHECK( (*it)->time() == 0.0f );
        ++it;
        //CHECK( (*it)->to_string() == "(n d4 e.)" );
        CHECK( (*it)->time() == 64.0f );
        ++it;
        //CHECK( (*it)->to_string() == "(n d4 s)" );
        CHECK( (*it)->time() == 112.0f );
        ++it;
        //CHECK( (*it)->to_string() == "(n e4 q)" );
        CHECK( (*it)->time() == 0.0f );
        ++it;
        //CHECK( (*it)->to_string() == "(barline )" );
        CHECK( (*it)->time() == 128.0f );
        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(ColStaffObjsTestFixture, ColStaffObjsStaffAssigned)
    {
        LdpParser parser(cout, m_pLdpFactory);
        SpLdpTree tree = parser.parse_text("(lenmusdoc (vers 0.0) (content (score (vers 1.6) (instrument (musicData (n c4 q p2)(n d4 e.)(n d4 s p3)(n e4 h)))) ))" );
        Analyser a(cout, m_pLdpFactory);
        BasicModel* pBasicModel = a.analyse_tree(tree);
        ImoDocument* pDoc = dynamic_cast<ImoDocument*>( pBasicModel->get_root() );
        ImoScore* pScore = dynamic_cast<ImoScore*>( pDoc->get_content_item(0) );
        ColStaffObjsBuilder builder(tree);
        ColStaffObjs* pColStaffObjs = builder.build(pScore, false);    //false: only creation, no sort
        ColStaffObjs::iterator it = pColStaffObjs->begin();
        //pColStaffObjs->dump();
        CHECK( pColStaffObjs->num_entries() == 4 );
        //CHECK( (*it)->to_string() == "(n c4 q p2)" );
        CHECK( (*it)->staff() == 1 );
        ++it;
        //CHECK( (*it)->to_string() == "(n d4 e.)" );
        CHECK( (*it)->staff() == 1 );
        ++it;
        //CHECK( (*it)->to_string() == "(n d4 s p3)" );
        CHECK( (*it)->staff() == 2 );
        ++it;
        //CHECK( (*it)->to_string() == "(n e4 h)" );
        CHECK( (*it)->staff() == 2 );
        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(ColStaffObjsTestFixture, ColStaffObjsLineAssigned)
    {
        LdpParser parser(cout, m_pLdpFactory);
        SpLdpTree tree = parser.parse_text("(lenmusdoc (vers 0.0) (content (score (vers 1.6) (instrument (musicData (n c4 q v1)(n d4 e.)(n d4 s v3)(n e4 h)))) ))" );
        Analyser a(cout, m_pLdpFactory);
        BasicModel* pBasicModel = a.analyse_tree(tree);
        ImoDocument* pDoc = dynamic_cast<ImoDocument*>( pBasicModel->get_root() );
        ImoScore* pScore = dynamic_cast<ImoScore*>( pDoc->get_content_item(0) );
        ColStaffObjsBuilder builder(tree);
        ColStaffObjs* pColStaffObjs = builder.build(pScore, false);    //false: only creation, no sort
        ColStaffObjs::iterator it = pColStaffObjs->begin();
        //pColStaffObjs->dump();
        CHECK( pColStaffObjs->num_entries() == 4 );
        //CHECK( (*it)->to_string() == "(n c4 q v1)" );
        CHECK( (*it)->line() == 0 );
        ++it;
        //CHECK( (*it)->to_string() == "(n d4 e.)" );
        CHECK( (*it)->line() == 0 );
        ++it;
        //CHECK( (*it)->to_string() == "(n d4 s v3)" );
        CHECK( (*it)->line() == 1 );
        ++it;
        //CHECK( (*it)->to_string() == "(n e4 h)" );
        CHECK( (*it)->line() == 1 );
        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(ColStaffObjsTestFixture, ColStaffObjsAssigLineToClef)
    {
        LdpParser parser(cout, m_pLdpFactory);
        SpLdpTree tree = parser.parse_text("(lenmusdoc (vers 0.0) (content (score (vers 1.6) (instrument (musicData (clef G)(n c4 q v2)(n d4 e.)(n d4 s v3)(n e4 h)))) ))" );
        Analyser a(cout, m_pLdpFactory);
        BasicModel* pBasicModel = a.analyse_tree(tree);
        ImoDocument* pDoc = dynamic_cast<ImoDocument*>( pBasicModel->get_root() );
        ImoScore* pScore = dynamic_cast<ImoScore*>( pDoc->get_content_item(0) );
        ColStaffObjsBuilder builder(tree);
        ColStaffObjs* pColStaffObjs = builder.build(pScore, false);    //false: only creation, no sort
        ColStaffObjs::iterator it = pColStaffObjs->begin();
        //pColStaffObjs->dump();
        CHECK( pColStaffObjs->num_entries() == 5 );
        //CHECK( (*it)->to_string() == "(clef G)" );
        CHECK( (*it)->line() == 0 );
        ++it;
        //CHECK( (*it)->to_string() == "(n c4 q v2)" );
        CHECK( (*it)->line() == 0 );
        ++it;
        //CHECK( (*it)->to_string() == "(n d4 e.)" );
        CHECK( (*it)->line() == 0 );
        ++it;
        //CHECK( (*it)->to_string() == "(n d4 s v3)" );
        CHECK( (*it)->line() == 1 );
        ++it;
        //CHECK( (*it)->to_string() == "(n e4 h)" );
        CHECK( (*it)->line() == 1 );
        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(ColStaffObjsTestFixture, ColStaffObjsAssigLineToKey)
    {
        LdpParser parser(cout, m_pLdpFactory);
        SpLdpTree tree = parser.parse_text("(lenmusdoc (vers 0.0) (content (score (vers 1.6) (instrument (staves 2)(musicData (clef G p1)(clef F4 p2)(key D)(n c4 q v2 p1)(n d4 e.)(n d4 s v3 p2)(n e4 h)))) ))" );
        Analyser a(cout, m_pLdpFactory);
        BasicModel* pBasicModel = a.analyse_tree(tree);
        ImoDocument* pDoc = dynamic_cast<ImoDocument*>( pBasicModel->get_root() );
        ImoScore* pScore = dynamic_cast<ImoScore*>( pDoc->get_content_item(0) );
        ColStaffObjsBuilder builder(tree);
        ColStaffObjs* pColStaffObjs = builder.build(pScore, false);    //false: only creation, no sort
        ColStaffObjs::iterator it = pColStaffObjs->begin();
        //pColStaffObjs->dump();
        CHECK( pColStaffObjs->num_entries() == 8 );
        //CHECK( (*it)->to_string() == "(clef G p1)" );
        CHECK( (*it)->line() == 0 );
        ++it;
        //CHECK( (*it)->to_string() == "(clef F4 p2)" );
        CHECK( (*it)->line() == 1 );
        ++it;
        //CHECK( (*it)->to_string() == "(key D)" );
        CHECK( (*it)->line() == 0 );
        ++it;
        //CHECK( (*it)->to_string() == "(key D)" );
        CHECK( (*it)->line() == 1 );
        ++it;
        //CHECK( (*it)->to_string() == "(n c4 q v2 p1)" );
        CHECK( (*it)->line() == 0 );
        ++it;
        //CHECK( (*it)->to_string() == "(n d4 e.)" );
        CHECK( (*it)->line() == 0 );
        ++it;
        //CHECK( (*it)->to_string() == "(n d4 s v3 p2)" );
        CHECK( (*it)->line() == 1 );
        ++it;
        //CHECK( (*it)->to_string() == "(n e4 h)" );
        CHECK( (*it)->line() == 1 );
        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(ColStaffObjsTestFixture, ColStaffObjsFullExample)
    {
        LdpParser parser(cout, m_pLdpFactory);
        SpLdpTree tree = parser.parse_text("(lenmusdoc (vers 0.0) (content (score (vers 1.6)"
                        "(instrument (staves 2)(musicData (clef G p1)(clef F4 p2)"
                        "(key D)(time 2 4)(n f4 w p1)(goBack w)(n c3 e g+ p2)"
                        "(n c3 e g-)(n d3 q)(barline)))"
                        "(instrument (staves 2)(musicData (clef G p1)(clef F4 p2)"
                        "(key D)(time 2 4)(n f4 q. p1)(clef F4 p1)(n a3 e)"
                        "(goBack h)(n c3 q p2)(n c3 e)(clef G p2)(clef F4 p2)"
                        "(n c3 e)(barline)))  )))" );
        Analyser a(cout, m_pLdpFactory);
        BasicModel* pBasicModel = a.analyse_tree(tree);
        ImoDocument* pDoc = dynamic_cast<ImoDocument*>( pBasicModel->get_root() );
        ImoScore* pScore = dynamic_cast<ImoScore*>( pDoc->get_content_item(0) );
        ColStaffObjsBuilder builder(tree);
        ColStaffObjs* pColStaffObjs = builder.build(pScore);
        ColStaffObjs::iterator it = pColStaffObjs->begin();
        //pColStaffObjs->dump();
        CHECK( pColStaffObjs->num_entries() == 24 );
        //CHECK( (*it)->to_string() == "(clef G p1)" );
        CHECK( (*it)->num_instrument() == 0 );
        CHECK( (*it)->time() == 0.0f );
        CHECK( (*it)->line() == 0 );
        CHECK( (*it)->staff() == 0 );
        ++it;
        //CHECK( (*it)->to_string() == "(key D)" );
        CHECK( (*it)->num_instrument() == 0 );
        CHECK( (*it)->time() == 0.0f );
        CHECK( (*it)->line() == 0 );
        CHECK( (*it)->staff() == 0 );
        ++it;
        //CHECK( (*it)->to_string() == "(time 2 4)" );
        CHECK( (*it)->num_instrument() == 0 );
        CHECK( (*it)->time() == 0.0f );
        CHECK( (*it)->line() == 0 );
        CHECK( (*it)->staff() == 0 );
        ++it;
        //CHECK( (*it)->to_string() == "(n f4 w p1)" );
        CHECK( (*it)->num_instrument() == 0 );
        CHECK( (*it)->time() == 0.0f );
        CHECK( (*it)->line() == 0 );
        CHECK( (*it)->staff() == 0 );
        ++it;
        //CHECK( (*it)->to_string() == "(clef F4 p2)" );
        CHECK( (*it)->num_instrument() == 0 );
        CHECK( (*it)->time() == 0.0f );
        CHECK( (*it)->line() == 1 );
        CHECK( (*it)->staff() == 1 );
        ++it;
        //CHECK( (*it)->to_string() == "(key D)" );
        CHECK( (*it)->num_instrument() == 0 );
        CHECK( (*it)->time() == 0.0f );
        CHECK( (*it)->line() == 1 );
        CHECK( (*it)->staff() == 1 );
        ++it;
        //CHECK( (*it)->to_string() == "(n c3 e g+ p2)" );
        CHECK( (*it)->num_instrument() == 0 );
        CHECK( (*it)->time() == 0.0f );
        CHECK( (*it)->line() == 1 );
        CHECK( (*it)->staff() == 1 );
        ++it;
        //CHECK( (*it)->to_string() == "(clef G p1)" );
        CHECK( (*it)->num_instrument() == 1 );
        CHECK( (*it)->time() == 0.0f );
        CHECK( (*it)->line() == 2 );
        CHECK( (*it)->staff() == 0 );
        ++it;
        //CHECK( (*it)->to_string() == "(key D)" );
        CHECK( (*it)->num_instrument() == 1 );
        CHECK( (*it)->time() == 0.0f );
        CHECK( (*it)->line() == 2 );
        CHECK( (*it)->staff() == 0 );
        ++it;
        //CHECK( (*it)->to_string() == "(time 2 4)" );
        CHECK( (*it)->num_instrument() == 1 );
        CHECK( (*it)->time() == 0.0f );
        CHECK( (*it)->line() == 2 );
        CHECK( (*it)->staff() == 0 );
        ++it;
        //CHECK( (*it)->to_string() == "(n f4 q. p1)" );
        CHECK( (*it)->num_instrument() == 1 );
        CHECK( (*it)->time() == 0.0f );
        CHECK( (*it)->line() == 2 );
        CHECK( (*it)->staff() == 0 );
        ++it;
        //CHECK( (*it)->to_string() == "(clef F4 p2)" );
        CHECK( (*it)->num_instrument() == 1 );
        CHECK( (*it)->time() == 0.0f );
        CHECK( (*it)->line() == 3 );
        CHECK( (*it)->staff() == 1 );
        ++it;
        //CHECK( (*it)->to_string() == "(key D)" );
        CHECK( (*it)->num_instrument() == 1 );
        CHECK( (*it)->time() == 0.0f );
        CHECK( (*it)->line() == 3 );
        CHECK( (*it)->staff() == 1 );
        ++it;
        //CHECK( (*it)->to_string() == "(n c3 q p2)" );
        CHECK( (*it)->num_instrument() == 1 );
        CHECK( (*it)->time() == 0.0f );
        CHECK( (*it)->line() == 3 );
        CHECK( (*it)->staff() == 1 );
        ++it;
        //CHECK( (*it)->to_string() == "(n c3 e g-)" );
        CHECK( (*it)->num_instrument() == 0 );
        CHECK( (*it)->time() == 32.0f );
        CHECK( (*it)->line() == 1 );
        CHECK( (*it)->staff() == 1 );
        ++it;
        //CHECK( (*it)->to_string() == "(n d3 q)" );
        CHECK( (*it)->num_instrument() == 0 );
        CHECK( (*it)->time() == 64.0f );
        CHECK( (*it)->line() == 1 );
        CHECK( (*it)->staff() == 1 );
        ++it;
        //CHECK( (*it)->to_string() == "(n c3 e)" );
        CHECK( (*it)->num_instrument() == 1 );
        CHECK( (*it)->time() == 64.0f );
        CHECK( (*it)->line() == 3 );
        CHECK( (*it)->staff() == 1 );
        ++it;
        //CHECK( (*it)->to_string() == "(clef F4 p1)" );
        CHECK( (*it)->num_instrument() == 1 );
        CHECK( (*it)->time() == 96.0f );
        CHECK( (*it)->line() == 2 );
        CHECK( (*it)->staff() == 0 );
        ++it;
        //CHECK( (*it)->to_string() == "(n a3 e)" );
        CHECK( (*it)->num_instrument() == 1 );
        CHECK( (*it)->time() == 96.0f );
        CHECK( (*it)->line() == 2 );
        CHECK( (*it)->staff() == 0 );
        ++it;
        //CHECK( (*it)->to_string() == "(clef G p2)" );
        CHECK( (*it)->num_instrument() == 1 );
        CHECK( (*it)->time() == 96.0f );
        CHECK( (*it)->line() == 3 );
        CHECK( (*it)->staff() == 1 );
        ++it;
        //CHECK( (*it)->to_string() == "(clef F4 p2)" );
        CHECK( (*it)->num_instrument() == 1 );
        CHECK( (*it)->time() == 96.0f );
        CHECK( (*it)->line() == 3 );
        CHECK( (*it)->staff() == 1 );
        ++it;
        //CHECK( (*it)->to_string() == "(n c3 e)" );
        CHECK( (*it)->num_instrument() == 1 );
        CHECK( (*it)->time() == 96.0f );
        CHECK( (*it)->line() == 3 );
        CHECK( (*it)->staff() == 1 );
        ++it;
        //CHECK( (*it)->to_string() == "(barline )" );
        CHECK( (*it)->num_instrument() == 0 );
        CHECK( (*it)->time() == 128.0f );
        CHECK( (*it)->line() == 0 );
        CHECK( (*it)->staff() == 0 );
        ++it;
        //CHECK( (*it)->to_string() == "(barline )" );
        CHECK( (*it)->num_instrument() == 1 );
        CHECK( (*it)->time() == 128.0f );
        CHECK( (*it)->line() == 2 );
        CHECK( (*it)->staff() == 0 );

        delete tree->get_root();
        delete pBasicModel;
    }

    TEST_FIXTURE(ColStaffObjsTestFixture, ColStaffObjsAddAnchor)
    {
        LdpParser parser(cout, m_pLdpFactory);
        SpLdpTree tree = parser.parse_text("(lenmusdoc (vers 0.0) (content (score (vers 1.6)"
                        "(instrument (musicData (clef G)(key C)"
                        "(n f4 q)(text \"Hello world\")(barline)))  )))" );
        Analyser a(cout, m_pLdpFactory);
        BasicModel* pBasicModel = a.analyse_tree(tree);
        ImoDocument* pDoc = dynamic_cast<ImoDocument*>( pBasicModel->get_root() );
        ImoScore* pScore = dynamic_cast<ImoScore*>( pDoc->get_content_item(0) );
        ColStaffObjsBuilder builder(tree);
        ColStaffObjs* pColStaffObjs = builder.build(pScore);
        ColStaffObjs::iterator it = pColStaffObjs->begin();
        //pColStaffObjs->dump();
        CHECK( pColStaffObjs->num_entries() == 5 );
        ++it;   //(key C)
        ++it;   //(n f4 q)
        ++it;
        //CHECK( (*it)->to_string() == "(text \"Hello world\")" );
        CHECK( (*it)->num_instrument() == 0 );
        CHECK( (*it)->time() == 64.0f );
        CHECK( (*it)->line() == 0 );
        CHECK( (*it)->staff() == 0 );
//        LdpText* pText = dynamic_cast<LdpText*>((*it)->element());
//        CHECK( pText != NULL );
//        ImoSpacer* pAnchor = dynamic_cast<ImoSpacer*>(pText->get_imobj());
        ImoSpacer* pAnchor = dynamic_cast<ImoSpacer*>( (*it)->imo_object() );
        CHECK( pAnchor != NULL );
        ++it;
        //CHECK( (*it)->to_string() == "(barline )" );
        CHECK( (*it)->num_instrument() == 0 );
        CHECK( (*it)->time() == 64.0f );
        CHECK( (*it)->line() == 0 );
        CHECK( (*it)->staff() == 0 );

        delete tree->get_root();
        delete pBasicModel;
    }

//Additional test for ColStaffObjs::iterator -------------------------------------

    TEST_FIXTURE(ColStaffObjsTestFixture, CSOIteratorAtEnd)
    {
        LdpParser parser(cout, m_pLdpFactory);
        SpLdpTree tree = parser.parse_text("(lenmusdoc (vers 0.0) (content (score (vers 1.6) (instrument (staves 2)(musicData (clef G p1)(clef F4 p2)(key D)(n c4 q v2 p1)(n d4 e.)(n d4 s v3 p2)(n e4 h)))) ))" );
        Analyser a(cout, m_pLdpFactory);
        BasicModel* pBasicModel = a.analyse_tree(tree);
        ImoDocument* pDoc = dynamic_cast<ImoDocument*>( pBasicModel->get_root() );
        ImoScore* pScore = dynamic_cast<ImoScore*>( pDoc->get_content_item(0) );
        ColStaffObjsBuilder builder(tree);
        ColStaffObjs* pColStaffObjs = builder.build(pScore, false);    //false: only creation, no sort
        ColStaffObjs::iterator it = pColStaffObjs->begin();
        //pColStaffObjs->dump();
        CHECK( pColStaffObjs->num_entries() == 8 );
        //CHECK( (*it)->to_string() == "(clef G p1)" );
        CHECK( (*it)->line() == 0 );
        ++it;
        //CHECK( (*it)->to_string() == "(clef F4 p2)" );
        CHECK( (*it)->line() == 1 );
        ++it;
        //CHECK( (*it)->to_string() == "(key D)" );
        CHECK( (*it)->line() == 0 );
        ++it;
        //CHECK( (*it)->to_string() == "(key D)" );
        CHECK( (*it)->line() == 1 );
        ++it;
        //CHECK( (*it)->to_string() == "(n c4 q v2 p1)" );
        CHECK( (*it)->line() == 0 );
        ++it;
        //CHECK( (*it)->to_string() == "(n d4 e.)" );
        CHECK( (*it)->line() == 0 );
        ++it;
        //CHECK( (*it)->to_string() == "(n d4 s v3 p2)" );
        CHECK( (*it)->line() == 1 );
        ++it;
        //CHECK( (*it)->to_string() == "(n e4 h)" );
        CHECK( (*it)->line() == 1 );
        ++it;
        CHECK( it == pColStaffObjs->end() );

        delete tree->get_root();
        delete pBasicModel;
    }

}


#endif  // _LM_DEBUG_

