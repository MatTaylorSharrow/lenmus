//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2006 Cecilio Salmeron
//
//    This program is free software; you can redistribute it and/or modify it under the 
//    terms of the GNU General Public License as published by the Free Software Foundation;
//    either version 2 of the License, or (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful, but WITHOUT ANY 
//    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A 
//    PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License along with this 
//    program; if not, write to the Free Software Foundation, Inc., 51 Franklin Street, 
//    Fifth Floor, Boston, MA  02110-1301, USA.
//
//    For any comment, suggestion or feature request, please contact the manager of 
//    the project at cecilios@users.sourceforge.net
//
//-------------------------------------------------------------------------------------
/*! @file Instrument.cpp
    @brief Implementation file for class lmInstrument
    @ingroup score_kernel
*/
/*! class lmInstrument    A collection of Staves

    An lmInstrument is a collection of Staves. Usually only one staff or two staves
    (piano grand staff) but could be any other number.

*/
#ifdef __GNUG__
// #pragma implementation
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "Score.h"
#include "wx/debug.h"

//implementation of the Instruments List
#include <wx/listimpl.cpp>
WX_DEFINE_LIST(InstrumentsList);


//Global variables used as default initializators
lmFontInfo tInstrumentDefaultFont = { _T("Arial"), 14, lmTEXT_BOLD };


lmInstrument::lmInstrument(lmScore* pScore, int nNumStaves, int nMIDIChannel,
                           int nMIDIInstr, wxString sName, wxString sAbbrev)
{
    //constructor: 
    //    pScore    - score to which this instrument belongs
    //    nStaves   - num of VStaves that will have. Usually only one lmVStaff.
    //    nChannel  - MIDI channel that will be used for playing this lmInstrument.
    //    nInstr    - MIDI instrument that will be used for playing this lmInstrument.
    //    sName     - name of the instrument
    //    sAbbrev   - abbreviation to use in second and following systems
                
    m_pScore = pScore;
    m_nMidiInstr = nMIDIInstr;
    m_nMidiChannel = nMIDIChannel;
    m_nIndentFirst = 0;
    m_nIndentOther = 0;

    //Normally, only one lmVStaff with one or two lmStaff
    //If more than one, they normally represent overlayered additional voices
    for (int i = 1; i <= nNumStaves; i++) {
        AddVStaff( (i!=1) );    //second and remaining overlayered
    }

    //create auxiliary objects for name and abbreviation
    m_pName = (lmText*)NULL;
    m_pAbbreviation = (lmText*)NULL;
    if (sName != _T("")) {
        m_pName = new lmText(m_pScore, sName, lmALIGN_LEFT,
                             tDefaultPos, tInstrumentDefaultFont);
    }
    if (sAbbrev != _T("")) {
        m_pAbbreviation = new lmText(m_pScore, sAbbrev, lmALIGN_LEFT,
                                     tDefaultPos, tInstrumentDefaultFont);
    }

}

lmInstrument::~lmInstrument()
{
    m_cStaves.DeleteContents(true);

    if (m_pName) delete m_pName;
    if (m_pAbbreviation) delete m_pAbbreviation;

}

void lmInstrument::SetIndent(lmLUnits* pIndent, lmLocation* pPos)
{
    if (pPos->xUnits == lmTENTHS) {
        lmVStaff *pVStaff = GetVStaff(1);
        *pIndent = pVStaff->TenthsToLogical(pPos->x, 1);
    }
    else {
        *pIndent = lmToLogicalUnits(pPos->x, pPos->xUnits);
    }

}

lmVStaff* lmInstrument::AddVStaff(bool fOverlayered)
{
    lmVStaff *pStaff = new lmVStaff(m_pScore, this, fOverlayered);
    m_cStaves.Append(pStaff);
    return pStaff;

}

//returns lmVStaff number nStaff (1..n)
lmVStaff* lmInstrument::GetVStaff(int nStaff)
{
    int i;
    VStavesList::Node *node;
    //iterate over the list to locate lmVStaff nStaff
    for (i=1, node = m_cStaves.GetFirst(); node && i < nStaff; node = node->GetNext(), i++ ) {}
//    wxASSERT_MSG{i != nStaff, _T("No existe el lmVStaff num. nStaff"));
    return node->GetData();
}

wxString lmInstrument::Dump()
{
    wxString sDump;
    wxVStavesListNode *pNode;
    lmVStaff* pVStaff;
    int i;
    //iterate over the list of lmVStaff to dump them
    for (i=1, pNode = m_cStaves.GetFirst(); pNode; pNode = pNode->GetNext(), i++)
    {
        pVStaff = (lmVStaff*) pNode->GetData();
        sDump += wxString::Format(_T("\nVStaff %d\n"), i );
        sDump += pVStaff->Dump();
    }
    return sDump;
        
}

wxString lmInstrument::SourceLDP()
{
    wxString sSource = 
        wxString::Format(_T("      (NumPartes %d)\n"), m_cStaves.GetCount() );

    //loop for each lmVStaff
    wxVStavesListNode *pNode;
    lmVStaff* pVStaff;
    int i;
    for (i=1, pNode = m_cStaves.GetFirst(); pNode; pNode = pNode->GetNext(), i++)
    {
        pVStaff = (lmVStaff*) pNode->GetData();
        sSource += wxString::Format(_T("      (Parte %d\n"), i);
        sSource += pVStaff->SourceLDP();
        sSource += _T("      )\n");
    }
    return sSource;

}

wxString lmInstrument::SourceXML()
{
    wxString sSource = _T("TODO: lmInstrument XML Source code generation methods");
    return sSource;

}

void lmInstrument::MeasureNames(lmPaper* pPaper)
{
    //when this method is invoked paper is positioned at top left corner of instrument
    //renderization point (x = left margin, y = top line of first staff)

    //As name/abbreviation are StaffObjs, method Draw() advances paper to
    //end of name/abbreviation. Let's save original position to restore it
    lmLUnits xPaper = pPaper->GetCursorX();

    m_nIndentFirst = 0;
    m_nIndentOther = 0;

    if (m_pName) {
        // measure text extent
        m_pName->Draw(DO_MEASURE, pPaper);
        // set indent =  text extend + after text space
        m_nIndentFirst = m_pName->GetSelRect().width + 30;    //! @todo user options
    }

    if (m_pAbbreviation) {
        // measure text extent
        m_pAbbreviation->Draw(DO_MEASURE, pPaper);
        // set indent =  text extend + after text space
        m_nIndentOther = m_pAbbreviation->GetSelRect().width + 30;    //! @todo user options
    }

    //restore original paper position
    pPaper->SetCursorX( xPaper );

}

void lmInstrument::DrawName(lmPaper* pPaper, wxColour colorC)
{
    //when this method is invoked paper is positioned at top left corner of instrument
    //renderization point (x = left margin, y = top line of first staff)
    //after rendering, paper position is not advanced

    if (m_pName) {
        //As name/abbreviation are StaffObjs, method Draw() should be invoked but
        //it draws the object not at current paper pos but at stored m_paperPos.
        //It also performs other non necessary thigs.
        //So, I will invoke directly DarwObject and, previouly, set the text
        //position at current paper position
        m_pName->MoveTo(wxPoint(pPaper->GetCursorX(), pPaper->GetCursorY()));
        m_pName->DrawObject(DO_DRAW, pPaper, colorC);
    }
}

void lmInstrument::DrawAbbreviation(lmPaper* pPaper, wxColour colorC)
{
    //when this method is invoked paper is positioned at top left corner of instrument
    //renderization point (x = left margin, y = top line of first staff)
    //after rendering, paper position is not advanced

    if (m_pAbbreviation) {
        //As name/abbreviation are StaffObjs, method Draw() should be invoked but
        //it draws the object not at current paper pos but at stored m_paperPos.
        //It also performs other non necessary thigs.
        //More: Abbreviation is written on every system but Draw will always
        //draw it at the same position.
        //So, I will invoke directly DarwObject and, previouly, set the text
        //position at current paper position.
        m_pAbbreviation->MoveTo(wxPoint(pPaper->GetCursorX(), pPaper->GetCursorY()));
        m_pAbbreviation->DrawObject(DO_DRAW, pPaper, colorC);
    }
}

