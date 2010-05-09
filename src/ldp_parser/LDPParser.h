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

#ifndef __LM_LDPPARSER_H        //to avoid nested includes
#define __LM_LDPPARSER_H

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "LDPParser.cpp"
#endif

#include <vector>
#include <set>

#include <wx/txtstrm.h>
#include <wx/wfstream.h>
#include "LDPNode.h"
#include "LDPToken.h"
#include "../score/Score.h"
#include "../score/FiguredBass.h"

//#include <iostream>
//#include <fstream>
using namespace std;

class lmInstrGroup;
class lmTieInfo;
class lmBeamInfo;
class lmFBLineInfo;

#if lmUSE_LIBRARY

#include "lenmus_elements.h"
using namespace lenmus;

#endif


enum lmETagLDP
{
	lm_eTag_Visible = 0,
	lm_eTag_Location_x,
	lm_eTag_Location_y,
	lm_eTag_StaffNum,
	//
	lm_eTag_Max				//to know the number of tags defined
};



//The parser
class lmLDPParser
{
public:
    lmLDPParser();
    ~lmLDPParser();

    //setings and options
    inline void SetIgnoreList(std::set<long>* pSet) { m_pIgnoreSet = pSet; }

    //std::map<long, lmScoreObj*>     m_ScoreObjs;

    //"Parse" methods work on source text
    lmScore*    ParseFile(const wxString& filename, bool fErrorMsg = true);
    lmLDPNode*  ParseText(const wxString& sSource);
    lmScore*    ParseScoreFromText(const wxString& sSource, bool fErrorMsg = true);

    //"Parse" methods work on source text (std::string)
    lmLDPNode*  ParseText(const std::string& sSource);

#if lmUSE_LIBRARY
    lmScore*    GenerateScoreFromDocument(Document* pDoc);
    lmScore*    ParseFile(const std::string& filename, Document* pDoc);
    lmScore*    ParseScoreFromText(const std::string& sSource,
                                   Document* pDoc = NULL,
                                   bool fErrorMsg = true);
#endif

    // "Analyze" methods: work on a tree of lmNodes generated by ParseText().
    // User is responsible for calling the appropiate Analyze method, depending on
    // the type of text parsed

    lmFiguredBass* AnalyzeFiguredBass(lmLDPNode* pNode, lmVStaff* pVStaff);
    void        AnalyzeMusicData(lmLDPNode* pNode, lmVStaff* pVStaff);
    lmNote*     AnalyzeNote(lmLDPNode* pNode, lmVStaff* pVStaff, bool fChord=false);
    lmStaffObj* AnalyzeStaffObj(lmLDPNode* pNode, lmVStaff* pVStaff);
    bool        AnalyzeText(lmLDPNode* pNode, lmVStaff* pVStaff,
                               lmStaffObj* pTarget = (lmStaffObj*)NULL);

    //for LDPOptionalTags
    void AnalysisError(lmLDPNode* pNode, const wxChar* szFormat, ...);
    int         AnalyzeNumStaff(const wxString& sNotation, lmLDPNode* pNode, long nNumStaves);
    void        AnalyzeLocation(lmLDPNode* pNode, lmLocation* pPos);

    //for lmLDPTokenBuilder
    const wxString& GetNewBuffer();
    void ParseMsje(wxString sMsg);
    inline wxString& GetFilename() { return m_sFileName; }

    bool ParenthesisMatch(const wxString& sSource);

    //for ScoreCanvas
    static float GetDefaultDuration(lmENoteType nNoteType, int nDots, int nActualNotes,
                                    int nNormalNotes);

protected:
    lmBarline*  AnalyzeBarline(lmLDPNode* pNode, lmVStaff* pVStaff);
    void        AnalyzeChord(lmLDPNode* pNode, lmVStaff* pVStaff);
    bool        AnalyzeClef(lmVStaff* pVStaff, lmLDPNode* pNode);
    lmEPlacement AnalyzeFermata(lmLDPNode* pNode, lmVStaff* pVStaff, lmLocation* pPos);
    void        AnalyzeFont(lmLDPNode* pNode, lmFontInfo* pFont);
    void        AnalyzeGraphicObj(lmLDPNode* pNode, lmVStaff* pVStaff);
    int         AnalyzeGroup(lmLDPNode* pNode, lmScore* pScore, int nInstr);
    void        AnalyzeInstrument105(lmLDPNode* pNode, lmScore* pScore, int nInstr,
                                     lmInstrGroup* pGroup = (lmInstrGroup*)NULL );
    bool        AnalyzeKeySignature(lmLDPNode* pNode, lmVStaff* pVStaff);
    void        AnalyzeLocation(lmLDPNode* pNode, float* pValue, lmEUnits* pUnit);
    bool        AnalyzeMetronome(lmLDPNode* pNode, lmVStaff* pVStaff);
    bool        AnalyzeNewSystem(lmLDPNode* pNode, lmVStaff* pVStaff);
    lmNoteRest* AnalyzeNoteRest(lmLDPNode* pNode, lmVStaff* pVStaff, bool fChord=false);
    void        AnalyzeOption(lmLDPNode* pNode, lmScoreObj* pObject);
	lmRest*     AnalyzeRest(lmLDPNode* pNode, lmVStaff* pVStaff);
    bool        AnalyzeSize(lmLDPNode* pNode, lmTenths* ptWidth, lmTenths* ptHeight);
    void        AnalyzeSpacer(lmLDPNode* pNode, lmVStaff* pVStaff);
    lmEStemType AnalyzeStem(lmLDPNode* pNode, lmVStaff* pVStaff);
    void        AnalyzeTextbox(lmLDPNode* pNode, lmVStaff* pVStaff, lmStaffObj* pTarget);
    bool        AnalyzeTitle(lmLDPNode* pNode, lmScore* pScore);
    void        AnalyzeTimeShift(lmLDPNode* pNode, lmVStaff* pStaff);
    bool        AnalyzeTimeSignature(lmVStaff* pVStaff, lmLDPNode* pNode);

	//analyze options
	int			AnalyzeVoiceNumber(const wxString& sNotation, lmLDPNode* pNode);



    // auxiliary methods
    int GetBeamingLevel(lmENoteType nNoteType);


protected:

#if lmUSE_LIBRARY

    ofstream*   m_reporter;
    LdpTree*    m_lastTree;

    void open_reporter();
    void display_errors(const wxString& title);
    void delete_last_tree();

#else

    enum EParsingStates
    {
        A0_WaitingForStartOfElement = 0,
        A1_WaitingForName,
        A2_WaitingForParameter,
        A3_ProcessingParameter,
        A4_Exit,
        A5_ExitError
    };

    lmLDPNode*  LexicalAnalysis();
    void PushNode(EParsingStates nPopState);
    bool PopNode();
    void Do_WaitingForStartOfElement();
    void Do_WaitingForName();
    void Do_WaitingForParameter();
    void Do_ProcessingParameter();
    void ParseError(EParsingStates nState, lmLDPToken* pTk);

    lmLDPTokenBuilder*  m_pTokenizer;       //ptr to token builder object
    lmLDPToken*         m_pTk;              //current token
    EParsingStates      m_nState;           //current automata state
    wxArrayInt          m_stackStates;      //returned node after PopNode
    std::vector<lmLDPNode*> m_StackNodes;   //satck of nodes
    lmLDPNode*          m_pCurNode;         //node in process
#endif

    void        AnalyzeAnchorLine(lmLDPNode* pNode, lmLocation* ptPos, lmTenths* ptWidth,
                                  lmELineStyle* pLineStyle, lmELineCap* pEndStyle,
                                  wxColour* pColor);
    void        AnalyzeAttachments(lmLDPNode* pNode, lmVStaff* pVStaff,
                                   lmLDPNode* pX, lmStaffObj* pAnchor);
    lmBeamInfo* AnalyzeBeam(lmLDPNode* pNode, lmVStaff* pVStaff);
    void        AnalyzeBezier(lmLDPNode* pNode, lmTPoint* pPoints);
    bool        AnalyzeBezierLocation(lmLDPNode* pNode, lmTPoint* pPoints);
    bool        AnalyzeBorder(lmLDPNode* pNode, lmTenths* ptWidth, lmELineStyle* pLineStyle,
                              wxColour* pColor);
    wxColour    AnalyzeColor(lmLDPNode* pNode);
    bool        AnalyzeCreationMode(lmLDPNode* pNode, lmScore* pScore);
    bool        AnalyzeCursor(lmLDPNode* pNode, lmScore* pScore);
    bool        AnalyzeDefineStyle(lmLDPNode* pNode, lmScore* pScore);
    lmFBLineInfo* AnalyzeFBLine(lmLDPNode* pNode, lmVStaff* pVStaff);
	bool		AnalyzeInfoMIDI(lmLDPNode* pNode, int* pChannel, int* pNumInstr);
    void        AnalyzeLine(lmLDPNode* pNode, lmVStaff* pVStaff, lmStaffObj* pTarget);
    void        AnalyzeLocationPoint(lmLDPNode* pNode, lmLocation* pPos);
    bool        AnalyzeNoteType(wxString& sNoteType, lmENoteType* pnNoteType, int* pNumDots);
	bool		AnalyzePageLayout(lmLDPNode* pNode, lmScore* pScore);
	bool		AnalyzeSystemLayout(lmLDPNode* pNode, lmScore* pScore);
    lmScore*    AnalyzeScore(lmLDPNode* pNode);
    lmScore*    AnalyzeScoreV105(lmLDPNode* pNode);
    void        AnalyzeStaff(lmLDPNode* pNode, lmVStaff* pVStaff);
    bool        AnalyzeTextString(lmLDPNode* pNode, wxString* pText, wxString* pStyle,
                                  lmEHAlign* pAlign, lmLocation* pPos, 
                                  lmFontInfo* pFont);
    lmTieInfo*  AnalyzeTie(lmLDPNode* pNode, lmVStaff* pVStaff);
    bool        AnalyzeTimeExpression(const wxString& sData, lmLDPNode* pNode, float* pValue);
    bool        AnalyzeTuplet(lmLDPNode* pNode, const wxString& sParent, bool fOpenAllowed,
                              bool fCloseAllowed,
                              lmTupletBracket** pTuplet, int* pActual, int* pNormal);
    void        AnalyzeUndoData(lmLDPNode* pNode);

    bool		GetFloatNumber(lmLDPNode* pNode, wxString& sValue, wxString& nodeName,
                               float* pValue);
    lmTextStyle* GetTextStyle(lmLDPNode* pNode, const wxString& sStyle);

    bool GetValueLineCap(lmLDPNode* pNode, lmELineCap* pEndStyle);
    bool GetValueFloatNumber(lmLDPNode* pNode, float* pValue, int iP = 1, float rDefault = 0.0f);
    bool GetValueIntNumber(lmLDPNode* pNode, int* pValue, int iP = 1, int nDefault = 0);
    bool GetValueLongNumber(lmLDPNode* pNode, long* pValue, int iP = 1, long nDefault = 0L);
    bool GetValueLineStyle(lmLDPNode* pNode, lmELineStyle* pLineStyle);
    bool GetValueYesNo(lmLDPNode* pNode, bool fDefault);

    //auxiliary
    void AddBeam(lmNoteRest* pNR, lmBeamInfo* pBeamInfo);
    void AddTie(lmNote* pNote, lmTieInfo* pTieInfo);
    void AddFBLine(lmFiguredBass* pFB, lmFBLineInfo* pFBLineInfo);

    void Clear();
    void FileParsingError(const wxString& sMsg);

    long GetNodeID(lmLDPNode* pNode);

    lmScore* CreateScore(lmLDPNode* pRoot, bool fShowErrorLog = true);


    // variables to store temporary values as the file is being analized.
    lmScore*            m_pScore;       //the score that is being created
    int					m_nCurStaff;	//default staff num. for the lmNoteRest being processed
    int					m_nCurVoice;	//default voice num. for the lmNoteRest being processed
    lmTupletBracket*	m_pTuplet;      //tuplet being analyzed
	lmNoteRest*			m_pLastNoteRest;	//last rest or note not in chord or base of chord

    long                m_nNumLine;         //number of line read
    long                m_nMaxID;           //maximun ID found

    int                 m_nLevel;           //number of nodes
    wxString            m_sVersion;         //score in process: used LDP version (i.e. "1.2")
    int                 m_nVersion;         //version in numeric format: 100*num+rev. (i.I.e. 1.2 = 102)

    //list of open relations, waiting for the stop element.
    std::list<lmTieInfo*>       m_PendingTies;
    std::list<lmBeamInfo*>      m_PendingBeams;
    std::list<lmFBLineInfo*>    m_PendingFBLines;

    //cursor data
    bool            m_fCursorData;          //true if cursor data found
    int             m_nCursorInstr;
    int             m_nCursorStaff;
    long            m_nCursorObjID;
    float           m_rCursorTime;
    lmStaffObj*     m_pCursorSO;

    // parsing control, options and error variables
    bool            m_fDebugMode;
    bool            m_fFromString;        // true: parsing a string. false: parsing a file
    bool            m_fStartingTextAnalysis;    //to signal the start of a new analysis
    wxString        m_sLastBuffer;        // to keep line under analysis
    long            m_nErrors;            // numebr of serious errors during parsing
    long            m_nWarnings;          // number of warnings during parsing
    wxString        m_sFileName;
    wxFileInputStream*      m_pFile;        // file being parsed
    wxTextInputStream*      m_pTextFile;    // file being parsed
    std::set<long>*         m_pIgnoreSet;   //set with elements to ignore


    //
    //variables to keep settings that propagate
    //

    // octave and duration for notes/rests
    wxString        m_sLastOctave;
    wxString        m_sLastDuration;

    // font and aligment for <title> elements
    lmEHAlign    m_nTitleAlignment;
    wxString        m_sTitleFontName;
    int             m_nTitleFontSize;
    int             m_nTitleStyle;
    wxFontWeight    m_nTitleWeight;

    // font for <text> elements
    wxString        m_sTextFontName;
    int             m_nTextFontSize;
    int             m_nTextStyle;
    wxFontWeight    m_nTextWeight;

    // tuplet options
    bool            m_fShowTupletBracket;
    bool            m_fShowNumber;
    bool            m_fTupletAbove;



};


// Helper class to analyze optional elements
class lmLDPOptionalTags
{
public:
	lmLDPOptionalTags(lmLDPParser* pParser);
	~lmLDPOptionalTags();

	void SetValid(lmETagLDP nTag, ...);
	void AnalyzeCommonOptions(lmLDPNode* pNode, int iP, lmVStaff* pVStaff,
							  bool* pfVisible, 
                              int* pStaffNum,
                              lmLocation* pLocation);

private:
	bool VerifyAllowed(lmETagLDP nTag, wxString sName, lmLDPNode* pNode);

	lmLDPParser*		m_pParser;					//owner parser
	std::vector<bool>	m_ValidTags;


};


#endif    // __LM_LDPPARSER_H
