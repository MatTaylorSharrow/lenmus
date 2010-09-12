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

#include <algorithm>
#include <math.h>                   //pow

#include "lenmus_staffobjs_table.h"
#include "lenmus_internal_model.h"
#include "lenmus_im_note.h"
#include "lenmus_ldp_elements.h"
#include "lenmus_basic_objects.h"

using namespace std;

namespace lenmus
{


//-------------------------------------------------------------------------------------
// ImoObj implementation
//-------------------------------------------------------------------------------------

ImoObj::ImoObj(int objtype, DtoObj& dto)
    : m_id( dto.get_id() )
    , m_objtype( objtype )
{
}

void ImoObj::accept_in(BaseVisitor& v)
{
    Visitor<ImoObj>* p = dynamic_cast<Visitor<ImoObj>*>(&v);
    if (p)
    {
        p->start_visit(this);
    }
}

void ImoObj::accept_out(BaseVisitor& v)
{
    Visitor<ImoObj>* p = dynamic_cast<Visitor<ImoObj>*>(&v);
    if (p)
    {
        p->start_visit(this);
    }
}


//-------------------------------------------------------------------------------------
// ImoBinaryRelObj implementation
//-------------------------------------------------------------------------------------

//ImoBinaryRelObj::ImoBinaryRelObj(ImoDocObj* pOwner, long id, int type,
//                               ImoStaffObj* pStartSO, ImoStaffObj* pEndSO)
//    : ImoRelObj(pOwner, id, type)
//    , m_pStartSO(pStartSO)
//    , m_pEndSO(pEndSO)
//{
//}
//
ImoBinaryRelObj::~ImoBinaryRelObj()
{
}

//void ImoBinaryRelObj::remove(ImoStaffObj* pSO)
//{
//    if (m_pStartSO == pSO)
//        m_pStartSO = (ImoStaffObj*)NULL;
//    else if (m_pEndSO == pSO)
//        m_pEndSO = (ImoStaffObj*)NULL;
//}



//-------------------------------------------------------------------------------------
// ImoMultiRelObj implementation
//-------------------------------------------------------------------------------------

//ImoMultiRelObj::ImoMultiRelObj(ImoDocObj* pOwner, long id, int type)
//    : ImoRelObj(pOwner, id, type)
//{
//}

ImoMultiRelObj::~ImoMultiRelObj()
{
    m_relatedObjects.clear();
}

//void ImoMultiRelObj::remove(ImoStaffObj* pSO)
//{
//    //remove StaffObj.
//	//AWARE: This method is always invoked by a SO. Therefore it will
//	//not inform back the SO, as this is unnecessary and causes problems when
//	//deleting the relationship object
//
//    std::list<ImoStaffObj*>::iterator it;
//    it = find(m_relatedObjects.begin(), m_relatedObjects.end(), pSO);
//    m_relatedObjects.erase(it);
//    on_relationship_modified();
//}

void ImoMultiRelObj::push_back(ImoStaffObj* pSO)
{
    m_relatedObjects.push_back(pSO);
    //on_relationship_modified();
}

//void ImoMultiRelObj::include(ImoStaffObj* pSO, int index)
//{
//    // Add a note to the relation. index is the position that the added StaffObj
//    // must occupy (0..n). If index == -1, StaffObj will be added at the end.
//
//	//add the StaffObj
//	if (index == -1 || index == get_num_objects())
//		m_relatedObjects.push_back(pSO);
//	else
//	{
//		int iN;
//		std::list<ImoStaffObj*>::iterator it;
//		for(iN=0, it=m_relatedObjects.begin(); it != m_relatedObjects.end(); ++it, iN++)
//		{
//			if (iN == index)
//			{
//				//insert before current item
//				m_relatedObjects.insert(it, pSO);
//				break;
//			}
//		}
//	}
//
//    on_relationship_modified();
//}
//
//int ImoMultiRelObj::get_object_index(ImoStaffObj* pSO)
//{
//	//returns the position in the StaffObjs list (0..n)
//
//	int iN;
//    std::list<ImoStaffObj*>::iterator it;
//    for(iN=0, it=m_relatedObjects.begin(); it != m_relatedObjects.end(); ++it, iN++)
//	{
//		if (pSO == *it)
//            return iN;
//	}
//	return 0;			//compiler happy
//}



//-------------------------------------------------------------------------------------
// ImoComponentObj implementation
//-------------------------------------------------------------------------------------

ImoComponentObj::ImoComponentObj(int objtype, DtoComponentObj& dto)
    : ImoDocObj(objtype, dto)
    , m_fVisible( dto.is_visible() )
    , m_color( dto.get_color() )
{
}

void ImoComponentObj::set_color(ImoColorInfo* pColor)
{
    m_color = rgba16(pColor->red(), pColor->green(), pColor->blue(), pColor->alpha());
    delete pColor;
}

void ImoComponentObj::set_color(rgba16 color)
{
    m_color = color;
}


//-------------------------------------------------------------------------------------
// ImoStaffObj implementation
//-------------------------------------------------------------------------------------

ImoStaffObj::ImoStaffObj(int objtype, DtoStaffObj& dto)
    : ImoComponentObj(objtype, dto)
    , m_staff( dto.get_staff() )
{
}


//-------------------------------------------------------------------------------------
// ImoBarline implementation
//-------------------------------------------------------------------------------------

ImoBarline::ImoBarline(DtoBarline& dto)
    : ImoStaffObj(ImoObj::k_barline, dto)
    , m_type( dto.get_barline_type() )
{
}



//-------------------------------------------------------------------------------------
// ImoBeamInfo implementation
//-------------------------------------------------------------------------------------

ImoBeamInfo::ImoBeamInfo()
    : ImoSimpleObj(ImoObj::k_beam_info)
    , m_beamNum(0)
    , m_pBeamElm(NULL)
    , m_pNR(NULL)
{
    for (int level=0; level < 6; level++)
    {
        m_beamType[level] = ImoBeam::k_none;
        m_repeat[level] = false;
    }
}

ImoBeamInfo::ImoBeamInfo(LdpElement* pBeamElm)
    : ImoSimpleObj(ImoObj::k_beam_info)
    , m_beamNum(0)
    , m_pBeamElm(pBeamElm)
    , m_pNR(NULL)
{
    for (int level=0; level < 6; level++)
    {
        m_beamType[level] = ImoBeam::k_none;
        m_repeat[level] = false;
    }
}

int ImoBeamInfo::get_line_number()
{
    if (m_pBeamElm)
        return m_pBeamElm->get_line_number();
    else
        return 0;
}

void ImoBeamInfo::set_beam_type(int level, int type)
{
    m_beamType[level] = type;
}

int ImoBeamInfo::get_beam_type(int level)
{
    return m_beamType[level];
}

bool ImoBeamInfo::is_end_of_beam()
{
    for (int level=0; level < 6; level++)
    {
        if (m_beamType[level] == ImoBeam::k_begin
            || m_beamType[level] == ImoBeam::k_forward
            || m_beamType[level] == ImoBeam::k_continue
            )
            return false;
    }
    return true;
}

void ImoBeamInfo::set_repeat(int level, bool value)
{
    m_repeat[level] = value;
}

bool ImoBeamInfo::get_repeat(int level)
{
    return m_repeat[level];
}


//-------------------------------------------------------------------------------------
// ImoClef implementation
//-------------------------------------------------------------------------------------

ImoClef::ImoClef(DtoClef& dto)
    : ImoStaffObj(ImoObj::k_clef, dto)
    , m_clefType( dto.get_clef_type() )
{
}

ImoClef::ImoClef(int clefType)
    : ImoStaffObj(ImoObj::k_clef)
    , m_clefType(clefType)
{
}



//-------------------------------------------------------------------------------------
// ImoColorInfo implementation
//-------------------------------------------------------------------------------------

ImoColorInfo::ImoColorInfo(int16u r, int16u g, int16u b, int16u a)
    : ImoSimpleObj(ImoObj::k_color_info)
    , m_color(r, g, b, a)
    , m_ok(true)
{
}

int16u ImoColorInfo::convert_from_hex(const std::string& hex)
{
    int value = 0;

    int a = 0;
    int b = static_cast<int>(hex.length()) - 1;
    for (; b >= 0; a++, b--)
    {
        if (hex[b] >= '0' && hex[b] <= '9')
        {
            value += (hex[b] - '0') * (1 << (a * 4));
        }
        else
        {
            switch (hex[b])
            {
                case 'A':
                case 'a':
                    value += 10 * (1 << (a * 4));
                    break;

                case 'B':
                case 'b':
                    value += 11 * (1 << (a * 4));
                    break;

                case 'C':
                case 'c':
                    value += 12 * (1 << (a * 4));
                    break;

                case 'D':
                case 'd':
                    value += 13 * (1 << (a * 4));
                    break;

                case 'E':
                case 'e':
                    value += 14 * (1 << (a * 4));
                    break;

                case 'F':
                case 'f':
                    value += 15 * (1 << (a * 4));
                    break;

                default:
                    m_ok = false;
                    //cout << "Error: invalid character '" << hex[b] << "' in hex number" << endl;
                    break;
            }
        }
    }

    return static_cast<int16u>(value);
}

rgba16& ImoColorInfo::get_from_rgb_string(const std::string& rgb)
{
    m_ok = true;

    if (rgb[0] == '#')
    {
        m_color.r = convert_from_hex( rgb.substr(1, 2) );
        m_color.g = convert_from_hex( rgb.substr(3, 2) );
        m_color.b = convert_from_hex( rgb.substr(5, 2) );
        m_color.a = 255;
    }

    if (!m_ok)
        m_color = rgba16(0,0,0,255);

    return m_color;
}

rgba16& ImoColorInfo::get_from_rgba_string(const std::string& rgba)
{
    m_ok = true;

    if (rgba[0] == '#')
    {
        m_color.r = convert_from_hex( rgba.substr(1, 2) );
        m_color.g = convert_from_hex( rgba.substr(3, 2) );
        m_color.b = convert_from_hex( rgba.substr(5, 2) );
        m_color.a = convert_from_hex( rgba.substr(7, 2) );
    }

    if (!m_ok)
        m_color = rgba16(0,0,0,255);

    return m_color;
}

rgba16& ImoColorInfo::get_from_string(const std::string& hex)
{
    if (hex.length() == 7)
        return get_from_rgb_string(hex);
    else if (hex.length() == 9)
        return get_from_rgba_string(hex);
    else
    {
        m_ok = false;
        m_color = rgba16(0,0,0,255);
        return m_color;
    }
}


//-------------------------------------------------------------------------------------
// ImoContent implementation
//-------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------
// ImoDocObj implementation
//-------------------------------------------------------------------------------------

ImoDocObj::ImoDocObj(int objtype)
    : ImoObj(objtype)
    , m_txUserLocation(0.0f)
    , m_tyUserLocation(0.0f)
{
    //append_child( new ImoCollection() );
}

ImoDocObj::ImoDocObj(long id, int objtype)
    : ImoObj(id, objtype)
    , m_txUserLocation(0.0f)
    , m_tyUserLocation(0.0f)
{
    //append_child( new ImoCollection() );
}

ImoDocObj::ImoDocObj(int objtype, DtoDocObj& dto)
    : ImoObj(objtype, dto)
    , m_txUserLocation( dto.get_user_location_x() )
    , m_tyUserLocation( dto.get_user_location_y() )
{
    //append_child( new ImoCollection() );
}

ImoDocObj::~ImoDocObj()
{
    std::list<ImoAuxObj*>::iterator it;
    for (it=m_attachments.begin(); it != m_attachments.end(); ++it)
        delete *it;
    m_attachments.clear();
}

//ImoAuxObj* ImoDocObj::get_attachment(int i)       //0..n-1
//{
//    std::list<ImoAuxObj*>::iterator it = m_attachments.begin();
//    for (; it != m_attachments.end() && i > 0; ++it, --i);
//    if (it != m_attachments.end())
//        return *it;
//    return NULL;
//}

void ImoDocObj::attach(ImoAuxObj* pAO)
{
    ImoObj* pAttachments = get_first_child();
    pAttachments->append_child(pAO);
}

ImoAuxObj* ImoDocObj::get_attachment(int i)
{
    ImoObj* pImo = get_first_child()->get_child(i);
    return dynamic_cast<ImoAuxObj*>( pImo );
}


//-------------------------------------------------------------------------------------
// ImoDocument implementation
//-------------------------------------------------------------------------------------

int ImoDocument::get_num_content_items()
{
    if (has_children())
    {
        ImoObj* pChild = get_first_child();
        int num = pChild->get_num_children();
        return num;
    }
    else
        return 0;
}

ImoDocObj* ImoDocument::get_content_item(int iItem)
{
    ImoContent* pContent = get_content();
    return dynamic_cast<ImoDocObj*>( pContent->get_child(iItem) );
}

ImoContent* ImoDocument::get_content()
{
    return dynamic_cast<ImoContent*>( get_first_child() );
}


//-------------------------------------------------------------------------------------
// ImoGoBackFwd implementation
//-------------------------------------------------------------------------------------

ImoGoBackFwd::ImoGoBackFwd(DtoGoBackFwd& dto)
    : ImoStaffObj(ImoObj::k_go_back_fwd, dto)
    , m_fFwd( dto.is_forward() )
    , m_rTimeShift( dto.get_time_shift() )
    , SHIFT_START_END(100000000.0f)
{
}


//-------------------------------------------------------------------------------------
// ImoInstrument implementation
//-------------------------------------------------------------------------------------

ImoInstrument::ImoInstrument()
    : ImoContainerObj(ImoObj::k_instrument)
    , m_nStaves(1)
    , m_pMusicData(NULL)
{
//	m_midiChannel = g_pMidi->DefaultVoiceChannel();
//	m_midiInstr = g_pMidi->DefaultVoiceInstr();
}

ImoInstrument::~ImoInstrument()
{
    if (m_pMusicData)
        delete m_pMusicData;
}

void ImoInstrument::set_name(ImoTextString* pText)
{
    m_name = *pText;
    delete pText;
}

void ImoInstrument::set_abbrev(ImoTextString* pText)
{
    m_abbrev = *pText;
    delete pText;
}

void ImoInstrument::set_midi_info(ImoMidiInfo* pInfo)
{
    m_midi = *pInfo;
    delete pInfo;
}


//-------------------------------------------------------------------------------------
// ImoInstrGroup implementation
//-------------------------------------------------------------------------------------


ImoInstrGroup::~ImoInstrGroup()
{
    delete_instruments();
}

void ImoInstrGroup::delete_instruments()
{
    std::list<ImoInstrument*>::iterator it;
    for (it= m_instruments.begin(); it != m_instruments.end(); ++it)
        delete *it;
    m_instruments.clear();
}

void ImoInstrGroup::set_name(ImoTextString* pText)
{
    m_name = *pText;
    delete pText;
}

void ImoInstrGroup::set_abbrev(ImoTextString* pText)
{
    m_abbrev = *pText;
    delete pText;
}


//-------------------------------------------------------------------------------------
// ImoKeySignature implementation
//-------------------------------------------------------------------------------------

ImoKeySignature::ImoKeySignature(DtoKeySignature& dto)
    : ImoStaffObj(ImoObj::k_key_signature, dto)
    , m_keyType( dto.get_key_type() )
{
}


//-------------------------------------------------------------------------------------
// ImoMusicData implementation
//-------------------------------------------------------------------------------------

ImoMusicData::~ImoMusicData()
{
    std::list<ImoStaffObj*>::iterator it;
    for (it=m_staffobjs.begin(); it != m_staffobjs.end(); ++it)
        delete *it;
    m_staffobjs.clear();
}


//-------------------------------------------------------------------------------------
// ImoScore implementation
//-------------------------------------------------------------------------------------

ImoScore::ImoScore()
    : ImoContainerObj(ImoObj::k_score)
    , m_version("")
    , m_pColStaffObjs(NULL)
    , m_pSystemLayoutFirst(NULL)
    , m_pSystemLayoutOther(NULL)
{
}

ImoScore::~ImoScore()
{
    delete_options();
    delete_instruments();
    delete_systems_layout();
    delete_staffobjs_collection();
}

void ImoScore::delete_options()
{
    std::list<ImoOption*>::iterator it;
    for (it= m_options.begin(); it != m_options.end(); ++it)
        delete *it;
    m_options.clear();
}

void ImoScore::delete_instruments()
{
    std::list<ImoInstrument*>::iterator it;
    for (it= m_instruments.begin(); it != m_instruments.end(); ++it)
        delete *it;
    m_instruments.clear();
}

void ImoScore::delete_systems_layout()
{
    if (m_pSystemLayoutFirst)
        delete m_pSystemLayoutFirst;
    if (m_pSystemLayoutOther)
        delete m_pSystemLayoutOther;
}

void ImoScore::delete_staffobjs_collection()
{
//    if (m_pColStaffObjs)
//        delete m_pColStaffObjs;
}

ImoInstrument* ImoScore::get_instrument(int iInstr)    //iInstr = 0..n-1
{
    std::list<ImoInstrument*>::iterator it = m_instruments.begin();
    for (; it != m_instruments.end() && iInstr > 0; ++it, --iInstr);
    if (it != m_instruments.end())
        return *it;
    return NULL;
}

ImoOption* ImoScore::get_option(const std::string& name)
{
    std::list<ImoOption*>::iterator it;
    for (it= m_options.begin(); it != m_options.end(); ++it)
    {
        if ((*it)->get_name() == name)
            return *it;
    }
    return NULL;
}

void ImoScore::add_sytem_layout(ImoSystemLayout* pSL)
{
    if (pSL->is_first())
    {
        if (m_pSystemLayoutFirst)
            delete m_pSystemLayoutFirst;
        m_pSystemLayoutFirst = pSL;
    }
    else
    {
        if (m_pSystemLayoutOther)
            delete m_pSystemLayoutOther;
        m_pSystemLayoutOther = pSL;
    }
}


//-------------------------------------------------------------------------------------
// ImoSpacer implementation
//-------------------------------------------------------------------------------------

ImoSpacer::ImoSpacer(DtoSpacer& dto)
    : ImoStaffObj(ImoObj::k_spacer, dto)
    , m_space( dto.get_width() )
{
}


//-------------------------------------------------------------------------------------
// ImoSystemLayout implementation
//-------------------------------------------------------------------------------------

ImoSystemLayout::~ImoSystemLayout()
{
    if (m_pMargins)
        delete m_pMargins;
}



//-------------------------------------------------------------------------------------
// ImoTie implementation
//-------------------------------------------------------------------------------------

ImoTie::~ImoTie()
{
    if (m_pStartBezier)
        delete m_pStartBezier;
    if (m_pEndBezier)
        delete m_pEndBezier;
}


//-------------------------------------------------------------------------------------
// ImoTieInfo implementation
//-------------------------------------------------------------------------------------

ImoTieInfo::~ImoTieInfo()
{
    if (m_pBezier)
        delete m_pBezier;
}

int ImoTieInfo::get_line_number()
{
    if (m_pTieElm)
        return m_pTieElm->get_line_number();
    else
        return 0;
}


//-------------------------------------------------------------------------------------
// ImoTimeSignature implementation
//-------------------------------------------------------------------------------------

ImoTimeSignature::ImoTimeSignature(DtoTimeSignature& dto)
    : ImoStaffObj(ImoObj::k_time_signature, dto)
    , m_beats( dto.get_beats() )
    , m_beatType( dto.get_beat_type() )
{
}


//-------------------------------------------------------------------------------------
// ImoTupletInfo implementation
//-------------------------------------------------------------------------------------

ImoTupletInfo::ImoTupletInfo()
    : ImoSimpleObj(ImoObj::k_tuplet_info)
    , m_fStartOfTuplet(true)
    , m_nActualNum(0)
    , m_nNormalNum(0)
    , m_fShowBracket(true)
    , m_fShowNumber(true)
    , m_nPlacement(k_default)
    , m_pTupletElm(NULL)
    , m_pNR(NULL)
{
}

ImoTupletInfo::ImoTupletInfo(LdpElement* pTupletElm)
    : ImoSimpleObj(ImoObj::k_tuplet_info)
    , m_fStartOfTuplet(true)
    , m_nActualNum(0)
    , m_nNormalNum(0)
    , m_fShowBracket(true)
    , m_fShowNumber(true)
    , m_nPlacement(k_default)
    , m_pTupletElm(pTupletElm)
    , m_pNR(NULL)
{
}

int ImoTupletInfo::get_line_number()
{
    if (m_pTupletElm)
        return m_pTupletElm->get_line_number();
    else
        return 0;
}


//-------------------------------------------------------------------------------------
// global functions related to notes
//-------------------------------------------------------------------------------------

int to_step(const char& letter)
{
	switch (letter)
    {
		case 'a':	return ImoNote::A;
		case 'b':	return ImoNote::B;
		case 'c':	return ImoNote::C;
		case 'd':	return ImoNote::D;
		case 'e':	return ImoNote::E;
		case 'f':	return ImoNote::F;
		case 'g':	return ImoNote::G;
	}
	return -1;
}

int to_octave(const char& letter)
{
	switch (letter)
    {
		case '0':	return 0;
		case '1':	return 1;
		case '2':	return 2;
		case '3':	return 3;
		case '4':	return 4;
		case '5':	return 5;
		case '6':	return 6;
		case '7':	return 7;
		case '8':	return 8;
		case '9':	return 9;
	}
	return -1;
}

int to_accidentals(const std::string& accidentals)
{
    switch (accidentals.length())
    {
        case 0:
            return ImoNote::k_no_accidentals;
            break;

        case 1:
            if (accidentals[0] == '+')
                return ImoNote::k_sharp;
            else if (accidentals[0] == '-')
                return ImoNote::k_flap;
            else if (accidentals[0] == '=')
                return ImoNote::k_natural;
            else if (accidentals[0] == 'x')
                return ImoNote::k_double_sharp;
            else
                return -1;
            break;

        case 2:
            if (accidentals.compare(0, 2, "++"))
                return ImoNote::k_sharp_sharp;
            else if (accidentals.compare(0, 2, "--"))
                return ImoNote::k_flat_flat;
            else if (accidentals.compare(0, 2, "=-"))
                return ImoNote::k_natural_flat;
            else
                return -1;
            break;

        default:
            return -1;
    }
}

int to_note_type(const char& letter)
{
    //  USA           UK                      ESP               LDP     NoteType
    //  -----------   --------------------    -------------     ---     ---------
    //  long          longa                   longa             l       k_longa = 0
    //  double whole  breve                   cuadrada, breve   b       k_breve = 1
    //  whole         semibreve               redonda           w       k_whole = 2
    //  half          minim                   blanca            h       k_half = 3
    //  quarter       crochet                 negra             q       k_quarter = 4
    //  eighth        quaver                  corchea           e       k_eighth = 5
    //  sixteenth     semiquaver              semicorchea       s       k_16th = 6
    //  32nd          demisemiquaver          fusa              t       k_32th = 7
    //  64th          hemidemisemiquaver      semifusa          i       k_64th = 8
    //  128th         semihemidemisemiquaver  garrapatea        o       k_128th = 9
    //  256th         ???                     semigarrapatea    f       k_256th = 10

    switch (letter)
    {
        case 'l':     return ImoNote::k_longa;
        case 'b':     return ImoNote::k_breve;
        case 'w':     return ImoNote::k_whole;
        case 'h':     return ImoNote::k_half;
        case 'q':     return ImoNote::k_quarter;
        case 'e':     return ImoNote::k_eighth;
        case 's':     return ImoNote::k_16th;
        case 't':     return ImoNote::k_32th;
        case 'i':     return ImoNote::k_64th;
        case 'o':     return ImoNote::k_128th;
        case 'f':     return ImoNote::k_256th;
        default:
            return -1;
    }
}

bool ldp_pitch_to_components(const string& pitch, int *step, int* octave, int* accidentals)
{
    //    Analyzes string pitch (LDP format), extracts its parts (step, octave and
    //    accidentals) and stores them in the corresponding parameters.
    //    Returns true if error (pitch is not a valid pitch name)
    //
    //    In LDP pitch is represented as a combination of the step of the diatonic scale, the
    //    chromatic alteration, and the octave.
    //      - The accidentals parameter represents chromatic alteration (does not include tonal
    //        key alterations)
    //      - The octave element is represented by the numbers 0 to 9, where 4 indicates
    //        the octave started by middle C.
    //
    //    pitch must be trimed (no spaces before or after real data) and lower case

    size_t i = pitch.length() - 1;
    if (i < 1)
        return true;   //error

    *octave = to_octave(pitch[i--]);
    if (*step == -1)
        return true;   //error

    *step = to_step(pitch[i--]);
    if (*step == -1)
        return true;   //error

    if (++i == 0)
    {
        *accidentals = ImoNote::k_no_accidentals;
        return false;   //no error
    }
    else
        *accidentals = to_accidentals(pitch.substr(0, i));
    if (*accidentals == -1)
        return true;   //error

    return false;  //no error
}

NoteTypeAndDots ldp_duration_to_components(const string& duration)
{
    // Return struct with noteType and dots.
    // If error, noteType is set to unknown and dots to zero

    size_t size = duration.length();
    if (size == 0)
        return NoteTypeAndDots(ImoNoteRest::k_unknown, 0);   //error

    //duration
    int noteType = to_note_type(duration[0]);
    if (noteType == -1)
        return NoteTypeAndDots(ImoNoteRest::k_unknown, 0);   //error

    //dots
    int dots = 0;
    for (size_t i=1; i < size; i++)
    {
        if (duration[i] == '.')
            dots++;
        else
            return NoteTypeAndDots(ImoNoteRest::k_unknown, 0);   //error
    }

    return NoteTypeAndDots(noteType, dots);   //no error
}

float to_duration(int nNoteType, int nDots)
{
    //compute duration without modifiers
    float rDuration = pow(2.0f, (10 - nNoteType));

    //take dots into account
    switch (nDots)
    {
        case 0:                             break;
        case 1: rDuration *= 1.5f;          break;
        case 2: rDuration *= 1.75f;         break;
        case 3: rDuration *= 1.875f;        break;
        case 4: rDuration *= 1.9375f;       break;
        case 5: rDuration *= 1.96875f;      break;
        case 6: rDuration *= 1.984375f;     break;
        case 7: rDuration *= 1.9921875f;    break;
        case 8: rDuration *= 1.99609375f;   break;
        case 9: rDuration *= 1.998046875f;  break;
        default:
            ;
            //wxLogMessage(_T("[to_duration] Program limit: do you really need more than nine dots?"));
    }

    return rDuration;
}


}  //namespace lenmus
