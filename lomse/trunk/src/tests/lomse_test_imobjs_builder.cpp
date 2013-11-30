//---------------------------------------------------------------------------------------
// This file is part of the Lomse library.
// Copyright (c) 2010-2012 Cecilio Salmeron. All rights reserved.
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

#include <UnitTest++.h>
#include <sstream>
#include "lomse_config.h"

//classes related to these tests
#include "lomse_injectors.h"
#include "lomse_parser.h"
#include "lomse_analyser.h"
#include "lomse_internal_model.h"
#include "lomse_im_note.h"
#include "lomse_model_builder.h"

using namespace UnitTest;
using namespace std;
using namespace lomse;


class ImObjsBuilderTestFixture
{
public:

    ImObjsBuilderTestFixture()     //SetUp fixture
    {
        m_pLibraryScope = new LibraryScope(cout);
        m_pLdpFactory = m_pLibraryScope->ldp_factory();
    }

    ~ImObjsBuilderTestFixture()    //TearDown fixture
    {
        delete m_pLibraryScope;
    }

    LibraryScope* m_pLibraryScope;
    LdpFactory* m_pLdpFactory;
};


SUITE(ImObjsBuilderTest)
{

//    TEST_FIXTURE(ImObjsBuilderTestFixture, ImObjsBuilder_Empty)
//    {
//        LdpParser parser(cout, m_pLdpFactory);
//        SpLdpTree tree = parser.parse_text("(lenmusdoc (vers 2.0) (content))" );
//        Analyser a(cout, m_pLdpFactory);
//        BasicModel* pBasicModel = a.analyse_tree(tree);
//        ImObjectsBuilder imb(cout);
//        ImoDocument* pDoc = imb.create_objects(pBasicModel);
//
//        CHECK( pDoc != NULL );
//        CHECK( pDoc->get_version() == "2.0" );
//        CHECK( pDoc->get_num_content_items() == 0 );
//    }

}

