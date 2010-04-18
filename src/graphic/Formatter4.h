//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2010 LenMus project
//
//    This program is free software; you can redistribute it and/or modify it under the
//    terms of the GNU General Public License as published by the Free Software Foundation,
//    either version 3 of the License, or (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful, but WITHOUT ANY
//    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
//    PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License along with this
//    program. If not, see <http://www.gnu.org/licenses/>.
//
//    For any comment, suggestion or feature request, please contact the manager of
//    the project at cecilios@users.sourceforge.net
//
//-------------------------------------------------------------------------------------

#ifndef __LM_FORMATTER4_H__        //to avoid nested includes
#define __LM_FORMATTER4_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "Formatter4.cpp"
#endif

class lmBoxScore;
class lmPaper;
class lmScore;

class lmFormatter
{
public:
    lmFormatter(lmPaper* pPaper) : m_pPaper(pPaper), m_pScore((lmScore*)NULL) {}
    virtual ~lmFormatter() {}

    //measure phase
    virtual lmBoxScore* LayoutScore(lmScore* pScore)=0;

protected:
    lmPaper*        m_pPaper;       //the paper to use
    lmScore*        m_pScore;       //the score to layout

};


#include <vector>

class lmSystemFormatter;
class lmColumnFormatter;
class lmBoxSystem;
class lmBoxSliceInstr;
class lmSystemCursor;

class lmFormatter5 : public lmFormatter
{
public:
    lmFormatter5(lmPaper* pPaper);
    ~lmFormatter5();

    //measure phase
    lmBoxScore* LayoutScore(lmScore* pScore);

    //Public methods coded only for Unit Tests
#if defined(_LM_DEBUG_)

    int GetNumSystemFormatters();
    int GetNumColumns(int iSys);      //iSys=[0..n-1]
    int GetNumLines(int iSys, int iCol);    //iSys=[0..n-1], iCol=[0..n-1]
    lmSystemFormatter* GetSystemFormatter(int iSys);  //iSys=[0..n-1]

#endif


private:
    bool SizeBarColumn(int nSystem, lmBoxSystem* pBoxSystem, lmLUnits nSystemIndent);
    lmLUnits AddEmptySystem(int nSystem, lmBoxSystem* pBoxSystem);
    void RedistributeFreeSpace(lmLUnits nAvailable, bool fLastSystem);
    bool SizeBar(lmBoxSliceInstr* pBSV, lmVStaff* pVStaff, int nInstr);
    void SplitColumn(lmLUnits uAvailable);
	void AddProlog(lmBoxSliceInstr* pBSV, bool fDrawTimekey, lmVStaff* pVStaff, int nInstr);
	void AddKey(lmKeySignature* pKey, lmBox* pBox, lmVStaff* pVStaff, int nInstr, bool fProlog);
	void AddTime(lmTimeSignature* pTime, lmBox* pBox, lmVStaff* pVStaff, int nInstr, bool fProlog);
    void AddColumnToSystem();

    void Initializations();
    void AddScoreTitlesToCurrentPage();
    void PositionCursorsAfterHeaders();
    void RepositionStaffObjs();
    bool AddNewPageIfRequired();
    void CreateSystemBox(bool fFirstSystemInPage);
    void MoveCursorToTopLeftCorner();
    void GetScoreRenderizationOptions();
    void PrepareFontsThatMatchesStavesSizes();
    void DecideSystemsIndentation();
    void DecideSpaceBeforeProlog();
    void CreateSystemCursor();
    void ComputeMeasuresSizesToJustifyCurrentSystem(bool fThisIsLastSystem);
    void AddInitialLineJoiningAllStavesInSystem();
    bool CreateColumnAndAddItToCurrentSystem();
    bool FillCurrentSystemWithColumns();
    void SetCurrentSystemLenght(bool fThisIsLastSystem);
    void GetSystemHeightAndAdvancePaperCursor();
    void UpdateBoxSlicesSizes();
    bool RequestedToFillScoreWithEmptyStaves();
    void FillPageWithEmptyStaves();

    void DeleteSystemFormatters();


        // member variables


    //auxiliary data for computing and justifying systems
    std::vector<lmSystemFormatter*> m_SysFormatters;  //the formatter object for each system
    int             m_nCurSystem;               //[1..n] Current system number
    int             m_nRelColumn;               //[0..n-1] number of column in process, relative to current system
    int             m_nAbsColumn;               //[1..n] number of column in process, absolute

    lmLUnits        m_uFreeSpace;               //free space available on current system
    int             m_nColumnsInSystem;         //the number of columns in current system

    //renderization options and parameters
    bool                m_fStopStaffLinesAtFinalBarline;
    bool                m_fJustifyFinalBarline;
    float               m_rSpacingFactor;           //for proportional spacing of notes
    lmESpacingMethod    m_nSpacingMethod;           //fixed, proportional, etc.
    lmTenths            m_nSpacingValue;            //spacing for 'fixed' method

    // variables for debugging
    bool            m_fDebugMode;           //debug on/off
    long            m_nTraceMeasure;        //measure to trace. 0 = all

    //spacings to use
	lmLUnits	    m_uSpaceBeforeProlog;   //space between start of system and clef
    lmLUnits        m_uFirstSystemIndent;
    lmLUnits        m_uOtherSystemIndent;

    //new global vars
    lmSystemCursor* m_pSysCursor;

    //
    lmBoxScore*     m_pBoxScore;                //the graphical model being created
    lmBoxPage*      m_pCurrentBoxPage;
    lmBoxSystem*    m_pCurrentBoxSystem;
    int             m_nCurrentPageNumber;       //1..n. if 0 no page yet created!
    lmLUnits        m_uStartOfCurrentSystem;
    lmLUnits        m_uLastSystemHeight;

};


#endif    // __LM_FORMATTER4_H__



