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

#ifndef __LOMSE_SHAPE_BASE_H__        //to avoid nested includes
#define __LOMSE_SHAPE_BASE_H__

#include "lomse_gm_basic.h"
//#include <vector>
#include <list>
using namespace std;

namespace lomse
{

////forward declarations
//class InternalModel;


////---------------------------------------------------------------------------------------
////level of shape
//enum lmEShapeLevel
//{
//    lm_eMainShape = 0,      //the normal, single shape generated by an ScoreObj
//    lm_eSecondaryShape,     //key and time signatures in system 1 for staves > 1
//    lm_ePrologShape,        //clef and key signatures in systems > 1
//};
//
//#define lmSELECTABLE         true
//#define lmNO_SELECTABLE      false





//---------------------------------------------------------------------------------------
// enums for common values: aligment, justification, placement, etc.

//line styles
enum ELineStyle { k_line_none=0, k_line_solid, k_line_long_dash,
                  k_line_short_dash, k_line_dot, k_line_dot_dash, };

//line termination styles
enum ELineCap { k_cap_none = 0, k_cap_arrowhead, k_cap_arrowtail,
                k_cap_circle, k_cap_square, k_cap_diamond, };

//line edges
enum ELineEdge
{
    k_edge_normal = 0,        // edge is perpendicular to line
    k_edge_vertical,          // edge is always a vertical line
    k_edge_horizontal         // edge is always a horizontal line
};


//---------------------------------------------------------------------------------------
//EVAlign is used to indicate vertical alignment within a block: to the top,
//middle or bottom
enum EVAlign
{
    k_valign_default = 0,   //alignment is not specified
    k_valign_top,
    k_valign_middle,
    k_valign_bottom,
};

//---------------------------------------------------------------------------------------
// EHAlign is used to indicate object justification
enum EHAlign
{
    k_halign_default = 0,   //alignment is not specified
    k_halign_left,          //object aligned on left side
    k_halign_right,         //object aligned on right side
    k_halign_justify,       //object justified on both sides
    k_halign_center,        //object centered
};

//---------------------------------------------------------------------------------------
enum ELinkType
{
	k_link_simple = 0,
    k_link_start,
	k_link_middle,
	k_link_end,
};



//---------------------------------------------------------------------------------------
// auxiliary, to identify staffobjs associated to a voice and manage voice data
class VoiceRelatedShape
{
protected:
    int m_voice;

public:
    VoiceRelatedShape() : m_voice(0) {}
    virtual ~VoiceRelatedShape() {}

    //overrides required by shapes related to scores
    bool is_voice_related_shape() { return true; }

    //specific info
    inline void set_voice(int voice) { m_voice = voice; }
    inline int get_voice() { return m_voice; }
};



//---------------------------------------------------------------------------------------
class GmoSimpleShape : public GmoShape
{
public:
    virtual ~GmoSimpleShape();

protected:
    GmoSimpleShape(ImoObj* pCreatorImo, int objtype, ShapeId idx, Color color);
};

//---------------------------------------------------------------------------------------
class GmoCompositeShape : public GmoShape
{
protected:
	std::list<GmoShape*> m_components;	//constituent shapes
    bool m_fLocked;                     //constituent shapes cannot be moved

public:
    virtual ~GmoCompositeShape();

    virtual int add(GmoShape* pShape);
    inline bool is_locked() { return m_fLocked; }
    inline void unlock() { m_fLocked = false; }
    void lock();

    //virtual methods from base class
    virtual void shift_origin(const USize& shift);
    void on_draw(Drawer* pDrawer, RenderOptions& opt);

    //for unit tests
    inline std::list<GmoShape*>& get_components() { return m_components; }


protected:
    GmoCompositeShape(ImoObj* pCreatorImo, int objtype, ShapeId idx, Color color);
	void recompute_bounds();
};


}   //namespace lomse

#endif    // __LOMSE_SHAPE_BASE_H__
