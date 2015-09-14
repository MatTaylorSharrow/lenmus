//---------------------------------------------------------------------------------------
// This file is part of the Lomse library.
// Copyright (c) 2010-2013 Cecilio Salmeron. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
//    * Redistributions of source code must retain the above copyright notice, this
//      list of conditions and the following disclaimer.
//
//    * Redistributions in binary form must reproduce the above copyright notice, this
//      list of conditions and the following disclaimer in the documentation and/or
//      other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
// OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
// SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
// TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
// BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
// ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
// DAMAGE.
//
// For any comment, suggestion or feature request, please contact the manager of
// the project at cecilios@users.sourceforge.net
//---------------------------------------------------------------------------------------

#include <UnitTest++.h>
#include <sstream>
#include "lomse_build_options.h"

//classes related to these tests
#include "lomse_injectors.h"
#include "lomse_staffobjs_table.h"
#include "lomse_ldp_parser.h"
#include "lomse_ldp_analyser.h"
#include "lomse_internal_model.h"
#include "lomse_im_note.h"
#include "lomse_document.h"
#include "lomse_score_iterator.h"
#include "lomse_model_builder.h"
#include "lomse_time.h"

using namespace UnitTest;
using namespace std;
using namespace lomse;


//=======================================================================================
// StaffVoiceLineTable tests
//=======================================================================================
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


//---------------------------------------------------------------------------------------
// helper macros
// CHECK_ENTRY: checks/displays ids
// CHECK_ENTRY0: does not check/displays ids
#define CHECK_ENTRY(it, _instr, _staff, _measure, _time, _line, _object) \
            CHECK( (*it)->num_instrument() == _instr );         \
            CHECK( (*it)->staff() == _staff );                  \
            CHECK( (*it)->measure() == _measure );              \
            CHECK( is_equal_time((*it)->time(), _time) );       \
            CHECK( (*it)->line() == _line );                    \
            CHECK( (*it)->to_string_with_ids() == _object );

#define CHECK_ENTRY0(it, _instr, _staff, _measure, _time, _line, _object) \
            CHECK( (*it)->num_instrument() == _instr );         \
            CHECK( (*it)->staff() == _staff );                  \
            CHECK( (*it)->measure() == _measure );              \
            CHECK( is_equal_time((*it)->time(), _time) );       \
            CHECK( (*it)->line() == _line );                    \
            CHECK( (*it)->to_string() == _object );             \
            ++it;


//=======================================================================================
// ColStaffObjsBuilder test
//=======================================================================================
class ColStaffObjsBuilderTestFixture
{
public:
    LibraryScope m_libraryScope;
    Document* m_pDoc;
    LdpTree* m_pTree;
    InternalModel* m_pIModel;
    LdpFactory* m_pLdpFactory;

    ColStaffObjsBuilderTestFixture()     //SetUp fixture
        : m_libraryScope(cout)
        , m_pDoc(NULL)
        , m_pTree(NULL)
        , m_pIModel(NULL)
    {
        m_pLdpFactory = m_libraryScope.ldp_factory();
    }

    ~ColStaffObjsBuilderTestFixture()    //TearDown fixture
    {
        delete m_pDoc;
    }

    ImoScore* create_score(const string &ldp)
    {
        m_pDoc = LOMSE_NEW Document(m_libraryScope);
        LdpParser parser(cout, m_libraryScope.ldp_factory());
        parser.parse_text(ldp);
        LdpTree* pTree = parser.get_ldp_tree();
        LdpAnalyser a(cout, m_libraryScope, m_pDoc);
        ImoObj* pImo = a.analyse_tree_and_get_object(pTree);
        delete pTree->get_root();
        return dynamic_cast<ImoScore*>(pImo);
    }

};

SUITE(ColStaffObjsBuilderTest)
{

    TEST_FIXTURE(ColStaffObjsBuilderTestFixture, builder_1)
    {
        //empty score creates empty table
        ImoScore* pScore = create_score(
            "(score (vers 2.0)(instrument (musicData "
            ")))"
        );
        ColStaffObjsBuilder builder;
        ColStaffObjs* pTable = builder.build(pScore);

        CHECK( pTable->num_lines() == 1 );
        CHECK( pTable->num_entries() == 0 );
        CHECK( pTable->is_anacrusis_start() == false );
    }

    TEST_FIXTURE(ColStaffObjsBuilderTestFixture, builder_2)
    {
        //each voice uses a different time counter
        ImoScore* pScore = create_score(
            "(score (vers 2.0)(instrument (musicData "
            "(clef G)(n c4 q v1)(n c5 e v2)(n e4 e v1)(n e5 e v2)"
            ")))"
        );
        ColStaffObjsBuilder builder;
        ColStaffObjs* pTable = builder.build(pScore);

        CHECK( pTable->num_lines() == 2 );
        CHECK( pTable->num_entries() == 5 );
        CHECK( pTable->is_anacrusis_start() == false );

        //cout << pTable->dump();
        ColStaffObjsIterator it = pTable->begin();
        //              instr, staff, meas. time, line, scr
        CHECK_ENTRY0(it, 0,    0,      0,   0,     0, "(clef G p1 )" );
        CHECK_ENTRY0(it, 0,    0,      0,   0,     0, "(n c4 q v1  p1 )" );
        CHECK_ENTRY0(it, 0,    0,      0,   0,     1, "(n c5 e v2  p1 )" );
        CHECK_ENTRY0(it, 0,    0,      0,  32,     1, "(n e5 e v2  p1 )" );
        CHECK_ENTRY0(it, 0,    0,      0,  64,     0, "(n e4 e v1  p1 )" );
    }

    TEST_FIXTURE(ColStaffObjsBuilderTestFixture, builder_3)
    {
        //barline is placed at maximum time reached in the measure
        ImoScore* pScore = create_score(
            "(score (vers 2.0)(instrument (musicData "
            "(clef G)(n c4 q v1)(n c5 e v2)(n e4 e v1)(n e5 w v2)(barline)"
            ")))"
        );
        ColStaffObjsBuilder builder;
        ColStaffObjs* pTable = builder.build(pScore);

        CHECK( pTable->num_lines() == 2 );
        CHECK( pTable->num_entries() == 6 );
        CHECK( pTable->is_anacrusis_start() == false );

        //cout << pTable->dump();
        ColStaffObjsIterator it = pTable->begin();
        //              instr, staff, meas. time, line, scr
        CHECK_ENTRY0(it, 0,    0,      0,   0,     0, "(clef G p1 )" );
        CHECK_ENTRY0(it, 0,    0,      0,   0,     0, "(n c4 q v1  p1 )" );
        CHECK_ENTRY0(it, 0,    0,      0,   0,     1, "(n c5 e v2  p1 )" );
        CHECK_ENTRY0(it, 0,    0,      0,  32,     1, "(n e5 w v2  p1 )" );
        CHECK_ENTRY0(it, 0,    0,      0,  64,     0, "(n e4 e v1  p1 )" );
        CHECK_ENTRY0(it, 0,    0,      0, 288,     0, "(barline simple)" );
    }

    TEST_FIXTURE(ColStaffObjsBuilderTestFixture, builder_4)
    {
        //after barline voices are correctly positioned
        ImoScore* pScore = create_score(
            "(score (vers 2.0)(instrument (musicData "
            "(clef G)(n c4 q v1)(n c5 e v2)(n e4 e v1)(n e5 w v2)(barline)"
            "(n c4 q v1)(n c5 e v2)(n e4 e v1)(n e5 w v2)"
            ")))"
        );
        ColStaffObjsBuilder builder;
        ColStaffObjs* pTable = builder.build(pScore);

        CHECK( pTable->num_lines() == 2 );
        CHECK( pTable->num_entries() == 10 );
        CHECK( pTable->is_anacrusis_start() == false );

        //cout << pTable->dump();
        ColStaffObjsIterator it = pTable->begin();
        //              instr, staff, meas. time, line, scr
        CHECK_ENTRY0(it, 0,    0,      0,   0,     0, "(clef G p1 )" );
        CHECK_ENTRY0(it, 0,    0,      0,   0,     0, "(n c4 q v1  p1 )" );
        CHECK_ENTRY0(it, 0,    0,      0,   0,     1, "(n c5 e v2  p1 )" );
        CHECK_ENTRY0(it, 0,    0,      0,  32,     1, "(n e5 w v2  p1 )" );
        CHECK_ENTRY0(it, 0,    0,      0,  64,     0, "(n e4 e v1  p1 )" );
        CHECK_ENTRY0(it, 0,    0,      0, 288,     0, "(barline simple)" );
        CHECK_ENTRY0(it, 0,    0,      1, 288,     0, "(n c4 q v1  p1 )" );
        CHECK_ENTRY0(it, 0,    0,      1, 288,     1, "(n c5 e v2  p1 )" );
        CHECK_ENTRY0(it, 0,    0,      1, 320,     1, "(n e5 w v2  p1 )" );
        CHECK_ENTRY0(it, 0,    0,      1, 352,     0, "(n e4 e v1  p1 )" );
    }

    TEST_FIXTURE(ColStaffObjsBuilderTestFixture, builder_5)
    {
        //key and time signatures generate secondary entries
        ImoScore* pScore = create_score(
            "(score (vers 2.0) (instrument (staves 2)(musicData "
            "(clef G p1)(clef F4 p2)(key C)(time 2 4)"
            ")))"
        );
        ColStaffObjsBuilder builder;
        ColStaffObjs* pTable = builder.build(pScore);

        CHECK( pTable->num_lines() == 2 );
        CHECK( pTable->num_entries() == 6 );
        CHECK( pTable->is_anacrusis_start() == false );

        //cout << pTable->dump();
        ColStaffObjsIterator it = pTable->begin();
        //              instr, staff, meas. time, line, scr
        CHECK_ENTRY0(it, 0,    0,      0,   0,     0, "(clef G p1 )" );
        CHECK_ENTRY0(it, 0,    1,      0,   0,     1, "(clef F4 p2 )" );
        CHECK_ENTRY0(it, 0,    0,      0,   0,     0, "(key C)" );
        CHECK_ENTRY0(it, 0,    1,      0,   0,     1, "(key C)" );
        CHECK_ENTRY0(it, 0,    0,      0,   0,     0, "(time 2 4)" );
        CHECK_ENTRY0(it, 0,    1,      0,   0,     1, "(time 2 4)" );
    }

    TEST_FIXTURE(ColStaffObjsBuilderTestFixture, builder_6)
    {
        //chord notes have time and staff properly assigned
        ImoScore* pScore = create_score(
            "(score (vers 2.0) (instrument (staves 2)(musicData "
            "(clef G p1)(clef F4 p2)(key C)(time 2 4)(chord (n c3 w p2)(n g3 w p2)"
            "(n e4 w p1)(n c5 w p1))(barline)"
            ")))"
        );
        ColStaffObjsBuilder builder;
        ColStaffObjs* pTable = builder.build(pScore);

        CHECK( pTable->num_lines() == 2 );
        CHECK( pTable->num_entries() == 11 );
        CHECK( pTable->is_anacrusis_start() == false );

        //cout << pTable->dump();
        ColStaffObjsIterator it = pTable->begin();
        //              instr, staff, meas. time, line, scr
        CHECK_ENTRY0(it, 0,    0,      0,   0,     0, "(clef G p1 )" );
        CHECK_ENTRY0(it, 0,    1,      0,   0,     1, "(clef F4 p2 )" );
        CHECK_ENTRY0(it, 0,    0,      0,   0,     0, "(key C)" );
        CHECK_ENTRY0(it, 0,    1,      0,   0,     1, "(key C)" );
        CHECK_ENTRY0(it, 0,    0,      0,   0,     0, "(time 2 4)" );
        CHECK_ENTRY0(it, 0,    1,      0,   0,     1, "(time 2 4)" );
        CHECK_ENTRY0(it, 0,    1,      0,   0,     1, "(chord (n c3 w v1  p2 )" );
        CHECK_ENTRY0(it, 0,    1,      0,   0,     1, "(n g3 w v1  p2 )" );
        CHECK_ENTRY0(it, 0,    0,      0,   0,     0, "(n e4 w v1  p1 )" );
        CHECK_ENTRY0(it, 0,    0,      0,   0,     0, "(n c5 w v1  p1 ))" );
        CHECK_ENTRY0(it, 0,    0,      0, 256,     0, "(barline simple)" );
    }

    TEST_FIXTURE(ColStaffObjsBuilderTestFixture, builder_7)
    {
        //anchor staffobjs
        ImoScore* pScore = create_score(
            "(score (vers 2.0) (instrument (musicData "
            "(clef G)(key C)(n f4 q)(text \"Hello world\")(barline)"
            ")))"
        );
        ColStaffObjsBuilder builder;
        ColStaffObjs* pTable = builder.build(pScore);

        CHECK( pTable->num_lines() == 1 );
        CHECK( pTable->num_entries() == 5 );
        CHECK( pTable->is_anacrusis_start() == false );

        //cout << pTable->dump();
        ColStaffObjsIterator it = pTable->begin();
        //              instr, staff, meas. time, line, scr
        CHECK_ENTRY0(it, 0,    0,      0,   0,     0, "(clef G p1 )" );
        CHECK_ENTRY0(it, 0,    0,      0,   0,     0, "(key C)" );
        CHECK_ENTRY0(it, 0,    0,      0,   0,     0, "(n f4 q v1  p1 )" );
        CHECK_ENTRY0(it, 0,    0,      0,  64,     0, "(spacer 0 p1 (text \"Hello world\"))" );
        CHECK_ENTRY0(it, 0,    0,      0,  64,     0, "(barline simple)" );
    }

    TEST_FIXTURE(ColStaffObjsBuilderTestFixture, builder_8)
    {
        //anacruxis
        ImoScore* pScore = create_score(
            "(score (vers 2.0) (instrument (musicData "
            "(clef G)(time 3 4)(n c4 q)(barline)(n d4 e.)(n d4 s)"
            ")))"
        );
        ColStaffObjsBuilder builder;
        ColStaffObjs* pTable = builder.build(pScore);

        CHECK( pTable->num_lines() == 1 );
        CHECK( pTable->num_entries() == 6 );
        CHECK( pTable->is_anacrusis_start() == true );
        CHECK( is_equal_time( pTable->anacrusis_missing_time(), 128.0f) == true );

        //cout << pTable->dump();
        ColStaffObjsIterator it = pTable->begin();
        //              instr, staff, meas. time, line, scr
        CHECK_ENTRY0(it, 0,    0,      0,   0,     0, "(clef G p1 )" );
        CHECK_ENTRY0(it, 0,    0,      0,   0,     0, "(time 3 4)" );
        CHECK_ENTRY0(it, 0,    0,      0,   0,     0, "(n c4 q v1  p1 )" );
        CHECK_ENTRY0(it, 0,    0,      0,  64,     0, "(barline simple)" );
        CHECK_ENTRY0(it, 0,    0,      1,  64,     0, "(n d4 e. v1  p1 )" );
        CHECK_ENTRY0(it, 0,    0,      1, 112,     0, "(n d4 s v1  p1 )" );
    }

    TEST_FIXTURE(ColStaffObjsBuilderTestFixture, builder_9)
    {
        //goFwd replaced by special invisible rest
        ImoScore* pScore = create_score(
            "(score (vers 2.0)(instrument (musicData "
            "(clef G)(n c4 e v1)(goFwd e v1)(n e4 e v1)"
            ")))"
        );
        ColStaffObjsBuilder builder;
        ColStaffObjs* pTable = builder.build(pScore);

        CHECK( pTable->num_lines() == 1 );
        CHECK( pTable->num_entries() == 4 );
        CHECK( pTable->is_anacrusis_start() == false );

        //cout << pTable->dump();
        ColStaffObjsIterator it = pTable->begin();
        //              instr, staff, meas. time, line, scr
        CHECK_ENTRY0(it, 0,    0,      0,   0,     0, "(clef G p1 )" );
        CHECK_ENTRY0(it, 0,    0,      0,   0,     0, "(n c4 e v1  p1 )" );
        CHECK_ENTRY0(it, 0,    0,      0,  32,     0, "(goFwd e v1 p1)" );
        CHECK_ENTRY0(it, 0,    0,      0,  64,     0, "(n e4 e v1  p1 )" );
    }

    TEST_FIXTURE(ColStaffObjsBuilderTestFixture, builder_10)
    {
        //notes in other voices intermixed in beamed group
        ImoScore* pScore = create_score(
            "(score (vers 2.0)(instrument (musicData "
            "(clef F4)(n e3 e g+)(n g3 e)(n c4 e g-)"
            "(n c2 w v3)(barline)"
            ")))"
        );
        ColStaffObjsBuilder builder;
        ColStaffObjs* pTable = builder.build(pScore);

        CHECK( pTable->num_lines() == 2 );
        CHECK( pTable->num_entries() == 6 );
        CHECK( pTable->is_anacrusis_start() == false );

        //cout << pTable->dump();
        ColStaffObjsIterator it = pTable->begin();
        //              instr, staff, meas. time, line, scr
        CHECK_ENTRY0(it, 0,    0,      0,   0,     0, "(clef F4 p1 )" );
        CHECK_ENTRY0(it, 0,    0,      0,   0,     0, "(n e3 e v1  p1 (beam 22 +))" );
        CHECK_ENTRY0(it, 0,    0,      0,   0,     1, "(n c2 w v3  p1 )" );
        CHECK_ENTRY0(it, 0,    0,      0,  32,     0, "(n g3 e v1  p1 (beam 22 =))" );
        CHECK_ENTRY0(it, 0,    0,      0,  64,     0, "(n c4 e v1  p1 (beam 22 -))" );
        CHECK_ENTRY0(it, 0,    0,      0, 256,     0, "(barline simple)" );
    }

    // ColStaffObjsBuilderEngine1x ------------------------------------------------------

    TEST_FIXTURE(ColStaffObjsBuilderTestFixture, ColStaffObjsAddEntries)
    {
        Document doc(m_libraryScope);
        LdpParser parser(cout, m_pLdpFactory);
        parser.parse_text("(lenmusdoc (vers 0.0) (content "
            "(score (vers 1.6) (instrument (musicData (n c4 q) (barline simple))))))" );
        LdpTree* tree = parser.get_ldp_tree();
        LdpAnalyser a(cout, m_libraryScope, &doc);
        InternalModel* pIModel = a.analyse_tree(tree, "string:");
        ImoDocument* pDoc = dynamic_cast<ImoDocument*>( pIModel->get_root() );
        ImoScore* pScore = dynamic_cast<ImoScore*>( pDoc->get_content_item(0) );
        ColStaffObjsBuilder builder;
        ColStaffObjs* pColStaffObjs = builder.build(pScore);

//        cout << pColStaffObjs->dump();

        CHECK( pColStaffObjs->num_entries() == 2 );
        CHECK( pColStaffObjs->num_lines() == 1 );

        delete tree->get_root();
        delete pIModel;
    }

    TEST_FIXTURE(ColStaffObjsBuilderTestFixture, ScoreIteratorPointsFirst)
    {
        LdpParser parser(cout, m_pLdpFactory);
        parser.parse_text("(lenmusdoc (vers 0.0) (content (score (vers 1.6) (instrument (musicData (n c4 q) (barline simple))))))" );
        LdpTree* tree = parser.get_ldp_tree();
        Document doc(m_libraryScope);
        LdpAnalyser a(cout, m_libraryScope, &doc);
        InternalModel* pIModel = a.analyse_tree(tree, "string:");
        ImoDocument* pDoc = dynamic_cast<ImoDocument*>( pIModel->get_root() );
        ImoScore* pScore = dynamic_cast<ImoScore*>( pDoc->get_content_item(0) );
        ColStaffObjsBuilder builder;
        ColStaffObjs* pColStaffObjs = builder.build(pScore);
        ColStaffObjsIterator it = pColStaffObjs->begin();

       //cout << (*it)->dump();
        //cout << (*it)->to_string() << endl;
        //CHECK( (*it)->to_string() == "(n c4 q)" );
        CHECK( (*it)->imo_object()->is_note() == true );
        CHECK( (*it)->measure() == 0 );
        CHECK( is_equal_time((*it)->time(), 0.0f) );

        delete tree->get_root();
        delete pIModel;
    }

    TEST_FIXTURE(ColStaffObjsBuilderTestFixture, ColStaffObjsChangeMeasure)
    {
        LdpParser parser(cout, m_pLdpFactory);
        parser.parse_text("(lenmusdoc (vers 0.0) (content "
            "(score (vers 1.6) (instrument (musicData (n c4 q) (barline simple)"
            "(n d4 e) (barline simple) (n e4 w))))))" );
        LdpTree* tree = parser.get_ldp_tree();
        Document doc(m_libraryScope);
        LdpAnalyser a(cout, m_libraryScope, &doc);
        InternalModel* pIModel = a.analyse_tree(tree, "string:");
        ImoDocument* pDoc = dynamic_cast<ImoDocument*>( pIModel->get_root() );
        ImoScore* pScore = dynamic_cast<ImoScore*>( pDoc->get_content_item(0) );
        ColStaffObjsBuilder builder;
        ColStaffObjs* pColStaffObjs = builder.build(pScore);
        ColStaffObjsIterator it = pColStaffObjs->begin();
//        cout << pColStaffObjs->dump();
        CHECK( pColStaffObjs->num_entries() == 5 );
        CHECK( pColStaffObjs->num_lines() == 1 );

        //CHECK( (*it)->to_string() == "(n c4 q)" );
        CHECK( (*it)->imo_object()->is_note() == true );
        CHECK( (*it)->measure() == 0 );
        ++it;
        //CHECK( (*it)->to_string() == "(barline simple)" );
        CHECK( (*it)->imo_object()->is_barline() == true );
        CHECK( (*it)->measure() == 0 );
        ++it;
        //CHECK( (*it)->to_string() == "(n d4 e)" );
        CHECK( (*it)->imo_object()->is_note() == true );
        CHECK( (*it)->measure() == 1 );
        ++it;
        //CHECK( (*it)->to_string() == "(barline simple)" );
        CHECK( (*it)->imo_object()->is_barline() == true );
        CHECK( (*it)->measure() == 1 );
        ++it;
        //CHECK( (*it)->to_string() == "(n e4 w)" );
        CHECK( (*it)->imo_object()->is_note() == true );
        CHECK( (*it)->measure() == 2 );

        delete tree->get_root();
        delete pIModel;
    }

    TEST_FIXTURE(ColStaffObjsBuilderTestFixture, ColStaffObjsTimeInSequence)
    {
        LdpParser parser(cout, m_pLdpFactory);
        parser.parse_text("(lenmusdoc (vers 0.0) (content "
                "(score (vers 1.6) (instrument (musicData "
                "(n c4 q)(n d4 e.)(n d4 s)(n e4 h)))) ))" );
        LdpTree* tree = parser.get_ldp_tree();
        Document doc(m_libraryScope);
        LdpAnalyser a(cout, m_libraryScope, &doc);
        InternalModel* pIModel = a.analyse_tree(tree, "string:");
        ImoDocument* pDoc = dynamic_cast<ImoDocument*>( pIModel->get_root() );
        ImoScore* pScore = dynamic_cast<ImoScore*>( pDoc->get_content_item(0) );
        ColStaffObjsBuilder builder;
        ColStaffObjs* pColStaffObjs = builder.build(pScore);
        ColStaffObjsIterator it = pColStaffObjs->begin();
//        cout << pColStaffObjs->dump();
        CHECK( pColStaffObjs->num_entries() == 4 );
        CHECK( pColStaffObjs->num_lines() == 1 );

        //CHECK( (*it)->to_string() == "(n c4 q)" );
        CHECK( (*it)->imo_object()->is_note() == true );
        CHECK( (*it)->measure() == 0 );
        CHECK( is_equal_time((*it)->time(), 0.0f) );
        ++it;
        //CHECK( (*it)->to_string() == "(n d4 e.)" );
        CHECK( (*it)->imo_object()->is_note() == true );
        CHECK( (*it)->measure() == 0 );
        CHECK( is_equal_time((*it)->time(), 64.0f) );
        ++it;
        //CHECK( (*it)->to_string() == "(n d4 s)" );
        CHECK( (*it)->imo_object()->is_note() == true );
        CHECK( (*it)->measure() == 0 );
        CHECK( is_equal_time((*it)->time(), 112.0f) );
        ++it;
        //CHECK( (*it)->to_string() == "(n e4 h)" );
        CHECK( (*it)->imo_object()->is_note() == true );
        CHECK( (*it)->measure() == 0 );
        CHECK( is_equal_time((*it)->time(), 128.0f) );

        delete tree->get_root();
        delete pIModel;
    }

    TEST_FIXTURE(ColStaffObjsBuilderTestFixture, ColStaffObjsTimeGoBack)
    {
        Document doc(m_libraryScope);
        LdpParser parser(cout, m_pLdpFactory);
        parser.parse_text("(lenmusdoc (vers 0.0) (content "
            "(score (vers 1.6) (instrument (musicData"
            "(n c4 q)(n d4 e.)(n d4 s)(goBack start)(n e4 h)(n g4 q)))) ))" );
        LdpTree* tree = parser.get_ldp_tree();
        LdpAnalyser a(cout, m_libraryScope, &doc);
        InternalModel* pIModel = a.analyse_tree(tree, "string:");
        ImoDocument* pDoc = dynamic_cast<ImoDocument*>( pIModel->get_root() );
        ImoScore* pScore = dynamic_cast<ImoScore*>( pDoc->get_content_item(0) );
        ColStaffObjsBuilder builder;
        ColStaffObjs* pColStaffObjs = builder.build(pScore);

        CHECK( pColStaffObjs->num_entries() == 5 );
        CHECK( pColStaffObjs->num_lines() == 1 );
//        cout << pColStaffObjs->dump();

        ColStaffObjsIterator it = pColStaffObjs->begin();
        //CHECK( (*it)->to_string() == "(n c4 q)" );
        CHECK( (*it)->imo_object()->is_note() == true );
        CHECK( (*it)->measure() == 0 );
        CHECK( is_equal_time((*it)->time(), 0.0f) );
        ++it;
        //CHECK( (*it)->to_string() == "(n e4 h)" );
        CHECK( (*it)->imo_object()->is_note() == true );
        CHECK( (*it)->measure() == 0 );
        CHECK( is_equal_time((*it)->time(), 0.0f) );
        ++it;
        //CHECK( (*it)->to_string() == "(n d4 e.)" );
        CHECK( (*it)->imo_object()->is_note() == true );
        CHECK( (*it)->measure() == 0 );
        CHECK( is_equal_time((*it)->time(), 64.0f) );
        ++it;
        //CHECK( (*it)->to_string() == "(n d4 s)" );
        CHECK( (*it)->imo_object()->is_note() == true );
        CHECK( (*it)->measure() == 0 );
        CHECK( is_equal_time((*it)->time(), 112.0f) );
        ++it;
        //CHECK( (*it)->to_string() == "(n g4 q)" );
        CHECK( (*it)->imo_object()->is_note() == true );
        CHECK( (*it)->measure() == 0 );
        CHECK( is_equal_time((*it)->time(), 128.0f) );

        delete tree->get_root();
        delete pIModel;
    }

    TEST_FIXTURE(ColStaffObjsBuilderTestFixture, GoBack_StartTime)
    {
        Document doc(m_libraryScope);
        LdpParser parser(cout, m_pLdpFactory);
        parser.parse_text("(lenmusdoc (vers 0.0) (content "
            "(score (vers 1.6) (instrument (musicData"
            "(n c4 q)(n d4 e.)(n e4 s)(barline)"
            "(n f4 q)(n g4 e.)(n a4 s)(goBack start)(n b4 q)(n c5 q)))) ))" );
        LdpTree* tree = parser.get_ldp_tree();
        LdpAnalyser a(cout, m_libraryScope, &doc);
        InternalModel* pIModel = a.analyse_tree(tree, "string:");
        ImoDocument* pDoc = dynamic_cast<ImoDocument*>( pIModel->get_root() );
        ImoScore* pScore = dynamic_cast<ImoScore*>( pDoc->get_content_item(0) );
        ColStaffObjsBuilder builder;
        ColStaffObjs* pColStaffObjs = builder.build(pScore);

        CHECK( pColStaffObjs->num_entries() == 9 );
        CHECK( pColStaffObjs->num_lines() == 1 );
//        pColStaffObjs->dump();

        ColStaffObjsIterator it = pColStaffObjs->begin();
        //CHECK( (*it)->to_string() == "(n c4 q)" );
        CHECK( (*it)->imo_object()->is_note() == true );
        CHECK( (*it)->measure() == 0 );
        CHECK( is_equal_time((*it)->time(), 0.0f) );
        ++it;
        //CHECK( (*it)->to_string() == "(n d4 e.)" );
        CHECK( (*it)->imo_object()->is_note() == true );
        CHECK( (*it)->measure() == 0 );
        CHECK( is_equal_time((*it)->time(), 64.0f) );
        ++it;
        //CHECK( (*it)->to_string() == "(n e4 s)" );
        CHECK( (*it)->imo_object()->is_note() == true );
        CHECK( (*it)->measure() == 0 );
        CHECK( is_equal_time((*it)->time(), 112.0f) );
        ++it;
        //CHECK( (*it)->to_string() == "(barline )" );
        CHECK( (*it)->imo_object()->is_barline() == true );
        CHECK( (*it)->measure() == 0 );
        CHECK( is_equal_time((*it)->time(), 128.0f) );
        ++it;
        //CHECK( (*it)->to_string() == "(n f4 q)" );
        CHECK( (*it)->imo_object()->is_note() == true );
        CHECK( (*it)->measure() == 1 );
        CHECK( is_equal_time((*it)->time(), 128.0f) );
        ++it;   //(n b4 q)
        CHECK( (*it)->imo_object()->is_note() == true );
        CHECK( (*it)->measure() == 1 );
        CHECK( is_equal_time((*it)->time(), 128.0f) );
        ++it;
        //CHECK( (*it)->to_string() == "(n g4 e.)" );
        CHECK( (*it)->imo_object()->is_note() == true );
        CHECK( (*it)->measure() == 1 );
        CHECK( is_equal_time((*it)->time(), 192.0f) );
        ++it;   //(n g4 q)
        CHECK( (*it)->imo_object()->is_note() == true );
        CHECK( (*it)->measure() == 1 );
        CHECK( is_equal_time((*it)->time(), 192.0f) );
        ++it;
        //CHECK( (*it)->to_string() == "(n a4 s)" );
        CHECK( (*it)->imo_object()->is_note() == true );
        CHECK( (*it)->measure() == 1 );
        CHECK( is_equal_time((*it)->time(), 240.0f) );

        delete tree->get_root();
        delete pIModel;
    }

    TEST_FIXTURE(ColStaffObjsBuilderTestFixture, ColStaffObjsTimeGoFwd)
    {
        Document doc(m_libraryScope);
        LdpParser parser(cout, m_pLdpFactory);
        parser.parse_text("(lenmusdoc (vers 0.0) (content "
            "(score (vers 1.6) (instrument (musicData "
            "(n c4 q)(n d4 e.)(n d4 s)(goBack start)(n e4 q)(goFwd end)(barline)))) ))" );
        LdpTree* tree = parser.get_ldp_tree();
        LdpAnalyser a(cout, m_libraryScope, &doc);
        InternalModel* pIModel = a.analyse_tree(tree, "string:");
        ImoDocument* pDoc = dynamic_cast<ImoDocument*>( pIModel->get_root() );
        ImoScore* pScore = dynamic_cast<ImoScore*>( pDoc->get_content_item(0) );
        ColStaffObjsBuilder builder;
        ColStaffObjs* pColStaffObjs = builder.build(pScore);

        CHECK( pColStaffObjs->num_entries() == 5 );
        CHECK( pColStaffObjs->num_lines() == 1 );
        //pColStaffObjs->dump();

        ColStaffObjsIterator it = pColStaffObjs->begin();
        //CHECK( (*it)->to_string() == "(n c4 q)" );
        CHECK( (*it)->imo_object()->is_note() == true );
        CHECK( is_equal_time((*it)->time(), 0.0f) );
        ++it;
        //CHECK( (*it)->to_string() == "(n e4 q)" );
        CHECK( (*it)->imo_object()->is_note() == true );
        CHECK( is_equal_time((*it)->time(), 0.0f) );
        ++it;
        //CHECK( (*it)->to_string() == "(n d4 e.)" );
        CHECK( (*it)->imo_object()->is_note() == true );
        CHECK( is_equal_time((*it)->time(), 64.0f) );
        ++it;
        //CHECK( (*it)->to_string() == "(n d4 s)" );
        CHECK( (*it)->imo_object()->is_note() == true );
        CHECK( is_equal_time((*it)->time(), 112.0f) );
        ++it;
        //CHECK( (*it)->to_string() == "(barline )" );
        CHECK( (*it)->imo_object()->is_barline() == true );
        CHECK( is_equal_time((*it)->time(), 128.0f) );

        delete tree->get_root();
        delete pIModel;
    }

    TEST_FIXTURE(ColStaffObjsBuilderTestFixture, ColStaffObjsStaffAssigned)
    {
        Document doc(m_libraryScope);
        LdpParser parser(cout, m_pLdpFactory);
        parser.parse_text("(lenmusdoc (vers 0.0) (content "
            "(score (vers 1.6) (instrument (musicData "
            "(n c4 q p2)(n d4 e.)(n d4 s p3)(n e4 h)))) ))" );
        LdpTree* tree = parser.get_ldp_tree();
        LdpAnalyser a(cout, m_libraryScope, &doc);
        InternalModel* pIModel = a.analyse_tree(tree, "string:");
        ImoDocument* pDoc = dynamic_cast<ImoDocument*>( pIModel->get_root() );
        ImoScore* pScore = dynamic_cast<ImoScore*>( pDoc->get_content_item(0) );
        ColStaffObjsBuilder builder;
        ColStaffObjs* pColStaffObjs = builder.build(pScore);
        ColStaffObjsIterator it = pColStaffObjs->begin();
        //pColStaffObjs->dump();
        CHECK( pColStaffObjs->num_entries() == 4 );

        //CHECK( (*it)->to_string() == "(n c4 q p2)" );
        CHECK( (*it)->imo_object()->is_note() == true );
        CHECK( (*it)->staff() == 1 );
        ++it;
        //CHECK( (*it)->to_string() == "(n d4 e.)" );
        CHECK( (*it)->imo_object()->is_note() == true );
        CHECK( (*it)->staff() == 1 );
        ++it;
        //CHECK( (*it)->to_string() == "(n d4 s p3)" );
        CHECK( (*it)->imo_object()->is_note() == true );
        CHECK( (*it)->staff() == 2 );
        ++it;
        //CHECK( (*it)->to_string() == "(n e4 h)" );
        CHECK( (*it)->imo_object()->is_note() == true );
        CHECK( (*it)->staff() == 2 );

        delete tree->get_root();
        delete pIModel;
    }

    TEST_FIXTURE(ColStaffObjsBuilderTestFixture, Anacrusis)
    {
        Document doc(m_libraryScope);
        LdpParser parser(cout, m_pLdpFactory);
        parser.parse_text("(lenmusdoc (vers 0.0) (content "
            "(score (vers 1.6) (instrument (musicData "
            "(clef G)(time 3 4)(n c4 q)(barline)(n d4 e.)(n d4 s)) )) ))" );
        LdpTree* tree = parser.get_ldp_tree();
        LdpAnalyser a(cout, m_libraryScope, &doc);
        InternalModel* pIModel = a.analyse_tree(tree, "string:");
        ImoDocument* pDoc = dynamic_cast<ImoDocument*>( pIModel->get_root() );
        ImoScore* pScore = dynamic_cast<ImoScore*>( pDoc->get_content_item(0) );
        ColStaffObjsBuilder builder;
        ColStaffObjs* pColStaffObjs = builder.build(pScore);
        ColStaffObjsIterator it = pColStaffObjs->begin();
        //pColStaffObjs->dump();
        CHECK( pColStaffObjs->num_lines() == 1 );
        CHECK( pColStaffObjs->num_entries() == 6 );
        CHECK( pColStaffObjs->is_anacrusis_start() == true );
        CHECK( is_equal_time( pColStaffObjs->anacrusis_missing_time(), 128.0f) == true );

                   // (clef G)
        ++it;       //(time 2 4)
        CHECK( (*it)->imo_object()->is_time_signature() == true );
        CHECK( (*it)->line() == 0 );
        ++it;       //(n c4 q)
        CHECK( (*it)->imo_object()->is_note() == true );
        CHECK( (*it)->line() == 0 );
        ++it;       //(barline)
        CHECK( (*it)->imo_object()->is_barline() == true );
        CHECK( (*it)->line() == 0 );
        CHECK( is_equal_time((*it)->time(), 64.0f) );
        ++it;       //(n d4 e.)
        CHECK( (*it)->imo_object()->is_note() == true );
        CHECK( (*it)->line() == 0 );
        ++it;       //(n d4 s)
        CHECK( (*it)->imo_object()->is_note() == true );
        CHECK( (*it)->line() == 0 );
        ++it;

        delete tree->get_root();
        delete pIModel;
    }

    TEST_FIXTURE(ColStaffObjsBuilderTestFixture, ColStaffObjsFullExample)
    {
        Document doc(m_libraryScope);
        LdpParser parser(cout, m_pLdpFactory);
        parser.parse_text("(lenmusdoc (vers 0.0) (content (score (vers 1.6)"
                        "(instrument (staves 2)(musicData (clef G p1)(clef F4 p2)"
                        "(key D)(time 2 4)(n f4 w p1)(goBack w)(n c3 e g+ p2)"
                        "(n c3 e g-)(n d3 q)(barline)))"
                        "(instrument (staves 2)(musicData (clef G p1)(clef F4 p2)"
                        "(key D)(time 2 4)(n f4 q. p1)(clef F4 p1)(n a3 e)"
                        "(goBack h)(n c3 q p2)(n c3 e)(clef G p2)(clef F4 p2)"
                        "(n c3 e)(barline)))  )))" );
        LdpTree* tree = parser.get_ldp_tree();
        LdpAnalyser a(cout, m_libraryScope, &doc);
        InternalModel* pIModel = a.analyse_tree(tree, "string:");
        ImoDocument* pDoc = dynamic_cast<ImoDocument*>( pIModel->get_root() );
        ImoScore* pScore = dynamic_cast<ImoScore*>( pDoc->get_content_item(0) );
        ColStaffObjsBuilder builder;
        ColStaffObjs* pColStaffObjs = builder.build(pScore);
        ColStaffObjsIterator it = pColStaffObjs->begin();
//        cout << pColStaffObjs->dump();
        CHECK( pColStaffObjs->num_lines() == 4 );
        CHECK( pColStaffObjs->num_entries() == 26 );
        CHECK( pColStaffObjs->is_anacrusis_start() == false );
        CHECK( is_equal_time( pColStaffObjs->anacrusis_missing_time(), 0.0f) == true );
        //CHECK( (*it)->to_string() == "(clef G p1)" );
        CHECK( (*it)->imo_object()->is_clef() == true );
        CHECK( (*it)->num_instrument() == 0 );
        CHECK( is_equal_time((*it)->time(), 0.0f) );
        CHECK( (*it)->line() == 0 );
        CHECK( (*it)->staff() == 0 );
        ++it;
        //CHECK( (*it)->to_string() == "(clef F4 p2)" );
        CHECK( (*it)->imo_object()->is_clef() == true );
        CHECK( (*it)->num_instrument() == 0 );
        CHECK( is_equal_time((*it)->time(), 0.0f) );
        CHECK( (*it)->line() == 1 );
        CHECK( (*it)->staff() == 1 );
        ++it;
        //CHECK( (*it)->to_string() == "(key D)" );
        CHECK( (*it)->imo_object()->is_key_signature() == true );
        CHECK( (*it)->num_instrument() == 0 );
        CHECK( is_equal_time((*it)->time(), 0.0f) );
        CHECK( (*it)->line() == 0 );
        CHECK( (*it)->staff() == 0 );
        ++it;
        //CHECK( (*it)->to_string() == "(key D)" );
        CHECK( (*it)->imo_object()->is_key_signature() == true );
        CHECK( (*it)->num_instrument() == 0 );
        CHECK( is_equal_time((*it)->time(), 0.0f) );
        CHECK( (*it)->line() == 1 );
        CHECK( (*it)->staff() == 1 );
        ++it;
        //CHECK( (*it)->to_string() == "(time 2 4)" );
        CHECK( (*it)->imo_object()->is_time_signature() == true );
        CHECK( (*it)->num_instrument() == 0 );
        CHECK( is_equal_time((*it)->time(), 0.0f) );
        CHECK( (*it)->line() == 0 );
        CHECK( (*it)->staff() == 0 );
        ++it;
        //CHECK( (*it)->to_string() == "(time 2 4)" );
        CHECK( (*it)->imo_object()->is_time_signature() == true );
        CHECK( (*it)->num_instrument() == 0 );
        CHECK( is_equal_time((*it)->time(), 0.0f) );
        CHECK( (*it)->line() == 1 );
        CHECK( (*it)->staff() == 1 );
        ++it;
        //CHECK( (*it)->to_string() == "(n f4 w p1)" );
        CHECK( (*it)->imo_object()->is_note() == true );
        CHECK( (*it)->num_instrument() == 0 );
        CHECK( is_equal_time((*it)->time(), 0.0f) );
        CHECK( (*it)->line() == 0 );
        CHECK( (*it)->staff() == 0 );
        ++it;
        //CHECK( (*it)->to_string() == "(n c3 e g+ p2)" );
        CHECK( (*it)->imo_object()->is_note() == true );
        CHECK( (*it)->num_instrument() == 0 );
        CHECK( is_equal_time((*it)->time(), 0.0f) );
        CHECK( (*it)->line() == 1 );
        CHECK( (*it)->staff() == 1 );
        ++it;
        //CHECK( (*it)->to_string() == "(clef G p1)" );
        CHECK( (*it)->imo_object()->is_clef() == true );
        CHECK( (*it)->num_instrument() == 1 );
        CHECK( is_equal_time((*it)->time(), 0.0f) );
        CHECK( (*it)->line() == 2 );
        CHECK( (*it)->staff() == 0 );
        ++it;
        //CHECK( (*it)->to_string() == "(clef F4 p2)" );
        CHECK( (*it)->imo_object()->is_clef() == true );
        CHECK( (*it)->num_instrument() == 1 );
        CHECK( is_equal_time((*it)->time(), 0.0f) );
        CHECK( (*it)->line() == 3 );
        CHECK( (*it)->staff() == 1 );
        ++it;
        //CHECK( (*it)->to_string() == "(key D)" );
        CHECK( (*it)->imo_object()->is_key_signature() == true );
        CHECK( (*it)->num_instrument() == 1 );
        CHECK( is_equal_time((*it)->time(), 0.0f) );
        CHECK( (*it)->line() == 2 );
        CHECK( (*it)->staff() == 0 );
        ++it;
        //CHECK( (*it)->to_string() == "(key D)" );
        CHECK( (*it)->imo_object()->is_key_signature() == true );
        CHECK( (*it)->num_instrument() == 1 );
        CHECK( is_equal_time((*it)->time(), 0.0f) );
        CHECK( (*it)->line() == 3 );
        CHECK( (*it)->staff() == 1 );
        ++it;
        //CHECK( (*it)->to_string() == "(time 2 4)" );
        CHECK( (*it)->imo_object()->is_time_signature() == true );
        CHECK( (*it)->num_instrument() == 1 );
        CHECK( is_equal_time((*it)->time(), 0.0f) );
        CHECK( (*it)->line() == 2 );
        CHECK( (*it)->staff() == 0 );
        ++it;
        //CHECK( (*it)->to_string() == "(time 2 4)" );
        CHECK( (*it)->imo_object()->is_time_signature() == true );
        CHECK( (*it)->num_instrument() == 1 );
        CHECK( is_equal_time((*it)->time(), 0.0f) );
        CHECK( (*it)->line() == 3 );
        CHECK( (*it)->staff() == 1 );
        ++it;
        //CHECK( (*it)->to_string() == "(n f4 q. p1)" );
        CHECK( (*it)->num_instrument() == 1 );
        CHECK( is_equal_time((*it)->time(), 0.0f) );
        CHECK( (*it)->line() == 2 );
        CHECK( (*it)->staff() == 0 );
        ++it;
        //CHECK( (*it)->to_string() == "(n c3 q p2)" );
        CHECK( (*it)->imo_object()->is_note() == true );
        CHECK( (*it)->num_instrument() == 1 );
        CHECK( is_equal_time((*it)->time(), 0.0f) );
        CHECK( (*it)->line() == 3 );
        CHECK( (*it)->staff() == 1 );
        ++it;
        //CHECK( (*it)->to_string() == "(n c3 e g-)" );
        CHECK( (*it)->imo_object()->is_note() == true );
        CHECK( (*it)->num_instrument() == 0 );
        CHECK( is_equal_time((*it)->time(), 32.0f) );
        CHECK( (*it)->line() == 1 );
        CHECK( (*it)->staff() == 1 );
        ++it;
        //CHECK( (*it)->to_string() == "(n d3 q)" );
        CHECK( (*it)->imo_object()->is_note() == true );
        CHECK( (*it)->num_instrument() == 0 );
        CHECK( is_equal_time((*it)->time(), 64.0f) );
        CHECK( (*it)->line() == 1 );
        CHECK( (*it)->staff() == 1 );
        ++it;
        //CHECK( (*it)->to_string() == "(n c3 e)" );
        CHECK( (*it)->imo_object()->is_note() == true );
        CHECK( (*it)->num_instrument() == 1 );
        CHECK( is_equal_time((*it)->time(), 64.0f) );
        CHECK( (*it)->line() == 3 );
        CHECK( (*it)->staff() == 1 );
        ++it;
        //CHECK( (*it)->to_string() == "(clef F4 p1)" );
        CHECK( (*it)->imo_object()->is_clef() == true );
        CHECK( (*it)->num_instrument() == 1 );
        CHECK( is_equal_time((*it)->time(), 96.0f) );
        CHECK( (*it)->line() == 2 );
        CHECK( (*it)->staff() == 0 );
        ++it;
        //CHECK( (*it)->to_string() == "(n a3 e)" );
        CHECK( (*it)->imo_object()->is_note() == true );
        CHECK( (*it)->num_instrument() == 1 );
        CHECK( is_equal_time((*it)->time(), 96.0f) );
        CHECK( (*it)->line() == 2 );
        CHECK( (*it)->staff() == 0 );
        ++it;
        //CHECK( (*it)->to_string() == "(clef G p2)" );
        CHECK( (*it)->imo_object()->is_clef() == true );
        CHECK( (*it)->num_instrument() == 1 );
        CHECK( is_equal_time((*it)->time(), 96.0f) );
        CHECK( (*it)->line() == 3 );
        CHECK( (*it)->staff() == 1 );
        ++it;
        //CHECK( (*it)->to_string() == "(clef F4 p2)" );
        CHECK( (*it)->imo_object()->is_clef() == true );
        CHECK( (*it)->num_instrument() == 1 );
        CHECK( is_equal_time((*it)->time(), 96.0f) );
        CHECK( (*it)->line() == 3 );
        CHECK( (*it)->staff() == 1 );
        ++it;
        //CHECK( (*it)->to_string() == "(n c3 e)" );
        CHECK( (*it)->imo_object()->is_note() == true );
        CHECK( (*it)->num_instrument() == 1 );
        CHECK( is_equal_time((*it)->time(), 96.0f) );
        CHECK( (*it)->line() == 3 );
        CHECK( (*it)->staff() == 1 );
        ++it;
        //CHECK( (*it)->to_string() == "(barline )" );
        CHECK( (*it)->imo_object()->is_barline() == true );
        CHECK( (*it)->num_instrument() == 0 );
        CHECK( is_equal_time((*it)->time(), 128.0f) );
        CHECK( (*it)->line() == 0 );
        CHECK( (*it)->staff() == 0 );
        ++it;
        //CHECK( (*it)->to_string() == "(barline )" );
        CHECK( (*it)->imo_object()->is_barline() == true );
        CHECK( (*it)->num_instrument() == 1 );
        CHECK( is_equal_time((*it)->time(), 128.0f) );
        CHECK( (*it)->line() == 2 );
        CHECK( (*it)->staff() == 0 );

        delete tree->get_root();
        delete pIModel;
    }

    TEST_FIXTURE(ColStaffObjsBuilderTestFixture, ColStaffObjsAddAnchor)
    {
        Document doc(m_libraryScope);
        LdpParser parser(cout, m_pLdpFactory);
        parser.parse_text("(lenmusdoc (vers 0.0) (content (score (vers 1.6)"
                        "(instrument (musicData (clef G)(key C)"
                        "(n f4 q)(text \"Hello world\")(barline)))  )))" );
        LdpTree* tree = parser.get_ldp_tree();
        LdpAnalyser a(cout, m_libraryScope, &doc);
        InternalModel* pIModel = a.analyse_tree(tree, "string:");
        ImoDocument* pDoc = dynamic_cast<ImoDocument*>( pIModel->get_root() );
        ImoScore* pScore = dynamic_cast<ImoScore*>( pDoc->get_content_item(0) );
        ColStaffObjsBuilder builder;
        ColStaffObjs* pColStaffObjs = builder.build(pScore);
        ColStaffObjsIterator it = pColStaffObjs->begin();
        //pColStaffObjs->dump();
        CHECK( pColStaffObjs->num_entries() == 5 );
        ++it;   //(key C)
        CHECK( (*it)->imo_object()->is_key_signature() == true );
        ++it;   //(n f4 q)
        CHECK( (*it)->imo_object()->is_note() == true );
        ++it;
        //CHECK( (*it)->to_string() == "(text \"Hello world\")" );
        CHECK( (*it)->imo_object()->is_spacer() == true );
        CHECK( (*it)->num_instrument() == 0 );
        CHECK( is_equal_time((*it)->time(), 64.0f) );
        CHECK( (*it)->line() == 0 );
        CHECK( (*it)->staff() == 0 );
        ImoSpacer* pAnchor = dynamic_cast<ImoSpacer*>( (*it)->imo_object() );
        CHECK( pAnchor != NULL );
        ++it;
        //CHECK( (*it)->to_string() == "(barline )" );
        CHECK( (*it)->imo_object()->is_barline() == true );
        CHECK( (*it)->num_instrument() == 0 );
        CHECK( is_equal_time((*it)->time(), 64.0f) );
        CHECK( (*it)->line() == 0 );
        CHECK( (*it)->staff() == 0 );

        delete tree->get_root();
        delete pIModel;
    }

    TEST_FIXTURE(ColStaffObjsBuilderTestFixture, ChordAcrossTwoStaves)
    {
        Document doc(m_libraryScope);
        LdpParser parser(cout, m_pLdpFactory);
        parser.parse_text("(lenmusdoc (vers 0.0) (content "
            "(score (vers 1.6) (instrument (staves 2)(musicData "
            "(clef G p1)(clef F4 p2)(key C)(time 2 4)(chord (n c3 w p2)(n g3 w p2)"
            "(n e4 w p1)(n c5 w p1))(barline)) )) ))" );
        LdpTree* tree = parser.get_ldp_tree();
        LdpAnalyser a(cout, m_libraryScope, &doc);
        InternalModel* pIModel = a.analyse_tree(tree, "string:");
        ImoDocument* pDoc = dynamic_cast<ImoDocument*>( pIModel->get_root() );
        ImoScore* pScore = dynamic_cast<ImoScore*>( pDoc->get_content_item(0) );
        ColStaffObjsBuilder builder;
        ColStaffObjs* pColStaffObjs = builder.build(pScore);
        CHECK( pColStaffObjs->num_lines() == 2 );
        CHECK( pColStaffObjs->num_entries() == 11 );
        CHECK( pColStaffObjs->is_anacrusis_start() == false );
//        cout << pColStaffObjs->dump();
        ColStaffObjsIterator it = pColStaffObjs->begin();
                   // (clef G p1)
        CHECK( (*it)->imo_object()->is_clef() == true );
        CHECK( (*it)->line() == 0 );
        ++it;       //(clef F4 p2)
        CHECK( (*it)->imo_object()->is_clef() == true );
        CHECK( (*it)->line() == 1 );
        ++it;       //(key C)
        CHECK( (*it)->imo_object()->is_key_signature() == true );
        CHECK( (*it)->line() == 0 );
        ++it;       //(key C)
        CHECK( (*it)->imo_object()->is_key_signature() == true );
        CHECK( (*it)->line() == 1 );
        ++it;       //(time 2 4)
        CHECK( (*it)->imo_object()->is_time_signature() == true );
        CHECK( (*it)->line() == 0 );
        ++it;       //(time 2 4)
        CHECK( (*it)->imo_object()->is_time_signature() == true );
        CHECK( (*it)->line() == 1 );
        ++it;       //(n c3 w p2)
        CHECK( (*it)->imo_object()->is_note() == true );
        CHECK( (*it)->line() == 1 );
        ++it;       //(n g3 w p2)
        CHECK( (*it)->imo_object()->is_note() == true );
        CHECK( (*it)->line() == 1 );
        ++it;       //(n e4 w p1)
        CHECK( (*it)->imo_object()->is_note() == true );
        CHECK( (*it)->line() == 0 );
        ++it;       //(n c5 w p1)
        CHECK( (*it)->imo_object()->is_note() == true );
        CHECK( (*it)->line() == 0 );
        ++it;       //(barline)
        CHECK( (*it)->imo_object()->is_barline() == true );
        CHECK( (*it)->line() == 0 );
        CHECK( is_equal_time((*it)->time(), 256.0f) );

        delete tree->get_root();
        delete pIModel;
    }

//Additional test for ColStaffObjsIterator -------------------------------------

    TEST_FIXTURE(ColStaffObjsBuilderTestFixture, CSOIteratorAtEnd)
    {
        Document doc(m_libraryScope);
        LdpParser parser(cout, m_pLdpFactory);
        parser.parse_text("(lenmusdoc (vers 0.0) (content "
            "(score (vers 1.6) (instrument (staves 2)"
            "(musicData (clef G p1)(clef F4 p2)(key D)(n c4 q v2 p1)(n d4 e.)"
            "(n d4 s v3 p2)(n e4 h)))) ))" );
        LdpTree* tree = parser.get_ldp_tree();
        LdpAnalyser a(cout, m_libraryScope, &doc);
        InternalModel* pIModel = a.analyse_tree(tree, "string:");
        ImoDocument* pDoc = dynamic_cast<ImoDocument*>( pIModel->get_root() );
        ImoScore* pScore = dynamic_cast<ImoScore*>( pDoc->get_content_item(0) );
        ColStaffObjsBuilder builder;
        ColStaffObjs* pColStaffObjs = builder.build(pScore);
        ColStaffObjsIterator it = pColStaffObjs->begin();
        //pColStaffObjs->dump();
        CHECK( pColStaffObjs->num_entries() == 8 );
        //CHECK( (*it)->to_string() == "(clef G p1)" );
        CHECK( (*it)->imo_object()->is_clef() == true );
        CHECK( (*it)->line() == 0 );
        ++it;
        //CHECK( (*it)->to_string() == "(clef F4 p2)" );
        CHECK( (*it)->imo_object()->is_clef() == true );
        CHECK( (*it)->line() == 1 );
        ++it;
        //CHECK( (*it)->to_string() == "(key D)" );
        CHECK( (*it)->imo_object()->is_key_signature() == true );
        CHECK( (*it)->line() == 0 );
        ++it;
        //CHECK( (*it)->to_string() == "(key D)" );
        CHECK( (*it)->imo_object()->is_key_signature() == true );
        CHECK( (*it)->line() == 1 );
        ++it;
        //CHECK( (*it)->to_string() == "(n c4 q v2 p1)" );
        CHECK( (*it)->imo_object()->is_note() == true );
        CHECK( (*it)->line() == 0 );
        ++it;
        //CHECK( (*it)->to_string() == "(n d4 e.)" );
        CHECK( (*it)->imo_object()->is_note() == true );
        CHECK( (*it)->line() == 0 );
        ++it;
        //CHECK( (*it)->to_string() == "(n d4 s v3 p2)" );
        CHECK( (*it)->imo_object()->is_note() == true );
        CHECK( (*it)->line() == 1 );
        ++it;
        //CHECK( (*it)->to_string() == "(n e4 h)" );
        CHECK( (*it)->imo_object()->is_note() == true );
        CHECK( (*it)->line() == 1 );
        ++it;
        CHECK( it == pColStaffObjs->end() );

        delete tree->get_root();
        delete pIModel;
    }

    TEST_FIXTURE(ColStaffObjsBuilderTestFixture, ColStaffObjs_Chord)
    {
        Document doc(m_libraryScope);
        LdpParser parser(cout, m_pLdpFactory);
        parser.parse_text("(lenmusdoc (vers 0.0) (content "
            "(score (vers 1.6) (instrument (musicData "
            "(clef G)(chord (n c4 q)(n e4 q)(n g4 q))"
            "))) ))" );
        LdpTree* tree = parser.get_ldp_tree();
        LdpAnalyser a(cout, m_libraryScope, &doc);
        InternalModel* pIModel = a.analyse_tree(tree, "string:");
        ImoDocument* pDoc = dynamic_cast<ImoDocument*>( pIModel->get_root() );
        ImoScore* pScore = dynamic_cast<ImoScore*>( pDoc->get_content_item(0) );
        ColStaffObjsBuilder builder;
        ColStaffObjs* pColStaffObjs = builder.build(pScore);
        ColStaffObjsIterator it = pColStaffObjs->begin();

        //pColStaffObjs->dump();
        CHECK( pColStaffObjs->num_entries() == 4 );

       // (clef G)
        CHECK( (*it)->imo_object()->is_clef() == true );
        CHECK( (*it)->measure() == 0 );
        CHECK( is_equal_time((*it)->time(), 0.0f) );
        ++it;

       // (n c4 q)
        CHECK( (*it)->imo_object()->is_note() == true );
        CHECK( (*it)->measure() == 0 );
        CHECK( is_equal_time((*it)->time(), 0.0f) );
        ++it;

       // (n e4 q)
        CHECK( (*it)->imo_object()->is_note() == true );
        CHECK( (*it)->measure() == 0 );
        CHECK( is_equal_time((*it)->time(), 0.0f) );
        ++it;

       // (n g4 q)
        CHECK( (*it)->imo_object()->is_note() == true );
        CHECK( (*it)->measure() == 0 );
        CHECK( is_equal_time((*it)->time(), 0.0f) );

        delete tree->get_root();
        delete pIModel;
    }

    TEST_FIXTURE(ColStaffObjsBuilderTestFixture, ColStaffObjs_NoMusicData)
    {
        Document doc(m_libraryScope);
        stringstream errormsg;
        LdpParser parser(errormsg, m_pLdpFactory);
        stringstream expected;
        expected << "Line 0. instrument: missing mandatory element 'musicData'." << endl;
        parser.parse_text("(lenmusdoc (vers 0.0) (content (score "
            "(vers 1.6) (instrument )) ))" );
        LdpTree* tree = parser.get_ldp_tree();
        LdpAnalyser a(errormsg, m_libraryScope, &doc);
        InternalModel* pIModel = a.analyse_tree(tree, "string:");
        ImoDocument* pDoc = dynamic_cast<ImoDocument*>( pIModel->get_root() );
        ImoScore* pScore = dynamic_cast<ImoScore*>( pDoc->get_content_item(0) );
        ColStaffObjsBuilder builder;
        ColStaffObjs* pColStaffObjs = builder.build(pScore);

        //cout << "[" << errormsg.str() << "]" << endl;
        //cout << "[" << expected.str() << "]" << endl;
        CHECK( errormsg.str() == expected.str() );
        CHECK( pColStaffObjs != NULL );
        CHECK( pColStaffObjs->num_entries() == 0 );

        delete tree->get_root();
        delete pIModel;
    }

    TEST_FIXTURE(ColStaffObjsBuilderTestFixture, ColStaffObjsTimeInSequenceWhenDecimals)
    {
        Document doc(m_libraryScope);
        LdpParser parser(cout, m_pLdpFactory);
        parser.parse_text("(lenmusdoc (vers 0.0) (content "
            "(score (vers 1.6) (instrument (staves 2)(musicData "
            "(n a3 w p1)(goBack start)"
            "(n f2 w p2)(barline)"
            "(n a3 q p1)"
            "(n a3 e g+ t3)(n c4 e)(n e4 e g- t-)"
            "(n a3 h)(goBack start)"
            "(n a2 h p2)"
            "(n f2 h p2)(barline)"
            ")) )))" );
        LdpTree* tree = parser.get_ldp_tree();
        LdpAnalyser a(cout, m_libraryScope, &doc);
        InternalModel* pIModel = a.analyse_tree(tree, "string:");
        ImoDocument* pDoc = dynamic_cast<ImoDocument*>( pIModel->get_root() );
        ImoScore* pScore = dynamic_cast<ImoScore*>( pDoc->get_content_item(0) );
        ColStaffObjsBuilder builder;
        ColStaffObjs* pColStaffObjs = builder.build(pScore);
        ColStaffObjsIterator it = pColStaffObjs->begin();

//        cout << pColStaffObjs->dump();
        CHECK( pColStaffObjs->num_entries() == 11 );
        CHECK( pColStaffObjs->num_lines() == 2 );

        //it        (n a3 w p1)
        CHECK( (*it)->imo_object()->is_note() == true );
        CHECK( (*it)->measure() == 0 );
        CHECK( is_equal_time((*it)->time(), 0.0f) );
        CHECK( (*it)->line() == 0 );
        ++it;       //(n f2 w p2)
        CHECK( (*it)->imo_object()->is_note() == true );
        CHECK( (*it)->measure() == 0 );
        CHECK( is_equal_time((*it)->time(), 0.0f) );
        CHECK( (*it)->line() == 1 );
        ++it;       //(barline)
        CHECK( (*it)->imo_object()->is_barline() == true );
        CHECK( (*it)->measure() == 0 );
        CHECK( is_equal_time((*it)->time(), 256.0f) );
        CHECK( (*it)->line() == 0 );
        ++it;       //(n a3 q p1)
        CHECK( (*it)->imo_object()->is_note() == true );
        CHECK( (*it)->measure() == 1 );
        CHECK( is_equal_time((*it)->time(), 256.0f) );
        CHECK( (*it)->line() == 0 );
        ++it;       //(n a2 h p2)
        CHECK( (*it)->imo_object()->is_note() == true );
        CHECK( (*it)->measure() == 1 );
        CHECK( is_equal_time((*it)->time(), 256.0f) );
        CHECK( (*it)->line() == 1 );
        ++it;       //(n a3 e g+ t3)
        CHECK( (*it)->imo_object()->is_note() == true );
        CHECK( (*it)->measure() == 1 );
        CHECK( is_equal_time((*it)->time(), 320.0f) );
        CHECK( (*it)->line() == 0 );
        ++it;       //(n c4 e)
        CHECK( (*it)->imo_object()->is_note() == true );
        CHECK( (*it)->measure() == 1 );
        CHECK( is_equal_time((*it)->time(), 341.33f) );
        CHECK( (*it)->line() == 0 );
        ++it;       //(n e4 e g- t-)
        CHECK( (*it)->imo_object()->is_note() == true );
        CHECK( (*it)->measure() == 1 );
        CHECK( is_equal_time((*it)->time(), 362.66f) );
        CHECK( (*it)->line() == 0 );
        ++it;       //(n a3 h)
        CHECK( (*it)->imo_object()->is_note() == true );
        CHECK( (*it)->measure() == 1 );
        CHECK( is_equal_time((*it)->time(), 384.0f) );
        CHECK( (*it)->line() == 0 );
        ++it;       //(n f2 h p2)
        CHECK( (*it)->imo_object()->is_note() == true );
        CHECK( (*it)->measure() == 1 );
        CHECK( is_equal_time((*it)->time(), 384.0f) );
        CHECK( (*it)->line() == 1 );
        ++it;       //(barline)
        CHECK( (*it)->imo_object()->is_barline() == true );
        CHECK( (*it)->measure() == 1 );
        CHECK( is_equal_time((*it)->time(), 512.0f) );
        CHECK( (*it)->line() == 0 );

        delete tree->get_root();
        delete pIModel;
    }

    TEST_FIXTURE(ColStaffObjsBuilderTestFixture, ColStaffObjsLineAssigned)
    {
        Document doc(m_libraryScope);
        LdpParser parser(cout, m_pLdpFactory);
        parser.parse_text("(lenmusdoc (vers 0.0) (content "
            "(score (vers 1.6) (instrument (musicData "
            "(n c4 q v1)(n d4 e.)(n d4 s v3)(n e4 h)))) ))" );
        LdpTree* tree = parser.get_ldp_tree();
        LdpAnalyser a(cout, m_libraryScope, &doc);
        InternalModel* pIModel = a.analyse_tree(tree, "string:");
        ImoDocument* pDoc = dynamic_cast<ImoDocument*>( pIModel->get_root() );
        ImoScore* pScore = dynamic_cast<ImoScore*>( pDoc->get_content_item(0) );
        ColStaffObjsBuilder builder;
        ColStaffObjs* pColStaffObjs = builder.build(pScore);
        ColStaffObjsIterator it = pColStaffObjs->begin();
        //pColStaffObjs->dump();
        CHECK( pColStaffObjs->num_entries() == 4 );
        CHECK( pColStaffObjs->num_lines() == 2 );

        //CHECK( (*it)->to_string() == "(n c4 q v1)" );
        CHECK( (*it)->imo_object()->is_note() == true );
        CHECK( (*it)->line() == 0 );
        ++it;
        //CHECK( (*it)->to_string() == "(n d4 e.)" );
        CHECK( (*it)->imo_object()->is_note() == true );
        CHECK( (*it)->line() == 0 );
        ++it;
        //CHECK( (*it)->to_string() == "(n d4 s v3)" );
        CHECK( (*it)->imo_object()->is_note() == true );
        CHECK( (*it)->line() == 1 );
        ++it;
        //CHECK( (*it)->to_string() == "(n e4 h)" );
        CHECK( (*it)->imo_object()->is_note() == true );
        CHECK( (*it)->line() == 1 );

        delete tree->get_root();
        delete pIModel;
    }

    TEST_FIXTURE(ColStaffObjsBuilderTestFixture, ColStaffObjsAssigLineToClef)
    {
        Document doc(m_libraryScope);
        LdpParser parser(cout, m_pLdpFactory);
        parser.parse_text("(lenmusdoc (vers 0.0) (content "
            "(score (vers 1.6) (instrument (musicData "
            "(clef G)(n c4 q v2)(n d4 e.)(n d4 s v3)(n e4 h)))) ))" );
        LdpTree* tree = parser.get_ldp_tree();
        LdpAnalyser a(cout, m_libraryScope, &doc);
        InternalModel* pIModel = a.analyse_tree(tree, "string:");
        ImoDocument* pDoc = dynamic_cast<ImoDocument*>( pIModel->get_root() );
        ImoScore* pScore = dynamic_cast<ImoScore*>( pDoc->get_content_item(0) );
        ColStaffObjsBuilder builder;
        ColStaffObjs* pColStaffObjs = builder.build(pScore);
        ColStaffObjsIterator it = pColStaffObjs->begin();
        //pColStaffObjs->dump();
        CHECK( pColStaffObjs->num_entries() == 5 );
        CHECK( pColStaffObjs->num_lines() == 2 );

        //CHECK( (*it)->to_string() == "(clef G)" );
        CHECK( (*it)->imo_object()->is_clef() == true );
        CHECK( (*it)->line() == 0 );
        ++it;
        //CHECK( (*it)->to_string() == "(n c4 q v2)" );
        CHECK( (*it)->imo_object()->is_note() == true );
        CHECK( (*it)->line() == 0 );
        ++it;
        //CHECK( (*it)->to_string() == "(n d4 e.)" );
        CHECK( (*it)->imo_object()->is_note() == true );
        CHECK( (*it)->line() == 0 );
        ++it;
        //CHECK( (*it)->to_string() == "(n d4 s v3)" );
        CHECK( (*it)->imo_object()->is_note() == true );
        CHECK( (*it)->line() == 1 );
        ++it;
        //CHECK( (*it)->to_string() == "(n e4 h)" );
        CHECK( (*it)->imo_object()->is_note() == true );
        CHECK( (*it)->line() == 1 );

        delete tree->get_root();
        delete pIModel;
    }

    TEST_FIXTURE(ColStaffObjsBuilderTestFixture, ColStaffObjsAssigLineToKey)
    {
        Document doc(m_libraryScope);
        LdpParser parser(cout, m_pLdpFactory);
        parser.parse_text("(lenmusdoc (vers 0.0) (content "
            "(score (vers 1.6) (instrument (staves 2)(musicData "
            "(clef G p1)(clef F4 p2)(key D)(n c4 q v2 p1)(n d4 e.)"
            "(n d4 s v3 p2)(n e4 h)))) ))" );
        LdpTree* tree = parser.get_ldp_tree();
        LdpAnalyser a(cout, m_libraryScope, &doc);
        InternalModel* pIModel = a.analyse_tree(tree, "string:");
        ImoDocument* pDoc = dynamic_cast<ImoDocument*>( pIModel->get_root() );
        ImoScore* pScore = dynamic_cast<ImoScore*>( pDoc->get_content_item(0) );
        ColStaffObjsBuilder builder;
        ColStaffObjs* pColStaffObjs = builder.build(pScore);
        ColStaffObjsIterator it = pColStaffObjs->begin();
        //pColStaffObjs->dump();
        CHECK( pColStaffObjs->num_entries() == 8 );
        CHECK( pColStaffObjs->num_lines() == 2 );

        //CHECK( (*it)->to_string() == "(clef G p1)" );
        CHECK( (*it)->imo_object()->is_clef() == true );
        CHECK( (*it)->line() == 0 );
        ++it;
        //CHECK( (*it)->to_string() == "(clef F4 p2)" );
        CHECK( (*it)->imo_object()->is_clef() == true );
        CHECK( (*it)->line() == 1 );
        ++it;
        //CHECK( (*it)->to_string() == "(key D)" );
        CHECK( (*it)->imo_object()->is_key_signature() == true );
        CHECK( (*it)->line() == 0 );
        ++it;
        //CHECK( (*it)->to_string() == "(key D)" );
        CHECK( (*it)->imo_object()->is_key_signature() == true );
        CHECK( (*it)->line() == 1 );
        ++it;
        //CHECK( (*it)->to_string() == "(n c4 q v2 p1)" );
        CHECK( (*it)->imo_object()->is_note() == true );
        CHECK( (*it)->line() == 0 );
        ++it;
        //CHECK( (*it)->to_string() == "(n d4 e.)" );
        CHECK( (*it)->imo_object()->is_note() == true );
        CHECK( (*it)->line() == 0 );
        ++it;
        //CHECK( (*it)->to_string() == "(n d4 s v3 p2)" );
        CHECK( (*it)->imo_object()->is_note() == true );
        CHECK( (*it)->line() == 1 );
        ++it;
        //CHECK( (*it)->to_string() == "(n e4 h)" );
        CHECK( (*it)->imo_object()->is_note() == true );
        CHECK( (*it)->line() == 1 );

        delete tree->get_root();
        delete pIModel;
    }

    TEST_FIXTURE(ColStaffObjsBuilderTestFixture, ColStaffObjsAssigLineToTime)
    {
        Document doc(m_libraryScope);
        LdpParser parser(cout, m_pLdpFactory);
        parser.parse_text("(lenmusdoc (vers 0.0) (content "
            "(score (vers 1.6) (instrument (staves 2)(musicData "
            "(clef G p1)(clef F4 p2)(key D)(time 2 4)(n c4 q v2 p1)"
            "(n d4 e.)(n d4 s v3 p2)(n e4 h)))) ))" );
        LdpTree* tree = parser.get_ldp_tree();
        LdpAnalyser a(cout, m_libraryScope, &doc);
        InternalModel* pIModel = a.analyse_tree(tree, "string:");
        ImoDocument* pDoc = dynamic_cast<ImoDocument*>( pIModel->get_root() );
        ImoScore* pScore = dynamic_cast<ImoScore*>( pDoc->get_content_item(0) );
        ColStaffObjsBuilder builder;
        ColStaffObjs* pColStaffObjs = builder.build(pScore);
        ColStaffObjsIterator it = pColStaffObjs->begin();
        //pColStaffObjs->dump();
        CHECK( pColStaffObjs->num_lines() == 2 );
        CHECK( pColStaffObjs->num_entries() == 10 );
                   // (clef G p1)
        ++it;       //(clef F4 p2)
        ++it;       //(key D)
        ++it;       //(key D)
        ++it;       //(time 2 4)
        CHECK( (*it)->imo_object()->is_time_signature() == true );
        CHECK( (*it)->line() == 0 );
        ++it;       //(time 2 4)
        CHECK( (*it)->imo_object()->is_time_signature() == true );
        CHECK( (*it)->line() == 1 );
        ++it;       //(n c4 q v2 p1)
        CHECK( (*it)->imo_object()->is_note() == true );
        CHECK( (*it)->line() == 0 );

        delete tree->get_root();
        delete pIModel;
    }

//    TEST_FIXTURE(ColStaffObjsBuilderTestFixture, voice_0)
//    {
//        //voice correctly assigned to second voice
//        Document doc(m_libraryScope);
//        LdpParser parser(cout, m_pLdpFactory);
//        parser.parse_text("(lenmusdoc (vers 0.0) (content "
//            "(score (vers 1.6)(instrument (musicData "
//            "(clef F4)(n e3 e g+)(n g3 e)(n c4 e g-)"
//            "(goBack start)(n c2 w)(barline)"
//            ")) )))" );
//        LdpTree* tree = parser.get_ldp_tree();
//        LdpAnalyser a(cout, m_libraryScope, &doc);
//        InternalModel* pIModel = a.analyse_tree(tree, "string:");
//        ImoDocument* pDoc = dynamic_cast<ImoDocument*>( pIModel->get_root() );
//        ImoScore* pScore = dynamic_cast<ImoScore*>( pDoc->get_content_item(0) );
//        ColStaffObjsBuilder builder;
//        ColStaffObjs* pColStaffObjs = builder.build(pScore);
//        ColStaffObjsIterator it = pColStaffObjs->begin();
//
//        //cout << pColStaffObjs->dump();
//        CHECK( pColStaffObjs->num_entries() == 6 );
//        CHECK( pColStaffObjs->num_lines() == 2 );
//
//        //             instr, staff, meas. time, line, scr
//        CHECK_ENTRY(it, 0,    0,      0,   0,     0, "(clef#21 F4 p1 )" );
//        ++it;
//        CHECK_ENTRY(it, 0,    0,      0,   0,     0, "(n#22 e3 e v1  p1 (beam 25 +))" );
//        ++it;
//        CHECK_ENTRY(it, 0,    0,      0,   0,     1, "(n#33 c2 w v2  p1 )" );
//        ++it;
//        CHECK_ENTRY(it, 0,    0,      0,  32,     0, "(n#23 g3 e v1  p1 (beam 25 =))" );
//        ++it;
//        CHECK_ENTRY(it, 0,    0,      0,  64,     0, "(n#24 c4 e v1  p1 (beam 25 -))" );
//        ++it;
//        CHECK_ENTRY(it, 0,    0,      0, 256,     0, "(barline#34 simple)" );
//
//        delete tree->get_root();
//        delete pIModel;
//    }


//    TEST_FIXTURE(ColStaffObjsBuilderTestFixture, display_colstaffobjs)
//    {
//        //test for displaying colstaffobjs table
//        Document doc(m_libraryScope);
//        LdpParser parser(cout, m_pLdpFactory);
//        parser.parse_text(
//            "(lenmusdoc (vers 0.0) (content "
//            "(score (vers 1.7)(instrument (musicData "
//            "(clef F4 p1 )(n e3 e v1 p1 (beam 37 +)(t + 3 2)(tm 2 3))"
//            "(goBack start)(n c2 w v1  p1 )(goBack 234.667)"
//            "(n g3 e v1 p1 (beam 37 =)(tm 2 3))(n c4 e v1 p1 (beam 37 -)(t -)(tm 2 3)) )))"
//            "))"
//        );
//        LdpTree* tree = parser.get_ldp_tree();
//        LdpAnalyser a(cout, m_libraryScope, &doc);
//        InternalModel* pIModel = a.analyse_tree(tree, "string:");
//        ImoDocument* pDoc = dynamic_cast<ImoDocument*>( pIModel->get_root() );
//        ImoScore* pScore = dynamic_cast<ImoScore*>( pDoc->get_content_item(0) );
//        ColStaffObjsBuilder builder;
//        ColStaffObjs* pColStaffObjs = builder.build(pScore);
//        ColStaffObjsIterator it = pColStaffObjs->begin();
//
//        cout << "Test: ColStaffObjsBuilderTestFixture, display_colstaffobjs" << endl;
//        cout << pColStaffObjs->dump();
//    }

}


