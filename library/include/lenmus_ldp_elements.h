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

#ifndef __LML_LDP_ELEMENTS_H__        //to avoid nested includes
#define __LML_LDP_ELEMENTS_H__

#include <vector>

#include "lenmus_smart_pointer.h"
#include "lenmus_tree.h"
#include "lenmus_visitor.h"


namespace lenmus
{

enum ELdpElements
{
    eElmFirst = 0,
    k_undefined = eElmFirst,
    
    //simple, generic elements
    k_label,
    k_number,
    k_string,

    //composite elements
    k_abbrev,
    k_above,
    k_anchorLine,
    k_barline,
    k_beam,
    k_below,
    k_bezier,
    k_bold,
    k_bold_italic,
    k_border,
    k_brace,
    k_bracket,
    k_center,
    k_chord,
    k_clef,
    k_color,
    k_content,
    k_ctrol1_x,
    k_ctrol1_y,
    k_creationMode,
    k_cursor,
    k_defineStyle,
    k_down,
    k_duration,
    k_dx,
    k_dy,
    k_end,
    k_endPoint,
    k_fermata,
    k_fbline,
    k_figuredBass,
    k_font,
    k_goBack,
    k_goFwd,
    k_graphic,
    k_group,
    k_hasWidth,
    k_height,
    k_infoMIDI,
    k_instrName,
    k_instrument,
    k_italic,
    k_joinBarlines,
    k_key,
    k_landscape,
    k_language,
    k_left,
    k_lenmusdoc,
    k_line,
    k_lineCapEnd,
    k_lineCapStart,
    k_lineStyle,
    k_lineThickness,
    k_metronome,
    k_musicData,
    k_name,
    k_newSystem,
    k_na,           //note in chord
    k_no,
    k_normal,
    k_note,             //n - note
    k_noVisible,
    k_opt,
    k_pageLayout,
    k_pageMargins,
    k_pageSize,
    k_parentheses,
    k_pitch,
    k_portrait,
    k_rest,         //r - rest 
    k_right,
    k_size,
    k_score,
    k_spacer,
    k_split,
    k_staff,
    k_staffDistance,
    k_staffLines,
    k_staffSpacing,
    k_staffType,
    k_start,
    k_start_x,
    k_start_y,
    k_startPoint,
    k_staves,
    k_stem,
    k_style,
    k_symbol,
    k_systemLayout,
    k_systemMargins,
    k_t,   //tuplet 
    k_text,
    k_textbox,
    k_tie,
    k_time,
    k_title,
    k_vers,
    k_width,
    k_undoData,
    k_up,
    k_yes,

    eElmLast,
};

class LdpElement;
class ImObj;
typedef SmartPtr<LdpElement>    SpLdpElement;

//-------------------------------------------------------------------------------
// A generic LDP element representation.
//
// An LdpElement is the content of a node in the Ldp source tree. I combines 
// links to other nodes as well as the actual element data.
// There are two types of elements:
//   - simple: it is just a type (label, string or number) and its value. They
//     are similar to LISP atoms.
//   - composite: they have a name and any number of parameters (zero 
//     is allowed). They are like LISP lists.
//-------------------------------------------------------------------------------

class LM_EXPORT LdpElement : public Visitable, virtual public RefCounted,
                             public NodeInTree<LdpElement>
{
protected:
	ELdpElements m_type;    // the element type
	std::string	m_name;     // for composite: element name
	std::string m_value;    // for simple: the element value
    bool m_fSimple;         // true for simple elements
    int m_numLine;          // file line in whicht the elemnt starts or 0
    long m_id;              // for composite: element ID (0..n)
    ImObj* m_pImo;          // for composite: attached ImObj

    LdpElement();

public:
    virtual ~LdpElement();

    //overrides to Visitable class members
	virtual void accept_in(BaseVisitor& v);
	virtual void accept_out(BaseVisitor& v);

    //getters and setters
	inline void set_value(const std::string& value) { m_value = value; }
    inline const std::string& get_value() { return m_value; }
    float get_value_as_float();
    inline void set_name(const std::string& name) { m_name = name; }
	inline const std::string& get_name() { return m_name; }
	inline ELdpElements get_type() { return m_type; }
    inline void set_num_line(int numLine) { m_numLine = numLine; }
    inline int get_line_number() { return m_numLine; }
    void set_imobj(ImObj* pImo);
    inline ImObj* get_imobj() { return m_pImo; }
    inline long get_id() { return m_id; }
    inline void set_id(long id) { m_id = id; }

	//! returns the element value as it is represented in source LDP
	std::string get_ldp_value();
	//! elements comparison
	bool operator ==(LdpElement& element);
	inline bool operator !=(LdpElement& element) { return !(*this == element); }

    std::string to_string();
    std::string to_string_with_ids();

    inline bool is_simple() { return m_fSimple; }
    inline void set_simple() { m_fSimple = true; }
	inline bool has_children() { return !is_terminal(); }
    int get_num_parameters();

    //helper methods
    inline bool is_type(ELdpElements type) { return get_type() == type; }

    //! random access to parameter i (1..n)
    LdpElement* get_parameter(int i);

    //---------------------------------------------------------------------------
    //TO_REMOVE
    // Compatibility with lmLDPNode
    //---------------------------------------------------------------------------
    inline bool IsProcessed() { return m_fProcessed; }
    inline void SetProcessed(bool fValue) { m_fProcessed = fValue; }
    inline bool IsSimple() { return is_simple(); }
    inline long GetNumLine() { return get_line_number(); }
    LdpElement* GetParameter(int i);
    LdpElement* GetParameterFromName(const std::string& name);
    LdpElement* StartIterator(long iP=1, bool fOnlyNotProcessed = true);
    LdpElement* GetNextParameter(bool fOnlyNotProcessed = true);

    protected:
        bool            m_fProcessed;       //the node has been processed
        int             m_iP;               //for sequential accsess
    //END_TO_REMOVE
    //---------------------------------------------------------------------------
};

/*!
 \brief A specific LDP element representation.

    For each ldp element we define a specific class. It is equivalent to defining
    specific lmLDPNodes for each ldp tag. In this way we have specific nodes
    LdpObject<type>.
*/
template <ELdpElements type>
class LdpObject : public LdpElement
{ 
    protected:	
        LdpObject() : LdpElement() { m_type = type; }

	public:
        //! static constructor to be used by Factory
		static LdpElement* new_ldp_object()	
			{ LdpObject<type>* o = new LdpObject<type>; assert(o!=0); return o; }

        //! implementation of Visitable interface
        virtual void accept_in(BaseVisitor& v) {
			if (Visitor<LdpObject<type> >* p = dynamic_cast<Visitor<LdpObject<type> >*>(&v))
            {
				p->start_visit(*this);
			}
			else LdpElement::accept_in(v);
		}

        virtual void accept_out(BaseVisitor& v) {
			if (Visitor<LdpObject<type> >* p = dynamic_cast<Visitor<LdpObject<type> >*>(&v))
            {
				p->end_visit(*this);
			}
			else LdpElement::accept_out(v);
		}
};

/// A tree of LdpElements
typedef Tree<LdpElement>        LdpTree;
typedef SmartPtr<LdpTree>       SpLdpTree; 
typedef NodeInTree<LdpElement>  LdpNode;
typedef SmartPtr<LdpNode>       SpLdpNode;

//typedefs for all LDP elements
typedef SmartPtr<LdpObject<k_abbrev> >       SpLdpAbbrev;
typedef SmartPtr<LdpObject<k_above> >        SpLdpAbove;
typedef SmartPtr<LdpObject<k_anchorLine> >   SpLdpAnchorLine;
typedef SmartPtr<LdpObject<k_barline> >      SpLdpBarline;
typedef SmartPtr<LdpObject<k_beam> >         SpLdpBeam;
typedef SmartPtr<LdpObject<k_below> >        SpLdpBelow;
typedef SmartPtr<LdpObject<k_bezier> >       SpLdpBezier;
typedef SmartPtr<LdpObject<k_bold> >         SpLdpBold;
typedef SmartPtr<LdpObject<k_bold_italic> >  SpLdpBoldItalic;
typedef SmartPtr<LdpObject<k_border> >       SpLdpBorder;
typedef SmartPtr<LdpObject<k_brace> >        SpLdpBrace;
typedef SmartPtr<LdpObject<k_bracket> >      SpLdpBracket;
typedef SmartPtr<LdpObject<k_center> >       SpLdpCenter;
typedef SmartPtr<LdpObject<k_chord> >        SpLdpChord;
typedef SmartPtr<LdpObject<k_clef> >         SpLdpClef;
typedef SmartPtr<LdpObject<k_color> >        SpLdpColor;  
typedef SmartPtr<LdpObject<k_content> >      SpLdpContent;
typedef SmartPtr<LdpObject<k_creationMode> > SpLdpCreationMode; 
typedef SmartPtr<LdpObject<k_ctrol1_x> >     SpLdpCtrol1X;
typedef SmartPtr<LdpObject<k_ctrol1_y> >     SpLdpCtrol1Y;
typedef SmartPtr<LdpObject<k_cursor> >       SpLdpCursor;            
typedef SmartPtr<LdpObject<k_defineStyle> >  SpLdpDefineStyle;
typedef SmartPtr<LdpObject<k_down> >         SpLdpDown;
typedef SmartPtr<LdpObject<k_duration> >     SpLdpDuration;
typedef SmartPtr<LdpObject<k_dx> >           SpLdpDx;
typedef SmartPtr<LdpObject<k_dy> >           SpLdpDy;
typedef SmartPtr<LdpObject<k_end> >          SpLdpEnd;
typedef SmartPtr<LdpObject<k_endPoint> >     SpLdpEndPoint;
typedef SmartPtr<LdpObject<k_fbline> >       SpLdpFbline;
typedef SmartPtr<LdpObject<k_fermata> >      SpLdpFermata;
typedef SmartPtr<LdpObject<k_figuredBass> >  SpLdpFiguredBass;
typedef SmartPtr<LdpObject<k_font> >         SpLdpFont;
typedef SmartPtr<LdpObject<k_goBack> >       SpLdpGoBack;
typedef SmartPtr<LdpObject<k_goFwd> >        SpLdpGoFwd;
typedef SmartPtr<LdpObject<k_graphic> >      SpLdpGraphic;
typedef SmartPtr<LdpObject<k_group> >        SpLdpGroup;
typedef SmartPtr<LdpObject<k_hasWidth> >     SpLdpHasWidth;
typedef SmartPtr<LdpObject<k_height> >       SpLdpHeight;
typedef SmartPtr<LdpObject<k_infoMIDI> >     SpLdpInfoMIDI;
typedef SmartPtr<LdpObject<k_instrName> >    SpLdpInstrName;
typedef SmartPtr<LdpObject<k_instrument> >   SpLdpInstrument;
typedef SmartPtr<LdpObject<k_italic> >       SpLdpItalic;
typedef SmartPtr<LdpObject<k_joinBarlines> > SpLdpJoinBarlines;
typedef SmartPtr<LdpObject<k_key> >          SpLdpKey;
typedef SmartPtr<LdpObject<k_landscape> >    SpLdpLandscape;
typedef SmartPtr<LdpObject<k_left> >         SpLdpLeft;
typedef SmartPtr<LdpObject<k_lenmusdoc> >    SpLdpLenmusdoc;
typedef SmartPtr<LdpObject<k_line> >         SpLdpLine;
typedef SmartPtr<LdpObject<k_lineCapEnd> >   SpLdpLineCapEnd;
typedef SmartPtr<LdpObject<k_lineCapStart> > SpLdpLineCapStart;
typedef SmartPtr<LdpObject<k_lineStyle> >    SpLdpLineStyle;
typedef SmartPtr<LdpObject<k_lineThickness> > SpLdpLineThickness;
typedef SmartPtr<LdpObject<k_metronome> >    SpLdpMetronome;
typedef SmartPtr<LdpObject<k_musicData> >    SpLdpMusicData;
typedef SmartPtr<LdpObject<k_name> >         SpLdpName;
typedef SmartPtr<LdpObject<k_newSystem> >    SpLdpNewSystem;
typedef SmartPtr<LdpObject<k_na> >           SpLdpNa;           //note in chord
typedef SmartPtr<LdpObject<k_no> >           SpLdpNo;
typedef SmartPtr<LdpObject<k_normal> >       SpLdpNormal;
typedef SmartPtr<LdpObject<k_note> >         SpLdpNote;   // "n"
typedef SmartPtr<LdpObject<k_noVisible> >    SpLdpNoVisible;
typedef SmartPtr<LdpObject<k_opt> >          SpLdpOpt;
typedef SmartPtr<LdpObject<k_pageLayout> >   SpLdpPageLayout;
typedef SmartPtr<LdpObject<k_pageMargins> >  SpLdpPageMargins;
typedef SmartPtr<LdpObject<k_pageSize> >     SpLdpPageSize;
typedef SmartPtr<LdpObject<k_parentheses> >  SpLdpParentheses;
typedef SmartPtr<LdpObject<k_pitch> >        SpLdpPitch;
typedef SmartPtr<LdpObject<k_portrait> >     SpLdpPortrait;
typedef SmartPtr<LdpObject<k_rest> >         SpLdpRest; 
typedef SmartPtr<LdpObject<k_right> >        SpLdpRight;
typedef SmartPtr<LdpObject<k_score> >        SpLdpScore;
typedef SmartPtr<LdpObject<k_size> >         SpLdpSize;
typedef SmartPtr<LdpObject<k_spacer> >       SpLdpSpacer;
typedef SmartPtr<LdpObject<k_split> >        SpLdpSplit;
typedef SmartPtr<LdpObject<k_staff> >        SpLdpStaff;
typedef SmartPtr<LdpObject<k_staffDistance> > SpLdpStaffDistance;
typedef SmartPtr<LdpObject<k_staffLines> >   SpLdpStaffLines;
typedef SmartPtr<LdpObject<k_staffSpacing> > SpLdpStaffSpacing;
typedef SmartPtr<LdpObject<k_staffType> >    SpLdpStaffType;
typedef SmartPtr<LdpObject<k_start> >        SpLdpStart;
typedef SmartPtr<LdpObject<k_start_x> >      SpLdpStartX;
typedef SmartPtr<LdpObject<k_start_y> >      SpLdpStartY;
typedef SmartPtr<LdpObject<k_startPoint> >   SpLdpStartPoint;
typedef SmartPtr<LdpObject<k_staves> >       SpLdpStaves;
typedef SmartPtr<LdpObject<k_stem> >         SpLdpStem;
typedef SmartPtr<LdpObject<k_style> >        SpLdpStyle;
typedef SmartPtr<LdpObject<k_string> >       SpLdpString;
typedef SmartPtr<LdpObject<k_symbol> >       SpLdpSymbol;
typedef SmartPtr<LdpObject<k_t> >            SpLdpT;   //tuplet 
typedef SmartPtr<LdpObject<k_text> >         SpLdpText;
typedef SmartPtr<LdpObject<k_textbox> >      SpLdpTextbox;
typedef SmartPtr<LdpObject<k_tie> >          SpLdpTie;
typedef SmartPtr<LdpObject<k_time> >         SpLdpTime;
typedef SmartPtr<LdpObject<k_title> >        SpLdpTitle;
typedef SmartPtr<LdpObject<k_undoData> >     SpLdpUndoData;
typedef SmartPtr<LdpObject<k_up> >           SpLdpUp;
typedef SmartPtr<LdpObject<k_vers> >         SpLdpVers;
typedef SmartPtr<LdpObject<k_width> >        SpLdpWidth;
typedef SmartPtr<LdpObject<k_yes> >          SpLdpYes;

//typedefs for all LDP elements
typedef LdpObject<k_abbrev>       LdpAbbrev;
typedef LdpObject<k_above>        LdpAbove;
typedef LdpObject<k_anchorLine>   LdpAnchorLine;
typedef LdpObject<k_barline>      LdpBarline;
typedef LdpObject<k_beam>         LdpBeam;
typedef LdpObject<k_below>        LdpBelow;
typedef LdpObject<k_bezier>       LdpBezier;
typedef LdpObject<k_bold>         LdpBold;
typedef LdpObject<k_bold_italic>  LdpBoldItalic;
typedef LdpObject<k_border>       LdpBorder;
typedef LdpObject<k_brace>        LdpBrace;
typedef LdpObject<k_bracket>      LdpBracket;
typedef LdpObject<k_center>       LdpCenter;
typedef LdpObject<k_chord>        LdpChord;
typedef LdpObject<k_clef>         LdpClef;
typedef LdpObject<k_color>        LdpColor;
typedef LdpObject<k_content>      LdpContent;
typedef LdpObject<k_creationMode> LdpCreationMode; 
typedef LdpObject<k_ctrol1_x>     LdpCtrol1X;
typedef LdpObject<k_ctrol1_y>     LdpCtrol1Y;
typedef LdpObject<k_cursor>       LdpCursor;            
typedef LdpObject<k_defineStyle>  LdpDefineStyle;
typedef LdpObject<k_down>         LdpDown;
typedef LdpObject<k_duration>     LdpDuration;
typedef LdpObject<k_dx>           LdpDx;
typedef LdpObject<k_dy>           LdpDy;
typedef LdpObject<k_end>          LdpEnd;
typedef LdpObject<k_endPoint>     LdpEndPoint;
typedef LdpObject<k_fbline>       LdpFbline;
typedef LdpObject<k_fermata>      LdpFermata;
typedef LdpObject<k_figuredBass>  LdpFiguredBass;
typedef LdpObject<k_font>         LdpFont;
typedef LdpObject<k_goBack>       LdpGoBack;
typedef LdpObject<k_goFwd>        LdpGoFwd;
typedef LdpObject<k_graphic>      LdpGraphic;
typedef LdpObject<k_group>        LdpGroup;
typedef LdpObject<k_hasWidth>     LdpHasWidth;
typedef LdpObject<k_height>       LdpHeight;
typedef LdpObject<k_infoMIDI>     LdpInfoMIDI;
typedef LdpObject<k_instrName>    LdpInstrName;
typedef LdpObject<k_instrument>   LdpInstrument;
typedef LdpObject<k_italic>       LdpItalic;
typedef LdpObject<k_joinBarlines> LdpJoinBarlines;
typedef LdpObject<k_key>          LdpKey;
typedef LdpObject<k_landscape>    LdpLandscape;
typedef LdpObject<k_left>         LdpLeft;
typedef LdpObject<k_lenmusdoc>    LdpLenmusdoc;
typedef LdpObject<k_line>         LdpLine;
typedef LdpObject<k_lineCapEnd>   LdpLineCapEnd;
typedef LdpObject<k_lineCapStart> LdpLineCapStart;
typedef LdpObject<k_lineStyle>    LdpLineStyle;
typedef LdpObject<k_lineThickness> LdpLineThickness;
typedef LdpObject<k_metronome>    LdpMetronome;
typedef LdpObject<k_musicData>    LdpMusicData;
typedef LdpObject<k_name>         LdpName;
typedef LdpObject<k_newSystem>    LdpNewSystem;
typedef LdpObject<k_na>           LdpNa;           //note in chord
typedef LdpObject<k_no>           LdpNo;
typedef LdpObject<k_normal>       LdpNormal;
typedef LdpObject<k_note>         LdpNote;   // "n"
typedef LdpObject<k_noVisible>    LdpNoVisible;
typedef LdpObject<k_opt>          LdpOpt;
typedef LdpObject<k_pageLayout>   LdpPageLayout;
typedef LdpObject<k_pageMargins>  LdpPageMargins;
typedef LdpObject<k_pageSize>     LdpPageSize;
typedef LdpObject<k_parentheses>  LdpParentheses;
typedef LdpObject<k_pitch>        LdpPitch;
typedef LdpObject<k_portrait>     LdpPortrait;
typedef LdpObject<k_rest>         LdpRest; 
typedef LdpObject<k_right>        LdpRight;
typedef LdpObject<k_score>        LdpScore;
typedef LdpObject<k_size>         LdpSize;
typedef LdpObject<k_spacer>       LdpSpacer;
typedef LdpObject<k_split>        LdpSplit;
typedef LdpObject<k_staff>        LdpStaff;
typedef LdpObject<k_staffDistance> LdpStaffDistance;
typedef LdpObject<k_staffLines>   LdpStaffLines;
typedef LdpObject<k_staffSpacing> LdpStaffSpacing;
typedef LdpObject<k_staffType>    LdpStaffType;
typedef LdpObject<k_start>        LdpStart;
typedef LdpObject<k_start_x>      LdpStartX;
typedef LdpObject<k_start_y>      LdpStartY;
typedef LdpObject<k_startPoint>   LdpStartPoint;
typedef LdpObject<k_staves>       LdpStaves;
typedef LdpObject<k_stem>         LdpStem;
typedef LdpObject<k_style>        LdpStyle;
typedef LdpObject<k_string>       LdpString;
typedef LdpObject<k_symbol>       LdpSymbol;
typedef LdpObject<k_t>            LdpT;   //tuplet 
typedef LdpObject<k_text>         LdpText;
typedef LdpObject<k_textbox>      LdpTextbox;
typedef LdpObject<k_tie>          LdpTie;
typedef LdpObject<k_time>         LdpTime;
typedef LdpObject<k_title>        LdpTitle;
typedef LdpObject<k_undoData>     LdpUndoData;
typedef LdpObject<k_up>           LdpUp;
typedef LdpObject<k_vers>         LdpVers;
typedef LdpObject<k_width>        LdpWidth;
typedef LdpObject<k_yes>          LdpYes;


}   //namespace lenmus
#endif    // __LML_LDP_ELEMENTS_H__

