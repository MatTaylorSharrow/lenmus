//--------------------------------------------------------------------------------------
//  LenMus Library
//  Copyright (c) 2010 LenMus project
//
//  This program is free software; you can redistribute it and/or modify it under the
//  terms of the GNU General Public License as published by the Free Software Foundation,
//  either version 3 of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful, but WITHOUT ANY
//  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
//  PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public License along
//  with this library; if not, see <http://www.gnu.org/licenses/> or write to the
//  Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston,
//  MA  02111-1307,  USA.
//
//  For any comment, suggestion or feature request, please contact the manager of
//  the project at cecilios@users.sourceforge.net
//
//-------------------------------------------------------------------------------------

#include "lenmus_im_note.h"

#include <math.h>               //pow
#include "lenmus_basic_objects.h"

namespace lenmus
{


//-------------------------------------------------------------------------------------
// ImoNoteRest implementation
//-------------------------------------------------------------------------------------

ImoNoteRest::ImoNoteRest(int objtype, DtoNoteRest& dto)
    : ImoStaffObj(objtype, dto)
    , m_nNoteType( dto.get_note_type() )
    , m_nDots( dto.get_dots() )
    , m_rDuration( to_duration(m_nNoteType, m_nDots) )
    , m_nVoice( dto.get_voice() )
    , m_pBeam(NULL)
    , m_pTuplet(NULL)
{
}

ImoNoteRest::ImoNoteRest(long id, int objtype, int nNoteType, float rDuration, int nDots,
               int nStaff, int nVoice, bool fVisible, ImoBeamInfo* pBeamInfo)
    : ImoStaffObj(id, objtype)
    , m_nNoteType(nNoteType)
    , m_nDots(nDots)
    , m_rDuration(rDuration)
    , m_nVoice(nVoice)
    , m_beamInfo(*pBeamInfo)
    , m_pBeam(NULL)
    , m_pTuplet(NULL)
{
    set_staff(nStaff);
}

int ImoNoteRest::get_beam_type(int level)
{
    return m_beamInfo.get_beam_type(level);
}

void ImoNoteRest::set_beam_type(int level, int type)
{
    m_beamInfo.set_beam_type(level, type);
}

//void ImoNoteRest::set_note_type_and_dots(int noteType, int dots)
//{
//    m_nNoteType = noteType;
//    m_nDots = dots;
//    m_rDuration = to_duration(m_nNoteType, m_nDots);
//}


//-------------------------------------------------------------------------------------
// ImoRest implementation
//-------------------------------------------------------------------------------------

ImoRest::ImoRest()
    : ImoNoteRest(ImoObj::k_rest)
{
}

ImoRest::ImoRest(DtoRest& dto)
    : ImoNoteRest(ImoObj::k_rest, dto)
{
}

ImoRest::ImoRest(long id, int nNoteType, float rDuration, int nDots, int nStaff,
               int nVoice, bool fVisible, bool fBeamed, ImoBeamInfo* pBeamInfo)
    : ImoNoteRest(id, ImoObj::k_rest, nNoteType, rDuration, nDots,
                 nStaff, nVoice, fVisible, pBeamInfo)
{
}


//-------------------------------------------------------------------------------------
// ImoNote implementation
//-------------------------------------------------------------------------------------

ImoNote::ImoNote()
    : ImoNoteRest(ImoObj::k_note)
    , m_step(ImoNote::C)
    , m_octave(4)
    , m_accidentals(ImoNote::k_no_accidentals)
    , m_stemDirection(ImoNote::k_default)
    , m_inChord(false)
    , m_pTieNext(NULL)
    , m_pTiePrev(NULL)
{
}

ImoNote::ImoNote(DtoNote& dto)
    : ImoNoteRest(ImoObj::k_note, dto)
    , m_step( dto.get_step() )
    , m_octave( dto.get_octave() )
    , m_accidentals( dto.get_accidentals() )
    , m_stemDirection( dto.get_stem_direction() )
    , m_inChord( dto.is_in_chord() )
    , m_pTieNext(NULL)
    , m_pTiePrev(NULL)
{
}

ImoNote::ImoNote(long id, int nNoteType, float rDuration, int nDots, int nStaff, int nVoice,
               bool fVisible, bool fBeamed, ImoBeamInfo* pBeamInfo)
    : ImoNoteRest(id, ImoObj::k_note, nNoteType, rDuration, nDots, nStaff, nVoice,
                 fVisible, pBeamInfo)
    , m_step(ImoNote::C)
    , m_octave(4)
    , m_accidentals(ImoNote::k_no_accidentals)
    , m_stemDirection(ImoNote::k_default)
    , m_inChord(false)
    , m_pTieNext(NULL)
    , m_pTiePrev(NULL)
{
}

ImoNote::~ImoNote()
{
    if (m_pTiePrev)
        remove_tie(m_pTiePrev);
    if (m_pTieNext)
        remove_tie(m_pTieNext);
}

void ImoNote::remove_tie(ImoTie* pTie)
{
    ImoNote* pStartNote = pTie->get_start_note();
    ImoNote* pEndNote = pTie->get_end_note();
    delete pTie;
    pStartNote->set_tie_next( NULL );
    pEndNote->set_tie_prev( NULL );
}



}  //namespace lenmus
