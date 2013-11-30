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

#include <UnitTest++.h>
#include <sstream>
#include "lomse_config.h"
#include "lomse_build_options.h"

//classes related to these tests
#include "lomse_injectors.h"
#include "lomse_image_reader.h"
#include "lomse_document.h"
//#include "lomse_ldp_compiler.h"
//#include "lomse_user_command.h"
//#include "lomse_view.h"
//#include "lomse_document_cursor.h"

using namespace UnitTest;
using namespace std;
using namespace lomse;


//---------------------------------------------------------------------------------------
class ImageReaderTestFixture
{
public:
    LibraryScope m_libraryScope;
    std::string m_scores_path;

    ImageReaderTestFixture()     //SetUp fixture
        : m_libraryScope(cout)
    {
        m_scores_path = TESTLIB_SCORES_PATH;
        m_libraryScope.set_default_fonts_path(TESTLIB_FONTS_PATH);
    }

    ~ImageReaderTestFixture()    //TearDown fixture
    {
    }
};


SUITE(ImageReaderTest)
{

//    TEST_FIXTURE(ImageReaderTestFixture, ImageReader_1)
//    {
//        ImageReader reader("");
//        CHECK( reader.is_valid() == true );
//    }

//    TEST_FIXTURE(ImageReaderTestFixture, ImageReader_1)
//    {
//        Document doc(m_libraryScope);
//        doc.from_file(m_scores_path + "90100-read-jpg-image.lms");
//    }

    TEST_FIXTURE(ImageReaderTestFixture, ImageReader_can_decode_png_1)
    {
        string path = m_scores_path + "test-image-1.png";
        InputStream* file = FileSystem::open_input_stream(path);
        PngImageDecoder dec;
        CHECK( dec.can_decode(file) == true );
        delete file;
    }

    TEST_FIXTURE(ImageReaderTestFixture, ImageReader_can_decode_png_2)
    {
        string path = m_scores_path + "test-image-2.jpg";
        InputStream* file = FileSystem::open_input_stream(path);
        PngImageDecoder dec;
        CHECK( dec.can_decode(file) == false );
        delete file;
    }

}
