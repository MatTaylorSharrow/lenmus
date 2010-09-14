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

#ifdef _LM_DEBUG_

#include <UnitTest++.h>
#include <sstream>
#include "lenmus_config.h"

//classes related to these tests
#include "lenmus_injectors.h"
#include "lenmus_document.h"
#include "lenmus_parser.h"
#include "lenmus_compiler.h"
#include "lenmus_internal_model.h"
#include "lenmus_im_note.h"
#include "lenmus_document_iterator.h"

using namespace UnitTest;
using namespace std;
using namespace lenmus;


//helper, to get last id
class TestDocument : public Document
{
public:
    TestDocument(LibraryScope& libraryScope, ostream& reporter=cout)
        : Document(libraryScope, reporter) {}
    TestDocument(LdpCompiler* pCompiler, IdAssigner* pIdAssigner)
        : Document(pCompiler, pIdAssigner) {}

        long test_last_id() { return m_pIdAssigner->get_last_id(); }
};



class DocumentTestFixture
{
public:

    DocumentTestFixture()     //SetUp fixture
    {
        m_pLibraryScope = new LibraryScope(cout);
        m_pLdpFactory = m_pLibraryScope->ldp_factory();
        m_scores_path = LML_TEST_SCORES_PATH;
    }

    ~DocumentTestFixture()    //TearDown fixture
    {
        delete m_pLibraryScope;
    }

    LibraryScope* m_pLibraryScope;
    LdpFactory* m_pLdpFactory;
    std::string m_scores_path;
};

SUITE(DocumentTest)
{

    TEST_FIXTURE(DocumentTestFixture, DocumentEmpty)
    {
        Document doc(*m_pLibraryScope);
        doc.create_empty();
        ImoDocument* pModel = doc.get_model();
        CHECK( pModel != NULL );
        CHECK( doc.is_modified() == false );
        //cout << doc.to_string() << endl;
        CHECK( doc.to_string() == "(lenmusdoc (vers 0.0) (content))" );
    }

    TEST_FIXTURE(DocumentTestFixture, DocumentFromFile)
    {
        Document doc(*m_pLibraryScope);
        doc.from_file(m_scores_path + "00011-empty-fill-page.lms");
        ImoDocument* pModel = doc.get_model();
        CHECK( pModel != NULL );
        CHECK( doc.is_modified() == false );
//        cout << doc.to_string() << endl;
//        CHECK( doc.to_string() == "(lenmusdoc (vers 0.0) (content (score (vers 1.6) (systemLayout first (systemMargins 0 0 0 2000)) (systemLayout other (systemMargins 0 0 1200 2000)) (opt Score.FillPageWithEmptyStaves true) (opt StaffLines.StopAtFinalBarline false) (instrument (musicData)))))" );
    }

    TEST_FIXTURE(DocumentTestFixture, DocumentFromString)
    {
        Document doc(*m_pLibraryScope);
        doc.from_string("(lenmusdoc (vers 0.0) (content (score (vers 1.6) "
            "(instrument (musicData (n c4 q))))))");
        ImoDocument* pModel = doc.get_model();
        CHECK( pModel != NULL );
        CHECK( doc.is_modified() == false );
//        cout << doc.to_string() << endl;
//        CHECK( doc.to_string() == "(lenmusdoc (vers 0.0) (content (score (vers 1.6) "
//            "(instrument (musicData (n c4 q))))))" );
    }

    TEST_FIXTURE(DocumentTestFixture, DocumentGetScoreInEmptyDoc)
    {
        Document doc(*m_pLibraryScope);
        doc.create_empty();
        ImoScore* pScore = doc.get_score();
        CHECK( pScore == NULL );
    }

    TEST_FIXTURE(DocumentTestFixture, DocumentGetScoreFromFile)
    {
        Document doc(*m_pLibraryScope);
        doc.from_file(m_scores_path + "00011-empty-fill-page.lms");
        ImoScore* pScore = doc.get_score();
        CHECK( pScore != NULL );
//        CHECK( pScore.to_string() == "(score (vers 1.6) (systemLayout first (systemMargins 0 0 0 2000)) (systemLayout other (systemMargins 0 0 1200 2000)) (opt Score.FillPageWithEmptyStaves true) (opt StaffLines.StopAtFinalBarline false) (instrument (musicData )))" );
    }

    TEST_FIXTURE(DocumentTestFixture, Document_CreateWithEmptyScore)
    {
        Document doc(*m_pLibraryScope);
        doc.create_with_empty_score();
        ImoDocument* pModel = doc.get_model();
        CHECK( pModel != NULL );
        CHECK( doc.is_modified() == false );
        ImoScore* pScore = doc.get_score();
        CHECK( pScore != NULL );
    }

    // Document::iterator ---------------------------------------------------------------------------

//    TEST_FIXTURE(DocumentTestFixture, DocumentIteratorAdvance)
//    {
//        Document doc(*m_pLibraryScope);
//        doc.create_empty();
//        Document::iterator it = doc.begin();
//        ++it;
//        CHECK( doc.to_string(it) == "(vers 0.0)" );
//        ++it;
//        CHECK( doc.to_string(it) == "0.0" );
//        ++it;
//        CHECK( doc.to_string(it) == "(content )" );
//        ++it;
//        CHECK( it == doc.end() );
//    }
//
//    TEST_FIXTURE(DocumentTestFixture, DocumentIteratorGoBack)
//    {
//        Document doc(*m_pLibraryScope);
//        doc.create_empty();
//        Document::iterator it = doc.begin();
//        ++it;
//        ++it;
//        ++it;
//        CHECK( doc.to_string(it) == "(content )" );
//        --it;
//        CHECK( doc.to_string(it) == "(vers 0.0)" );
//        --it;
//        CHECK( doc.to_string(it) == "(lenmusdoc (vers 0.0) (content ))" );
//    }
//
//    TEST_FIXTURE(DocumentTestFixture, DocumentGetContent)
//    {
//        Document doc(*m_pLibraryScope);
//        doc.create_empty();
//        Document::iterator it = doc.content();
//        CHECK( doc.to_string(it) == "(content )" );
//    }
//
//      // commands ---------------------------------------------------------------------------------
//
//    TEST_FIXTURE(DocumentTestFixture, DocumentPushBack)
//    {
//        Document doc(*m_pLibraryScope);
//        doc.create_empty();
//        Document::iterator it = doc.content();
//        LdpParser parser(cout, m_pLdpFactory);
//        SpLdpTree tree = parser.parse_text("(text ''Title of this book'')");
//        LdpElement* elm = tree->get_root();
//        doc.add_param(it, elm);
//        //cout << doc.to_string(it) << endl;
//        CHECK( doc.to_string() == "(lenmusdoc (vers 0.0) (content (text \"Title of this book\")))" );
//    }
//
//    TEST_FIXTURE(DocumentTestFixture, DocumentRemoveParam)
//    {
//        Document doc(*m_pLibraryScope);
//        doc.create_empty();
//        Document::iterator it = doc.begin();
//        ++it;   //vers
//        LdpElement* elm = doc.remove(it);
//        //cout << doc.to_string() << endl;
//        CHECK( doc.to_string() == "(lenmusdoc (content ))" );
//        CHECK( doc.is_modified() == false );
//        delete elm;
//    }
//
//    TEST_FIXTURE(DocumentTestFixture, DocumentInsertParam)
//    {
//        Document doc(*m_pLibraryScope);
//        doc.create_empty();
//        LdpParser parser(cout, m_pLdpFactory);
//        SpLdpTree tree = parser.parse_text("(dx 20)");
//        LdpElement* elm = tree->get_root();
//        Document::iterator it = doc.begin();
//        ++it;   //vers
//        doc.insert(it, elm);
//        //cout << doc.to_string() << endl;
//        CHECK( doc.is_modified() == false );
//        CHECK( doc.to_string() == "(lenmusdoc (dx 20) (vers 0.0) (content ))" );
//    }
//
//    TEST_FIXTURE(DocumentTestFixture, DocumentHasImObjs)
//    {
//        Document doc(*m_pLibraryScope);
//        doc.from_string("(lenmusdoc (vers 0.0) (content (score (vers 1.6) (instrument (musicData (n c4 q)(n b3 e.)(n c4 s)))) ))" );
//        Document::iterator it = doc.get_score();
//        //cout << doc.to_string() << endl;
//        CHECK( doc.to_string() == "(lenmusdoc (vers 0.0) (content (score (vers 1.6) (instrument (musicData (n c4 q) (n b3 e.) (n c4 s))))))" );
//        ImScore* pScore = dynamic_cast<ImScore*>( (*it)->get_imobj() );
//        CHECK( pScore != NULL );
//        ++it;
//        //cout << (*it)->to_string() << endl;
//        CHECK( (*it)->to_string() == "(vers 1.6)" );
//        ++it;
//        ++it;
//        CHECK( (*it)->to_string() == "(instrument (musicData (n c4 q) (n b3 e.) (n c4 s)))" );
//        ImInstrument* pInstr = dynamic_cast<ImInstrument*>( (*it)->get_imobj() );
//        CHECK( pInstr != NULL );
//        ++it;
//        CHECK( (*it)->to_string() == "(musicData (n c4 q) (n b3 e.) (n c4 s))" );
//        ++it;
//        CHECK( (*it)->to_string() == "(n c4 q)" );
//        ImNote* pNote = dynamic_cast<ImNote*>( (*it)->get_imobj() );
//        CHECK( pNote != NULL );
//        ++it;
//        ++it;
//        ++it;
//        CHECK( (*it)->to_string() == "(n b3 e.)" );
//        pNote = dynamic_cast<ImNote*>( (*it)->get_imobj() );
//        CHECK( pNote != NULL );
//        ++it;
//        ++it;
//        ++it;
//        CHECK( (*it)->to_string() == "(n c4 s)" );
//        pNote = dynamic_cast<ImNote*>( (*it)->get_imobj() );
//        CHECK( pNote != NULL );
//    }
//
//    // undo/redo --------------------------------------------------------
//
//    TEST_FIXTURE(DocumentTestFixture, DocumentPushBackCommandIsStored)
//    {
//        Document doc(*m_pLibraryScope);
//        doc.create_empty();
//        Document::iterator it = doc.begin();
//        LdpParser parser(cout, m_pLdpFactory);
//        SpLdpTree tree = parser.parse_text("(text ''Title of this book'')");
//        LdpElement* elm = tree->get_root();
//        DocCommandExecuter ce(&doc);
//        ce.execute( new DocCommandPushBack(it, elm) );
//        CHECK( ce.undo_stack_size() == 1 );
//        CHECK( doc.to_string() == "(lenmusdoc (vers 0.0) (content ) (text \"Title of this book\"))" );
//        CHECK( doc.is_modified() == true );
//    }
//
//    TEST_FIXTURE(DocumentTestFixture, DocumentUndoPushBackCommand)
//    {
//        Document doc(*m_pLibraryScope);
//        doc.create_empty();
//        Document::iterator it = doc.begin();
//        LdpParser parser(cout, m_pLdpFactory);
//        SpLdpTree tree = parser.parse_text("(text ''Title of this book'')");
//        LdpElement* elm = tree->get_root();
//        DocCommandExecuter ce(&doc);
//        ce.execute( new DocCommandPushBack(it, elm) );
//        ce.undo();
//        CHECK( ce.undo_stack_size() == 0 );
//        CHECK( doc.to_string() == "(lenmusdoc (vers 0.0) (content ))" );
//        CHECK( doc.is_modified() == false );
//    }
//
//    TEST_FIXTURE(DocumentTestFixture, DocumentUndoRedoPushBackCommand)
//    {
//        Document doc(*m_pLibraryScope);
//        doc.create_empty();
//        Document::iterator it = doc.begin();
//        LdpParser parser(cout, m_pLdpFactory);
//        SpLdpTree tree = parser.parse_text("(text ''Title of this book'')");
//        LdpElement* elm = tree->get_root();
//        DocCommandExecuter ce(&doc);
//        ce.execute( new DocCommandPushBack(it, elm) );
//        ce.undo();
//        ce.redo();
//        CHECK( ce.undo_stack_size() == 1 );
//        CHECK( doc.to_string() == "(lenmusdoc (vers 0.0) (content ) (text \"Title of this book\"))" );
//        CHECK( doc.is_modified() == true );
//    }
//
//    TEST_FIXTURE(DocumentTestFixture, DocumentUndoRedoUndoPushBackCommand)
//    {
//        Document doc(*m_pLibraryScope);
//        doc.create_empty();
//        Document::iterator it = doc.begin();
//        LdpParser parser(cout, m_pLdpFactory);
//        SpLdpTree tree = parser.parse_text("(text ''Title of this book'')");
//        LdpElement* elm = tree->get_root();
//        DocCommandExecuter ce(&doc);
//        ce.execute( new DocCommandPushBack(it, elm) );
//        ce.undo();
//        ce.redo();
//        ce.undo();
//        //cout << doc.to_string() << endl;
//        CHECK( ce.undo_stack_size() == 0 );
//        CHECK( doc.to_string() == "(lenmusdoc (vers 0.0) (content ))" );
//        CHECK( doc.is_modified() == false );
//    }
//
//    TEST_FIXTURE(DocumentTestFixture, DocumentRemoveCommandIsStored)
//    {
//        Document doc(*m_pLibraryScope);
//        doc.create_empty();
//        Document::iterator it = doc.begin();
//        ++it;   //vers
//        DocCommandExecuter ce(&doc);
//        ce.execute( new DocCommandRemove(it) );
//        CHECK( ce.undo_stack_size() == 1 );
//        CHECK( doc.to_string() == "(lenmusdoc (content ))" );
//        CHECK( doc.is_modified() == true );
//    }
//
//    TEST_FIXTURE(DocumentTestFixture, DocumentUndoRemoveCommand)
//    {
//        Document doc(*m_pLibraryScope);
//        doc.create_empty();
//        Document::iterator it = doc.begin();
//        ++it;   //vers
//        DocCommandExecuter ce(&doc);
//        ce.execute( new DocCommandRemove(it) );
//        ce.undo();
//        CHECK( ce.undo_stack_size() == 0 );
//        //cout << doc.to_string() << endl;
//        CHECK( doc.to_string() == "(lenmusdoc (vers 0.0) (content ))" );
//        CHECK( doc.is_modified() == false );
//    }
//
//    TEST_FIXTURE(DocumentTestFixture, DocumentRedoRemoveCommand)
//    {
//        Document doc(*m_pLibraryScope);
//        doc.create_empty();
//        Document::iterator it = doc.begin();
//        ++it;   //vers
//        DocCommandExecuter ce(&doc);
//        ce.execute( new DocCommandRemove(it) );
//        ce.undo();
//        ce.redo();
//        //cout << doc.to_string() << endl;
//        CHECK( doc.to_string() == "(lenmusdoc (content ))" );
//        CHECK( doc.is_modified() == true );
//    }
//
//    TEST_FIXTURE(DocumentTestFixture, DocumentUndoRedoUndoRemoveCommand)
//    {
//        Document doc(*m_pLibraryScope);
//        doc.create_empty();
//        Document::iterator it = doc.begin();
//        ++it;   //vers
//        DocCommandExecuter ce(&doc);
//        ce.execute( new DocCommandRemove(it) );
//        ce.undo();
//        ce.redo();
//        ce.undo();
//        //cout << doc.to_string() << endl;
//        CHECK( doc.to_string() == "(lenmusdoc (vers 0.0) (content ))" );
//        CHECK( doc.is_modified() == false );
//    }
//
//    TEST_FIXTURE(DocumentTestFixture, DocumentInsertCommandIsStored)
//    {
//        TestDocument doc(*m_pLibraryScope);
//        doc.create_empty();
//        LdpParser parser(cout, m_pLdpFactory);
//        SpLdpTree tree = parser.parse_text("(dx 20)");
//        LdpElement* elm = tree->get_root();
//        Document::iterator it = doc.begin();
//        ++it;   //vers
//        DocCommandExecuter ce(&doc);
//        ce.execute( new DocCommandInsert(it, elm) );
//        CHECK( ce.undo_stack_size() == 1 );
//        //cout << doc.to_string() << endl;
//        CHECK( doc.to_string() == "(lenmusdoc (dx 20) (vers 0.0) (content ))" );
//        CHECK( doc.is_modified() == true );
//    }
//
//    TEST_FIXTURE(DocumentTestFixture, DocumentUndoInsertCommandIsStored)
//    {
//        Document doc(*m_pLibraryScope);
//        doc.create_empty();
//        LdpParser parser(cout, m_pLdpFactory);
//        SpLdpTree tree = parser.parse_text("(dx 20)");
//        LdpElement* elm = tree->get_root();
//        Document::iterator it = doc.begin();
//        ++it;   //vers
//        DocCommandExecuter ce(&doc);
//        ce.execute( new DocCommandInsert(it, elm) );
//        ce.undo();
//        CHECK( ce.undo_stack_size() == 0 );
//        //cout << doc.to_string() << endl;
//        CHECK( doc.to_string() == "(lenmusdoc (vers 0.0) (content ))" );
//        CHECK( doc.is_modified() == false );
//    }
//
//    TEST_FIXTURE(DocumentTestFixture, DocumentUndoRedoInsertCommandIsStored)
//    {
//        Document doc(*m_pLibraryScope);
//        doc.create_empty();
//        LdpParser parser(cout, m_pLdpFactory);
//        SpLdpTree tree = parser.parse_text("(dx 20)");
//        LdpElement* elm = tree->get_root();
//        Document::iterator it = doc.begin();
//        ++it;   //vers
//        DocCommandExecuter ce(&doc);
//        ce.execute( new DocCommandInsert(it, elm) );
//        ce.undo();
//        ce.redo();
//        CHECK( ce.undo_stack_size() == 1 );
//        //cout << doc.to_string() << endl;
//        CHECK( doc.to_string() == "(lenmusdoc (dx 20) (vers 0.0) (content ))" );
//        CHECK( doc.is_modified() == true );
//    }
//
//    TEST_FIXTURE(DocumentTestFixture, DocumentUndoRedoUndoInsertCommandIsStored)
//    {
//        Document doc(*m_pLibraryScope);
//        doc.create_empty();
//        LdpParser parser(cout, m_pLdpFactory);
//        SpLdpTree tree = parser.parse_text("(dx 20)");
//        LdpElement* elm = tree->get_root();
//        Document::iterator it = doc.begin();
//        ++it;   //vers
//        DocCommandExecuter ce(&doc);
//        ce.execute( new DocCommandInsert(it, elm) );
//        ce.undo();
//        ce.redo();
//        ce.undo();
//        CHECK( ce.undo_stack_size() == 0 );
//        //cout << doc.to_string() << endl;
//        CHECK( doc.to_string() == "(lenmusdoc (vers 0.0) (content ))" );
//        CHECK( doc.is_modified() == false );
//    }
//
//    TEST_FIXTURE(DocumentTestFixture, DocumentRemoveNotLast)
//    {
//        Document doc(*m_pLibraryScope);
//        doc.create_empty();
//        LdpParser parser(cout, m_pLdpFactory);
//        SpLdpTree tree = parser.parse_text("(musicData (n c4 q)(r e)(n b3 e)(dx 20))");
//        LdpElement* elm = tree->get_root();
//        Document::iterator it = doc.begin();
//        ++it;   //vers
//        Document::iterator itNew = doc.insert(it, elm);
//        CHECK( doc.to_string() == "(lenmusdoc (musicData (n c4 q) (r e) (n b3 e) (dx 20)) (vers 0.0) (content ))" );
//        CHECK( doc.to_string( itNew ) == "(musicData (n c4 q) (r e) (n b3 e) (dx 20))" );
//        ++itNew;    //n c4
//        ++itNew;    //c4
//        ++itNew;    //q
//        ++itNew;    //r
//        CHECK( doc.to_string( itNew ) == "(r e)" );
//
//        DocCommandExecuter ce(&doc);
//        ce.execute( new DocCommandRemove(itNew) );
//        CHECK( ce.undo_stack_size() == 1 );
//        //cout << doc.to_string() << endl;
//        CHECK( doc.to_string() == "(lenmusdoc (musicData (n c4 q) (n b3 e) (dx 20)) (vers 0.0) (content ))" );
//        CHECK( doc.is_modified() == true );
//    }
//
//    TEST_FIXTURE(DocumentTestFixture, DocumentUndoRemoveNotLast)
//    {
//        Document doc(*m_pLibraryScope);
//        doc.create_empty();
//        LdpParser parser(cout, m_pLdpFactory);
//        SpLdpTree tree = parser.parse_text("(dx 20)");
//        LdpElement* elm = tree->get_root();
//        Document::iterator it = doc.begin();
//        ++it;   //vers
//        Document::iterator itNew = doc.insert(it, elm);
//        DocCommandExecuter ce(&doc);
//        ce.execute( new DocCommandRemove(itNew) );
//        ce.undo();
//        CHECK( ce.undo_stack_size() == 0 );
//        //cout << doc.to_string() << endl;
//        CHECK( doc.to_string() == "(lenmusdoc (dx 20) (vers 0.0) (content ))" );
//    }
//
//    TEST_FIXTURE(DocumentTestFixture, DocumentUndoRedoRemoveNotLast)
//    {
//        Document doc(*m_pLibraryScope);
//        doc.create_empty();
//        LdpParser parser(cout, m_pLdpFactory);
//        SpLdpTree tree = parser.parse_text("(dx 20)");
//        LdpElement* elm = tree->get_root();
//        Document::iterator it = doc.begin();
//        ++it;   //vers
//        Document::iterator itNew = doc.insert(it, elm);
//        DocCommandExecuter ce(&doc);
//        ce.execute( new DocCommandRemove(itNew) );
//        ce.undo();
//        ce.redo();
//        CHECK( ce.undo_stack_size() == 1 );
//        //cout << doc.to_string() << endl;
//        CHECK( doc.to_string() == "(lenmusdoc (vers 0.0) (content ))" );
//        CHECK( doc.is_modified() == true );
//    }
//
//    TEST_FIXTURE(DocumentTestFixture, DocumentUndoRedoUndoRemoveNotLast)
//    {
//        Document doc(*m_pLibraryScope);
//        doc.create_empty();
//        LdpParser parser(cout, m_pLdpFactory);
//        SpLdpTree tree = parser.parse_text("(dx 20)");
//        LdpElement* elm = tree->get_root();
//        Document::iterator it = doc.begin();
//        ++it;   //vers
//        Document::iterator itNew = doc.insert(it, elm);
//        DocCommandExecuter ce(&doc);
//        ce.execute( new DocCommandRemove(itNew) );
//        ce.undo();
//        ce.redo();
//        ce.undo();
//        CHECK( ce.undo_stack_size() == 0 );
//        //cout << doc.to_string() << endl;
//        CHECK( doc.to_string() == "(lenmusdoc (dx 20) (vers 0.0) (content ))" );
//    }
//
//    TEST_FIXTURE(DocumentTestFixture, DocumentRemoveFirst)
//    {
//        Document doc(*m_pLibraryScope);
//        doc.create_empty();
//        LdpParser parser(cout, m_pLdpFactory);
//        SpLdpTree tree = parser.parse_text("(musicData (n c4 q)(r e)(n b3 e)(dx 20))");
//        LdpElement* elm = tree->get_root();
//        Document::iterator it = doc.begin();
//        ++it;   //vers
//        Document::iterator itNew = doc.insert(it, elm);
//        CHECK( doc.to_string() == "(lenmusdoc (musicData (n c4 q) (r e) (n b3 e) (dx 20)) (vers 0.0) (content ))" );
//        CHECK( doc.to_string( itNew ) == "(musicData (n c4 q) (r e) (n b3 e) (dx 20))" );
//        ++itNew;    //n c4
//        CHECK( doc.to_string( itNew ) == "(n c4 q)" );
//
//        DocCommandExecuter ce(&doc);
//        ce.execute( new DocCommandRemove(itNew) );
//        CHECK( ce.undo_stack_size() == 1 );
//        //cout << doc.to_string() << endl;
//        CHECK( doc.to_string() == "(lenmusdoc (musicData (r e) (n b3 e) (dx 20)) (vers 0.0) (content ))" );
//    }
//
//    TEST_FIXTURE(DocumentTestFixture, DocumentNodesMarkedAsModified)
//    {
//        Document doc(*m_pLibraryScope);
//        doc.from_string("(lenmusdoc (vers 0.0) (content (score (vers 1.6) (instrument (musicData (clef G)(key e)(n c4 q)(r q)(barline simple))))))");
//        Document::iterator it = doc.begin();
//        ++it;   //vers
//        ++it;   //0.0
//        ++it;   //content
//        ++it;   //score
//        ++it;   //vers
//        ++it;   //1.6
//        ++it;   //instrument
//        ++it;   //musicData
//        ++it;   //clef
//        ++it;   //G
//        ++it;   //key
//        DocCommandExecuter ce(&doc);
//        ce.execute( new DocCommandRemove(it) );
//        CHECK( doc.to_string() == "(lenmusdoc (vers 0.0) (content (score (vers 1.6) (instrument (musicData (clef G) (n c4 q) (r q) (barline simple))))))" );
//        it = doc.begin();   //lenmusdoc
//        CHECK( (*it)->is_modified() );
//        ++it;   //vers
//        CHECK( !(*it)->is_modified() );
//        ++it;   //0.0
//        CHECK( !(*it)->is_modified() );
//        ++it;   //content
//        CHECK( (*it)->is_modified() );
//        ++it;   //score
//        CHECK( (*it)->is_modified() );
//        ++it;   //vers
//        CHECK( !(*it)->is_modified() );
//        ++it;   //1.6
//        CHECK( !(*it)->is_modified() );
//        ++it;   //instrument
//        CHECK( (*it)->is_modified() );
//        ++it;   //musicData
//        CHECK( (*it)->is_modified() );
//        ++it;   //clef
//        CHECK( !(*it)->is_modified() );
//        ++it;   //G
//        CHECK( !(*it)->is_modified() );
//        ++it;   //n
//        CHECK( !(*it)->is_modified() );
//    }
//
//    TEST_FIXTURE(DocumentTestFixture, DocumentUndoNodesMarkedAsModified)
//    {
//        Document doc(*m_pLibraryScope);
//        doc.from_string("(lenmusdoc (vers 0.0) (content (score (vers 1.6) (instrument (musicData (clef G)(key e)(n c4 q)(r q)(barline simple))))))");
//        Document::iterator it = doc.begin();
//        ++it;   //vers
//        ++it;   //0.0
//        ++it;   //content
//        ++it;   //score
//        ++it;   //vers
//        ++it;   //1.6
//        ++it;   //instrument
//        ++it;   //musicData
//        ++it;   //clef
//        ++it;   //G
//        ++it;   //key
//        DocCommandExecuter ce(&doc);
//        ce.execute( new DocCommandRemove(it) );
//        ce.undo();
//        it = doc.begin();   //lenmusdoc
//        CHECK( !(*it)->is_modified() );
//        ++it;   //vers
//        CHECK( !(*it)->is_modified() );
//        ++it;   //0.0
//        CHECK( !(*it)->is_modified() );
//        ++it;   //content
//        CHECK( !(*it)->is_modified() );
//        ++it;   //score
//        CHECK( !(*it)->is_modified() );
//        ++it;   //vers
//        CHECK( !(*it)->is_modified() );
//        ++it;   //1.6
//        CHECK( !(*it)->is_modified() );
//        ++it;   //instrument
//        CHECK( !(*it)->is_modified() );
//        ++it;   //musicData
//        CHECK( !(*it)->is_modified() );
//        ++it;   //clef
//        CHECK( !(*it)->is_modified() );
//        ++it;   //G
//        CHECK( !(*it)->is_modified() );
//        ++it;   //key
//        CHECK( !(*it)->is_modified() );
//    }
//
//    TEST_FIXTURE(DocumentTestFixture, DocumentKnowsLastId_FromString)
//    {
//        TestDocument doc(*m_pLibraryScope);
//        doc.from_string("(lenmusdoc#0 (vers#1 0.0) (content#2 (score#3 (vers#4 1.6) (instrument#5 (musicData#6 (n#7 c4 q) (r#8 q)))) (text#9 \"this is text\")))" );
//        //cout << "Last Id = " << doc.test_last_id() << endl;
//        CHECK( doc.test_last_id() == 9L );
//    }
//
//    TEST_FIXTURE(DocumentTestFixture, DocumentKnowsLastId_Empty)
//    {
//        TestDocument doc(*m_pLibraryScope);
//        doc.create_empty();
//        //cout << "Last Id = " << doc.test_last_id() << endl;
//        CHECK( doc.test_last_id() == 2L );
//    }
//
//    TEST_FIXTURE(DocumentTestFixture, DocumentKnowsLastId_FromFile)
//    {
//        TestDocument doc(*m_pLibraryScope);
//        doc.from_file(m_scores_path + "90013-two-instruments-four-staves.lms");
//        //cout << "Last Id = " << doc.test_last_id() << endl;
//        CHECK( doc.test_last_id() == 48L );
//    }
//
//    TEST_FIXTURE(DocumentTestFixture, DocumentInsertCommandUpdatesId)
//    {
//        TestDocument doc(*m_pLibraryScope);
//        doc.create_empty();
//        LdpParser parser(cout, m_pLdpFactory);
//        SpLdpTree tree = parser.parse_text("(dx 20)");
//        LdpElement* elm = tree->get_root();
//        Document::iterator it = doc.begin();
//        ++it;   //vers
//        DocCommandExecuter ce(&doc);
//        ce.execute( new DocCommandInsert(it, elm) );
//        //cout << "Last Id = " << doc.test_last_id() << endl;
//        CHECK( doc.test_last_id() == 3L );
//    }
//
//    TEST_FIXTURE(DocumentTestFixture, DocumentPushBackCommandUpdatesId)
//    {
//        TestDocument doc(*m_pLibraryScope);
//        doc.create_empty();
//        Document::iterator it = doc.begin();
//        LdpParser parser(cout, m_pLdpFactory);
//        SpLdpTree tree = parser.parse_text("(text ''Title of this book'')");
//        LdpElement* elm = tree->get_root();
//        DocCommandExecuter ce(&doc);
//        ce.execute( new DocCommandPushBack(it, elm) );
//        //cout << "Last Id = " << doc.test_last_id() << endl;
//        CHECK( doc.test_last_id() == 3L );
//    }

};

#endif  // _LM_DEBUG_

