//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2007 Cecilio Salmeron
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

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "BoxSystem.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "BoxPage.h"
#include "BoxSystem.h"
#include "BoxSlice.h"
#include "BoxInstrSlice.h"

//access to colors
#include "../globals/Colors.h"
extern lmColors* g_pColors;

//-----------------------------------------------------------------------------------------

lmBoxSystem::lmBoxSystem(lmBoxPage* pParent, int nNumPage)
{
    m_nNumMeasures = 0;
    m_nNumPage = nNumPage;
    m_pBPage = pParent;
}


lmBoxSystem::~lmBoxSystem()
{
    lmBoxInstrSlice* pBIS;
    for (int i=0; i < (int)m_InstrSlices.size(); i++)
    {
        pBIS = m_InstrSlices[i];
        delete pBIS;
    }
    m_InstrSlices.clear();

    //delete BoxSlices
    lmBoxSlice* pBSlice;
    for (int i=0; i < (int)m_Slices.size(); i++)
    {
        pBSlice = m_Slices[i];
        delete pBSlice;
    }
    m_Slices.clear();
}

void lmBoxSystem::Render(int nSystem, lmScore* pScore, lmPaper* pPaper)
{
    //At this point paper position is not in the right place. Therefore, we move
    //to the start of system position.
    pPaper->SetCursorY( m_yPos );

    //for each lmInstrument
    for (int i=0; i < (int)m_InstrSlices.size(); i++)
    {
        m_yBottomLeftLine = m_InstrSlices[i]->Render(pPaper, m_xPos, m_nNumPage, nSystem);
    }

    //Draw the initial barline that joins all staffs in a system
    if (pScore->GetOptionBool(_T("Staff.DrawLeftBarline")) )
    {
        lmLUnits uLineThickness = lmToLogicalUnits(0.2, lmMILLIMETERS);        // thin line width will be 0.2 mm @todo user options
        pPaper->SolidLine(m_xLeftLine, m_yTopLeftLine,
                          m_xLeftLine, m_yBottomLeftLine,
                          uLineThickness, eEdgeNormal, *wxBLACK);
    }

    ////DEBUG: draw a border around system region -------------------------------------
    ////pPaper->SketchLine(m_xLeftLine, m_yTopLeftLine,
    ////              m_xFinal, m_yBottomLeftLine, *wxRED);
    //pPaper->SketchRectangle(lmUPoint(m_xLeftLine, m_yTopLeftLine),
    //                        lmUSize(m_xFinal - m_xLeftLine, m_yBottomLeftLine - m_yTopLeftLine),
    //                        *wxRED);
    ////-------------------------------------------------------------------------------

}

void lmBoxSystem::RenderMeasure(lmVStaff* pVStaff, int nMeasure, lmPaper* pPaper)
{
    //
    // Draw all StaffObjs in measure nMeasure, including the barline.
    // It is assumed that all positioning information is already computed
    //

    wxASSERT(nMeasure <= pVStaff->GetNumMeasures());

    /*! @todo
        Review this commented code. Implies to review also comented
        code in lmFormatter4::SizeMeasure
    */
    //el posicionamiento relativo de objetos (en LDP) requiere conocer la
    //posición de inicio del compas. Para ello, se guarda aquí, de forma
    //que el método GetXInicioCompas pueda devolver este valor
    //pVStaff->SetXInicioCompas = pPaper->GetCursorX()

    //! @todo Review this
    ////si no es el primer compas de la partitura avanza separación con la barra de compas
    ////o con prólogo, si es comienzo de línea.
    //if (nMeasure != 1) {
    //    m_oCanvas.Avanzar        //separación con la barra de compas
    //}

    //space occupied by clefs is computed only when all clefs has been drawn, so that we
    //can properly align notes and othe StaffObjs. The next flag is used to signal that
    //it is pending to compute clefs space.
    bool fSpacePending = false;        //initialy not clefs ==> no space pending
    lmLUnits xClefs=0;                //x position of first clef. To align all clefs
    lmLUnits nMaxClefWidth=0;        //to save the width of the wider clef

    //loop to process all StaffObjs in this measure
    lmStaffObj* pSO = (lmStaffObj*)NULL;
    lmStaffObjIterator* pIT = pVStaff->CreateIterator(eTR_AsStored);
    pIT->AdvanceToMeasure(nMeasure);
    while(!pIT->EndOfList())
    {
        pSO = pIT->GetCurrent();

        if (pSO->GetClass() == eSFOT_Clef) {
            //clefs don't consume space until a lmStaffObj of other type is found
            if (!fSpacePending) {
                //This is the first cleft. Save paper position
                xClefs = pPaper->GetCursorX();
                fSpacePending = true;
                nMaxClefWidth = 0;
            } else {
                /*! @todo
                    Review this. I thing that now, with lmTimeposTable mechanism,
                    it is useless.
                */
                pPaper->SetCursorX(xClefs);        //force position to align all clefs
            }
            pSO->Draw(DO_DRAW, pPaper);
            if (fSpacePending) {
                nMaxClefWidth = wxMax(nMaxClefWidth, pPaper->GetCursorX() - xClefs);
            }

        } else {
            //It is not a clef. Just draw it
            if (fSpacePending) {
                pPaper->SetCursorX(xClefs + nMaxClefWidth);
                fSpacePending = false;
            }
            pSO->Draw(DO_DRAW, pPaper);

        }

        //for visual highlight we need to know the page in wich the StaffObj to highlight
        //is located. To this end we are going to store the page number in each
        //StaffObj
        pSO->SetPageNumber(m_nNumPage);

        // if barline, exit loop: end of measure reached
        if (pSO->GetClass() == eSFOT_Barline) break;

        pIT->MoveNext();
    }
    delete pIT;

}

void lmBoxSystem::DrawSelRectangle(lmPaper* pPaper)
{
    //draw a border around system region -------------------------------------
    pPaper->SketchRectangle(lmUPoint(m_xLeftLine, m_yTopLeftLine),
                            lmUSize(m_xFinal - m_xLeftLine, m_yBottomLeftLine - m_yTopLeftLine),
                            *wxRED);
}

void lmBoxSystem::SetNumMeasures(int nMeasures, lmScore* pScore)
{
    //Now we have all the information about the system. Let's create the collection
    //of BoxSlices

    m_nNumMeasures = nMeasures;

    //Build the slices
    int iInstr = 1;
	int nLastMeasure = m_nFirstMeasure + m_nNumMeasures - 1;
    for (lmInstrument* pInstr = pScore->GetFirstInstrument();
         pInstr;
         pInstr = pScore->GetNextInstrument(), iInstr++)
    {
            m_InstrSlices.push_back(
                    new lmBoxInstrSlice(this, m_nFirstMeasure, nLastMeasure,
                                        pInstr, iInstr) );

    }

}

lmBoxSlice* lmBoxSystem::FindStaffAtPosition(lmUPoint& pointL)
{
    lmURect rectL(m_xLeftLine, m_yTopLeftLine,
                  m_xFinal - m_xLeftLine, m_yBottomLeftLine - m_yTopLeftLine);
    if (rectL.Contains(pointL))
    {
        //identify the measure
        for (int iS=0; iS < (int)m_Slices.size(); iS++)
        {
            if (m_Slices[iS]->FindMeasureAt(pointL))
            {
                return m_Slices[iS];
            }
        }
        wxMessageBox( wxString::Format( _T("Page %d, measure not identified!!! Between measure %d and %d"),
            m_nNumPage, m_nFirstMeasure, m_nFirstMeasure+m_nNumMeasures-1) );
        return (lmBoxSlice*)NULL;
    }
    return (lmBoxSlice*)NULL;
}

lmBoxSlice* lmBoxSystem::AddSlice(int nMeasure, lmLUnits xStart, lmLUnits xEnd)
{
    lmBoxSlice* pBSlice = new lmBoxSlice(this, nMeasure, xStart, xEnd);
    m_Slices.push_back(pBSlice);
    return pBSlice;
}
