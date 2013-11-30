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
#include "lomse_config.h"

//classes related to these tests
#include "lomse_injectors.h"
#include "lomse_ldp_factory.h"
#include "lomse_document.h"
#include "lomse_staffobjs_table.h"
#include "lomse_internal_model.h"
#include "lomse_score_layouter.h"
#include "lomse_staffobjs_cursor.h"
#include "lomse_im_note.h"
#include "lomse_time.h"

using namespace UnitTest;
using namespace std;
using namespace lomse;


class ColumnBreakerTestFixture
{
public:
    LibraryScope m_libraryScope;
    std::string m_scores_path;

    ColumnBreakerTestFixture()     //SetUp fixture
        : m_libraryScope(cout)
    {
        m_scores_path = TESTLIB_SCORES_PATH;
        m_libraryScope.set_default_fonts_path(TESTLIB_FONTS_PATH);
    }

    ~ColumnBreakerTestFixture()    //TearDown fixture
    {
    }

    void find_next_break(ColumnBreaker& breaker, StaffObjsCursor& soCursor)
    {
        while(!soCursor.is_end() )
        {
            ImoStaffObj* pSO = soCursor.get_staffobj();
//            int iInstr = soCursor.num_instrument();
//            int iStaff = soCursor.staff();
            int iLine = soCursor.line();
            TimeUnits rTime = soCursor.time();

            if ( breaker.column_should_be_finished(pSO, rTime, iLine) )
                break;

            soCursor.move_next();
        }
    }

};

SUITE(ColumnBreakerTest)
{

    TEST_FIXTURE(ColumnBreakerTestFixture, OneInstrument_AfterBarline)
    {
        //@ in one staff scores, break after barlines

        Document doc(m_libraryScope);
        doc.from_string("(lenmusdoc (vers 0.0) (content (score (vers 1.6) "
            "(instrument (musicData (clef G)(n c4 q)(barline)(n d4 q)(barline)"
            "(n e4 q) ))) ))" );

        ImoScore* pScore = static_cast<ImoScore*>( doc.get_imodoc()->get_content_item(0) );
        StaffObjsCursor soCursor(pScore);

        ColumnBreaker breaker(pScore->get_num_instruments(), &soCursor);
        find_next_break(breaker, soCursor);

//        cout << pScore->get_staffobjs_table()->dump() << endl;
//        cout << "cursor id=" << soCursor.imo_object()->get_id() << endl;
        CHECK( soCursor.imo_object()->get_id() == 24L );
        CHECK( is_equal_time(soCursor.time(), 64.0f) );
    }

    TEST_FIXTURE(ColumnBreakerTestFixture, OneInstrument_AfterEndOfBeam)
    {
        //@ in one staff scores, break after end-of beam notes

        Document doc(m_libraryScope);
        doc.from_string("(lenmusdoc (vers 0.0) (content (score (vers 1.6) "
            "(instrument (musicData "
            "(clef G)(key C)(n a4 s g+)(n g4 s)(n f4 s)(n e4 s g-)"
            "(n d4 q)(n b4 q)(n c4 q)"
            ")) )))" );

        ImoScore* pScore = static_cast<ImoScore*>( doc.get_imodoc()->get_content_item(0) );
        StaffObjsCursor soCursor(pScore);

        ColumnBreaker breaker(pScore->get_num_instruments(), &soCursor);
        find_next_break(breaker, soCursor);

//        cout << pScore->get_staffobjs_table()->dump() << endl;
//        cout << "cursor id=" << soCursor.imo_object()->get_id() << endl;
        CHECK( soCursor.imo_object()->get_id() == 36L );
        CHECK( is_equal_time(soCursor.time(), 64.0f) );
    }

    TEST_FIXTURE(ColumnBreakerTestFixture, OneInstrument_AfterNotBeamedNoterest)
    {
        //@ in one staff scores, break after each not beamed note and rest

        Document doc(m_libraryScope);
        doc.from_string("(lenmusdoc (vers 0.0) (content (score (vers 1.6) "
            "(instrument (musicData "
            "(clef G)(key C)(n a4 s)(r s)(n f4 s)(n e4 s)"
            "(n d4 q)(n b4 q)(n c4 q)"
            ")) )))" );

        ImoScore* pScore = static_cast<ImoScore*>( doc.get_imodoc()->get_content_item(0) );
        StaffObjsCursor soCursor(pScore);

        ColumnBreaker breaker(pScore->get_num_instruments(), &soCursor);
        find_next_break(breaker, soCursor);

//        cout << pScore->get_staffobjs_table()->dump() << endl;
//        cout << "cursor id=" << soCursor.imo_object()->get_id() << endl;
        CHECK( soCursor.imo_object()->get_id() == 24L );
        CHECK( is_equal_time(soCursor.time(), 16.0f) );
    }

    TEST_FIXTURE(ColumnBreakerTestFixture, CommonOnsetPoint)
    {
        //@ in multi-staves scores, at common beats

        Document doc(m_libraryScope);
        doc.from_string("(lenmusdoc (vers 0.0) (content (score (vers 1.6) "
            "(instrument (staves 2) (musicData "
            "(clef G p1)(clef F4 p2)(key A-)"
            "(n -a4 w v1 p1)(n a4 h v1)(n xa4 h v1)(n a4 q v1)"
            "(n a4 q v1)(n +a4 e g+ v1)(n a4 e g- v1)(n a4 s g+ v1)"
            "(n a4 s g- v1)(n a4 t g+ v1)(n a4 t g- v1)"
            "(goBack start)(n f3 w v2 p2)(n +f3 h v2)(n f3 h v2)"
            "(n -f3 q v2)(n f3 q v2)(n f3 e g+ v2)(n +f3 e g- v2)"
            "(n f3 s g+ v2)(n f3 s g- v2)(n f3 t g+ v2)(n f3 t g- v2)"
            ")) )))" );

        ImoScore* pScore = static_cast<ImoScore*>( doc.get_imodoc()->get_content_item(0) );
        StaffObjsCursor soCursor(pScore);

        ColumnBreaker breaker(pScore->get_num_instruments(), &soCursor);
        find_next_break(breaker, soCursor);

//        cout << pScore->get_staffobjs_table()->dump() << endl;
//        cout << "cursor id=" << soCursor.imo_object()->get_id() << endl;
        CHECK( soCursor.imo_object()->get_id() == 25L );
        CHECK( is_equal_time(soCursor.time(), 256.0f) );
    }

    TEST_FIXTURE(ColumnBreakerTestFixture, DoNotBreakNotes)
    {
        //@ in multi-staves scores, take duration of maximum note/rest

        Document doc(m_libraryScope);
        doc.from_string("(lenmusdoc (vers 0.0) (content (score (vers 1.6) "
            "(instrument (staves 2) (musicData (clef G p1)(clef F4 p2)(key C)"
            "(n c4 h p1)(n d4 q p1)(goBack start)(n c3 q p2)(n d3 q p2)(n e3 q p2)"
            ")) )))" );

        ImoScore* pScore = static_cast<ImoScore*>( doc.get_imodoc()->get_content_item(0) );
        StaffObjsCursor soCursor(pScore);
        //cout << pScore->get_staffobjs_table()->dump() << endl;

        ColumnBreaker breaker(pScore->get_num_instruments(), &soCursor);
        find_next_break(breaker, soCursor);

//        cout << pScore->get_staffobjs_table()->dump() << endl;
//        cout << "cursor id=" << soCursor.imo_object()->get_id() << endl;
        CHECK( soCursor.imo_object()->get_id() == 25L );
        CHECK( is_equal_time(soCursor.time(), 128.0f) );
    }

    TEST_FIXTURE(ColumnBreakerTestFixture, DoNotBreakBeam_1)
    {
        //@ in multi-staves scores, do not break beams

        Document doc(m_libraryScope);
        doc.from_string("(lenmusdoc (vers 0.0) (content (score (vers 1.6) "
            "(instrument (staves 2) (musicData (clef G p1)(clef F4 p2)(key C)"
            "(n c4 e p1 g+)(n d4 s p1)(n e4 s p1 g-)(n f4 s p1 g+)(n g4 s p1 g-)"
            "(goBack start)(n c3 q p2)(n d3 q p2)"
            ")) )))" );

        ImoScore* pScore = static_cast<ImoScore*>( doc.get_imodoc()->get_content_item(0) );
        StaffObjsCursor soCursor(pScore);

        ColumnBreaker breaker(pScore->get_num_instruments(), &soCursor);
        find_next_break(breaker, soCursor);

//        cout << pScore->get_staffobjs_table()->dump() << endl;
//        cout << "cursor id=" << soCursor.imo_object()->get_id() << endl;
        CHECK( soCursor.imo_object()->get_id() == 34L );
        CHECK( is_equal_time(soCursor.time(), 64.0f) );
    }

    TEST_FIXTURE(ColumnBreakerTestFixture, DoNotBreakBeam_2)
    {
        //@ in multi-staves scores, do not break beams

        Document doc(m_libraryScope);
        doc.from_string("(lenmusdoc (vers 0.0) (content (score (vers 1.6) "
            "(instrument (staves 2) (musicData (clef G p1)(clef F4 p2)(key C)"
            "(n c4 s p1 g+)(n d4 s p1)(n e4 s p1)(n f4 s p1 g-)(n g4 q p1)"
            "(goBack start)(n c3 e p2)(r e p2)(n e3 e p2)(n g3 e p2 g+)(n b3 e p2 g-)"
            ")) )))" );

        ImoScore* pScore = static_cast<ImoScore*>( doc.get_imodoc()->get_content_item(0) );
        StaffObjsCursor soCursor(pScore);

        ColumnBreaker breaker(pScore->get_num_instruments(), &soCursor);
        find_next_break(breaker, soCursor);

//        cout << pScore->get_staffobjs_table()->dump() << endl;
//        cout << "cursor id=" << soCursor.imo_object()->get_id() << endl;
        CHECK( soCursor.imo_object()->get_id() == 37L );
        CHECK( is_equal_time(soCursor.time(), 64.0f) );
    }

    TEST_FIXTURE(ColumnBreakerTestFixture, Multimetrics_DoNotBreakBeam_3)
    {
        //@ in multi-staves scores, do not break beams

        Document doc(m_libraryScope);
        doc.from_string("(lenmusdoc (vers 0.0) (content (score (vers 1.6) "
            "(instrument (musicData (clef G)(key G)(time 3 4)"
            "(n g5 s g+)(n f5 s)(n g5 e g-)(barline)(n g5 q) ))"
            "(instrument (musicData (clef G)(key G)(time 2 4)"
            "(n b5 e g+)(n a5 s)(n g5 s g-)(n g5 e g+)(n g5 e g-) ))"
            ")))" );

        ImoScore* pScore = static_cast<ImoScore*>( doc.get_imodoc()->get_content_item(0) );
        StaffObjsCursor soCursor(pScore);

        ColumnBreaker breaker(pScore->get_num_instruments(), &soCursor);
        find_next_break(breaker, soCursor);

//        cout << pScore->get_staffobjs_table()->dump() << endl;
//        cout << "cursor id=" << soCursor.imo_object()->get_id() << endl;
        CHECK( soCursor.imo_object()->get_id() == 35L );
        CHECK( is_equal_time(soCursor.time(), 64.0f) );
    }

    TEST_FIXTURE(ColumnBreakerTestFixture, NotBeforeBarline)
    {
        //@ in multi-staves scores, do not breakk just before barline

        Document doc(m_libraryScope);
        doc.from_string("(lenmusdoc (vers 0.0) (content (score (vers 1.6) "
            "(instrument (musicData (clef G)(key C)"
            "(n c4 e g+)(n d4 e g-)(barline)(n e4 q) ))"
            "(instrument (musicData (clef F4)(key C)"
            "(n c3 q)(barline)(n d3 q) ))"
            ")))" );

        ImoScore* pScore = static_cast<ImoScore*>( doc.get_imodoc()->get_content_item(0) );
        StaffObjsCursor soCursor(pScore);

        ColumnBreaker breaker(pScore->get_num_instruments(), &soCursor);
        find_next_break(breaker, soCursor);

//        cout << pScore->get_staffobjs_table()->dump() << endl;
//        cout << "cursor id=" << soCursor.imo_object()->get_id() << endl;
        CHECK( soCursor.imo_object()->get_id() == 31L );
        CHECK( is_equal_time(soCursor.time(), 64.0f) );
    }

    TEST_FIXTURE(ColumnBreakerTestFixture, Multimetrics_Ok)
    {
        //@ test a multimetric score

        Document doc(m_libraryScope);
        doc.from_string("(lenmusdoc (vers 0.0) (content (score (vers 1.6) "
            "(instrument (musicData "
            "(clef G)(key G)(time 3 4)(chord (n g3 q)(n d4 q))"
            "(r e)(n g5 e)(n g5 s g+)(n f5 s)(n g5 e g-)(barline)"
            "(r h.)(barline)"
            "(chord (n g3 h)(n d4 h))(chord (n g3 q)(n d4 q))(barline)"
            "(chord (n a4 q)(n e5 q))(r q)"
            "(chord (n d4 q)(n g4 q)(n f5 q))(barline) ))"
            "(instrument (musicData "
            "(clef G)(key G)(time 2 4)"
            "(n g4 q)(n d5 e g+)(n d5 e g-)(barline)"
            "(n b5 e g+)(n a5 s)(n g5 s g-)"
            "(n g5 e g+)(n g5 e g-)(barline)"
            "(n e5 e g+)(n d5 s)(n c5 s g-)"
            "(n e5 e g+)(n e5 e g-)(barline)"
            "(n d5 e g+)(n g5 e)(n g5 e)(n g5 e g-)(barline)"
            "(n b5 e g+)(n g5 e g-)"
            "(n e5 s g+)(n c5 s)(n e5 s)(n c5 s g-)(barline)"
            "(n b4 s g+)(n d5 s)(n b4 s)(n d5 s g-)"
            "(n c5 e g+)(n b4 s)(n a4 s g-)(barline) ))"
            ")))" );

        ImoScore* pScore = static_cast<ImoScore*>( doc.get_imodoc()->get_content_item(0) );
        StaffObjsCursor soCursor(pScore);
//        cout << pScore->get_staffobjs_table()->dump() << endl;

        ColumnBreaker breaker1(pScore->get_num_instruments(), &soCursor);
        find_next_break(breaker1, soCursor);
        CHECK( soCursor.imo_object()->get_id() == 29L );
        CHECK( is_equal_time(soCursor.time(), 64.0f) );

        ColumnBreaker breaker2(pScore->get_num_instruments(), &soCursor);
        find_next_break(breaker2, soCursor);
        CHECK( soCursor.imo_object()->get_id() == 31L );
        CHECK( is_equal_time(soCursor.time(), 128.0f) );

        ColumnBreaker breaker3(pScore->get_num_instruments(), &soCursor);
        find_next_break(breaker3, soCursor);
        CHECK( soCursor.imo_object()->get_id() == 42L );
        CHECK( is_equal_time(soCursor.time(), 192.0f) );

        ColumnBreaker breaker4(pScore->get_num_instruments(), &soCursor);
        find_next_break(breaker4, soCursor);
        CHECK( soCursor.imo_object()->get_id() == 101L );
        CHECK( is_equal_time(soCursor.time(), 256.0f) );

        ColumnBreaker breaker5(pScore->get_num_instruments(), &soCursor);
        find_next_break(breaker5, soCursor);
        CHECK( soCursor.imo_object()->get_id() == 111L );
        CHECK( is_equal_time(soCursor.time(), 320.0f) );

        ColumnBreaker breaker6(pScore->get_num_instruments(), &soCursor);
        find_next_break(breaker6, soCursor);
        CHECK( soCursor.imo_object()->get_id() == 45L );
        CHECK( is_equal_time(soCursor.time(), 384.0f) );

        ColumnBreaker breaker7(pScore->get_num_instruments(), &soCursor);
        find_next_break(breaker7, soCursor);
        CHECK( soCursor.imo_object()->get_id() == 50L );
        CHECK( is_equal_time(soCursor.time(), 512.0f) );

        ColumnBreaker breaker8(pScore->get_num_instruments(), &soCursor);
        find_next_break(breaker8, soCursor);
        CHECK( soCursor.imo_object()->get_id() == 56L );
        CHECK( is_equal_time(soCursor.time(), 576.0f) );

        ColumnBreaker breaker9(pScore->get_num_instruments(), &soCursor);
        find_next_break(breaker9, soCursor);
        CHECK( soCursor.imo_object()->get_id() == 60L );
        CHECK( is_equal_time(soCursor.time(), 640.0f) );

        ColumnBreaker breaker10(pScore->get_num_instruments(), &soCursor);
        find_next_break(breaker10, soCursor);
        CHECK( soCursor.imo_object()->get_id() == 62L );
        CHECK( is_equal_time(soCursor.time(), 704.0f) );
    }

////    TEST_FIXTURE(ColumnBreakerTestFixture, DumpStaffobsTable)
////    {
////        //@ auxiliary, for dumping table in scores with problems
////
////        Document doc(m_libraryScope);
////        doc.from_file(m_scores_path + "00201-systems-are-justified.lms");
////        ImoScore* pScore = static_cast<ImoScore*>( doc.get_imodoc()->get_content_item(0) );
////        cout << pScore->get_staffobjs_table()->dump() << endl;
////    }

}


