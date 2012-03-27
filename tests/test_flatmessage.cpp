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
#include "built_combined_flatmessage.hpp"
#include <cstdio>
#include <memory>

using namespace fudgeproto;
using namespace built;

namespace
{
    static const std::string defaultDescription ( "Insert description here: \"Escape sequences, \'\\\n\"" );
}

DEFINE_TEST( FlatOne )
    static const fudge_i32 sourceIdentifier ( 123 );
    static const fudge::string sourceDescription ( "This is a string!" );

    // Construct, check the defaults and populate
    std::auto_ptr<FlatMessageOne> message ( new FlatMessageOne );

    TEST_EQUALS( message->identifier ( ), -1 );
    TEST_EQUALS_TRUE( message->description ( ) );
    TEST_EQUALS( ( *message->description ( ) ).convertToStdString ( ), defaultDescription );

    message->setidentifier ( sourceIdentifier );
    message->setdescription ( sourceDescription );

    TEST_EQUALS( message->identifier ( ), sourceIdentifier );
    TEST_EQUALS_TRUE( message->description ( ) );
    TEST_EQUALS( ( *message->description ( ) ).convertToStdString ( ), sourceDescription.convertToStdString ( ) );

    // Encode and decode
    std::pair<fudge_byte *, fudge_i32> encoded ( encode ( *message, "flatone.dat" ) );
    TEST_EQUALS( encoded.second, 81 );
    message.reset ( decode<FlatMessageOne> ( encoded ) );
    TEST_EQUALS_TRUE( message.get ( ) != 0 );

    // Check the content is correct
    TEST_EQUALS( message->identifier ( ), sourceIdentifier );
    TEST_EQUALS_TRUE( message->description ( ) );
    TEST_EQUALS( ( *message->description ( ) ).convertToStdString ( ), sourceDescription.convertToStdString ( ) );

    // Blank out the optional field
    message->setdescription ( fudge::optional<fudge::string> ( ) );
    TEST_EQUALS_TRUE( ! message->description ( ) );

    // Encode and decode
    encoded = encode ( *message, "flatone_blanked.dat" );
    TEST_EQUALS( encoded.second, 49 );
    message.reset ( decode<FlatMessageOne> ( encoded ) );
    TEST_EQUALS_TRUE( message.get ( ) != 0 );

    // Check the content
    TEST_EQUALS( message->identifier ( ), sourceIdentifier );
    TEST_EQUALS_TRUE( message->description ( ) );
    TEST_EQUALS( ( *message->description ( ) ).convertToStdString ( ), defaultDescription );

    message.reset ( );
END_TEST

DEFINE_TEST( FlatTwo )
    // Construct test values
    std::vector< std::vector<fudge_f64> > emptyCoords, sourceCoords, brokenCoords;
    sourceCoords.resize ( 4 );
    for ( size_t y ( 0 ); y < sourceCoords.size ( ); ++y )
    {
        sourceCoords [ y ].resize ( 2 );
        for ( size_t x ( 0 ); x < sourceCoords [ y ].size ( ); ++x )
            sourceCoords [ y ] [ x ] = static_cast<double> ( y ) / static_cast<double> ( x + 1 );
    }

    brokenCoords.resize ( 8 );
    for ( size_t y ( 0 ); y < brokenCoords.size ( ); ++y )
    {
        brokenCoords [ y ].resize ( y + 1 );
        for ( size_t x ( 0 ); x < brokenCoords [ y ].size ( ); ++x )
            brokenCoords [ y ] [ x ] = static_cast<double> ( x );
    }

    std::vector< std::vector< std::vector<fudge::string> > > emptyTags, sourceTags, brokenTags;
    sourceTags.resize ( 2 );
    for ( size_t z ( 0 ); z < sourceTags.size ( ); ++z )
    {
        sourceTags [ z ].resize ( 3 );
        for ( size_t y ( 0 ); y < sourceTags [ z ].size ( ); ++y )
        {
            sourceTags [ z ] [ y ].resize ( 4 );
            for ( size_t x ( 0 ); x < sourceTags [ z ] [ y ].size ( ); ++x )
            {
                std::stringstream buf;
                buf << "String " << z << ", " << y << ", " << x;
                sourceTags [ z ] [ y ] [ x ] = fudge::string ( buf.str ( ) );
            }
        }
    }

    brokenTags.resize ( 1 );
    brokenTags [ 0 ].resize ( 1 );
    brokenTags [ 0 ] [ 0 ].resize ( 3 );

    // Construct, check the defaults and populate
    std::auto_ptr<FlatMessageTwo> message ( new FlatMessageTwo );

    TEST_EQUALS_TRUE( message->coords ( ).empty ( ) );
    message->setcoords ( sourceCoords );
    message->settags ( sourceTags );
    TEST_EQUALS_TRUE( message->coords ( ) == sourceCoords );

    // Encode and decode
    std::pair<fudge_byte *, fudge_i32> encoded ( encode ( *message, "flattwo.dat" ) );
    TEST_EQUALS( encoded.second, 560 );
    message.reset ( decode<FlatMessageTwo> ( encoded ) );
    TEST_EQUALS_TRUE( message.get ( ) != 0 );

    // Check the content is correct
    TEST_EQUALS_VECTOR( message->coords ( ), sourceCoords );
    TEST_EQUALS_TRUE( message->tags ( ) );
    TEST_EQUALS_VECTOR( *message->tags ( ), sourceTags );

    // Test empty vectors encoding
    message.reset ( new FlatMessageTwo );
    message->setcoords ( emptyCoords );
    TEST_EQUALS_TRUE( message->coords ( ).empty ( ) );
    encoded = encode ( *message, "flattwo_empty.dat" );
    TEST_EQUALS( encoded.second, 43 );
    message.reset ( decode<FlatMessageTwo> ( encoded ) );
    TEST_EQUALS_TRUE( message.get ( ) != 0 );

    TEST_EQUALS_VECTOR( message->coords ( ), emptyCoords );
    TEST_EQUALS_TRUE( ! message->tags ( ) );

    // Test invalid coord vector
    message->setcoords ( brokenCoords );
    TEST_THROWS_EXCEPTION( encoded = encode ( *message, "flattwo_broken.dat" ), std::runtime_error );

    // Test invalid tags vector
    message->setcoords ( emptyCoords );
    message->settags ( brokenTags );
    TEST_THROWS_EXCEPTION( encoded = encode ( *message, "flattwo_broken.dat" ), std::runtime_error );
END_TEST

DEFINE_TEST( CombinedFlatMessage )
    // Construct test values
    static const fudge_i32 sourceIdentifier ( -456789 );
    static const fudge::string sourceString ( "Yet another string" );

    std::vector< std::vector<fudge_f64> > sourceCoords;
    sourceCoords.resize ( 8 );
    for ( size_t y ( 0 ); y < sourceCoords.size ( ); ++y )
    {
        sourceCoords [ y ].resize ( 2 );
        for ( size_t x ( 0 ); x < 2; ++x )
            sourceCoords [ y ] [ x ] = static_cast<double> ( y + 1 ) / static_cast<double> ( x + 1 );
    }

    std::vector<int> emptyIntegers, sourceIntegers;
    for ( size_t index ( 0 ); index < 4; ++index )
        sourceIntegers.push_back ( index * index );

    std::vector< std::vector<float> > sourceMatrix;
    sourceMatrix.resize ( 2 );
    for ( size_t y ( 0 ); y < sourceMatrix.size ( ); ++y )
    {
        sourceMatrix [ y ].resize ( 2 );
        for ( size_t x ( 0 ); x < 2; ++x )
            sourceMatrix [ y ] [ x ] = static_cast<float> ( ( 1 + y )  * ( 1 +  y ) ) / static_cast<float> ( 1 + x );
    }

    std::vector<FlatMessageTwo::FlatEnum> sourceEnums;
    sourceEnums.push_back ( FlatMessageTwo::FirstValue );
    sourceEnums.push_back ( FlatMessageTwo::FifthValue );
    sourceEnums.push_back ( FlatMessageTwo::SecondValue );
    sourceEnums.push_back ( FlatMessageTwo::FourthValue );
    sourceEnums.push_back ( FlatMessageTwo::ThirdValue );

    // Construct, check the defaults and populate
    std::auto_ptr<Combined::FlatMessage> message ( new Combined::FlatMessage );

    TEST_EQUALS( message->identifier ( ), -1 );
    TEST_EQUALS_TRUE( message->description ( ) );
    TEST_EQUALS( ( *message->description ( ) ).convertToStdString ( ), defaultDescription );
    TEST_EQUALS_TRUE( message->coords ( ).empty ( ) );
    TEST_EQUALS_TRUE( ! message->integers ( ) );
    TEST_EQUALS_TRUE( ! message->matrix ( ) );
    TEST_EQUALS_TRUE( ! message->enumerations ( ) );

    message->setidentifier ( sourceIdentifier );
    message->setdescription ( sourceString );
    message->setcoords ( sourceCoords );
    message->setintegers ( sourceIntegers );
    message->setmatrix ( sourceMatrix );
    message->setenumerations ( sourceEnums );

    TEST_EQUALS( message->identifier ( ), sourceIdentifier );
    TEST_EQUALS_TRUE( message->description ( ) );
    TEST_EQUALS( ( *message->description ( ) ).convertToStdString ( ), sourceString.convertToStdString ( ) );
    TEST_EQUALS_TRUE( message->coords ( ) == sourceCoords );
    TEST_EQUALS_TRUE( message->integers ( ) );
    TEST_EQUALS_TRUE( *message->integers ( ) == sourceIntegers );
    TEST_EQUALS_TRUE( *message->matrix ( ) == sourceMatrix );
    TEST_EQUALS_TRUE( message->enumerations ( ) );
    TEST_EQUALS_TRUE( *message->enumerations ( ) == sourceEnums );

    // Encode and decode
    std::pair<fudge_byte *, fudge_i32> encoded ( encode ( *message, "combinedflat.dat" ) );
    TEST_EQUALS( encoded.second, 344 );
    message.reset ( decode<Combined::FlatMessage> ( encoded ) );
    TEST_EQUALS_TRUE( message.get ( ) != 0 );

    // Check the content is correct
    TEST_EQUALS( message->identifier ( ), sourceIdentifier );
    TEST_EQUALS_TRUE( message->description ( ) );
    TEST_EQUALS( ( *message->description ( ) ).convertToStdString ( ), sourceString.convertToStdString ( ) );
    TEST_EQUALS_VECTOR( message->coords ( ), sourceCoords );
    TEST_EQUALS_TRUE( message->integers ( ) );
    TEST_EQUALS_VECTOR( *message->integers ( ), sourceIntegers );
    TEST_EQUALS_TRUE( message->matrix ( ) );
    TEST_EQUALS_VECTOR( *message->matrix ( ), sourceMatrix );
    TEST_EQUALS_TRUE( message->enumerations ( ) );
    TEST_EQUALS_VECTOR( *message->enumerations ( ), sourceEnums );

    // Test invalid (zero) sized integers
    message->setintegers ( emptyIntegers );
    TEST_THROWS_EXCEPTION( encoded = encode ( *message, "combinedflat_broken.dat" ), std::runtime_error );

    // Test invalid (over-) sized matrix
    message->setintegers ( sourceIntegers );
    sourceMatrix.push_back ( std::vector<float> ( 2 ) );
    message->setmatrix ( sourceMatrix );
    TEST_THROWS_EXCEPTION( encoded = encode ( *message, "combinedflat_broken.dat" ), std::runtime_error );
END_TEST

DEFINE_TEST_SUITE( FlatMessage )
    REGISTER_TEST( FlatOne )
    REGISTER_TEST( FlatTwo )
    REGISTER_TEST( CombinedFlatMessage )
END_TEST_SUITE
