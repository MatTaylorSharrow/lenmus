//---------------------------------------------------------------------------------------
// This file is part of the Lomse library.
// Copyright (c) 2010-2016 Cecilio Salmeron. All rights reserved.
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
#include "lomse_internal_model.h"
#include "lomse_im_figured_bass.h"
#include "lomse_im_note.h"
#include "lomse_time.h"
#include "lomse_document.h"
#include "lomse_im_factory.h"

using namespace UnitTest;
using namespace std;
using namespace lomse;


//---------------------------------------------------------------------------------------
class InternalModelTestFixture
{
public:
    LibraryScope m_libraryScope;
    std::string m_scores_path;

    InternalModelTestFixture()     //SetUp fixture
        : m_libraryScope(cout)
    {
        m_scores_path = TESTLIB_SCORES_PATH;
        m_libraryScope.set_default_fonts_path(TESTLIB_FONTS_PATH);
    }

    ~InternalModelTestFixture()    //TearDown fixture
    {
    }

};

//---------------------------------------------------------------------------------------
SUITE(InternalModelTest)
{

    TEST_FIXTURE(InternalModelTestFixture, EmptyDocument)
    {
        Document doc(m_libraryScope);
        ImoDocument* pDoc = static_cast<ImoDocument*>(ImFactory::inject(k_imo_document, &doc));
        pDoc->set_version("3.7");
        CHECK( pDoc->get_content() == NULL );
        CHECK( pDoc->get_num_content_items() == 0 );
        CHECK( pDoc->get_version() == "3.7" );
        delete pDoc;
    }

    TEST_FIXTURE(InternalModelTestFixture, all_imos_have_name)
    {
        bool fOk = true;
        for (int i=0; i < k_imo_last; ++i)
        {
            if (ImoObj::get_name(i) == "unknown" )
            {
                fOk = false;
                cout << "Name for ImoObj type " << i << " is not defined" << endl;
            }
        }
        CHECK (fOk);
    }

    TEST_FIXTURE(InternalModelTestFixture, DocumentWithText)
    {
        Document doc(m_libraryScope);
        doc.create_empty();
        ImoDocument* pDoc = doc.get_imodoc();
        ImoContent* pContent = pDoc->get_content();
        ImoScoreText* pText = static_cast<ImoScoreText*>(
                                    ImFactory::inject(k_imo_score_text, &doc));
        pText->set_text("Hello world");
        pContent->append_child_imo(pText);
        CHECK( pDoc->get_num_content_items() == 1 );
        CHECK( pDoc->get_content_item(0) == pText );
    }

    TEST_FIXTURE(InternalModelTestFixture, EmptyInstrument_OneStaff)
    {
        Document doc(m_libraryScope);
        ImoInstrument* pInstr = static_cast<ImoInstrument*>(
                                    ImFactory::inject(k_imo_instrument, &doc));
        CHECK( pInstr->get_musicdata() == NULL );
        CHECK( pInstr->get_num_staves() == 1 );
        CHECK( pInstr->is_in_group() == false );

        delete pInstr;
    }

    TEST_FIXTURE(InternalModelTestFixture, EmptyInstrument_AddStaff)
    {
        Document doc(m_libraryScope);
        ImoInstrument* pInstr = static_cast<ImoInstrument*>(
                                    ImFactory::inject(k_imo_instrument, &doc));
        pInstr->add_staff();
        CHECK( pInstr->get_musicdata() == NULL );
        CHECK( pInstr->get_num_staves() == 2 );
        CHECK( pInstr->is_in_group() == false );
        delete pInstr;
    }

    TEST_FIXTURE(InternalModelTestFixture, NoreDefaults)
    {
        Document doc(m_libraryScope);
        ImoNote* pNote = static_cast<ImoNote*>(ImFactory::inject(k_imo_note, &doc));
        CHECK( pNote->get_num_attachments() == 0 );
        CHECK( pNote->get_notated_accidentals() == k_no_accidentals );
        CHECK( pNote->get_dots() == 0 );
        CHECK( pNote->get_note_type() == k_quarter );
        CHECK( pNote->get_octave() == 4 );
        CHECK( pNote->get_step() == k_no_pitch );

        delete pNote;
    }

    TEST_FIXTURE(InternalModelTestFixture, Instrument_SetNumStaves)
    {
        Document doc(m_libraryScope);
        ImoInstrument* pInstr = static_cast<ImoInstrument*>(
                                    ImFactory::inject(k_imo_instrument, &doc));
        pInstr->add_staff();
        pInstr->add_staff();
        CHECK( pInstr->get_num_staves() == 3 );
        ImoStaffInfo* pStaff = pInstr->get_staff(0);
        CHECK( pStaff != NULL );
        CHECK( pStaff->get_num_lines() == 5 );
        CHECK( pStaff->get_staff_margin() == 1000.0f );
        CHECK( pStaff->get_line_spacing() == 180.0f );
        CHECK( pStaff->get_line_thickness() == 15.0f );
        CHECK( pStaff->get_height() == 735.0f );
        pStaff = pInstr->get_staff(2);
        CHECK( pStaff != NULL );
        CHECK( pStaff->get_num_lines() == 5 );
        CHECK( pStaff->get_staff_margin() == 1000.0f );
        CHECK( pStaff->get_line_spacing() == 180.0f );
        CHECK( pStaff->get_line_thickness() == 15.0f );
        CHECK( pStaff->get_height() == 735.0f );
        delete pInstr;
    }

    TEST_FIXTURE(InternalModelTestFixture, InstrumentWithContent)
    {
        Document doc(m_libraryScope);
        ImoInstrument* pInstr = static_cast<ImoInstrument*>(
                                    ImFactory::inject(k_imo_instrument, &doc));
        ImoMusicData* pMD = static_cast<ImoMusicData*>(
                                ImFactory::inject(k_imo_music_data, &doc) );
        pInstr->append_child_imo(pMD);
        CHECK( pInstr->get_musicdata() == pMD );
        CHECK( pMD->get_instrument() == pInstr );
        delete pInstr;
    }

    TEST_FIXTURE(InternalModelTestFixture, Instrument_ReplaceStaffInfo)
    {
        Document doc(m_libraryScope);
        ImoInstrument* pInstr = static_cast<ImoInstrument*>(
                                    ImFactory::inject(k_imo_instrument, &doc));
        pInstr->add_staff();
        pInstr->add_staff();
        ImoStaffInfo* pInfo = static_cast<ImoStaffInfo*>(ImFactory::inject(k_imo_staff_info, &doc));
        pInfo->set_staff_number(2);
        pInfo->set_line_spacing(400.0f);
        pInstr->replace_staff_info(pInfo);

        CHECK( pInstr->get_num_staves() == 3 );
        ImoStaffInfo* pStaff = pInstr->get_staff(2);
        CHECK( pStaff != NULL );
        CHECK( pStaff->get_num_lines() == 5 );
        CHECK( pStaff->get_staff_margin() == 1000.0f );
        CHECK( pStaff->get_line_spacing() == 400.0f );
        CHECK( pStaff->get_line_thickness() == 15.0f );
        CHECK( pStaff->get_height() == 1615.0f );
        pStaff = pInstr->get_staff(1);
        CHECK( pStaff != NULL );
        CHECK( pStaff->get_num_lines() == 5 );
        CHECK( pStaff->get_staff_margin() == 1000.0f );
        CHECK( pStaff->get_line_spacing() == 180.0f );
        CHECK( pStaff->get_line_thickness() == 15.0f );
        CHECK( pStaff->get_height() == 735.0f );
        delete pInstr;
    }

    TEST_FIXTURE(InternalModelTestFixture, ScoreInitialize)
    {
        Document doc(m_libraryScope);
        ImoScore* pScore = static_cast<ImoScore*>(ImFactory::inject(k_imo_score, &doc));
        CHECK( pScore->has_options() == true );
        CHECK( pScore->get_options()->get_num_items() == 9 );
        CHECK( pScore->get_option("Render.SpacingFactor")->get_float_value() == 0.547f );
        CHECK( pScore->get_option("Score.FillPageWithEmptyStaves")->get_bool_value() == false );
        CHECK( pScore->get_option("StaffLines.StopAtFinalBarline")->get_bool_value() == true );
        CHECK( pScore->get_option("Score.JustifyFinalBarline")->get_bool_value() == false );
        CHECK( pScore->get_option("StaffLines.Hide")->get_bool_value() == false );
        CHECK( pScore->get_option("Staff.DrawLeftBarline")->get_bool_value() == true );
        CHECK( pScore->get_option("Staff.UpperLegerLines.Displacement")->get_long_value() == 0L  );
        CHECK( pScore->get_option("Render.SpacingMethod")->get_long_value() == long(k_spacing_proportional) );
        CHECK( pScore->get_option("Render.SpacingValue")->get_long_value() == 35L );
        ImoInstruments* pColInstr = pScore->get_instruments();
        CHECK( pColInstr != NULL );
        CHECK( pColInstr->get_num_children() == 0 );
        CHECK( pScore->get_num_instruments() == 0 );
        delete pScore;
    }

    // score options --------------------------------------------------------------------

    TEST_FIXTURE(InternalModelTestFixture, ChangeFloatOption)
    {
        Document doc(m_libraryScope);
        ImoScore* pScore = static_cast<ImoScore*>(ImFactory::inject(k_imo_score, &doc));
        CHECK( pScore->has_options() == true );
        CHECK( pScore->get_options()->get_num_items() == 9 );
        CHECK( pScore->get_option("Render.SpacingFactor")->get_float_value() == 0.547f );

        pScore->set_float_option("Render.SpacingFactor", 0.600f);

        CHECK( pScore->get_options()->get_num_items() == 9 );
        CHECK( pScore->get_option("Render.SpacingFactor")->get_float_value() == 0.600f );

        delete pScore;
    }

    TEST_FIXTURE(InternalModelTestFixture, ChangeBoolOption)
    {
        Document doc(m_libraryScope);
        ImoScore* pScore = static_cast<ImoScore*>(ImFactory::inject(k_imo_score, &doc));
        CHECK( pScore->has_options() == true );
        CHECK( pScore->get_options()->get_num_items() == 9 );
        CHECK( pScore->get_option("StaffLines.Hide")->get_bool_value() == false );

        pScore->set_bool_option("StaffLines.Hide", true);

        CHECK( pScore->get_options()->get_num_items() == 9 );
        CHECK( pScore->get_option("StaffLines.Hide")->get_bool_value() == true );

        delete pScore;
    }

    TEST_FIXTURE(InternalModelTestFixture, ScoreWithBoolOption)
    {
        Document doc(m_libraryScope);
        ImoScore* pScore = static_cast<ImoScore*>(ImFactory::inject(k_imo_score, &doc));
        ImoOptionInfo* pOpt = static_cast<ImoOptionInfo*>(
                                    ImFactory::inject(k_imo_option, &doc) );
        pOpt->set_name("Staff.Green");
        pOpt->set_bool_value(true);
        CHECK( pOpt->get_bool_value() == true );
        pScore->add_or_replace_option(pOpt);
        CHECK( pScore->has_options() == true );
        ImoOptionInfo* pOpt2 = pScore->get_option("Staff.Green");
        CHECK( pOpt2 != NULL );
        CHECK( pOpt2->get_bool_value() == true );
        CHECK( pScore->get_num_instruments() == 0 );
        delete pScore;
    }

    TEST_FIXTURE(InternalModelTestFixture, ScoreWithLongOption)
    {
        Document doc(m_libraryScope);
        ImoScore* pScore = static_cast<ImoScore*>(ImFactory::inject(k_imo_score, &doc));
        ImoOptionInfo* pOpt = static_cast<ImoOptionInfo*>(
                                    ImFactory::inject(k_imo_option, &doc) );
        pOpt->set_name("Staff.Dots");
        pOpt->set_long_value(27L);
        CHECK( pOpt->get_long_value() == 27L );
        pScore->add_or_replace_option(pOpt);
        CHECK( pScore->has_options() == true );
        ImoOptionInfo* pOpt2 = pScore->get_option("Staff.Dots");
        CHECK( pOpt2 != NULL );
        CHECK( pOpt2->get_long_value() == 27L );
        CHECK( pScore->get_num_instruments() == 0 );
        delete pScore;
    }

    TEST_FIXTURE(InternalModelTestFixture, ScoreWithFloatOption)
    {
        Document doc(m_libraryScope);
        ImoScore* pScore = static_cast<ImoScore*>(ImFactory::inject(k_imo_score, &doc));
        ImoOptionInfo* pOpt = static_cast<ImoOptionInfo*>(
                                    ImFactory::inject(k_imo_option, &doc) );
        pOpt->set_name("Staff.Pi");
        pOpt->set_float_value(3.1416f);
        CHECK( pOpt->get_float_value() == 3.1416f );
        pScore->add_or_replace_option(pOpt);
        CHECK( pScore->has_options() == true );
        ImoOptionInfo* pOpt2 = pScore->get_option("Staff.Pi");
        CHECK( pOpt2 != NULL );
        CHECK( pOpt2->get_float_value() == 3.1416f );
        CHECK( pScore->get_num_instruments() == 0 );
        delete pScore;
    }

    TEST_FIXTURE(InternalModelTestFixture, ScoreAddBoolOption)
    {
        Document doc(m_libraryScope);
        ImoScore* pScore = static_cast<ImoScore*>(ImFactory::inject(k_imo_score, &doc));

        pScore->set_bool_option("Staff.Green", true);

        CHECK( pScore->has_options() == true );
        CHECK( pScore->get_options()->get_num_items() == 10 );
        CHECK( pScore->get_option("Staff.Green")->get_bool_value() == true );

        delete pScore;
    }

    TEST_FIXTURE(InternalModelTestFixture, ScoreAddLongOption)
    {
        Document doc(m_libraryScope);
        ImoScore* pScore = static_cast<ImoScore*>(ImFactory::inject(k_imo_score, &doc));

        pScore->set_long_option("Staff.LongWidth", 700L);

        CHECK( pScore->has_options() == true );
        CHECK( pScore->get_options()->get_num_items() == 10 );
        CHECK( pScore->get_option("Staff.LongWidth")->get_long_value() == 700L );

        delete pScore;
    }

    TEST_FIXTURE(InternalModelTestFixture, ScoreAddFloatOption)
    {
        Document doc(m_libraryScope);
        ImoScore* pScore = static_cast<ImoScore*>(ImFactory::inject(k_imo_score, &doc));

        pScore->set_float_option("Staff.Green", 0.66f);

        CHECK( pScore->has_options() == true );
        CHECK( pScore->get_options()->get_num_items() == 10 );
        CHECK( pScore->get_option("Staff.Green")->get_float_value() == 0.66f );

        delete pScore;
    }

    TEST_FIXTURE(InternalModelTestFixture, ScoreWithInstrument)
    {
        Document doc(m_libraryScope);
        ImoScore* pScore = static_cast<ImoScore*>(ImFactory::inject(k_imo_score, &doc));
        ImoOptionInfo* pOpt = static_cast<ImoOptionInfo*>(
                                    ImFactory::inject(k_imo_option, &doc) );
        pOpt->set_name("Staff.Green");
        pOpt->set_bool_value(true);
        pScore->add_or_replace_option(pOpt);

        ImoInstrument* pInstr = static_cast<ImoInstrument*>(
                                    ImFactory::inject(k_imo_instrument, &doc));
        ImoMusicData* pMD = static_cast<ImoMusicData*>(
                                ImFactory::inject(k_imo_music_data, &doc) );
        pInstr->append_child_imo(pMD);
        ImoClef* pClef = static_cast<ImoClef*>(ImFactory::inject(k_imo_clef, &doc));
        pClef->set_clef_type(k_clef_G2);
        ImoScoreText* pText = static_cast<ImoScoreText*>(
                                    ImFactory::inject(k_imo_score_text, &doc));
        pText->set_text("Hello world");
        pClef->add_attachment(&doc, pText);
        pMD->append_child_imo(pClef);
        pScore->add_instrument(pInstr);

        CHECK( pScore->has_options() == true );
        ImoOptionInfo* pOpt2 = pScore->get_option("Staff.Green");
        CHECK( pOpt2 != NULL );
        CHECK( pOpt2->get_bool_value() == true );
        CHECK( pScore->get_num_instruments() == 1 );
        CHECK( pScore->get_instrument(0) == pInstr );

        delete pScore;
    }


    // instruments group ----------------------------------------------------------------

    TEST_FIXTURE(InternalModelTestFixture, GroupTwoInstruments)
    {
        Document doc(m_libraryScope);
        ImoInstrGroup* pGroup = static_cast<ImoInstrGroup*>(
                                    ImFactory::inject(k_imo_instr_group, &doc));
        CHECK( pGroup->get_num_instruments() == 0 );
        ImoInstrument* pInstr1 = static_cast<ImoInstrument*>(
                                    ImFactory::inject(k_imo_instrument, &doc));
        CHECK( pInstr1->is_in_group() == false );
        CHECK( pInstr1->get_group() == NULL );

        pGroup->add_instrument(pInstr1);
        CHECK( pGroup->get_num_instruments() == 1 );
        CHECK( pGroup->get_instrument(0) == pInstr1 );
        CHECK( pInstr1->is_in_group() == true );
        CHECK( pInstr1->get_group() == pGroup );

        ImoInstrument* pInstr2 = static_cast<ImoInstrument*>(
                                    ImFactory::inject(k_imo_instrument, &doc));
        pGroup->add_instrument(pInstr2);
        CHECK( pGroup->get_num_instruments() == 2 );
        CHECK( pGroup->get_instrument(0) == pInstr1 );
        CHECK( pGroup->get_instrument(1) == pInstr2 );
        CHECK( pInstr2->is_in_group() == true );
        CHECK( pInstr2->get_group() == pGroup );

        delete pGroup;
        delete pInstr1;
        delete pInstr2;
    }

    TEST_FIXTURE(InternalModelTestFixture, ScoreWithInstrGroup)
    {
        Document doc(m_libraryScope);
        ImoScore* pScore = static_cast<ImoScore*>(
                                    ImFactory::inject(k_imo_score, &doc));
        ImoInstrGroup* pGroup = static_cast<ImoInstrGroup*>(
                                    ImFactory::inject(k_imo_instr_group, &doc));
        ImoInstrument* pInstr1 = static_cast<ImoInstrument*>(
                                    ImFactory::inject(k_imo_instrument, &doc));
        pGroup->add_instrument(pInstr1);
        ImoInstrument* pInstr2 = static_cast<ImoInstrument*>(
                                    ImFactory::inject(k_imo_instrument, &doc));
        pGroup->add_instrument(pInstr2);

        pScore->add_instruments_group(pGroup);
        CHECK( pScore->get_num_instruments() == 2 );
        CHECK( pScore->get_instrument(0) == pInstr1 );
        CHECK( pScore->get_instrument(1) == pInstr2 );

        ImoInstrument* pInstr3 = static_cast<ImoInstrument*>(
                                    ImFactory::inject(k_imo_instrument, &doc));
        pScore->add_instrument(pInstr3);

        CHECK( pScore->get_num_instruments() == 3 );
        CHECK( pScore->get_instrument(2) == pInstr3 );

        delete pScore;
    }

    TEST_FIXTURE(InternalModelTestFixture, DocumentWithScore)
    {
        Document doc(m_libraryScope);
        doc.create_empty();
        ImoDocument* pDoc = doc.get_imodoc();
        ImoContent* pContent = pDoc->get_content();

        ImoScore* pScore = static_cast<ImoScore*>(ImFactory::inject(k_imo_score, &doc));
        pContent->append_child_imo(pScore);
        ImoOptionInfo* pOpt = static_cast<ImoOptionInfo*>(
                                    ImFactory::inject(k_imo_option, &doc) );
        pOpt->set_name("Staff.Green");
        pOpt->set_bool_value(true);
        pScore->add_or_replace_option(pOpt);
        ImoInstrument* pInstr = static_cast<ImoInstrument*>(
                                    ImFactory::inject(k_imo_instrument, &doc));
        ImoMusicData* pMD = static_cast<ImoMusicData*>(
                                ImFactory::inject(k_imo_music_data, &doc) );
        pInstr->append_child_imo(pMD);
        ImoClef* pClef = static_cast<ImoClef*>(ImFactory::inject(k_imo_clef, &doc));
        pClef->set_clef_type(k_clef_G2);
        ImoScoreText* pText = static_cast<ImoScoreText*>(
                                    ImFactory::inject(k_imo_score_text, &doc));
        pText->set_text("Hello world");
        pClef->add_attachment(&doc, pText);
        pMD->append_child_imo(pClef);
        pScore->add_instrument(pInstr);

        CHECK( pDoc->get_num_content_items() == 1 );
        CHECK( pDoc->get_content_item(0) == pScore );
    }

    // ImoPageInfo ----------------------------------------------------------------------

    TEST_FIXTURE(InternalModelTestFixture, PageInfoDefaults)
    {
        Document doc(m_libraryScope);
        ImoPageInfo* pInfo = static_cast<ImoPageInfo*>(
                                    ImFactory::inject(k_imo_page_info, &doc));
        CHECK( pInfo->is_page_info() == true );
        CHECK( pInfo->get_top_margin() == 2000.0f );
        CHECK( pInfo->get_bottom_margin() == 2000.0f );
        CHECK( pInfo->get_left_margin() == 1500.0f );
        CHECK( pInfo->get_right_margin() == 1500.0f );
        CHECK( pInfo->get_binding_margin() == 0.0f );
        CHECK( pInfo->is_portrait() == true );
        delete pInfo;
    }
    // ImoTextBlockInfo -----------------------------------------------------------------------

    TEST_FIXTURE(InternalModelTestFixture, BoxInfoDefaults)
    {
        ImoTextBlockInfo info;
        CHECK( info.is_textblock_info() == true );
        CHECK( info.get_height() == 100.0f );
        CHECK( info.get_width() == 160.0f );
        CHECK( info.get_position() == TPoint(0.0f, 0.0f) );
        CHECK( is_equal(info.get_bg_color(), Color(255,255,255,255)) );
        CHECK( is_equal(info.get_border_color(), Color(0,0,0,255)) );
        CHECK( info.get_border_width() == 1.0f );
        CHECK( info.get_border_style() == k_line_solid );
    }

    // ImoCursorInfo --------------------------------------------------------------------

    TEST_FIXTURE(InternalModelTestFixture, CursorInfoDefaults)
    {
        Document doc(m_libraryScope);
        ImoCursorInfo* info = static_cast<ImoCursorInfo*>(
                                    ImFactory::inject(k_imo_cursor_info, &doc) );
        CHECK( info->is_cursor_info() == true );
        CHECK( info->get_instrument() == 0 );
        CHECK( info->get_staff() == 0 );
        CHECK( info->get_time() == 0.0f );
        CHECK( info->get_id() == -1L );
        delete info;
    }

    // ImoFiguredBassInfo ---------------------------------------------------------------

    TEST_FIXTURE(InternalModelTestFixture, FigBasInfoFromString_63)
    {
        ImoFiguredBassInfo info("6 3");
        CHECK( info.get_quality(3) == k_interval_as_implied );
        CHECK( info.get_source(3) == "3" );
        CHECK( info.get_prefix(3) == "" );
        CHECK( info.get_suffix(3) == "" );
        CHECK( info.get_over(3) == "" );
        CHECK( info.get_figured_bass_string() == "6 3" );
    }

    TEST_FIXTURE(InternalModelTestFixture, FigBasInfoFromString_5)
    {
        ImoFiguredBassInfo info("5");
        //CHECK( info.get_quality(3) == k_interval_as_implied );
        //CHECK( info.get_source(3) == "3" );
        //CHECK( info.get_prefix(3) == "" );
        //CHECK( info.get_suffix(3) == "" );
        //CHECK( info.get_over(3) == "" );
        CHECK( info.get_figured_bass_string() == "5" );
    }

    TEST_FIXTURE(InternalModelTestFixture, FigBasInfoFromString_6)
    {
        ImoFiguredBassInfo info("6");
        //CHECK( info.get_quality(3) == k_interval_as_implied );
        //CHECK( info.get_source(3) == "3" );
        //CHECK( info.get_prefix(3) == "" );
        //CHECK( info.get_suffix(3) == "" );
        //CHECK( info.get_over(3) == "" );
        CHECK( info.get_figured_bass_string() == "6" );
    }

    TEST_FIXTURE(InternalModelTestFixture, FigBasInfoFromString_64)
    {
        ImoFiguredBassInfo info("6 4");
        //CHECK( info.get_quality(3) == k_interval_as_implied );
        //CHECK( info.get_source(3) == "3" );
        //CHECK( info.get_prefix(3) == "" );
        //CHECK( info.get_suffix(3) == "" );
        //CHECK( info.get_over(3) == "" );
        CHECK( info.get_figured_bass_string() == "6 4" );
    }

    TEST_FIXTURE(InternalModelTestFixture, FigBasInfoFromString_7)
    {
        ImoFiguredBassInfo info("7");
        //CHECK( info.get_quality(3) == k_interval_as_implied );
        //CHECK( info.get_source(3) == "3" );
        //CHECK( info.get_prefix(3) == "" );
        //CHECK( info.get_suffix(3) == "" );
        //CHECK( info.get_over(3) == "" );
        CHECK( info.get_figured_bass_string() == "7" );
    }

    TEST_FIXTURE(InternalModelTestFixture, FigBasInfoFromString_7m)
    {
        ImoFiguredBassInfo info("7/");
        //CHECK( info.get_quality(3) == k_interval_as_implied );
        //CHECK( info.get_source(3) == "3" );
        //CHECK( info.get_prefix(3) == "" );
        //CHECK( info.get_suffix(3) == "" );
        //CHECK( info.get_over(3) == "" );
        CHECK( info.get_figured_bass_string() == "7/" );
    }

    TEST_FIXTURE(InternalModelTestFixture, FigBasInfoFromString_4)
    {
        ImoFiguredBassInfo info("4");   //5 4
        CHECK( info.is_sounding(3) == false );
        CHECK( info.is_sounding(5) == true );
        CHECK( info.get_figured_bass_string() == "4" );
    }

    TEST_FIXTURE(InternalModelTestFixture, FigBasInfoFromString_9)
    {
        ImoFiguredBassInfo info("9");
        //CHECK( info.get_quality(3) == k_interval_as_implied );
        //CHECK( info.get_source(3) == "3" );
        //CHECK( info.get_prefix(3) == "" );
        //CHECK( info.get_suffix(3) == "" );
        //CHECK( info.get_over(3) == "" );
        CHECK( info.get_figured_bass_string() == "9" );
    }


    // attachments ----------------------------------------------------------------------

    TEST_FIXTURE(InternalModelTestFixture, ClefNoAttachments)
    {
        Document doc(m_libraryScope);
        ImoClef* pClef = static_cast<ImoClef*>(ImFactory::inject(k_imo_clef, &doc));
        pClef->set_clef_type(k_clef_G2);
        CHECK( pClef->has_attachments() == false );

        delete pClef;
    }

    TEST_FIXTURE(InternalModelTestFixture, AddAttachment)
    {
        Document doc(m_libraryScope);
        ImoClef* pClef = static_cast<ImoClef*>(ImFactory::inject(k_imo_clef, &doc));
        pClef->set_clef_type(k_clef_G2);
        ImoScoreText* pText = static_cast<ImoScoreText*>(
                                    ImFactory::inject(k_imo_score_text, &doc));
        pText->set_text("Hello world");
        pClef->add_attachment(&doc, pText);

        CHECK( pClef->has_attachments() == true );

        delete pClef;
    }

    TEST_FIXTURE(InternalModelTestFixture, GetNumAttachments)
    {
        Document doc(m_libraryScope);
        ImoClef* pClef = static_cast<ImoClef*>(ImFactory::inject(k_imo_clef, &doc));
        pClef->set_clef_type(k_clef_G2);
        ImoScoreText* pText = static_cast<ImoScoreText*>(
                                    ImFactory::inject(k_imo_score_text, &doc));
        pText->set_text("Hello world");
        pClef->add_attachment(&doc, pText);
        ImoFermata* pFmt = static_cast<ImoFermata*>(
                                ImFactory::inject(k_imo_fermata, &doc) );
        pClef->add_attachment(&doc, pFmt);

        CHECK( pClef->get_num_attachments() == 2 );

        delete pClef;
    }

    TEST_FIXTURE(InternalModelTestFixture, GetAttachment)
    {
        Document doc(m_libraryScope);
        ImoClef* pClef = static_cast<ImoClef*>(ImFactory::inject(k_imo_clef, &doc));
        pClef->set_clef_type(k_clef_G2);

        ImoFermata* pFmt = static_cast<ImoFermata*>(
                                ImFactory::inject(k_imo_fermata, &doc) );
        pClef->add_attachment(&doc, pFmt);

        ImoScoreText* pText = static_cast<ImoScoreText*>(
                                    ImFactory::inject(k_imo_score_text, &doc));
        pText->set_text("Hello world");
        pClef->add_attachment(&doc, pText);

        CHECK( pClef->get_attachment(0) == pFmt );
        CHECK( pClef->get_attachment(1) == pText );

        delete pClef;
    }

    TEST_FIXTURE(InternalModelTestFixture, RemoveAttachment)
    {
        Document doc(m_libraryScope);
        ImoClef* pClef = static_cast<ImoClef*>(ImFactory::inject(k_imo_clef, &doc));
        pClef->set_clef_type(k_clef_G2);
        ImoScoreText* pText = static_cast<ImoScoreText*>(
                                    ImFactory::inject(k_imo_score_text, &doc));
        pText->set_text("Hello world");
        pClef->add_attachment(&doc, pText);
        ImoFermata* pFmt = static_cast<ImoFermata*>(
                                ImFactory::inject(k_imo_fermata, &doc) );
        pClef->add_attachment(&doc, pFmt);

        pClef->remove_attachment(pText);

        CHECK( pClef->get_num_attachments() == 1 );
        CHECK( pClef->get_attachment(0) == pFmt );

        delete pClef;
    }

    TEST_FIXTURE(InternalModelTestFixture, RemoveAllAttachments)
    {
        Document doc(m_libraryScope);
        ImoClef* pClef = static_cast<ImoClef*>(ImFactory::inject(k_imo_clef, &doc));
        pClef->set_clef_type(k_clef_G2);
        ImoScoreText* pText = static_cast<ImoScoreText*>(
                                    ImFactory::inject(k_imo_score_text, &doc));
        pText->set_text("Hello world");
        pClef->add_attachment(&doc, pText);
        ImoFermata* pFmt = static_cast<ImoFermata*>(
                                ImFactory::inject(k_imo_fermata, &doc) );
        pClef->add_attachment(&doc, pFmt);

        pClef->remove_attachment(pText);
        pClef->remove_attachment(pFmt);

        CHECK( pClef->get_num_attachments() == 0 );
        CHECK( pClef->get_attachments() != NULL );

        delete pClef;
    }

    TEST_FIXTURE(InternalModelTestFixture, IncludeInRelation)
    {
        Document doc(m_libraryScope);
        ImoNote* pNote = static_cast<ImoNote*>(ImFactory::inject(k_imo_note, &doc));
        pNote->set_step(1);
        pNote->set_octave(4);
        pNote->set_notated_accidentals(k_no_accidentals);
        pNote->set_note_type(k_eighth);
        pNote->set_dots(0);

        ImoTieDto dto;
        ImoTieData* pData = ImFactory::inject_tie_data(&doc, &dto);
        ImoTie* pTie = static_cast<ImoTie*>( ImFactory::inject(k_imo_tie, &doc) );
        pNote->include_in_relation(&doc, pTie, pData);

        CHECK( pNote->get_num_relations() == 1 );
        CHECK( pNote->get_relation(0) == pTie );
        CHECK( pTie->get_data_for(pNote) == pData );

        delete pNote;
    }

    TEST_FIXTURE(InternalModelTestFixture, RemoveFromRelation)
    {
        Document doc(m_libraryScope);
        ImoNote* pNote = static_cast<ImoNote*>(ImFactory::inject(k_imo_note, &doc));
        pNote->set_step(1);
        pNote->set_octave(4);
        pNote->set_notated_accidentals(k_no_accidentals);
        pNote->set_note_type(k_eighth);
        pNote->set_dots(0);

        ImoTieDto dto;
        ImoTieData* pData = ImFactory::inject_tie_data(&doc, &dto);
        ImoTie* pTie = static_cast<ImoTie*>( ImFactory::inject(k_imo_tie, &doc) );
        pNote->include_in_relation(&doc, pTie, pData);

        pNote->remove_from_relation(pTie);

        CHECK( pNote->get_num_relations() == 0 );
        CHECK( pNote->get_relations() != NULL );

        delete pNote;
    }

    TEST_FIXTURE(InternalModelTestFixture, RemoveFromAllRelations)
    {
        Document doc(m_libraryScope);
        ImoNote* pNote = static_cast<ImoNote*>(ImFactory::inject(k_imo_note, &doc));
        pNote->set_step(1);
        pNote->set_octave(4);
        pNote->set_notated_accidentals(k_no_accidentals);
        pNote->set_note_type(k_eighth);
        pNote->set_dots(0);

        ImoTieDto dto;
        ImoTieData* pData = ImFactory::inject_tie_data(&doc, &dto);
        ImoTie* pTie = static_cast<ImoTie*>( ImFactory::inject(k_imo_tie, &doc) );
        pNote->include_in_relation(&doc, pTie, pData);

        ImoBeamDto dtoBeam;
        ImoBeamData* pBeamData = ImFactory::inject_beam_data(&doc, &dtoBeam);
        ImoBeam* pBeam = static_cast<ImoBeam*>( ImFactory::inject(k_imo_beam, &doc) );
        pNote->include_in_relation(&doc, pBeam, pBeamData);

        CHECK( pNote->get_num_relations() == 2 );

        ImoRelations* pRelations = pNote->get_relations();
        pRelations->remove_from_all_relations(pNote);

        CHECK( pNote->get_num_relations() == 0 );

        delete pNote;
    }

    TEST_FIXTURE(InternalModelTestFixture, AutoDelete)
    {
        Document doc(m_libraryScope);
        ImoNote* pNote = ImFactory::inject_note(&doc, k_step_D, 4, k_eighth);

        ImoTieDto dto;
        ImoTieData* pData = ImFactory::inject_tie_data(&doc, &dto);
        ImoTie* pTie = static_cast<ImoTie*>( ImFactory::inject(k_imo_tie, &doc) );
        pNote->include_in_relation(&doc, pTie, pData);

        pTie->remove_all();
        delete pTie;

        CHECK( pNote->get_num_attachments() == 0 );

        delete pNote;
    }

    TEST_FIXTURE(InternalModelTestFixture, RelationsOrdered)
    {
        //@ Relations must be rendered in a predefined order,
        //@ i.e. beams before tuplets. For this, they must be stored in
        //@ renderization order.

        Document doc(m_libraryScope);
        ImoNote* pNote = ImFactory::inject_note(&doc, k_step_A, 4, k_eighth);

        ImoTieDto dtoTie;
        ImoTieData* pTieData = ImFactory::inject_tie_data(&doc, &dtoTie);
        ImoTie* pTie = static_cast<ImoTie*>( ImFactory::inject(k_imo_tie, &doc) );
        pNote->include_in_relation(&doc, pTie, pTieData);

        ImoBeamDto dtoBeam;
        ImoBeamData* pBeamData = ImFactory::inject_beam_data(&doc, &dtoBeam);
        ImoBeam* pBeam = static_cast<ImoBeam*>( ImFactory::inject(k_imo_beam, &doc) );
        pNote->include_in_relation(&doc, pBeam, pBeamData);

        CHECK( pNote->get_relation(0) == pTie );
        CHECK( pNote->get_relation(1) == pBeam );
//        cout << "order = " << pNote->get_relation(0)->get_obj_type() << ", "
//             << pNote->get_relation(1)->get_obj_type() <<  endl;

        delete pNote;
    }


    // ImoNote --------------------------------------------------------------------------

    TEST_FIXTURE(InternalModelTestFixture, Note_ConstructorDefaults)
    {
        Document doc(m_libraryScope);
        ImoNote* pNote = ImFactory::inject_note(&doc, k_step_A, 4, k_eighth);

        CHECK( pNote->get_step() == k_step_A );
        CHECK( pNote->get_octave() == 4 );
        CHECK( pNote->get_note_type() == k_eighth );
        CHECK( pNote->get_dots() == 0 );
        CHECK( pNote->get_voice() == 0 );
        CHECK( pNote->get_staff() == 0 );
        CHECK( pNote->get_duration() == 32.0f );
        CHECK( pNote->get_notated_accidentals() == k_no_accidentals );
        CHECK( pNote->get_stem_direction() == k_stem_default );

        delete pNote;
    }

    TEST_FIXTURE(InternalModelTestFixture, Note_ConstructorFull)
    {
        Document doc(m_libraryScope);
        ImoNote* pNote = ImFactory::inject_note(&doc, k_step_A, 4, k_eighth, k_flat, 1, 2, 3, k_stem_up);

        CHECK( pNote->get_step() == k_step_A );
        CHECK( pNote->get_octave() == 4 );
        CHECK( pNote->get_note_type() == k_eighth );
        CHECK( pNote->get_dots() == 1 );
        CHECK( pNote->get_voice() == 3 );
        CHECK( pNote->get_staff() == 2 );
        CHECK( pNote->get_duration() == 48.0f );
        CHECK( pNote->get_notated_accidentals() == k_flat );
        CHECK( pNote->get_stem_direction() == k_stem_up );

        delete pNote;
    }

    // ImoStyle -----------------------------------------------------------------

    TEST_FIXTURE(InternalModelTestFixture, Score_GetDefaultStyle)
    {
        Document doc(m_libraryScope);
        ImoScore* pScore = static_cast<ImoScore*>(ImFactory::inject(k_imo_score, &doc));
        ImoStyle* pStyle = pScore->get_default_style();

        CHECK( pStyle != NULL );
        CHECK( pStyle->get_name() == "Default style" );
        CHECK( is_equal(pStyle->color(), Color(0,0,0,255)) );
        CHECK( pStyle->font_file() == "" );
        CHECK( pStyle->font_name() == "Liberation serif" );
        CHECK( pStyle->font_style() == ImoStyle::k_font_style_normal );
        CHECK( pStyle->font_weight() == ImoStyle::k_font_weight_normal );
        CHECK( pStyle->font_size() == 12.0f );
        CHECK( pStyle->line_height() == 1.5f );
        CHECK( pStyle->min_height() == 0.0f );
        CHECK( pStyle->max_height() == 0.0f );
        CHECK( pStyle->height() == 0.0f );

        delete pScore;
    }

    TEST_FIXTURE(InternalModelTestFixture, Score_GetStyle)
    {
        Document doc(m_libraryScope);
        ImoScore* pScore = static_cast<ImoScore*>(ImFactory::inject(k_imo_score, &doc));
	    ImoStyle* pStyle = static_cast<ImoStyle*>(ImFactory::inject(k_imo_style, &doc));
        pStyle->set_name("Test style");
	    pStyle->font_name( "Callamet");
        pStyle->font_size( 12.0f);
        pStyle->font_style( ImoStyle::k_font_style_normal);
        pStyle->font_weight( ImoStyle::k_font_weight_bold);
        pStyle->color( Color(15,16,27,132) );
        pScore->add_style(pStyle);

        ImoStyle* pStyle2 = pScore->find_style("Test style");
        CHECK( pStyle == pStyle2 );
        delete pScore;
    }

    TEST_FIXTURE(InternalModelTestFixture, DocumentHasDefaultStyle)
    {
        Document doc(m_libraryScope);
        doc.create_empty();
        ImoDocument* pDoc = doc.get_imodoc();
        ImoStyle* pStyle = pDoc->get_style();

        CHECK( pStyle != NULL );
        CHECK( pStyle->get_name() == "Default style" );
        CHECK( is_equal(pStyle->color(), Color(0,0,0,255)) );
        CHECK( pStyle->font_name() == "Liberation serif" );
        CHECK( pStyle->font_style() == ImoStyle::k_font_style_normal );
        CHECK( pStyle->font_weight() == ImoStyle::k_font_weight_normal );
        CHECK( pStyle->font_size() == 12.0f );
    }

    TEST_FIXTURE(InternalModelTestFixture, ObjectInheritsParentStyle)
    {
        Document doc(m_libraryScope);
        doc.from_string("(lenmusdoc (vers 0.0)(content "
            "(para (txt \"hello\")) ))" );
        ImoDocument* pDoc = doc.get_imodoc();
        ImoContent* pContent = pDoc->get_content();
        TreeNode<ImoObj>::children_iterator it = pContent->begin();
        CHECK( (*it)->is_paragraph() == true );
        ImoContentObj* pImo = dynamic_cast<ImoContentObj*>(*it);
        ImoStyle* pStyle = pImo->get_style();
        CHECK( pStyle != NULL );
        CHECK( pStyle->get_name() == "Default style" );
        CHECK( is_equal(pStyle->color(), Color(0,0,0,255)) );
        CHECK( pStyle->font_name() == "Liberation serif" );
        CHECK( pStyle->font_style() == ImoStyle::k_font_style_normal );
        CHECK( pStyle->font_weight() == ImoStyle::k_font_weight_normal );
        CHECK( pStyle->font_size() == 12.0f );
    }

    TEST_FIXTURE(InternalModelTestFixture, ObjectInheritsParentStyleAttributes)
    {
        Document doc(m_libraryScope);
        doc.from_string("(lenmusdoc (vers 0.0)"
            "(styles "
                "(defineStyle \"Default style\" (margin-bottom 300))"
                "(defineStyle \"para\" (margin-top 400))"
            ")"
            "(content "
                "(para (style \"para\")(txt \"hello\"))"
            "))" );
        ImoDocument* pDoc = doc.get_imodoc();
        ImoContent* pContent = pDoc->get_content();
        TreeNode<ImoObj>::children_iterator it = pContent->begin();
        CHECK( (*it)->is_paragraph() == true );
        ImoContentObj* pPara = dynamic_cast<ImoContentObj*>(*it);
        CHECK( pPara->margin_top() == 400 );
        CHECK( pPara->margin_bottom() == 300 );
    }

    TEST_FIXTURE(InternalModelTestFixture, PrivateStyle_overrides_values)
    {
        Document doc(m_libraryScope);
        doc.create_empty();
        ImoDocument* pDoc = doc.get_imodoc();
        ImoStyle* pStyle = pDoc->create_private_style();

        CHECK( pStyle != NULL );
        CHECK( pStyle->font_size() == 12.0f );

        pStyle->font_size( 21.0f) ;
        CHECK( pStyle->font_size() == 21.0f );
    }


    // ImoArticulationSymbol ------------------------------------------------------------

    TEST_FIXTURE(InternalModelTestFixture, articulation_symbol_0)
    {
        //0. defaults ok

        Document doc(m_libraryScope);
        ImoArticulationSymbol* pAccent = static_cast<ImoArticulationSymbol*>(
                            ImFactory::inject(k_imo_articulation_symbol, &doc));

        CHECK( pAccent->get_articulation_type() == k_articulation_unknown );
        CHECK( pAccent->get_placement() == k_placement_default );
        CHECK( pAccent->is_articulation() == true );
        CHECK( pAccent->is_articulation_symbol() == true );
        CHECK( pAccent->is_auxobj() == true );

        delete pAccent;
    }

    TEST_FIXTURE(InternalModelTestFixture, articulation_symbol_1)
    {
        //1. settings

        Document doc(m_libraryScope);
        ImoArticulationSymbol* pAccent = static_cast<ImoArticulationSymbol*>(
                            ImFactory::inject(k_imo_articulation_symbol, &doc));
        pAccent->set_articulation_type(k_articulation_accent);
        pAccent->set_placement(k_placement_below);

        CHECK( pAccent->get_articulation_type() == k_articulation_accent );
        CHECK( pAccent->get_placement() == k_placement_below );

        delete pAccent;
    }


    // ImoArticulationLine --------------------------------------------------------------

    TEST_FIXTURE(InternalModelTestFixture, articulation_line_0)
    {
        //0. defaults ok

        Document doc(m_libraryScope);
        ImoArticulationLine* pDoit = static_cast<ImoArticulationLine*>(
                                    ImFactory::inject(k_imo_articulation_line, &doc));

        CHECK( pDoit->get_articulation_type() == k_articulation_unknown );
        CHECK( pDoit->get_placement() == k_placement_default );
        CHECK( pDoit->get_line_shape() == k_line_shape_straight );
        CHECK( pDoit->get_line_type() == k_line_type_solid );
        CHECK( pDoit->get_dash_length() == 4.0 );
        CHECK( pDoit->get_dash_space() == 2.0 );

        CHECK( pDoit->is_articulation() == true );
        CHECK( pDoit->is_articulation_line() == true );
        CHECK( pDoit->is_auxobj() == true );

        delete pDoit;
    }

    TEST_FIXTURE(InternalModelTestFixture, articulation_line_1)
    {
        //1. settings

        Document doc(m_libraryScope);
        ImoArticulationLine* pDoit = static_cast<ImoArticulationLine*>(
                                    ImFactory::inject(k_imo_articulation_line, &doc));
        pDoit->set_articulation_type(k_articulation_doit);
        pDoit->set_placement(k_placement_below);
        pDoit->set_line_shape(k_line_shape_curved);
        pDoit->set_line_type(k_line_type_dotted);
        pDoit->set_dash_length(6.0);
        pDoit->set_dash_space(2.5);

        CHECK( pDoit->get_articulation_type() == k_articulation_doit );
        CHECK( pDoit->get_placement() == k_placement_below );
        CHECK( pDoit->get_line_shape() == k_line_shape_curved );
        CHECK( pDoit->get_line_type() == k_line_type_dotted );
        CHECK( pDoit->get_dash_length() == 6.0 );
        CHECK( pDoit->get_dash_space() == 2.5 );

        delete pDoit;
    }

//    TEST_FIXTURE(InternalModelTestFixture, articulation_2)
//    {
//        //2. Attach to note
//
//        Document doc(m_libraryScope);
//        ImoNote* pNote = ImFactory::inject_note(&doc, k_step_A, 4, k_eighth);
//
//        ImoArticulation* pAccent = static_cast<ImoArticulation*>(
//                                    ImFactory::inject(k_imo_articulation, &doc));
//        pAccent->set_articulation_type(k_articulation_accent);
//
//        pNote->add_attachment(&doc, pText);
//        ImoFermata* pFmt = static_cast<ImoFermata*>(
//                                ImFactory::inject(k_imo_fermata, &doc) );
//        pClef->add_attachment(&doc, pFmt);
//
//        CHECK( pClef->get_num_attachments() == 2 );
//
//        delete pClef;
//    }


    // ImoDynamicsMark ------------------------------------------------------------------

    TEST_FIXTURE(InternalModelTestFixture, dynamics_mark_0)
    {
        //0. defaults ok

        Document doc(m_libraryScope);
        ImoDynamicsMark* pDM = static_cast<ImoDynamicsMark*>(
                            ImFactory::inject(k_imo_dynamics_mark, &doc));

        CHECK( pDM->get_mark_type() == "" );
        CHECK( pDM->get_placement() == k_placement_default );
        CHECK( pDM->is_dynamics_mark() == true );
        CHECK( pDM->is_auxobj() == true );

        delete pDM;
    }

    TEST_FIXTURE(InternalModelTestFixture, dynamics_mark_1)
    {
        //1. settings

        Document doc(m_libraryScope);
        ImoDynamicsMark* pDM = static_cast<ImoDynamicsMark*>(
                            ImFactory::inject(k_imo_dynamics_mark, &doc));
        pDM->set_mark_type("sfz");
        pDM->set_placement(k_placement_below);

        CHECK( pDM->get_mark_type() == "sfz" );
        CHECK( pDM->get_placement() == k_placement_below );

        delete pDM;
    }


    // ImoOrnament ----------------------------------------------------------------------

    TEST_FIXTURE(InternalModelTestFixture, ornament_0)
    {
        //0. defaults ok

        Document doc(m_libraryScope);
        ImoOrnament* pOrnament = static_cast<ImoOrnament*>(
                            ImFactory::inject(k_imo_ornament, &doc));

        CHECK( pOrnament->get_ornament_type() == k_ornament_unknown );
        CHECK( pOrnament->get_placement() == k_placement_default );
        CHECK( pOrnament->is_ornament() == true );
        CHECK( pOrnament->is_auxobj() == true );

        delete pOrnament;
    }

    TEST_FIXTURE(InternalModelTestFixture, ornament_1)
    {
        //1. settings

        Document doc(m_libraryScope);
        ImoOrnament* pOrnament = static_cast<ImoOrnament*>(
                            ImFactory::inject(k_imo_ornament, &doc));
        pOrnament->set_ornament_type(k_ornament_turn);
        pOrnament->set_placement(k_placement_below);

        CHECK( pOrnament->get_ornament_type() == k_ornament_turn );
        CHECK( pOrnament->get_placement() == k_placement_below );

        delete pOrnament;
    }


    // ImoTechnical ----------------------------------------------------------------------

    TEST_FIXTURE(InternalModelTestFixture, technical_0)
    {
        //0. defaults ok

        Document doc(m_libraryScope);
        ImoTechnical* pTechnical = static_cast<ImoTechnical*>(
                            ImFactory::inject(k_imo_technical, &doc));

        CHECK( pTechnical->get_technical_type() == k_technical_unknown );
        CHECK( pTechnical->get_placement() == k_placement_default );
        CHECK( pTechnical->is_technical() == true );
        CHECK( pTechnical->is_auxobj() == true );

        delete pTechnical;
    }

//    TEST_FIXTURE(InternalModelTestFixture, technical_1)
//    {
//        //1. settings
//
//        Document doc(m_libraryScope);
//        ImoTechnical* pTechnical = static_cast<ImoTechnical*>(
//                            ImFactory::inject(k_imo_technical, &doc));
//        pTechnical->set_technical_type(k_technical_turn);
//        pTechnical->set_placement(k_placement_below);
//
//        CHECK( pTechnical->get_technical_type() == k_technical_turn );
//        CHECK( pTechnical->get_placement() == k_placement_below );
//
//        delete pTechnical;
//    }

    // API ------------------------------------------------------------------------------

    TEST_FIXTURE(InternalModelTestFixture, API_AddParagraphToDocument)
    {
        Document doc(m_libraryScope);
        doc.create_empty();

        doc.add_paragraph();

        ImoDocument* pImoDoc = doc.get_imodoc();
        ImoContent* pContent = pImoDoc->get_content();
        TreeNode<ImoObj>::children_iterator it = pContent->begin();
        CHECK( (*it)->is_paragraph() == true );
        CHECK( pContent->get_num_children() == 1 );
    }

    TEST_FIXTURE(InternalModelTestFixture, API_AddParagraphToContent)
    {
        Document doc(m_libraryScope);
        doc.create_empty();
        ImoDocument* pImoDoc = doc.get_imodoc();
        ImoContent* pContent = pImoDoc->get_content();

        pContent->add_paragraph();

        TreeNode<ImoObj>::children_iterator it = pContent->begin();
        CHECK( (*it)->is_paragraph() == true );
        CHECK( pContent->get_num_children() == 1 );
    }

    TEST_FIXTURE(InternalModelTestFixture, API_AddTextItemToParagraph)
    {
        Document doc(m_libraryScope);
        doc.create_empty();
        ImoParagraph* pPara = doc.add_paragraph();

        pPara->add_text_item("hello world");

        TreeNode<ImoObj>::children_iterator it = pPara->begin();
        CHECK( (*it)->is_text_item() == true );
        ++it;
        CHECK( it == pPara->end() );
    }

    TEST_FIXTURE(InternalModelTestFixture, API_AddScore)
    {
        Document doc(m_libraryScope);
        doc.create_empty();

        doc.add_score();

        ImoDocument* pImoDoc = doc.get_imodoc();
        ImoContent* pContent = pImoDoc->get_content();
        CHECK( pContent->get_num_children() == 1 );
        TreeNode<ImoObj>::children_iterator it = pContent->begin();
        CHECK( (*it)->is_score() == true );
        ImoScore* pScore = static_cast<ImoScore*>(*it);
        CHECK( pScore->get_num_instruments() == 0 );
    }

    TEST_FIXTURE(InternalModelTestFixture, API_AddInstrument)
    {
        Document doc(m_libraryScope);
        doc.create_empty();
        ImoScore* pScore = doc.add_score();

        pScore->add_instrument();

        ImoDocument* pImoDoc = doc.get_imodoc();
        ImoContent* pContent = pImoDoc->get_content();
        CHECK( pContent->get_num_children() == 1 );
        CHECK( pScore->get_num_instruments() == 1 );
    }

    TEST_FIXTURE(InternalModelTestFixture, API_AddObjects)
    {
        Document doc(m_libraryScope);
        doc.create_empty();
        ImoScore* pScore = doc.add_score();
        ImoInstrument* pInstr = pScore->add_instrument();
        pInstr->add_clef(k_clef_G2);
        pInstr->add_key_signature(k_key_C);
        pInstr->add_time_signature(4 ,4);
        ImoMusicData* pMD = pInstr->get_musicdata();
        CHECK( pMD->get_num_items() == 3 );

        pInstr->add_staff_objects("(n c4 q)(n d4 q)(n e4 q)(n f4 q)");

        CHECK( pMD->get_num_items() == 7 );
    }

    // ImoMultiColumn -------------------------------------------------------------------

    TEST_FIXTURE(InternalModelTestFixture, MultiColumn_creates_columns)
    {
        Document doc(m_libraryScope);
        ImoMultiColumn* pMC = ImFactory::inject_multicolumn(&doc);

        CHECK( pMC != NULL );
        CHECK( pMC->is_multicolumn() == true );
        CHECK( pMC->get_num_columns() == 0 );

        delete pMC;
    }

    TEST_FIXTURE(InternalModelTestFixture, MultiColumn_initial_width)
    {
        Document doc(m_libraryScope);
        doc.create_empty();
        ImoMultiColumn* pMC = doc.add_multicolumn_wrapper(3);

        CHECK( pMC->get_num_columns() == 3 );
        CHECK( pMC->get_column_width(0) == 100.0f/3.0f );   //in percentage 33.3%
        CHECK( pMC->get_column_width(1) == 100.0f/3.0f );
        CHECK( pMC->get_column_width(2) == 100.0f/3.0f );
    }

    // ImoWidget -------------------------------------------------------------------------

//    TEST_FIXTURE(InternalModelTestFixture, Panel_create)
//    {
//        Document doc(m_libraryScope);
//        ImoWidget* pImo = static_cast<ImoWidget*>( ImFactory::inject(k_imo_widget, &doc) );
//
//        CHECK( pImo != NULL );
//        CHECK( pImo->is_imo_widget() == true );
////        CHECK( pImo->get_size() == USize(0.0f, 0.0f) );
//
//        delete pImo;
//    }

//    TEST_FIXTURE(InternalModelTestFixture, Panel_add_to_block_api)
//    {
//        Document doc(m_libraryScope);
//        doc.create_empty();
//        ImoWidget* pImo = doc.add_widget((1000.0f, 600.0f);
//
//        CHECK( pImo != NULL );
//        CHECK( pImo->is_imo_widget() == true );
//        CHECK( pImo->get_size() == USize(1000.0f, 600.0f) );
//    }

    //TEST_FIXTURE(InternalModelTestFixture, Panel_add_child)
    //{
    //    Document doc(m_libraryScope);
    //    doc.create_empty();
    //    ImoWidget* pImo = doc.add_panel(1000.0f, 600.0f);
    //
    //    pImo->add()

    //    CHECK( pImo != NULL );
    //    CHECK( pImo->is_imo_widget() == true );
    //    CHECK( pImo->get_size() == USize(1000.0f, 600.0f) );
    //}

    // dirty bits -----------------------------------------------------------------------

    TEST_FIXTURE(InternalModelTestFixture, dirty_bit_initially_set)
    {
        //AWARE: score and instrument can not be used directly in these tests as, at
        //creation, also some children are automatically created

        Document doc(m_libraryScope);
        doc.create_empty();
        ImoScore* pScore = doc.add_score();
        ImoInstrument* pInstr = pScore->add_instrument();
        ImoClef* pClef = pInstr->add_clef(k_clef_G2);

        CHECK( pScore->is_dirty() == true );
        CHECK( pScore->are_children_dirty() == true );
        CHECK( pClef->is_dirty() == true );
        CHECK( pClef->are_children_dirty() == false );
        CHECK( doc.is_dirty() == true );
    }

    TEST_FIXTURE(InternalModelTestFixture, clear_dirty)
    {
        //clear dirty clears both: dirty and children dirty. But does'n propagate down
        Document doc(m_libraryScope);
        doc.create_empty();
        ImoScore* pScore = doc.add_score();
        ImoInstrument* pInstr = pScore->add_instrument();
        ImoClef* pClef = pInstr->add_clef(k_clef_G2);

        pScore->set_dirty(false);
        CHECK( pScore->is_dirty() == false );
        CHECK( pScore->are_children_dirty() == true );  //set dirty(false) doesn't propagate down
        CHECK( pClef->is_dirty() == true );
        CHECK( doc.is_dirty() == true );

        doc.clear_dirty();

        CHECK( pClef->is_dirty() == true );
        CHECK( doc.is_dirty() == false );
    }

    TEST_FIXTURE(InternalModelTestFixture, set_dirty_propagates_up)
    {
        Document doc(m_libraryScope);
        doc.create_empty();
        ImoScore* pScore = doc.add_score();
        pScore->set_dirty(false);
        ImoInstrument* pInstr = pScore->add_instrument();
        ImoClef* pClef = pInstr->add_clef(k_clef_G2);
        ImoMusicData* pMD = pInstr->get_musicdata();
        pScore->set_dirty(false);
        pInstr->set_dirty(false);
        pMD->set_dirty(false);
        pClef->set_dirty(false);
        doc.clear_dirty();

        ImoKeySignature* pKey = pInstr->add_key_signature(k_key_C);

        CHECK( pKey->is_dirty() == true );
        CHECK( pKey->are_children_dirty() == false );
        CHECK( pMD->is_dirty() == true );
        CHECK( pMD->are_children_dirty() == false );
        CHECK( pInstr->is_dirty() == false );
        CHECK( pInstr->are_children_dirty() == true );
        CHECK( pScore->is_dirty() == false );
        CHECK( pScore->are_children_dirty() == true );
        CHECK( pScore->are_children_dirty() == true );
        CHECK( doc.is_dirty() == true );
    }

    // ImoTextItem ----------------------------------------------------------------------

    TEST_FIXTURE(InternalModelTestFixture, TextItem_default_language)
    {
        Document doc(m_libraryScope);
        doc.create_empty();
        ImoDocument* pDoc = doc.get_imodoc();
        pDoc->set_language("zh_CN");
        ImoParagraph* pPara = doc.add_paragraph();
        ImoTextItem* pText = pPara->add_text_item("编辑名称，缩写，MIDI设置和其他特性");

        CHECK( pText->get_language() == "zh_CN" );
    }

    // ImoStaffObj ----------------------------------------------------------------------

    TEST_FIXTURE(InternalModelTestFixture, staffobj_has_instrument)
    {
        Document doc(m_libraryScope);
        doc.create_empty();
        ImoScore* pScore = doc.add_score();
        ImoInstrument* pInstr = pScore->add_instrument();
        ImoClef* pClef = pInstr->add_clef(k_clef_G2);

        CHECK( pClef->get_instrument() == pInstr );
    }

    TEST_FIXTURE(InternalModelTestFixture, staffobj_attributes)
    {
        Document doc(m_libraryScope);
        doc.create_empty();
        ImoScore* pScore = doc.add_score();
        ImoInstrument* pInstr = pScore->add_instrument();
        ImoClef* pClef = pInstr->add_clef(k_clef_G2);
        ImoStaffObj* pImo = static_cast<ImoStaffObj*>(pClef);

        list<int> supported = pImo->get_supported_attributes();

        //cout << "staffobj supported = " << supported.size() << endl;
        CHECK( supported.size() == 6 );
    }

    // ImoBarline -----------------------------------------------------------------------

    TEST_FIXTURE(InternalModelTestFixture, barline_attributes)
    {
        Document doc(m_libraryScope);
        ImoBarline* pImo = static_cast<ImoBarline*>(ImFactory::inject(k_imo_barline, &doc));

        list<int> supported = pImo->get_supported_attributes();

        //cout << "barline supported = " << supported.size() << endl;
        CHECK( supported.size() == 5 );
//        CHECK( supported.find(k_attr_barline) == ? );

        delete pImo;
    }

}


