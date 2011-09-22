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
#include "built_safe_safemessageone.hpp"
#include "built_unsafe_safemessageone.hpp"
#include <fudge-cpp/exception.hpp>
#include <cstdio>

using namespace fudgeproto;
using namespace built;

static fudge::message buildMessage ( bool safe )
{
    fudge::message innermessage, outermessage;
    innermessage.addField ( 2.048f, fudge::string ( "floating" ) );

    if ( safe )
        innermessage.addField ( fudge::string ( "built.safe.SafeMessageTwo" ),
                                fudge::message::noname, 0 );

    outermessage.addField ( 1024, fudge::string ( "integer" ) );
    outermessage.addField ( innermessage, fudge::string ( "nested" ) );

    if ( safe )
        outermessage.addField ( fudge::string ( "built.safe.SafeMessageOne" ),
                                fudge::message::noname, 0 );

    return outermessage;
}

DEFINE_TEST( SafeWorking )
    fudge::message encoded ( buildMessage ( true ) );

    safe::SafeMessageOne decoded;
    TEST_THROWS_NOTHING( decoded.fromFudgeMessage ( encoded ) );

    TEST_EQUALS_INT( decoded.integer ( ), 1024 );
    TEST_EQUALS_TRUE( decoded.nested ( ) != 0 );
    TEST_EQUALS_FLOAT( decoded.nested ( )->floating ( ), 2.048f, 0.0001f );
END_TEST

DEFINE_TEST( SafeFailed )
    fudge::message encoded ( buildMessage ( false ) );

    safe::SafeMessageOne decoded;
    TEST_THROWS_EXCEPTION( decoded.fromFudgeMessage ( encoded ), fudge::exception );
END_TEST

DEFINE_TEST( UnsafeWorking )
    fudge::message encoded ( buildMessage ( false ) );

    unsafe::SafeMessageOne decoded;
    TEST_THROWS_NOTHING( decoded.fromFudgeMessage ( encoded ) );

    TEST_EQUALS_INT( decoded.integer ( ), 1024 );
    TEST_EQUALS_TRUE( decoded.nested ( ) != 0 );
    TEST_EQUALS_FLOAT( decoded.nested ( )->floating ( ), 2.048f, 0.0001f );
END_TEST

DEFINE_TEST_SUITE( SafeMessage )
    REGISTER_TEST( SafeWorking )
    REGISTER_TEST( SafeFailed )
END_TEST_SUITE

