//---------------------------------------------------------------------------------------
// This file is part of the Lomse library.
// Lomse is copyrighted work (c) 2010-2016. All rights reserved.
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

#ifndef __LOMSE_SCORE_LAYOUTER_H__        //to avoid nested includes
#define __LOMSE_SCORE_LAYOUTER_H__

#include "lomse_layouter.h"
#include "lomse_basic.h"
#include "lomse_injectors.h"
#include "lomse_score_enums.h"
#include "lomse_logger.h"
#include "lomse_shapes_storage.h"
#include <vector>
using namespace std;

namespace lomse
{

//forward declarations
class InternalModel;
class FontStorage;
class GraphicModel;
class ImoContentObj;
class ImoScore;
class ImoStaffObj;
class ImoAuxObj;
class ImoInstrument;
class ImoRelObj;
class ImoAuxRelObj;
class ImoTimeSignature;
class GmoBoxScorePage;
class GmoBoxSlice;
class GmoBoxSystem;
class GmoBoxSliceInstr;
class InstrumentEngraver;
class SystemLayouter;
class StaffObjsCursor;
class GmoShape;
class ScoreMeter;
class ColumnLayouter;
class ColumnStorage;
class ColumnsBuilder;
class ShapesCreator;
class ScoreStub;
class ColumnBreaker;
class PartsEngraver;
class LyricEngraver;
class GmoShapeNote;

//---------------------------------------------------------------------------------------
// helper struct to store data about aux objs to be engraved when the system is ready
struct PendingAuxObjs
{
    ImoStaffObj* m_pSO;
    GmoShape* m_pMainShape;
    ImoInstrument* m_pInstr;
    int m_iInstr;
    int m_iStaff;
    int m_iCol;
    int m_iLine;

    PendingAuxObjs(ImoStaffObj* pSO, GmoShape* pMainShape, int iInstr, int iStaff,
                   int iCol, int iLine, ImoInstrument* pInstr)
        : m_pSO(pSO)
        , m_pMainShape(pMainShape)
        , m_pInstr(pInstr)
        , m_iInstr(iInstr)
        , m_iStaff(iStaff)
        , m_iCol(iCol)
        , m_iLine(iLine)
    {
    }

};

//---------------------------------------------------------------------------------------
// helper struct to store data about lyric shapes pending to be completed with details
struct PendingLyrics
{
    ImoStaffObj* m_pSO;
    GmoShape* m_pMainShape;
    ImoInstrument* m_pInstr;
    int m_iInstr;
    int m_iStaff;
    int m_iCol;
    int m_iLine;

    PendingLyrics(ImoStaffObj* pSO, GmoShape* pMainShape, int iInstr, int iStaff,
                  int iCol, int iLine, ImoInstrument* pInstr)
        : m_pSO(pSO)
        , m_pMainShape(pMainShape)
        , m_pInstr(pInstr)
        , m_iInstr(iInstr)
        , m_iStaff(iStaff)
        , m_iCol(iCol)
        , m_iLine(iLine)
    {
    }

};


//---------------------------------------------------------------------------------------
class ScoreLayouter : public Layouter
{
protected:
    LibraryScope&   m_libraryScope;
    ImoScore*       m_pScore;
    ScoreMeter*     m_pScoreMeter;
    ColumnsBuilder* m_pColsBuilder;
    ShapesStorage   m_shapesStorage;
    ShapesCreator*  m_pShapesCreator;
    PartsEngraver*  m_pPartsEngraver;
    UPoint          m_cursor;
    LUnits          m_startTop;

    std::vector<ColumnLayouter*> m_ColLayouters;
    std::vector<SystemLayouter*> m_sysLayouters;
    std::vector<int> m_breaks;

    //temporary data about current page being laid out
    int m_iCurPage;     //[0..n-1] current page. (-1 if no page yet created!)

    //temporary data about current system being laid out
    int m_iCurSystem;   //[0..n-1] Current system (-1 if no system yet created!)
    SystemLayouter* m_pCurSysLyt;
    int m_iSysPage;     //value of m_iCurPage when system was engraved
    UPoint m_sysCursor; //value of m_cursor when system was engraved

    int m_iCurColumn;   //[0..n-1] current column. (-1 if no column yet created!)

    //renderization options and parameters
    bool                m_fStopStaffLinesAtFinalBarline;
    bool                m_fJustifyFinalBarline;

    //space values to use
    LUnits              m_uFirstSystemIndent;
    LUnits              m_uOtherSystemIndent;

    //score stub and current boxes being laid out
    ScoreStub*          m_pStub;
    GmoBoxScorePage*    m_pCurBoxPage;
    GmoBoxSystem*       m_pCurBoxSystem;

    //support for debug and unit test
    int                 m_iColumnToTrace;
    int                 m_nTraceLevel;

public:
    ScoreLayouter(ImoContentObj* pImo, Layouter* pParent, GraphicModel* pGModel,
                  LibraryScope& libraryScope);
    virtual ~ScoreLayouter();

    void prepare_to_start_layout();
    void layout_in_box();
    void create_main_box(GmoBox* pParentBox, UPoint pos, LUnits width, LUnits height);

    //info
    virtual int get_num_columns();
    SystemLayouter* get_system_layouter(int iSys) { return m_sysLayouters[iSys]; }

    //support for helper classes
    virtual LUnits get_target_size_for_system(int iSystem);
    virtual LUnits get_main_width(int iCol);
    virtual LUnits get_trimmed_width(int iCol);
    virtual bool column_has_system_break(int iCol);
    virtual float get_column_penalty(int iCol);

    //support for debuggin and unit tests
    void dump_column_data(int iCol, ostream& outStream=dbgLogger);
    void delete_not_used_objects();
    void trace_column(int iCol, int level);

protected:
    void add_error_message(const string& msg);

    friend class ColumnsBuilder;
    friend class SystemLayouter;
    friend class PartsEngraver;

    //helpers
    inline bool is_first_page() { return m_iCurPage == 0; }
    inline LUnits get_system_indent() {
        return (m_iCurSystem == 0 ? m_uFirstSystemIndent : m_uOtherSystemIndent);
    }
    inline int get_num_systems() { return int(m_breaks.size()); }
    inline bool is_last_system() { return m_iCurSystem == get_num_systems() - 1; }
    inline bool is_first_system_in_score() { return m_iCurSystem == 0; }

    //---------------------------------------------------------------
    void initialice_score_layouter();
    void create_parts_engraver();
    void decide_systems_indentation();
    void split_content_in_columns();
    void create_stub();
    void add_score_titles();
    bool enough_space_for_empty_system();
    void create_system();
    void add_system_to_page();
    void decide_line_breaks();
    void page_initializations(GmoBox* pContainerBox);
    void decide_line_sizes();
    void fill_page_with_empty_systems_if_required();
    bool score_page_is_the_only_content_of_parent_box();
    void remove_unused_space();

    void delete_system_layouters();
    void get_score_renderization_options();

    bool m_fFirstSystemInPage;
    inline void is_first_system_in_page(bool value) { m_fFirstSystemInPage = value; }
    inline bool is_first_system_in_page() { return m_fFirstSystemInPage; }

    bool system_created();
    bool enough_space_in_page_for_system();
    void delete_system();

    //---------------------------------------------------------------
    void move_cursor_to_top_left_corner();
    void move_cursor_after_headers();
    LUnits remaining_height();
    void create_system_layouter();
    void create_system_box();
    void engrave_system();
    void create_empty_system();
    void engrave_empty_system();

    void reposition_system_if_page_has_changed();
    void move_paper_cursor_to_bottom_of_added_system();
    LUnits get_first_system_staves_size();
    LUnits get_other_systems_staves_size();

    //---------------------------------------------------------------
    LUnits determine_system_top_margin();
    LUnits determine_top_space(int nInstr, bool fFirstSystemInScore=false,
                               bool fFirstSystemInPage=false);

    void determine_staff_lines_horizontal_position(int iInstr);
    LUnits space_used_by_prolog(int iSystem);
    LUnits distance_to_top_of_system(int iSystem, bool fFirstInPage);

    std::list<PendingAuxObjs*> m_pendingAuxObjs;


    //---------------------------------------------------------------
    int get_system_containing_column(int iCol);

    void delete_column_layouters();

    bool is_system_empty(int iSystem);

};


//---------------------------------------------------------------------------------------
// ColumnsBuilder: algorithm to build the columns for one score
class ColumnsBuilder
{
protected:
    LibraryScope&   m_libraryScope;
    ScoreMeter*     m_pScoreMeter;
    ScoreLayouter*  m_pScoreLyt;
    ImoScore*       m_pScore;
    ShapesStorage&  m_shapesStorage;
    ShapesCreator*  m_pShapesCreator;
    PartsEngraver*  m_pPartsEngraver;
    std::vector<ColumnLayouter*>& m_ColLayouters;   //layouter for each column
    StaffObjsCursor* m_pSysCursor;  //cursor for traversing the score
    ColumnBreaker*  m_pBreaker;
    std::vector<LUnits> m_SliceInstrHeights;
    LUnits m_stavesHeight;      //system height without top and bottom margins
    UPoint m_pagePos;           //to track current position

    int m_iColumn;   //[0..n-1] current column. (-1 if no column yet created!)

    int m_iColumnToTrace;   //support for debug and unit test
    int m_nTraceLevel;

public:
    ColumnsBuilder(LibraryScope& libraryScope, ScoreMeter* pScoreMeter,
                   ScoreLayouter* pScoreLyt, ImoScore* pScore,
                   ShapesStorage& shapesStorage,
                   ShapesCreator* pShapesCreator,
                   std::vector<ColumnLayouter*>& colLayouters,
                   PartsEngraver* pPartsEngraver);
    ~ColumnsBuilder();


    void create_columns();
    inline LUnits get_staves_height() { return m_stavesHeight; }

    //support for debuggin and unit tests
    inline void set_debug_options(int iCol, int level) {
        m_iColumnToTrace = iCol;
        m_nTraceLevel = level;
    }

protected:
    void determine_staves_vertical_position();
    void create_column();

    void create_column_layouter();
    void create_column_boxes();
    void collect_content_for_this_column();
    void layout_column();
    void assign_width_to_column();

    GmoBoxSlice* create_slice_box();
    void find_and_save_context_info_for_this_column();

    void delete_column_storage();
    LUnits determine_initial_fixed_space();

    void store_info_about_attached_objects(ImoStaffObj* pSO, GmoShape* pShape,
                                  int iInstr, int iStaff, int iCol, int iLine,
                                  ImoInstrument* pInstr);

    //column creation
    void start_column_measurements(int iCol, LUnits uxStart, LUnits fixedSpace);
    void include_object(int iCol, int iLine, int iInstr, ImoStaffObj* pSO,
                        TimeUnits rTime, int nStaff, GmoShape* pShape, bool fInProlog=false);
    void finish_column_measurements(int iCol, LUnits xStart);
    bool determine_if_is_in_prolog(TimeUnits rTime);

    //helpers
    inline bool is_first_column() { return m_iColumn == 0; }

};


//---------------------------------------------------------------------------------------
// ColumnBreaker: algorithm to decide when to finish a column
class ColumnBreaker
{
protected:
    int m_numInstruments;
    int m_consecutiveBarlines;
    int m_numInstrWithTS;
    TimeUnits m_targetBreakTime;
    TimeUnits m_lastBarlineTime;
    TimeUnits m_maxMeasureDuration;
    TimeUnits m_lastBreakTime;

    int m_numLines;
    std::vector<TimeUnits> m_measures;
    std::vector<bool> m_beamed;
    std::vector<bool> m_tied;

public:
    ColumnBreaker(int numInstruments, StaffObjsCursor* pSysCursor);
    ~ColumnBreaker() {}

    bool feasible_break_before_this_obj(ImoStaffObj* pSO, TimeUnits rTime,
                                        int iInstr, int iLine);

protected:
    bool is_suitable_note_rest(ImoStaffObj* pSO, TimeUnits rTime);

};


//---------------------------------------------------------------------------------------
// ShapesCreator: helper fcatory to create staffobjs shapes
class ShapesCreator
{
protected:
    LibraryScope& m_libraryScope;
    ScoreMeter* m_pScoreMeter;
    ShapesStorage& m_shapesStorage;
    PartsEngraver* m_pPartsEngraver;
    map<string, LyricEngraver*> m_lyricEngravers;

public:
    ShapesCreator(LibraryScope& libraryScope, ScoreMeter* pScoreMeter,
                  ShapesStorage& shapesStorage, PartsEngraver* pPartsEngraver);
    ~ShapesCreator() {}

    enum {k_flag_small_clef=1, };

    //StaffObj shapes
    GmoShape* create_staffobj_shape(ImoStaffObj* pSO, int iInstr, int iStaff,
                                    UPoint pos, int clefType=0, unsigned flags=0);
    GmoShape* create_auxobj_shape(ImoAuxObj* pAO, int iInstr, int iStaff,
                                  GmoShape* pParentShape);
    GmoShape* create_invisible_shape(ImoObj* pSO, int iInstr, int iStaff,
                                     UPoint uPos, LUnits width);

    //RelObj shapes
    void start_engraving_relobj(ImoRelObj* pRO, ImoStaffObj* pSO,
                                GmoShape* pStaffObjShape, int iInstr, int iStaff,
                                int iSystem, int iCol, int iLine, ImoInstrument* pInstr);
    void continue_engraving_relobj(ImoRelObj* pRO, ImoStaffObj* pSO,
                                   GmoShape* pStaffObjShape, int iInstr, int iStaff,
                                   int iSystem, int iCol, int iLine,
                                   ImoInstrument* pInstr);
    void finish_engraving_relobj(ImoRelObj* pRO, ImoStaffObj* pSO,
                                 GmoShape* pStaffObjShape, int iInstr, int iStaff,
                                 int iSystem, int iCol, int iLine, LUnits prologWidth,
                                 ImoInstrument* pInstr);

    //AuxRelObj shapes
    void start_engraving_auxrelobj(ImoAuxRelObj* pARO, ImoStaffObj* pSO, const string& tag,
                                   GmoShape* pStaffObjShape, int iInstr, int iStaff,
                                   int iSystem, int iCol, int iLine, ImoInstrument* pInstr);
    void continue_engraving_auxrelobj(ImoAuxRelObj* pARO, ImoStaffObj* pSO, const string& tag,
                                   GmoShape* pStaffObjShape, int iInstr, int iStaff,
                                   int iSystem, int iCol, int iLine,
                                   ImoInstrument* pInstr);
    void finish_engraving_auxrelobj(ImoAuxRelObj* pARO, ImoStaffObj* pSO, const string& tag,
                                    GmoShape* pStaffObjShape, int iInstr, int iStaff,
                                    int iSystem, int iCol, int iLine, LUnits prologWidth,
                                    ImoInstrument* pInstr);

protected:

};


//---------------------------------------------------------------------------------------
// LinesBreaker: base class for any lines break algorithm
class LinesBreaker
{
protected:
    ScoreLayouter* m_pScoreLyt;
    std::vector<int>& m_breaks;

public:
    LinesBreaker(ScoreLayouter* pScoreLyt, std::vector<int>& breaks)
        : m_pScoreLyt(pScoreLyt)
        , m_breaks(breaks)
    {
    }
    virtual ~LinesBreaker() {}

    virtual void decide_line_breaks() = 0;
};


//---------------------------------------------------------------------------------------
// Simple lines break algorithm: just fill systems with columns
class LinesBreakerSimple : public LinesBreaker
{
public:
    LinesBreakerSimple(ScoreLayouter* pScoreLyt, std::vector<int>& breaks);
    virtual ~LinesBreakerSimple() {}

    void decide_line_breaks();
};


//---------------------------------------------------------------------------------------
// Optimal lines break algorithm, based on Knuths' algorithm for text processors
class LinesBreakerOptimal : public LinesBreaker
{
public:
    LinesBreakerOptimal(ScoreLayouter* pScoreLyt, std::vector<int>& breaks);
    virtual ~LinesBreakerOptimal() {}

    void decide_line_breaks();

    //support for debug and tests
    void dump_entries(ostream& outStream=dbgLogger);

protected:
    struct Entry
    {
        float penalty;          //total penalty for the score
        int predecessor;        //previous break point
        int system;             //system [0..n] started by this entry
        float product;          //total product of not used space
    };
    std::vector<Entry> m_entries;
    int m_numCols;
    bool m_fJustifyLastLine;

    void initialize_entries_table();
    void compute_optimal_break_sequence();
    void retrieve_breaks_sequence();
    float determine_penalty_for_line(int iSystem, int i, int j);
    bool is_better_option(float totalPenalty, float curPenalty, int i, int j);

};



}   //namespace lomse

#endif    // __LOMSE_SCORE_LAYOUTER_H__

