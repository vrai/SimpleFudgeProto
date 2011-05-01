/**
 * Copyright (C) 2010 - 2011, Vrai Stacey.
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
#include "simpletest.hpp"
#include "memoryutil.hpp"
#include "parser.hpp"
#include "astrenamer.hpp"
#include "astflattener.hpp"
#include "astindexer.hpp"
#include "astresolver.hpp"
#include <memory>

using namespace fudgeproto;

DEFINE_TEST( Parsing )
    // Construct the post-parsing processes
    astindex index;
    astextrefs extrefs;
    std::auto_ptr<astwalker> renamer ( new astrenamer );
    std::auto_ptr<astflattener> flattener ( new astflattener );
    std::auto_ptr<astindexer> indexer ( new astindexer ( index ) );
    std::auto_ptr<astresolver> resolver ( new astresolver ( extrefs, index ) );

    // Test flat messages
    refptr<namespacedef> root;
    TEST_THROWS_NOTHING( root = parser::parse ( "./test_files/flat.proto" ) );
    TEST_THROWS_NOTHING( renamer->walk ( root.get ( ) ) );
    TEST_THROWS_NOTHING( flattener->walk ( root.get ( ) ) );
    TEST_THROWS_NOTHING( indexer->walk ( root.get ( ) ) );
    TEST_EQUALS_INT( index.numEnums ( ), 1 );
    TEST_EQUALS_INT( index.numMessages ( ), 3 );
    TEST_THROWS_NOTHING( resolver->walk ( root.get ( ) ) );
    TEST_EQUALS_INT( extrefs.allrefs ( ).size ( ), 3 );

    TEST_THROWS_NOTHING( renamer->reset ( ) );
    TEST_THROWS_NOTHING( flattener->reset ( ) );
    TEST_THROWS_NOTHING( indexer->reset ( ) );
    TEST_THROWS_NOTHING( resolver->reset ( ) );

    // Test nested messages
    TEST_THROWS_NOTHING( root = parser::parse ( "./test_files/nested.proto" ) );
    TEST_THROWS_NOTHING( renamer->walk ( root.get ( ) ) );
    TEST_THROWS_NOTHING( flattener->walk ( root.get ( ) ) );
    TEST_THROWS_NOTHING( indexer->walk ( root.get ( ) ) );
    TEST_EQUALS_INT( index.numEnums ( ), 0 );
    TEST_EQUALS_INT( index.numMessages ( ), 4 );
    TEST_THROWS_NOTHING( resolver->walk ( root.get ( ) ) );
    TEST_EQUALS_INT( extrefs.allrefs ( ).size ( ), 3 );

    TEST_THROWS_NOTHING( renamer->reset ( ) );
    TEST_THROWS_NOTHING( flattener->reset ( ) );
    TEST_THROWS_NOTHING( indexer->reset ( ) );
    TEST_THROWS_NOTHING( resolver->reset ( ) );

    // Test identical, but non-clashing names
    TEST_THROWS_NOTHING( root = parser::parse ( "./test_files/field_notclash.proto" ) );
    TEST_THROWS_NOTHING( renamer->walk ( root.get ( ) ) );
    TEST_THROWS_NOTHING( flattener->walk ( root.get ( ) ) );
    TEST_THROWS_NOTHING( indexer->walk ( root.get ( ) ) );
    TEST_EQUALS_INT( index.numEnums ( ), 0 );
    TEST_EQUALS_INT( index.numMessages ( ), 2 );
    TEST_THROWS_NOTHING( resolver->walk ( root.get ( ) ) );
    TEST_EQUALS_INT( extrefs.allrefs ( ).size ( ), 1 );
END_TEST

DEFINE_TEST( ProcessingFailures )
    // Construct the post-parsing processes
    astindex index;
    astextrefs extrefs;
    std::auto_ptr<astwalker> renamer ( new astrenamer );
    std::auto_ptr<astflattener> flattener ( new astflattener );
    std::auto_ptr<astindexer> indexer ( new astindexer ( index ) );
    std::auto_ptr<astresolver> resolver ( new astresolver ( extrefs, index ) );

    // Zero ordinals are verboten
    refptr<namespacedef> root;
    TEST_THROWS_NOTHING( root = parser::parse ( "./test_files/zero_ordinal.proto" ) );
    TEST_THROWS_NOTHING( renamer->walk ( root.get ( ) ) );
    TEST_THROWS_NOTHING( flattener->walk ( root.get ( ) ) );
    TEST_THROWS_NOTHING( indexer->walk ( root.get ( ) ) );
    TEST_EQUALS_INT( index.numEnums ( ), 0 );
    TEST_EQUALS_INT( index.numMessages ( ), 1 );
    TEST_THROWS_EXCEPTION( resolver->walk ( root.get ( ) ), std::runtime_error );

    TEST_THROWS_NOTHING( renamer->reset ( ) );
    TEST_THROWS_NOTHING( flattener->reset ( ) );
    TEST_THROWS_NOTHING( indexer->reset ( ) );
    TEST_THROWS_NOTHING( resolver->reset ( ) );

    // Default values must be compatible with the field type
    TEST_THROWS_EXCEPTION( root = parser::parse ( "./test_files/wrong_default_type.proto" ), std::runtime_error );

    // Field names must be unique
    TEST_THROWS_NOTHING( root = parser::parse ( "./test_files/field_clash.proto" ) );
    TEST_THROWS_NOTHING( renamer->walk ( root.get ( ) ) );
    TEST_THROWS_NOTHING( flattener->walk ( root.get ( ) ) );
    TEST_THROWS_EXCEPTION( indexer->walk ( root.get ( ) ), std::runtime_error );

    TEST_THROWS_NOTHING( renamer->reset ( ) );
    TEST_THROWS_NOTHING( flattener->reset ( ) );
    TEST_THROWS_NOTHING( indexer->reset ( ) );

    // Field ordinals must be unique
    TEST_THROWS_NOTHING( root = parser::parse ( "./test_files/ordinal_clash.proto" ) );
    TEST_THROWS_NOTHING( renamer->walk ( root.get ( ) ) );
    TEST_THROWS_NOTHING( flattener->walk ( root.get ( ) ) );
    TEST_THROWS_EXCEPTION( indexer->walk ( root.get ( ) ), std::runtime_error );

    // Field modifiers must not clash
    TEST_THROWS_EXCEPTION( root = parser::parse ( "./test_files/clashing_modifiers.proto" ), std::runtime_error );

    // Field modifiers must be valid
    TEST_THROWS_EXCEPTION( root = parser::parse ( "./test_files/invalid_modifier.proto" ), std::runtime_error );

END_TEST

DEFINE_TEST_SUITE( Parser )
    REGISTER_TEST( Parsing )
    REGISTER_TEST( ProcessingFailures )
END_TEST_SUITE

