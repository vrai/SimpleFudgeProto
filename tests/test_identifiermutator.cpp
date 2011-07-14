/**
 * Copyright (C) 2011 - 2011, Vrai Stacey.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "identifiermutator.hpp"
#include "simpletest.hpp"
#include <memory>

using namespace fudgeproto;

namespace
{
    bool operator== ( const std::auto_ptr<identifier> & x, const std::auto_ptr<identifier> & y )
    {
        if ( ! x.get ( ) || ! y.get ( ) )
            return x.get ( ) == y.get ( );
        else
            return x->equals ( *( y.get ( ) ) );
    }

    std::ostream & operator<< ( std::ostream & stream, const std::auto_ptr<identifier> & id )
    {
        return stream << ( id.get ( ) ? id->asString ( "." ) : "NULL" );
    }
}

DEFINE_TEST( IdentifierSearch )
    identifiermutator mutator;

    // Populate the mutuator
    std::auto_ptr<identifier> id1 ( identifier::createFromString ( "first.second.third", "." ) ),
                              id2 ( identifier::createFromString ( "first..second.third", "." ) ),
                              id3 ( identifier::createFromString ( "first->second->third->fourth->", "->" ) ),
                              id4 ( identifier::createFromString ( "first.split.third", "." ) ),
                              id5 ( identifier::createFromString ( "", "->" ) ),
                              id6 ( identifier::createFromString ( "second", "." ) ),
                              replace1 ( identifier::createFromString ( "replace1.replace2", "." ) ),
                              replace2 ( identifier::createFromString ( "replace3", "." ) ),
                              replace3 ( identifier::createFromString ( "", "." ) );

    TEST_EQUALS_INT( mutator.add ( *( id1.get ( ) ), *( replace1.get ( ) ) ), std::string::npos );  // No collision
    TEST_EQUALS_INT( mutator.add ( *( id2.get ( ) ), *( replace1.get ( ) ) ), 2 );                  // Collision at index 2, "third"
    TEST_EQUALS_INT( mutator.add ( *( id3.get ( ) ), *( replace1.get ( ) ) ), 2 );                  // Collision at index 2, "third"
    TEST_EQUALS_INT( mutator.add ( *( id4.get ( ) ), *( replace2.get ( ) ) ), std::string::npos );  // No collision
    TEST_THROWS_EXCEPTION( mutator.add ( *( id5.get ( ) ), *( replace2.get ( ) ) ), std::runtime_error );
    TEST_EQUALS_INT( mutator.add ( *( id6.get ( ) ), *( replace3.get ( ) ) ), std::string::npos );  // No collision
    TEST_EQUALS_INT( mutator.add ( *( id6.get ( ) ), *( replace3.get ( ) ) ), 0 );                  // Collision at index 0, "second"

    // Commence mutation
    std::auto_ptr<identifier> test1 ( identifier::createFromString ( "first.second.third.fourth.fifth", "." ) ),
                              test2 ( identifier::createFromString ( "first.second.split", "." ) ),
                              test3 ( identifier::createFromString ( "first.split.third", "." ) ),
                              test4 ( identifier::createFromString ( "third.split.third", "." ) ),
                              test5 ( identifier::createFromString ( "second", "." ) ),
                              test6 ( identifier::createFromString ( "second.third.fourth.", "." ) ),
                              expected1 ( identifier::createFromString ( "replace1.replace2.fourth.fifth", "." ) ),
                              expected3 ( identifier::createFromString ( "replace3", "->" ) ),
                              expected5 ( identifier::createFromString ( "", "." ) ),
                              expected6 ( identifier::createFromString ( "third.fourth", "." ) ),
                              result;

    TEST_THROWS_NOTHING( result.reset ( mutator.mutatedClone ( *( test1.get ( ) ) ) ) );
    TEST_EQUALS( result, expected1 );
    TEST_THROWS_NOTHING( result.reset ( mutator.mutatedClone ( *( test2.get ( ) ) ) ) );
    TEST_EQUALS( result, test2 );
    TEST_THROWS_NOTHING( result.reset ( mutator.mutatedClone ( *( test3.get ( ) ) ) ) );
    TEST_EQUALS( result, expected3 );
    TEST_THROWS_NOTHING( result.reset ( mutator.mutatedClone ( *( test4.get ( ) ) ) ) );
    TEST_EQUALS( result, test4 );
    TEST_THROWS_NOTHING( result.reset ( mutator.mutatedClone ( *( test5.get ( ) ) ) ) );
    TEST_EQUALS( result, expected5 );
    TEST_THROWS_NOTHING( result.reset ( mutator.mutatedClone ( *( test6.get ( ) ) ) ) );
    TEST_EQUALS( result, expected6 );

    // Test stem mutation
    std::auto_ptr<identifier> teststem1 ( identifier::createFromString ( "first.second.third.fourth.fifth", "." ) ),
                              teststem2 ( identifier::createFromString ( "first.second.third", "." ) ),
                              teststem3 ( identifier::createFromString ( "second", "." ) ),
                              teststem4 ( identifier::createFromString ( "second.third", "." ) ),
                              expectedstem4 ( identifier::createFromString ( "third", "." ) );

    TEST_THROWS_NOTHING( result.reset ( mutator.mutatedCloneStem ( *( teststem1.get ( ) ) ) ) );
    TEST_EQUALS( result, expected1 );
    TEST_THROWS_NOTHING( result.reset ( mutator.mutatedCloneStem ( *( teststem2.get ( ) ) ) ) );
    TEST_EQUALS( result, teststem2 );
    TEST_THROWS_NOTHING( result.reset ( mutator.mutatedCloneStem ( *( teststem3.get ( ) ) ) ) );
    TEST_EQUALS( result, teststem3 );
    TEST_THROWS_NOTHING( result.reset ( mutator.mutatedCloneStem ( *( teststem4.get ( ) ) ) ) );
    TEST_EQUALS( result, expectedstem4 );

    // Test prefix mutation
    std::auto_ptr<identifier> testpstem1  ( identifier::createFromString ( "first", "." ) ),
                              testpstem2  ( identifier::createFromString ( "first.second.third", "." ) ),
                              testprefix1 ( identifier::createFromString ( "minus.zero", "." ) ),
                              pexpected1  ( identifier::createFromString ( "minus.zero.first", "." ) ),
                              pexpected2  ( identifier::createFromString ( "minus.zero.first.second.third", "." ) );

    mutator.add ( *testprefix1.get ( ) );

    TEST_THROWS_NOTHING( result.reset ( mutator.mutatedCloneStem ( *( testpstem1.get ( ) ) ) ) );
    TEST_EQUALS( result, pexpected1 );
    TEST_THROWS_NOTHING( result.reset ( mutator.mutatedCloneStem ( *( testpstem2.get ( ) ) ) ) );
    TEST_EQUALS( result, pexpected2 );
END_TEST

DEFINE_TEST_SUITE( IdentifierMutator )
    REGISTER_TEST( IdentifierSearch )
END_TEST_SUITE
