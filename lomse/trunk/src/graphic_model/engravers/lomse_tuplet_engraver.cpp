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

#include "lomse_tuplet_engraver.h"

#include "lomse_glyphs.h"
#include "lomse_shape_beam.h"
#include "lomse_score_meter.h"
#include "lomse_engraving_options.h"
#include "lomse_im_note.h"
#include "lomse_gm_basic.h"
#include "lomse_shape_note.h"
#include "lomse_shape_tuplet.h"
#include "lomse_shape_text.h"
#include "lomse_text_engraver.h"

#include <sstream>
using namespace std;

namespace lomse
{

//---------------------------------------------------------------------------------------
// TupletEngraver implementation
//---------------------------------------------------------------------------------------
TupletEngraver::TupletEngraver(LibraryScope& libraryScope, ScoreMeter* pScoreMeter)
    : RelObjEngraver(libraryScope, pScoreMeter)
    , m_pTupletShape(NULL)
    , m_pTuplet(NULL)
{
}

//---------------------------------------------------------------------------------------
TupletEngraver::~TupletEngraver()
{
}

//---------------------------------------------------------------------------------------
void TupletEngraver::set_start_staffobj(ImoRelObj* pRO, ImoStaffObj* pSO,
                                        GmoShape* pStaffObjShape, int iInstr, int iStaff,
                                        int iSystem, int iCol, LUnits UNUSED(xRight),
                                        LUnits UNUSED(xLeft), LUnits UNUSED(yTop))
{
    m_iInstr = iInstr;
    m_iStaff = iStaff;
    m_pTuplet = dynamic_cast<ImoTuplet*>( pRO );

    ImoNoteRest* pNR = dynamic_cast<ImoNoteRest*>(pSO);
    m_noteRests.push_back( make_pair(pNR, pStaffObjShape) );

    m_shapesInfo[0].iCol = iCol;
    m_shapesInfo[0].iInstr = iInstr;
    m_shapesInfo[0].iSystem = iSystem;
}

//---------------------------------------------------------------------------------------
void TupletEngraver::set_middle_staffobj(ImoRelObj* UNUSED(pRO), ImoStaffObj* pSO,
                                         GmoShape* pStaffObjShape, int UNUSED(iInstr),
                                         int UNUSED(iStaff), int UNUSED(iSystem),
                                         int UNUSED(iCol), LUnits UNUSED(xRight),
                                         LUnits UNUSED(xLeft), LUnits UNUSED(yTop))
{
    ImoNoteRest* pNR = dynamic_cast<ImoNoteRest*>(pSO);
    m_noteRests.push_back( make_pair(pNR, pStaffObjShape) );
}

//---------------------------------------------------------------------------------------
void TupletEngraver::set_end_staffobj(ImoRelObj* UNUSED(pRO), ImoStaffObj* pSO,
                                      GmoShape* pStaffObjShape, int UNUSED(iInstr),
                                      int UNUSED(iStaff), int UNUSED(iSystem),
                                      int UNUSED(iCol), LUnits UNUSED(xRight),
                                      LUnits UNUSED(xLeft), LUnits UNUSED(yTop))
{
    ImoNoteRest* pNR = dynamic_cast<ImoNoteRest*>(pSO);
    m_noteRests.push_back( make_pair(pNR, pStaffObjShape) );
}

//---------------------------------------------------------------------------------------
int TupletEngraver::create_shapes(Color color)
{
    m_color = color;
    decide_if_show_bracket();
    determine_tuplet_text();

    if (m_fDrawNumber || m_fDrawBracket)
    {
        compute_y_coordinates();
        create_shape();
        set_shape_details();
        m_numShapes = 1;
    }
    else
        m_numShapes = 0;

    return m_numShapes;
}

//---------------------------------------------------------------------------------------
void TupletEngraver::create_shape()
{
    m_pStyle = m_pMeter->get_tuplets_style_info();

    m_pTupletShape = LOMSE_NEW GmoShapeTuplet(m_pTuplet, m_color);
    m_pShape = m_pTupletShape;
    m_shapesInfo[0].pShape = m_pTupletShape;

    if (m_fDrawNumber)
        add_text_shape();
}

//---------------------------------------------------------------------------------------
void TupletEngraver::set_shape_details()
{
	LUnits uBorderLength = tenths_to_logical(LOMSE_TUPLET_BORDER_LENGHT);
    LUnits uBracketDistance = tenths_to_logical(LOMSE_TUPLET_BRACKET_DISTANCE);
    LUnits uLineThick = tenths_to_logical(LOMSE_TUPLET_BRACKET_THICKNESS);
    LUnits uNumberDistance = tenths_to_logical(LOMSE_TUPLET_NUMBER_DISTANCE);
    LUnits uSpaceToNumber = tenths_to_logical(LOMSE_TUPLET_SPACE_TO_NUMBER);

    bool fAbove = decide_if_tuplet_placement_above();

    m_pTupletShape->set_layout_data(fAbove, m_fDrawBracket, m_yStart, m_yEnd,
                                    uBorderLength, uBracketDistance,
                                    uLineThick, uNumberDistance, uSpaceToNumber,
                                    get_start_noterest_shape(),
                                    get_end_noterest_shape());
}

//---------------------------------------------------------------------------------------
void TupletEngraver::decide_if_show_bracket()
{
    bool fIsBeamedGroup = check_if_single_beamed_group();
    m_fDrawBracket = m_pTuplet->get_show_bracket() == k_yesno_yes
           || (m_pTuplet->get_show_bracket() == k_yesno_default && !fIsBeamedGroup);
}

//---------------------------------------------------------------------------------------
void TupletEngraver::determine_tuplet_text()
{
    if (m_pTuplet->get_show_number() != ImoTuplet::k_number_none)
    {
        m_fDrawNumber = true;
        stringstream label;
        label << m_pTuplet->get_actual_number();
        m_label = label.str();
    }
    else
        m_fDrawNumber = false;
}


//---------------------------------------------------------------------------------------
bool TupletEngraver::decide_if_tuplet_placement_above()
{
    GmoShapeNote* pStart = get_first_note();
    bool fNotesUp = (pStart ? pStart->is_up() : true);
    int placement = m_pTuplet->get_placement();
	return placement == k_placement_above
           || (placement == k_placement_default && fNotesUp);
}

//---------------------------------------------------------------------------------------
void TupletEngraver::add_text_shape()
{
    TextEngraver engr(m_libraryScope, m_pMeter, m_label, "", m_pStyle);
    GmoShapeText* pShape = engr.create_shape(m_pTuplet, 0.0f, 0.0f);
    m_pTupletShape->add_label(pShape);
}

//---------------------------------------------------------------------------------------
void TupletEngraver::compute_y_coordinates()
{
    GmoShapeNote* pStart = get_first_note();
    GmoShapeNote* pEnd = get_last_note();

    //determine y start/end coordinates
    GmoShapeBeam* pBeamShape = dynamic_cast<GmoShapeBeam*>(
                                    pStart->find_related_shape(GmoObj::k_shape_beam) );
    if (pBeamShape)
    {
        //case a): tuplet encloses a whole beamed group
        UPoint pos = pBeamShape->get_outer_left_reference_point();
        m_yStart = pos.y;
        pos = pBeamShape->get_outer_right_reference_point();
        m_yEnd = pos.y;
    }
    else
    {
        //case b): tuplet encloses a whole group but not beamed
        //case c): tuplet encloses several beamed groups and notes/rests
        if (pStart->is_up())
        {
            m_yStart = pStart->get_top();
            m_yEnd = pEnd->get_top();
        }
        else
        {
            m_yStart = pStart->get_bottom();
            m_yEnd = pEnd->get_bottom();
        }
    }
}

//---------------------------------------------------------------------------------------
GmoShapeNote* TupletEngraver::get_first_note()
{
    std::list< pair<ImoNoteRest*, GmoShape*> >::iterator it;
    for (it = m_noteRests.begin(); it != m_noteRests.end(); ++it)
    {
        if ((*it).first->is_note())
            return dynamic_cast<GmoShapeNote*>((*it).second);
    }
    return NULL;    //imposible case unless all group are rests!
}

//---------------------------------------------------------------------------------------
GmoShapeNote* TupletEngraver::get_last_note()
{
    std::list< pair<ImoNoteRest*, GmoShape*> >::reverse_iterator it;
    for (it = m_noteRests.rbegin(); it != m_noteRests.rend(); ++it)
    {
        if ((*it).first->is_note())
            return dynamic_cast<GmoShapeNote*>((*it).second);
    }
    return NULL;    //imposible case unless all group are rests!
}

//---------------------------------------------------------------------------------------
bool TupletEngraver::check_if_single_beamed_group()
{
    ImoNoteRest* pStart = get_start_noterest();
    if (pStart->is_beamed())
    {
        ImoNoteRest* pEnd = get_end_noterest();
        if (pEnd->is_beamed())
            return pStart->get_beam() == pEnd->get_beam();
    }
    return false;
}


}  //namespace lomse
