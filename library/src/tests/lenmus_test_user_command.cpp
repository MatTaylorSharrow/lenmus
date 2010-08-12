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

//classes related to these tests
#include "lenmus_injectors.h"
#include "lenmus_document.h"
#include "lenmus_document_cursor.h"
#include "lenmus_user_command.h"
#include "lenmus_model_builder.h"
#include "lenmus_compiler.h"

using namespace UnitTest;
using namespace std;
using namespace lenmus;


class TestUserCommand : public UserCommand
{
public:
    TestUserCommand(DocCursor& cursor1, DocCursor& cursor2) 
        : UserCommand("test command")
        , m_it1(*cursor1)
        , m_it2(*cursor2) 
    {
    }
    ~TestUserCommand() {};

protected:
    bool do_actions(DocCommandExecuter* dce)
    {
        dce->execute( new DocCommandRemove(m_it1) );
        dce->execute( new DocCommandRemove(m_it2) );
        return true;
    }

    Document::iterator m_it1;
    Document::iterator m_it2;

};

class MockModelBuilder : public ModelBuilder
{
public:
    MockModelBuilder(ostream& reporter) 
        : ModelBuilder(reporter)
        , m_nBuildInvoked(0)
        , m_nUpdateInvoked(0)
    {
    }
    void build_model(LdpTree* tree) { ++m_nBuildInvoked; }
    void update_model(LdpTree* tree) { ++m_nUpdateInvoked; }

    int m_nBuildInvoked;
    int m_nUpdateInvoked;

};


class UserCommandTestFixture
{
public:

    UserCommandTestFixture()     //SetUp fixture
    {
    }

    ~UserCommandTestFixture()    //TearDown fixture
    {
    }

    void point_cursors_1(Document* pDoc, DocCursor* pCursor1, DocCursor* pCursor2)
    {
        DocCursor cursor(pDoc);     //score
        cursor.enter_element();     //(clef G)
        ++cursor;       //(key e)
        *pCursor1 = cursor;
        ++cursor;       //(n c4 q)
        ++cursor;       //(r q)
        *pCursor2 = cursor;
    }

    void point_cursors_2(Document* pDoc, DocCursor* pCursor1, DocCursor* pCursor2)
    {
        DocCursor cursor(pDoc);     //score
        *pCursor1 = cursor;
        ++cursor;       //(text "this is a text")
        ++cursor;       //(text "to be removed")
        *pCursor2 = cursor;
    }

};

SUITE(UserCommandTest)
{

    TEST_FIXTURE(UserCommandTestFixture, UserCommandExecuteTestCommand)
    {
        LibraryScope libraryScope(cout);
        Document doc(libraryScope);
        doc.from_string("(lenmusdoc (vers 0.0) (content (score (vers 1.6) (instrument (musicData (clef G)(key e)(n c4 q)(r q)(barline simple))))))" );
        DocCursor cursor1(&doc);
        DocCursor cursor2(&doc);
        point_cursors_1(&doc, &cursor1, &cursor2);
        UserCommandExecuter executer(&doc);
        TestUserCommand cmd(cursor1, cursor2);
        executer.execute(cmd);
        //cout << doc.to_string() << endl;
        CHECK( executer.undo_stack_size() == 1 );
        CHECK( doc.to_string() == "(lenmusdoc (vers 0.0) (content (score (vers 1.6) (instrument (musicData (clef G) (n c4 q) (barline simple))))))" );
        CHECK( doc.is_modified() == true );
    }

    TEST_FIXTURE(UserCommandTestFixture, UserCommandUndoTestCommand)
    {
        LibraryScope libraryScope(cout);
        Document doc(libraryScope);
        doc.from_string("(lenmusdoc (vers 0.0) (content (score (vers 1.6) (instrument (musicData (clef G)(key e)(n c4 q)(r q)(barline simple))))))" );
        DocCursor cursor1(&doc);
        DocCursor cursor2(&doc);
        point_cursors_1(&doc, &cursor1, &cursor2);
        UserCommandExecuter executer(&doc);
        TestUserCommand cmd(cursor1, cursor2);
        executer.execute(cmd);
        executer.undo();
        //cout << doc.to_string() << endl;
        CHECK( executer.undo_stack_size() == 0 );
        CHECK( doc.to_string() == "(lenmusdoc (vers 0.0) (content (score (vers 1.6) (instrument (musicData (clef G) (key e) (n c4 q) (r q) (barline simple))))))" );
        CHECK( doc.is_modified() == false );
    }

    TEST_FIXTURE(UserCommandTestFixture, UserCommandUndoRedoTestCommand)
    {
        LibraryScope libraryScope(cout);
        Document doc(libraryScope);
        doc.from_string("(lenmusdoc (vers 0.0) (content (score (vers 1.6) (instrument (musicData (clef G)(key e)(n c4 q)(r q)(barline simple))))))" );
        DocCursor cursor1(&doc);
        DocCursor cursor2(&doc);
        point_cursors_1(&doc, &cursor1, &cursor2);
        UserCommandExecuter executer(&doc);
        TestUserCommand cmd(cursor1, cursor2);
        executer.execute(cmd);
        executer.undo();
        executer.redo();
        //cout << doc.to_string() << endl;
        CHECK( executer.undo_stack_size() == 1 );
        CHECK( doc.to_string() == "(lenmusdoc (vers 0.0) (content (score (vers 1.6) (instrument (musicData (clef G) (n c4 q) (barline simple))))))" );
        CHECK( doc.is_modified() == true );
    }

    TEST_FIXTURE(UserCommandTestFixture, UserCommandUndoRedoUndoTestCommand)
    {
        LibraryScope libraryScope(cout);
        Document doc(libraryScope);
        doc.from_string("(lenmusdoc (vers 0.0) (content (score (vers 1.6) (instrument (musicData (clef G)(key e)(n c4 q)(r q)(barline simple))))))" );
        DocCursor cursor1(&doc);
        DocCursor cursor2(&doc);
        point_cursors_1(&doc, &cursor1, &cursor2);
        UserCommandExecuter executer(&doc);
        TestUserCommand cmd(cursor1, cursor2);
        executer.execute(cmd);
        executer.undo();
        executer.redo();
        executer.undo();
        //cout << doc.to_string() << endl;
        CHECK( executer.undo_stack_size() == 0 );
        CHECK( doc.to_string() == "(lenmusdoc (vers 0.0) (content (score (vers 1.6) (instrument (musicData (clef G) (key e) (n c4 q) (r q) (barline simple))))))" );
        CHECK( doc.is_modified() == false );
    }

    TEST_FIXTURE(UserCommandTestFixture, UserCommandRemoveTopLevel)
    {
        LibraryScope libraryScope(cout);
        Document doc(libraryScope);
        doc.from_string("(lenmusdoc (vers 0.0) (content (score (vers 1.6) (instrument (musicData (clef G)(key e)(n c4 q)(r q)(barline simple)))) (text \"this is text\") (text \"to be removed\") ))" );
        DocCursor cursor1(&doc);
        DocCursor cursor2(&doc);
        point_cursors_2(&doc, &cursor1, &cursor2);
        UserCommandExecuter executer(&doc);
        TestUserCommand cmd(cursor1, cursor2);
        executer.execute(cmd);
        //cout << doc.to_string() << endl;
        CHECK( executer.undo_stack_size() == 1 );
        CHECK( doc.to_string() == "(lenmusdoc (vers 0.0) (content (text \"this is text\")))" );
        CHECK( doc.is_modified() == true );
    }

    TEST_FIXTURE(UserCommandTestFixture, UserCommandUndoTopLevel)
    {
        LibraryScope libraryScope(cout);
        Document doc(libraryScope);
        doc.from_string("(lenmusdoc (vers 0.0) (content (score (vers 1.6) (instrument (musicData (clef G)(key e)(n c4 q)(r q)(barline simple)))) (text \"this is text\") (text \"to be removed\") ))" );
        DocCursor cursor1(&doc);
        DocCursor cursor2(&doc);
        point_cursors_2(&doc, &cursor1, &cursor2);
        UserCommandExecuter executer(&doc);
        TestUserCommand cmd(cursor1, cursor2);
        executer.execute(cmd);
        executer.undo();
        //cout << doc.to_string() << endl;
        CHECK( doc.to_string() == "(lenmusdoc (vers 0.0) (content (score (vers 1.6) (instrument (musicData (clef G) (key e) (n c4 q) (r q) (barline simple)))) (text \"this is text\") (text \"to be removed\")))" );
        CHECK( doc.is_modified() == false );
    }

    TEST_FIXTURE(UserCommandTestFixture, UserCommandModelBuilderInvoked)
    {
        LibraryScope libraryScope(cout);
        Document doc(libraryScope);
        doc.from_string("(lenmusdoc (vers 0.0) (content (score (vers 1.6) (instrument (musicData (clef G)(key e)(n c4 q)(r q)(barline simple))))))" );
        DocCursor cursor1(&doc);
        DocCursor cursor2(&doc);
        point_cursors_1(&doc, &cursor1, &cursor2);
        MockModelBuilder* pBuilder = new MockModelBuilder(cout);
        UserCommandExecuter executer(&doc, pBuilder);
        TestUserCommand cmd(cursor1, cursor2);
        executer.execute(cmd);
        //cout << doc.to_string() << endl;
        CHECK( executer.undo_stack_size() == 1 );
        CHECK( doc.to_string() == "(lenmusdoc (vers 0.0) (content (score (vers 1.6) (instrument (musicData (clef G) (n c4 q) (barline simple))))))" );
        CHECK( doc.is_modified() == true );
        CHECK( pBuilder->m_nBuildInvoked == 0 );
        //cout << "Update invoked " << pBuilder->m_nUpdateInvoked << " times" << endl;
        CHECK( pBuilder->m_nUpdateInvoked == 1 );
    } 

}

#endif  // _LM_DEBUG_
