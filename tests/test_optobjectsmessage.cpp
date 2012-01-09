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
#include "built_optobjects_outer.hpp"

using namespace fudgeproto;
using namespace built::optobjects;

namespace
{
    static Inner * createInner ( const std::string & name )
    {
        Inner * inner ( new Inner );
        inner->setname ( name );
        return inner;
    }
}

DEFINE_TEST( StandardConstructor )
    static const std::string firstname ( "First" ),
                             secondname ( "Second" );

    // Set first
    Outer source;
    source.setfirst ( createInner ( firstname ) );

    fudge::message message ( source.asFudgeMessage ( ) );
    const Outer first ( message );
    TEST_EQUALS_TRUE( ( bool ) first.first ( ) );
    TEST_EQUALS_TRUE( ! ( bool ) first.second ( ) );
    TEST_EQUALS( first.first ( )->name ( ).convertToStdString ( ), firstname );

    // Set second
    source.setfirst ( 0 );
    source.setsecond ( createInner ( secondname ) );
    message = source.asFudgeMessage ( );
    const Outer second ( message );
    TEST_EQUALS_TRUE( ! ( bool ) second.first ( ) );
    TEST_EQUALS_TRUE( ( bool ) second.second ( ) );
    TEST_EQUALS( second.second ( )->name ( ).convertToStdString ( ), secondname );

    // Set neither
    source.setsecond ( 0 );
    message = source.asFudgeMessage ( );
    const Outer neither ( message );
    TEST_EQUALS_TRUE( ! ( bool ) neither.first ( ) );
    TEST_EQUALS_TRUE( ! ( bool ) neither.second ( ) );
END_TEST

DEFINE_TEST_SUITE( OptionalObjects )
    REGISTER_TEST( StandardConstructor )
END_TEST_SUITE
