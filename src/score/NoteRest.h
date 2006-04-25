// RCS-ID: $Id: NoteRest.h,v 1.4 2006/02/23 19:23:54 cecilios Exp $
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
/*! @file NoteRest.h
    @brief Header file for class lmNoteRest
    @ingroup score_kernel
*/

//-------------------------------------------------------------------------------------------
//Este programa maneja tres tipos de alturas:
// AltDiatonica: Altura diat�nica. Se refiere al nombre de la nota, sin tener en cuenta las
//   alteraciones.
//       0 - silencio
//       1 - C0  - Do2 de la subcontraoctava (16,35 Hz)
//       8 - C1  - Do1 de la contraoctava
//      15 - C2  - Do de la gran octava
//      22 - C3  - do de la peque�a octava
//      29 - C4  - do1 de la octava primera (la1 = 440Hz)
//      36 - C5  - do2 de la octava segunda
//      43 - C6  - do3 de la octava tercera
//      50 - C7  - do4 de la octava cuarta
//      57 - C8  - do5 de la octava quinta (4.186 Hz)
//      etc.
//
// AlturaRel: Altura relativa. Igual que la absoluta pero referida a la primera l�nea del
//   pentagrama correspondiente a la clave en que se dibuja la partitura:
//       0 - una linea inferior (Do en clave de Sol, Mi en clave de Fa4)
//       1 - en espacio debajo de primera l�nea (Re en clave de Sol)
//       2 - en primera l�nea (Mi en clave de Sol)
//       3 - en primer espacio
//       4 - en segunda l�nea
//       5 - en segundo espacio
//       etc.
//
// AlturaMIDI: Altura crom�trica. Es la nota MIDI equivalente, incluyendo las alteraciones que hubiera.
//   Es un dato derivado que se obtiene cuando hace falta
//-------------------------------------------------------------------------------------------
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

#ifdef __GNUG__
// #pragma interface
#endif

#ifndef __NOTEREST_H__        //to avoid nested includes
#define __NOTEREST_H__

#define DEFINE_REST        true
#define DEFINE_NOTE        false

// struct BeamInfo represents raw info about this note beaming. It is the same information
// present on a <beam> MusicXML tag.
struct lmTBeamInfo
{
    EBeamType    Type;
    bool        Repeat;
};


class lmNoteRest:  public lmCompositeObj
{
public:
    //ctors and dtor
    lmNoteRest(lmVStaff* pVStaff, bool IsRest, ENoteType nNoteType, float rDuration,
             bool fDotted, bool fDoubleDotted, wxInt32 nStaff);
    virtual ~lmNoteRest();

    virtual void DrawObject(bool fMeasuring, lmPaper* pPaper, wxColour colorC) = 0;
    virtual wxBitmap* GetBitmap(double rScale) = 0;
    virtual wxString Dump() = 0;

    bool IsRest() { return m_fIsRest; }
    virtual bool IsInChord() = 0;    

    //implementation of virtual methods of base class lmCompositeObj
    virtual lmScoreObj* FindSelectableObject(wxPoint& pt)=0;

    // methods related to note/rest positioning information
    lmLUnits GetStaffOffset() { return m_pVStaff->GetStaffOffset(m_nStaffNum); }

    //methods related to associated AuxObjs management
    void AddFermata(bool fOverNote = true);
    void AddLyric(lmLyric* pLyric);

    //methods related to duration
    float GetDuration() { return m_rDuration; }

    // methods related to beams
    void CreateBeam(bool fBeamed, lmTBeamInfo BeamInfo[]);
    bool IsBeamed() { return m_fBeamed; }
    EBeamType GetBeamType(int level) { return m_BeamInfo[level].Type; }
    void SetBeamType(int level, EBeamType type) { m_BeamInfo[level].Type = type; }

    //methods related to tuplets
    void SetTupletBracket(lmTupletBracket* pTB) { m_pTupletBracket = pTB; }

    //methods related to sound
    void AddMidiEvents(lmSoundManager* pSM, float rMeasureStartTime, int nChannel,
                       int nMeasure);

    //accessors
    ENoteType GetNoteType() { return m_nNoteType; }


protected:
    bool ParseTipoNota(wxString& sData);
    wxString GetLDPNoteType();
        
        //
        // member variables
        //

    bool        m_fIsRest;          //This lmNoteRest is a rest

    wxInt32     m_nPentagrama;      //num of staff on which this lmNoteRest is drawn 
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

    //wxInt32        m_xPos, m_yPos;    //par�metros para Redibujar

    //AuxObjs associated to this note
    AuxObjsList*    m_pNotations;     //list of Notations
    AuxObjsList*    m_pLyrics;        //list of Lyrics

};

// declare a list of NoteRests class
#include "wx/list.h"
WX_DECLARE_LIST(lmNoteRest, NoteRestsList);

// global functions related to noterests
extern int LDPNoteTypeToEnumNoteType(wxString sNoteType);
extern float LDPNoteTypeToDuration(wxString sNoteType);
extern float NoteTypeToDuration(ENoteType nNoteType, bool fDotted, bool fDoubleDotted);



#endif    // __NOTEREST_H__
