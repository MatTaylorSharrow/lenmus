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

#ifndef __LML_SCORE_ITERATOR_H__
#define __LML_SCORE_ITERATOR_H__

#include <vector>
#include "lenmus_staffobjs_table.h"

using namespace std;

namespace lenmus
{

//forward declarations
//class Document;
class ColStaffObjs;


//-------------------------------------------------------------------------------------
// ScoreIterator: A cursor to traverse the ColStaffObjs
//-------------------------------------------------------------------------------------

class ScoreIterator
{
protected:
    ColStaffObjs*              m_pColStaffObjs;
    ColStaffObjs::iterator     m_it;

public:
    ScoreIterator(ColStaffObjs* pColStaffObjs);
    ~ScoreIterator();

 //   inline bool FirstOfCollection() { 
 //                   return (m_pSO && m_pSO == m_pColStaffObjs->GetFirstSO()); }
 //   inline bool LastOfCollection() { 
 //                   return (m_pSO && m_pSO == m_pColStaffObjs->GetLastSO()); }
 //   inline bool StartOfCollection() { 
 //                   return FirstOfCollection() || m_pColStaffObjs->IsEmpty(); }
 //   inline bool EndOfCollection() { 
 //                   return (m_pSO == (lmStaffObj*)NULL || (FirstOfCollection() && m_fEnd)); }

 //   inline bool ChangeOfMeasure() { return m_fChangeOfMeasure; }
	//inline lmStaffObj* GetCurrent() { return m_pSO; }
 //   inline int GetNumSegment() { return (m_pSO ? m_pSO->GetSegment()->GetNumSegment() 
 //                                              : m_pColStaffObjs->GetNumSegments()-1 ); }
 //   inline void ResetFlags() { m_fChangeOfMeasure = false; }
 //   inline bool IsManagingCollection(lmColStaffObjs* pCSO) { 
 //                   return pCSO == m_pColStaffObjs; }


 //   void AdvanceToMeasure(int nBar);
    void first();
    inline void operator ++() { ++m_it; }
    inline void operator --() { --m_it; }
 //   void MoveLast();
 //   void MoveTo(lmStaffObj* pSO);

    inline int segment() const { return (*m_it)->segment(); }

    ColStaffObjsEntry* operator *() const { return *m_it; }


protected:

};


}   //namespace lenmus

#endif      //__LML_SCORE_ITERATOR_H__
