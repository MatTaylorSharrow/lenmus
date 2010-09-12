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

#ifndef __LML_COMPILER_H__
#define __LML_COMPILER_H__


#include "lenmus_ldp_elements.h"

using namespace std;

namespace lenmus
{

//forward declarations
class LdpParser;
class Analyser;
class ModelBuilder;
class DocumentScope;
class LibraryScope;
class IdAssigner;
class BasicModel;
class ImoDocument;


//------------------------------------------------------------------------------------
// LdpCompiler: builds the tree for a document
//------------------------------------------------------------------------------------

class LdpCompiler
{
protected:
    LdpParser*      m_pParser;
    Analyser*       m_pAnalyser;
    ModelBuilder*   m_pModelBuilder;
    IdAssigner*     m_pIdAssigner;
    LdpTree*        m_pFinalTree;
    ImoDocument*    m_pImDocument;

public:
    LdpCompiler(LibraryScope& libraryScope, DocumentScope& documentScope);
    LdpCompiler(LdpParser* p, Analyser* a, ModelBuilder* mb, IdAssigner* ida);   //for testing: direct inyection of dependencies
    ~LdpCompiler();

    ImoDocument* compile_file(const std::string& filename);
    ImoDocument* compile_string(const std::string& source);
    ImoDocument* create_empty();
    ImoDocument* create_with_empty_score();

    int get_num_errors();
//    LdpElement* create_element(const std::string& source);
//    BasicModel* create_basic_model(const std::string& source);
//    inline BasicModel* get_outcome() { return m_pBasicModel; }
//
//    //access to Ldp tree result
//    LdpTree* get_final_tree() { return m_pFinalTree; }


protected:
    ImoDocument* compile(LdpTree* pParseTree);
    LdpTree* wrap_score_in_lenmusdoc(LdpTree* pParseTree);
    LdpTree* parse_empty_doc();

};


}   //namespace lenmus

#endif      //__LML_COMPILER_H__
