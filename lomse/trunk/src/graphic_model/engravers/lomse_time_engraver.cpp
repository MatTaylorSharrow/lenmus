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

#include "lomse_time_engraver.h"

#include "lomse_glyphs.h"
#include "lomse_shapes.h"
#include "lomse_score_meter.h"
#include "lomse_internal_model.h"


namespace lomse
{

//---------------------------------------------------------------------------------------
// TimeEngraver implementation
//---------------------------------------------------------------------------------------
TimeEngraver::TimeEngraver(LibraryScope& libraryScope, ScoreMeter* pScoreMeter,
                           int iInstr, int iStaff)
    : Engraver(libraryScope, pScoreMeter, iInstr, iStaff)
{
}

//---------------------------------------------------------------------------------------
GmoShape* TimeEngraver::create_shape_normal(ImoTimeSignature* pCreatorImo,
                                            UPoint uPos, int beats, int beat_type,
                                            Color color)
{
    m_fontSize = determine_font_size();
    m_pCreatorImo = pCreatorImo;
    m_color = color;

    create_top_digits(uPos, beats);
    create_bottom_digits(uPos, beat_type);
    center_numbers();
    create_main_container_shape(uPos);
    add_all_shapes_to_container();

    return m_pTimeShape;
}

//---------------------------------------------------------------------------------------
void TimeEngraver::create_main_container_shape(UPoint uPos)
{
    ShapeId idx = 0;
    m_pTimeShape = LOMSE_NEW GmoShapeTimeSignature(m_pCreatorImo, idx, uPos, m_color,
                                                   m_libraryScope);
    //m_pTimeShape->SetShapeLevel(lm_eMainShape);
}

//---------------------------------------------------------------------------------------
void TimeEngraver::create_top_digits(UPoint uPos, int beats)
{
    m_uPos = uPos;
    create_digits(beats, m_pShapesTop);
    m_uTopWidth = m_uPos.x - uPos.x;
}

//---------------------------------------------------------------------------------------
void TimeEngraver::create_bottom_digits(UPoint uPos, int beat_type)
{
    m_uPos = uPos;
    m_uPos.y += m_pMeter->tenths_to_logical(20.0f, m_iInstr, m_iStaff);
    create_digits(beat_type, m_pShapesBottom);
    m_uBottomWidth = m_uPos.x - uPos.x;
}

//---------------------------------------------------------------------------------------
void TimeEngraver::create_digits(int digits, GmoShape* pShape[])
{
    if (digits > 9)
    {
        pShape[0] = create_digit(1);
        pShape[1] = create_digit(digits - 10);
    }
    else
    {
        pShape[0] = create_digit(digits);
        pShape[1] = NULL;
    }
}

//---------------------------------------------------------------------------------------
GmoShape* TimeEngraver::create_digit(int digit)
{
    int iGlyph = k_glyph_number_0 + digit;
    Tenths yOffset = glyphs_lmbasic2[iGlyph].GlyphOffset + 40.0f;
    LUnits y = m_uPos.y + m_pMeter->tenths_to_logical(yOffset, m_iInstr, m_iStaff);
    GmoShape* pShape =
        LOMSE_NEW GmoShapeTimeDigit(m_pCreatorImo, 0, iGlyph, UPoint(m_uPos.x, y),
                                    m_color, m_libraryScope, m_fontSize);
    m_uPos.x += pShape->get_width();
    return pShape;
}

//---------------------------------------------------------------------------------------
void TimeEngraver::center_numbers()
{
    if (m_uTopWidth > m_uBottomWidth)
    {
        //bottom number wider
        USize shift((m_uBottomWidth - m_uTopWidth) / 2.0f, 0.0f);
        m_pShapesTop[0]->shift_origin(shift);
        if (m_pShapesTop[1])
            m_pShapesTop[1]->shift_origin(shift);
    }
    else
    {
        //top number wider
        USize shift((m_uTopWidth - m_uBottomWidth) / 2.0f, 0.0f);
        m_pShapesBottom[0]->shift_origin(shift);
        if (m_pShapesBottom[1])
            m_pShapesBottom[1]->shift_origin(shift);
    }
}

//---------------------------------------------------------------------------------------
void TimeEngraver::add_all_shapes_to_container()
{
    m_pTimeShape->add(m_pShapesTop[0]);
    if (m_pShapesTop[1])
        m_pTimeShape->add(m_pShapesTop[1]);

    m_pTimeShape->add(m_pShapesBottom[0]);
    if (m_pShapesBottom[1])
        m_pTimeShape->add(m_pShapesBottom[1]);
}

//---------------------------------------------------------------------------------------
GmoShape* TimeEngraver::create_shape_common(ImoTimeSignature* pCreatorImo, UPoint uPos)
{
    //TODO TimeEngraver::create_shape_common

    create_main_container_shape(uPos);

//        case eTS_Common:        // a C symbol
//        case eTS_Cut:           // a C/ symbol
//        {
//            int nGlyph = (m_nType==eTS_Common ? GLYPH_COMMON_TIME : GLYPH_CUT_TIME);
//		    LUnits uyPos = uyPosTop
//						    + m_pVStaff->TenthsToLogical(aGlyphsInfo[nGlyph].GlyphOffset, m_nStaffNum );
//            lmShape* pShape = LOMSE_NEW lmShapeGlyph(this, nShapeIdx, nGlyph, pPaper,
//                                               UPoint(uxPosTop, uyPos),
//									           _T("Time signature"), lmNO_DRAGGABLE,
//                                               m_color );
//		    uxPosTop += m_pVStaff->TenthsToLogical(aGlyphsInfo[nGlyph].thWidth, m_nStaffNum );
//            return pShape;
//        }
    return m_pTimeShape;
}

//---------------------------------------------------------------------------------------
GmoShape* TimeEngraver::create_shape_cut(ImoTimeSignature* pCreatorImo, UPoint uPos)
{
    //TODO: TimeEngraver::create_shape_cut

    create_main_container_shape(uPos);
    return m_pTimeShape;
}

}  //namespace lomse
