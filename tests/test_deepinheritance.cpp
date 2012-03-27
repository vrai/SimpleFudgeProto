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
#include "built_basemessage.hpp"
#include "built_intermediatemessage.hpp"
#include "built_topmessage.hpp"

using namespace fudgeproto;
using namespace built;

DEFINE_TEST( EncodeDecode )
    // Construct the source message
    std::auto_ptr<TopMessage> msg ( new TopMessage );
    msg->setbasefld ( 100 );
    msg->setinterfld ( 200 );
    msg->settopfld ( 300 );

    // Encode the source message and destroy it
    std::pair<fudge_byte *, fudge_i32> encoded ( encode ( *msg, "deep.dat" ) );
    msg.reset ( new TopMessage );

    // Clear the target message - then decode the source in to it
    msg->setbasefld ( -1 );
    msg->setinterfld ( -1 );
    msg->settopfld ( -1 );

    fudge::message payload ( fudge::codec ( ).decode ( encoded.first,
                                                       encoded.second ).payload ( ) );
    free ( encoded.first );
    msg->fromFudgeMessage ( payload );

    // Check the results
    TEST_EQUALS( msg->basefld ( ), 100 );
    TEST_EQUALS( msg->interfld ( ), 200 );
    TEST_EQUALS( msg->topfld ( ), 300 );

END_TEST

DEFINE_TEST_SUITE( DeepInheritance )
    REGISTER_TEST( EncodeDecode )
END_TEST_SUITE
