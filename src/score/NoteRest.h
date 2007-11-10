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

//THINK: -----------------------------------------------------------------------------------
//Pregunta: �No deber�a existir un constructor en lmNoteRest capaz de crear una nota a partir del fuente?
//Resp: Ning�n PentObj deben crearse fuera de un pentagrama y luego agregarse a �l.
//Pregunta: �Quiz�, entonces, una funcion en CStaff que admita un fuente? �Hay algo as� para
//   otros tipos de PentObjs?
//Resp: Si; ahora mismo existen algunas. Todos los restantes PentObjs
//   se crean a partir de par�metros, no de un fuente.
//Pregunta: �Deben mantenerse o eliminarse?
//Resp: No veo razones por las que una funcion de creacion en CStaff no pueda admitir ambos
//   tipos de argumentos: un fuente o sus par�metros ya procesados. El �nico argumento es que
//   si admite un fuente, el an�lisis del fuente no debe duplicarse, sino que debe estar
//   centralizado en algun sitio, quiza en MParseFile. Ello implica que la esta funci�n de
//   analisis tiene que devolver par�metros, con lo que terminamos en que la creaci�n se
//   hace, al final, a partir de sus par�metros no de su fuente. Sin embargo, puede dar
//   flexibilidad.
//Conclusi�n: Pueden existir funciones de creaci�n a partir de los fuentes, pero siempre
//   son auxiliares ya que tienen que tener la
//   restricci�n de que, internamente, llamen a la funci�n de an�lisis y, tras ella, a la
//   de creaci�n por par�metros.
//Problema: La funci�n de an�lisis de MParse trabaja con una estructura de CNodos no con el
//   fuente plano. Realizar otra para an�lisis del fuente plano duplicar�a el trabajo s�lo
//   con el objeto de no llamar internamente con par�mtros sino con fuente plano
//Conclusi�n final: Se rechaza la creaci�n directa a partir de fuente. Puede hacerse s�lo para
//   encapsular la llamada a la funci�n de an�lisis
//---------------------------------------------------------------------------------------------

#ifndef __NOTEREST_H__        //to avoid nested includes
#define __NOTEREST_H__

#ifdef __GNUG__
#pragma interface "NoteRest.cpp"
#endif

#define DEFINE_REST        true
#define DEFINE_NOTE        false

// struct BeamInfo represents raw info about this note beaming. It is the same information
// present on a <beam> MusicXML tag.
struct lmTBeamInfo
{
    EBeamType    Type;
    bool        Repeat;
};


class lmNoteRest:  public lmStaffObj
{
public:
    //ctors and dtor
    lmNoteRest(lmVStaff* pVStaff, bool IsRest, ENoteType nNoteType, float rDuration,
             bool fDotted, bool fDoubleDotted, int nStaff, bool fVisible);
    virtual ~lmNoteRest();

    virtual void DrawObject(bool fMeasuring, lmPaper* pPaper, wxColour colorC, 
                            bool fHighlight) = 0;
    virtual wxBitmap* GetBitmap(double rScale) = 0;
    virtual wxString Dump() = 0;

    bool IsRest() const { return m_fIsRest; }
    virtual bool IsInChord() = 0;    

    //implementation of virtual methods of base class lmStaffObj
    virtual lmScoreObj* FindSelectableObject(lmUPoint& pt)=0;
    virtual bool IsComposite() { return true; }


    // methods related to note/rest positioning information
    lmLUnits GetStaffOffset() const { return m_pVStaff->GetStaffOffset(m_nStaffNum); }

    //methods related to associated AuxObjs management
    void AddFermata(const lmEPlacement nPlacement);
    void AddLyric(lmLyric* pLyric);

    //methods related to duration
    float GetDuration() const { return m_rDuration; }

    // methods related to beams
    void CreateBeam(bool fBeamed, lmTBeamInfo BeamInfo[]);
    bool IsBeamed() const { return m_fBeamed; }
    EBeamType GetBeamType(int level) { return m_BeamInfo[level].Type; }
    void SetBeamType(int level, EBeamType type) { m_BeamInfo[level].Type = type; }
	inline lmBeam* GetBeam() { return m_pBeam; }

    //methods related to tuplets
    void SetTupletBracket(lmTupletBracket* pTB) { m_pTupletBracket = pTB; }

    //methods related to sound
    void AddMidiEvents(lmSoundManager* pSM, float rMeasureStartTime, int nChannel,
                       int nMeasure);

    //accessors
    ENoteType GetNoteType() const { return m_nNoteType; }


protected:
    wxString GetLDPNoteType();
    lmLUnits DrawDot(bool fMeasuring, lmPaper* pPaper, lmLUnits xPos, lmLUnits yPos, 
                     wxColour colorC, bool fUseFont);
    lmLUnits AddDotShape(lmCompositeShape* pCS, lmPaper* pPaper, lmLUnits xPos, lmLUnits yPos, 
                         wxColour colorC);
        
        //
        // member variables
        //

    bool        m_fIsRest;          //This lmNoteRest is a rest
    ENoteType   m_nNoteType;        //type of note / rest
    
    //duration and time modifiers
    float       m_rDuration;            //duration as defined in MusicXML: duration/divisions
    bool        m_fDotted;
    bool        m_fDoubleDotted;
    bool        m_fCalderon;            //tiene calder�n

    // beaming information
    bool        m_fBeamed;              // note is beamed
    lmBeam*     m_pBeam;                //beaming information
    lmTBeamInfo m_BeamInfo[6];          //beam mode for each level

    //tuplet related variables
    lmTupletBracket*    m_pTupletBracket;    //ptr to lmTupletBracket if this note/rest is part of a tuplet

    //AuxObjs associated to this note
    AuxObjsList*    m_pNotations;     //list of Notations
    AuxObjsList*    m_pLyrics;        //list of Lyrics

};

// declare a list of NoteRests class
#include "wx/list.h"
WX_DECLARE_LIST(lmNoteRest, NoteRestsList);

// global functions related to noterests
extern int LDPNoteTypeToEnumNoteType(const wxString& sNoteType);
extern float LDPNoteTypeToDuration(const wxString& sNoteType);
extern float NoteTypeToDuration(ENoteType nNoteType, bool fDotted, bool fDoubleDotted);



#endif    // __NOTEREST_H__
