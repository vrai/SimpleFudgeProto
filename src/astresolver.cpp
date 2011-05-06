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

#include "astresolver.hpp"
#include <iostream>
#include <typeinfo>

using namespace fudgeproto;

astresolver::astresolver ( const astindex & index )
    : m_index ( index )
{
}

void astresolver::walk ( definition * node )
{
    astwalker::walk ( node );
}

void astresolver::walk ( enumdef & node )
{
    throw std::domain_error ( "Enum walker not implemented in resolver" );
}

void astresolver::walk ( fielddef & node )
{
    // Make sure the ordinal is non-zero
    if ( node.hasOrdinal ( ) && node.ordinal ( ) < 1 )
        throw std::runtime_error ( "Field \"" + node.idString ( ) + "\" in message \"" +
                                   peekStack ( 1 )->idString ( ) + "\" has an invalid ordinal "
                                   "must be one or greater" );

    // Don't need to resolve non-user types
    if ( node.type ( ).type ( ) != FUDGEPROTO_TYPE_USER )
        return;

    // User type - resolve the type and replace the type with the full version
    const definition * type ( findType ( node.type ( ).name ( ), peekStack ( 1 ) ) );
    if ( ! type )
        throw std::runtime_error ( "Cannot find field type \"" + node.type ( ).name ( ).asString ( "." ) +
                                   "\" for field \"" + node.idString ( ) + "\" in message \"" +
                                   peekStack ( 1 )->idString ( ) + "\"" );
    node.type ( ).resetIdentifier ( &type->id ( ) );
    node.type ( ).setDefinition ( type );
    refcounted::dec ( type );
}

void astresolver::walk ( messagedef & node )
{
    std::vector<const definition *> parents;
    try
    {
        // Populate the parents vector with resolved versions of the node's parents
        parents.reserve ( node.parents ( ).size ( ) );
        std::transform ( node.parents ( ).begin ( ),
                         node.parents ( ).end ( ),
                         std::back_inserter ( parents ),
                         std::bind1st ( std::mem_fun ( &astresolver::walkParent ), this ) );
    }
    catch ( ... )
    {
        std::for_each ( parents.begin ( ), parents.end ( ), refcounted::dec );
        throw;
    }

    // No failures - can safely process the resolved parents locally
    for ( size_t index ( 0 ); index < parents.size ( ); ++index )
    {
        node.replaceParent ( index, &( parents [ index ]->id ( ) ) );
        refcounted::dec ( parents [ index ] );
    }

    walkCollection ( node.messages ( ) );
    walkCollection ( node.fields ( ) );
}

void astresolver::walk ( namespacedef & node )
{
    if ( peekStack ( 1 ) || node.hasId ( ) )
        throw std::invalid_argument ( "AST resolver should only be passed an anonymous namespace at the top-level" );
    walkCollection ( node.content ( ) );
}

const definition * astresolver::walkParent ( const identifier * id )
{
    // Resolve the parent type and make sure it's a message (messages cannot extend enums!)
    const definition * type ( findType ( *id, peekStack ( ) ) );
    if ( ! type )
        throw std::runtime_error ( "Cannot find parent type \"" + id->asString ( "." ) + "\" for message \"" +
                                   peekStack ( )->idString ( ) + "\"" );
    if ( typeid ( *type ) != typeid ( messagedef ) )
        throw std::runtime_error ( "Parent type \"" + id->asString ( "." ) + "\" of message \"" +
                                   peekStack ( )->idString ( ) + "\" is not a message" );
    return type;
}

const definition * astresolver::findType ( const identifier & type, const definition * scope )
{
    if ( ! scope )
        throw std::invalid_argument ( "AST resolver cannot find type in a NULL scope" );

    const definition * result ( m_index.find ( type.asString ( "." ) ) );
    if ( result )
        return result;

    identifier * id ( scope->id ( ).clone ( ) );
    for ( ; id->size ( ) && ! result; id->pop ( ) )
    {
        identifier * copy ( id->clone ( ) );
        copy->extend ( type );
        result = m_index.find ( copy->asString ( "." ) );
        refcounted::dec ( copy );
    }
    refcounted::dec ( id );
    return result;
}

