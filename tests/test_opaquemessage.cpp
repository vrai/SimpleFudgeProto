/**
 * Copyright (C) 2012 - 2012, Vrai Stacey.
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
#include "built_opaqueholdermessage.hpp"

using namespace fudgeproto;
using namespace built;

DEFINE_TEST( EncodeDecode )
    ::fudge::message empty, numbers;
    numbers.addField ( 1, ::fudge::string ( "one" ) );
    numbers.addField ( "Two", ::fudge::string ( "two" ) );
        
    // Construct the first source message
    std::auto_ptr<OpaqueHolderMessage> msg ( new OpaqueHolderMessage );
    msg->setflag ( FUDGE_FALSE );
    msg->setopaque ( empty );

    // Encode the first source message and destroy it
    std::pair<fudge_byte *, fudge_i32> encoded ( encode ( *msg, "opaque_1.dat" ) );
    msg.reset ( new OpaqueHolderMessage );
    TEST_EQUALS( msg->flag ( ), FUDGE_TRUE );

    // Decode the message
    fudge::message payload (
        fudge::codec ( ).decode ( encoded.first,
                                  encoded.second ).payload ( ) );
    free ( encoded.first );
    msg->fromFudgeMessage ( payload );

    // Check the results
    TEST_EQUALS_INT( msg->flag ( ), FUDGE_FALSE );
    TEST_EQUALS_INT( msg->opaque ( ).size ( ), 0 );
    TEST_EQUALS_TRUE( ! msg->opaqueArray ( ) );

    // Construct the second source message (using the array)
    std::vector< ::fudge::message > msgs;
    msgs.push_back ( empty );
    msgs.push_back ( numbers );

    msg->setflag ( FUDGE_TRUE );
    msg->setopaque ( numbers );
    msg->setopaqueArray ( msgs );

    // Encode the second source message and destroy it
    encoded = encode ( *msg, "opaque_2.dat" );
    msg.reset ( new OpaqueHolderMessage );
    TEST_EQUALS_INT( msg->opaque ( ).size ( ), 0 );
    TEST_EQUALS_TRUE( ! msg->opaqueArray ( ) );

    // Decode the message
    payload =  fudge::codec ( ).decode (
        encoded.first,
        encoded.second ).payload ( );
    free ( encoded.first );
    msg->fromFudgeMessage ( payload );

    // Check the results
    TEST_EQUALS_INT( msg->flag ( ), FUDGE_TRUE );
    TEST_EQUALS_INT( msg->opaque ( ).size ( ), numbers.size ( ) );
    TEST_EQUALS_TRUE( msg->opaqueArray ( ) );
    TEST_EQUALS_INT( ( *msg->opaqueArray ( ) ).size ( ), msgs.size ( ) );
    for ( size_t idx ( 0 ); idx < msgs.size ( ); ++idx )
    {
        TEST_EQUALS_INT( ( *msg->opaqueArray ( ) ) [ idx ].size ( ),
                         msgs [ idx ].size ( ) );
    }

END_TEST

DEFINE_TEST_SUITE( OpaqueMessage )
    REGISTER_TEST( EncodeDecode )
END_TEST_SUITE
