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

#define __STDC_LIMIT_MACROS
#include <stdint.h>
#include "encoderutils.hpp"
#include "simpletest.hpp"
#include "memoryutil.hpp"
#include "built_complex_nestedmessagetwo.hpp"

using namespace fudgeproto;
using namespace built;

DEFINE_TEST( NestedOne )
    // Construct the test values
    static const fudge_i32 sourceIdentifier ( 12345678 );
    static const fudge::string sourceString ( "Source string for FlatMessageOne" );
    static const fudge_i64 sourceChecksum ( INT64_MAX );

    std::vector< std::vector<fudge_f64> > sourceCoords ( 8 );
    for ( size_t y ( 0 ); y < 8; ++y )
    {
        sourceCoords [ y ].resize ( 2 );
        for ( size_t x ( 0 ); x < sourceCoords [ y ].size ( ); ++x )
            sourceCoords [ y ] [ x ] = static_cast<double> ( y ) * static_cast<double> ( x );
    }

    std::vector< std::vector<float> > sourceMatrix;
    sourceMatrix.resize ( 2 );
    for ( size_t y ( 0 ); y < sourceMatrix.size ( ); ++y )
    {
        sourceMatrix [ y ].resize ( 2 );
        for ( size_t x ( 0 ); x < sourceMatrix [ x ].size ( ); ++x )
            sourceMatrix [ y ] [ x ] = static_cast<float> ( y * x );
    }

    std::vector<FlatMessageTwo::FlatEnum> sourceEnums ( 2 );
    sourceEnums [ 0 ] = FlatMessageTwo::SecondValue;
    sourceEnums [ 1 ] = FlatMessageTwo::FifthValue;

    std::vector<int> sourceIntegers;
    for ( size_t index ( 0 ); index < 4; ++index )
        sourceIntegers.push_back ( index * index );

    // Create/populate inner messages
    std::auto_ptr<FlatMessageOne> flatone ( new FlatMessageOne );
    flatone->setidentifier ( sourceIdentifier );
    flatone->setdescription ( sourceString );

    std::auto_ptr<Combined::FlatMessage> combined ( new Combined::FlatMessage );
    combined->setidentifier ( -sourceIdentifier );
    combined->setdescription ( sourceString );
    combined->setcoords ( sourceCoords );
    combined->setenumerations ( sourceEnums );
    combined->setmatrix ( sourceMatrix );
    combined->setintegers ( sourceIntegers );

    // Create/populate main message
    std::auto_ptr<NestedMessageOne> nestedone ( new NestedMessageOne );
    nestedone->setcombined ( combined.release ( ) );
    nestedone->setflatone ( flatone.release ( ) );
    nestedone->setchecksum ( sourceChecksum );

    // Encode and decode
    std::pair<fudge_byte *, fudge_i32> encoded ( encode ( *nestedone, "nestedone.dat" ) );
    TEST_EQUALS( encoded.second, 514 );
    nestedone.reset ( decode<NestedMessageOne> ( encoded ) );
    TEST_EQUALS_TRUE( nestedone.get ( ) != 0 );

    // Check decoded values
    TEST_EQUALS_TRUE( nestedone->flatone ( ) != 0 );
    TEST_EQUALS( nestedone->flatone ( )->identifier ( ), sourceIdentifier );
    TEST_EQUALS_TRUE( nestedone->flatone ( )->description ( ) );
    TEST_EQUALS( ( *nestedone->flatone ( )->description ( ) ).convertToStdString ( ), sourceString.convertToStdString ( ) );

    TEST_EQUALS_TRUE( nestedone->combined ( ) != 0 );
    TEST_EQUALS( nestedone->combined ( )->identifier ( ), -sourceIdentifier );
    TEST_EQUALS_TRUE( nestedone->combined ( )->description ( ) );
    TEST_EQUALS( ( *nestedone->combined ( )->description ( ) ).convertToStdString ( ), sourceString.convertToStdString ( ) );
    TEST_EQUALS_VECTOR( nestedone->combined ( )->coords ( ), sourceCoords );
    TEST_EQUALS_TRUE( nestedone->combined ( )->enumerations ( ) );
    TEST_EQUALS_VECTOR( *nestedone->combined ( )->enumerations ( ), sourceEnums );
    TEST_EQUALS_TRUE( ! nestedone->combined ( )->tags ( ) );
    TEST_EQUALS_TRUE( nestedone->combined ( )->matrix ( ) );
    TEST_EQUALS_VECTOR( *nestedone->combined ( )->matrix ( ), sourceMatrix );
    TEST_EQUALS_TRUE( nestedone->combined ( )->integers ( ) );
    TEST_EQUALS_VECTOR( *nestedone->combined ( )->integers ( ), sourceIntegers );

    TEST_EQUALS( nestedone->checksum ( ), sourceChecksum );
END_TEST

DEFINE_TEST( NestedTwoEmpty )
    std::auto_ptr<Complex::NestedMessageTwo> message ( new Complex::NestedMessageTwo );

    // Despite being optional, flag has a default value. Remove it to make the message truely empty
    message->setflag ( fudge::optional<fudge_bool> ( ) );

    std::pair<fudge_byte *, fudge_i32> encoded ( encode ( *message, "nestedtwoempty.dat" ) );
    TEST_EQUALS( encoded.second, 59 );
    message.reset ( decode<Complex::NestedMessageTwo> ( encoded ) );
    TEST_EQUALS_TRUE( message.get ( ) != 0 );
END_TEST

DEFINE_TEST( NestedTwo )
    // Construct the test values
    static const fudge_i32 sourceIdentifier ( INT16_MIN );
    static const fudge::string sourceString ( "Source string for NestTwo message" );
    static const fudge_i64 sourceChecksum ( 2 );

    std::vector< std::vector< std::vector< fudge::string> > > sourceTags ( 6 );
    for ( size_t z ( 0 ); z < sourceTags.size ( ); ++z )
    {
        sourceTags [ z ].resize ( 8 - z );
        for ( size_t y ( 0 ); y < sourceTags [ z ].size ( ); ++y )
        {
            sourceTags [ z ] [ y ].resize ( 4 );
            for ( size_t x ( 0 ); x < sourceTags [ x ].size ( ); ++x )
            {
                std::ostringstream buffer;
                buffer << "Tag[" << z << y << x << "]";
                fudge::string string ( buffer.str ( ) );
                sourceTags [ z ] [ y ] [ x ] = string;
            }
        }
    }

    std::vector<int> sourceIntegers;
    for ( size_t index ( 0 ); index < 4; ++index )
        sourceIntegers.push_back ( index );

    // Create/populate messages for use as array payloads
    std::vector<FlatMessageOne *> sourceElementMessages;
    for ( size_t index ( 0 ); index < 4; ++index )
    {
        std::auto_ptr<FlatMessageOne> message ( new FlatMessageOne );
        message->setidentifier ( index );
        message->setdescription ( fudge::optional<fudge::string> ( ) );
        sourceElementMessages.push_back ( message.release ( ) );
    }

    // Create/populate inner messages
    std::auto_ptr<FlatMessageOne> flatone ( new FlatMessageOne );
    flatone->setidentifier ( sourceIdentifier );
    flatone->setdescription ( sourceString );

    std::auto_ptr<Combined::FlatMessage> combined ( new Combined::FlatMessage );
    combined->setidentifier ( -sourceIdentifier );
    combined->setdescription ( sourceString );
    combined->settags ( sourceTags );
    combined->setintegers ( sourceIntegers );

    std::auto_ptr<NestedMessageOne> nestedone ( new NestedMessageOne );
    nestedone->setcombined ( combined.release ( ) );
    nestedone->setflatone ( flatone.release ( ) );
    nestedone->setchecksum ( sourceChecksum );

    // Create/populate main message
    std::auto_ptr<Complex::NestedMessageTwo> nestedtwo ( new Complex::NestedMessageTwo );
    nestedtwo->setinner ( nestedone.release ( ) );

    TEST_EQUALS_TRUE( nestedtwo->flag ( ) );
    TEST_EQUALS( *nestedtwo->flag ( ), FUDGE_FALSE );
    nestedtwo->setflag ( FUDGE_TRUE );

    std::vector< std::vector<FlatMessageOne *> > messageArray;
    messageArray.resize ( 1 );

    messageArray [ 0 ].resize ( 2 );
    messageArray [ 0 ] [ 0 ] = sourceElementMessages [ 0 ];
    messageArray [ 0 ] [ 1 ] = sourceElementMessages [ 1 ];
    nestedtwo->setmessageArray ( messageArray );

    std::vector< std::vector< std::vector<FlatMessageOne *> > > optMessageArray;
    optMessageArray.resize ( 1 );
    optMessageArray [ 0 ].resize ( 2 );
    optMessageArray [ 0 ] [ 0 ].push_back ( sourceElementMessages [ 2 ] );
    optMessageArray [ 0 ] [ 1 ].push_back ( sourceElementMessages [ 3 ] );
    nestedtwo->setoptMessageArray ( optMessageArray );

    // Source array is no longer usable - the messages are owned by nestedtwo
    sourceElementMessages.clear ( );
    messageArray.clear ( );
    optMessageArray.clear ( );

    // Encode and decode
    std::pair<fudge_byte *, fudge_i32> encoded ( encode ( *nestedtwo, "nestedtwo.dat" ) );
    TEST_EQUALS( encoded.second, 1805 );
    nestedtwo.reset ( decode<Complex::NestedMessageTwo> ( encoded ) );
    TEST_EQUALS_TRUE( nestedtwo.get ( ) != 0 );

    // Check decoded values
    TEST_EQUALS_TRUE( nestedtwo->inner ( ) != 0 );
    TEST_EQUALS( nestedtwo->inner ( )->checksum ( ), sourceChecksum );

    TEST_EQUALS_TRUE( nestedtwo->inner ( )->flatone ( ) != 0 );
    TEST_EQUALS( nestedtwo->inner( )->flatone ( )->identifier ( ), sourceIdentifier );
    TEST_EQUALS_TRUE( nestedtwo->inner( )->flatone ( )->description ( ) );
    TEST_EQUALS( ( *nestedtwo->inner( )->flatone ( )->description ( ) ).convertToStdString ( ), sourceString.convertToStdString ( ) );

    TEST_EQUALS_TRUE( nestedtwo->inner ( ) != 0 );
    TEST_EQUALS_TRUE( nestedtwo->inner ( )->combined ( ) != 0 );
    TEST_EQUALS( nestedtwo->inner ( )->combined ( )->identifier ( ), -sourceIdentifier );
    TEST_EQUALS_TRUE( nestedtwo->inner ( )->combined ( )->description ( ) );
    TEST_EQUALS( ( *nestedtwo->inner ( )->combined ( )->description ( ) ).convertToStdString ( ), sourceString.convertToStdString ( ) );
    TEST_EQUALS_TRUE( nestedtwo->inner ( )->combined ( )->tags ( ) );
    TEST_EQUALS_VECTOR( *nestedtwo->inner ( )->combined ( )->tags ( ), sourceTags );
    TEST_EQUALS_TRUE( nestedtwo->inner ( )->combined ( )->coords ( ).empty ( ) );
    TEST_EQUALS_TRUE( ! nestedtwo->inner ( )->combined ( )->enumerations ( ) );
    TEST_EQUALS_TRUE( ! nestedtwo->inner ( )->combined ( )->matrix ( ) );
    TEST_EQUALS_TRUE( nestedtwo->inner ( )->combined ( )->integers ( ) );
    TEST_EQUALS_VECTOR( *nestedtwo->inner ( )->combined ( )->integers ( ), sourceIntegers );

    TEST_EQUALS_TRUE( nestedtwo->flag ( ) );
    TEST_EQUALS( *nestedtwo->flag ( ), FUDGE_TRUE );

    TEST_EQUALS_INT( nestedtwo->messageArray ( ).size ( ), 1 );
    TEST_EQUALS_INT( nestedtwo->messageArray ( ) [ 0 ].size ( ), 2 );
    TEST_EQUALS_TRUE( nestedtwo->messageArray ( ) [ 0 ] [ 0 ] != 0 );
    TEST_EQUALS_TRUE( nestedtwo->messageArray ( ) [ 0 ] [ 1 ] != 0 );
    TEST_EQUALS_INT( nestedtwo->messageArray ( ) [ 0 ] [ 0 ]->identifier ( ), 0 );
    TEST_EQUALS_INT( nestedtwo->messageArray ( ) [ 0 ] [ 1 ]->identifier ( ), 1 );

    TEST_EQUALS_TRUE( nestedtwo->optMessageArray ( ) );
    TEST_EQUALS_INT( ( *nestedtwo->optMessageArray ( ) ).size ( ), 1 );
    TEST_EQUALS_INT( ( *nestedtwo->optMessageArray ( ) ) [ 0 ].size ( ), 2 );
    TEST_EQUALS_INT( ( *nestedtwo->optMessageArray ( ) ) [ 0 ] [ 0 ].size ( ), 1 );
    TEST_EQUALS_INT( ( *nestedtwo->optMessageArray ( ) ) [ 0 ] [ 1 ].size ( ), 1 );
    TEST_EQUALS_TRUE( ( *nestedtwo->optMessageArray ( ) ) [ 0 ] [ 0 ] [ 0 ] != 0 );
    TEST_EQUALS_TRUE( ( *nestedtwo->optMessageArray ( ) ) [ 0 ] [ 1 ] [ 0 ] != 0 );
    TEST_EQUALS_INT( ( *nestedtwo->optMessageArray ( ) ) [ 0 ] [ 0 ] [ 0 ]->identifier ( ), 2 );
    TEST_EQUALS_INT( ( *nestedtwo->optMessageArray ( ) ) [ 0 ] [ 1 ] [ 0 ]->identifier ( ), 3 );
END_TEST

DEFINE_TEST_SUITE( NestedMessage )
    REGISTER_TEST( NestedOne )
    REGISTER_TEST( NestedTwoEmpty )
    REGISTER_TEST( NestedTwo )
END_TEST_SUITE

