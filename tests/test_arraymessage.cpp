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

#include "encoderutils.hpp"
#include "simpletest.hpp"
#include "memoryutil.hpp"
#include "built_array_arraymessage.hpp"

using namespace fudgeproto;
using namespace built::array;

namespace
{
    static ElementMessage * createElementMessage ( const std::string & value )
    {
        ElementMessage * message ( new ElementMessage );
        message->setvalue ( value );
        return message;
    }
}

DEFINE_TEST( NullArrayElements )
    const std::string first ( "First" ),
                      third ( "Third" ),
                      fifth ( "Fifth" );

    // Construct an array of ElementMessages that includes some null pointers
    std::vector<ElementMessage *> elements;
    elements.push_back ( createElementMessage ( first ) );
    elements.push_back ( 0 );
    elements.push_back ( createElementMessage ( third ) );
    elements.push_back ( 0 );
    elements.push_back ( createElementMessage ( fifth ) );

    // Place the ElementMessages in an ArrayMessage, then convert that to a
    // fudge message and back again.
    ArrayMessage input;
    input.setelements ( elements );
    elements.clear ( );

    ArrayMessage output ( input.asFudgeMessage ( ) );
    elements = output.elements ( );

    // Check the content
    TEST_EQUALS_INT( elements.size ( ), 5 );
    TEST_EQUALS( elements [ 0 ]->value ( ).convertToStdString ( ), first );
    TEST_EQUALS_TRUE( ! elements [ 1 ] );
    TEST_EQUALS( elements [ 2 ]->value ( ).convertToStdString ( ), third );
    TEST_EQUALS_TRUE( ! elements [ 3 ] );
    TEST_EQUALS( elements [ 4 ]->value ( ).convertToStdString ( ), fifth );
END_TEST

DEFINE_TEST_SUITE( ArrayMessage )
    REGISTER_TEST( NullArrayElements )
END_TEST_SUITE
