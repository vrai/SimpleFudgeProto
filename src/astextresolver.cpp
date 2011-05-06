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

#include "astextresolver.hpp"
#include <typeinfo>

using namespace fudgeproto;

astextresolver::astextresolver ( astextrefs & extrefs,
                                 const astindex & index )
    : m_extrefs ( extrefs )
    , m_index ( index )
{
}

void astextresolver::walk ( definition * node )
{
    astwalker::walk ( node );

    if ( ! peekStack ( ) )
        m_extrefs.load ( m_references );
}

void astextresolver::reset ( )
{
    astwalker::reset ( );
    m_references.clear ( );
}

void astextresolver::walk ( enumdef & node )
{
    throw std::domain_error ( "Enum walker not implemented in resolver" );
}

void astextresolver::walk ( fielddef & node )
{
    if ( node.type ( ).type ( ) == FUDGEPROTO_TYPE_USER )
        addExternalReference ( node.type ( ).def ( ), *peekStack ( 1 ) );
}

void astextresolver::walk ( messagedef & node )
{
    for ( size_t index ( 0 ); index < node.parents ( ).size ( ); ++index )
    {
        const std::string id ( node.parents ( ) [ index ]->asString ( "." ) );
        const definition * parent ( m_index.find ( id ) );
        if ( ! parent )
            throw std::logic_error ( "Cannot find parent \"" + id +
                                     "\" for message \"" + node.idString ( ) + "\"" );
        if ( typeid ( *parent ) != typeid ( messagedef ) )
            throw std::logic_error ( "Cannot use non-message \"" + id +
                                     "\" as parent of \"" + node.idString ( ) + "\"" );

        addExternalReference ( *parent, node );
    }

    walkCollection ( node.messages ( ) );
    walkCollection ( node.fields ( ) );
}

void astextresolver::walk ( namespacedef & node )
{
    if ( peekStack ( 1 ) || node.hasId ( ) )
        throw std::invalid_argument ( "AST extern resolver should only be passed an anonymous namespace at the top-level" );

    walkCollection ( node.content ( ) );
}

void astextresolver::addExternalReference ( const definition & target, const definition & message )
{
    // Can't be dependent on an enum - drop the enum elements, leaving the parent message
    identifier * id ( target.id ( ).clone ( ) );
    if ( typeid ( target ) == typeid ( enumdef ) )
        id->pop ( );

    // Find the message type corresponding to the identifier
    const definition * type ( m_index.find ( id->asString ( "." ) ) );
    if ( ! type )
        throw std::logic_error ( "Cannot add unknown external reference \"" + id->asString ( "." ) +
                                 "\" to message \"" + message.idString ( ) + "\"" );
    if ( typeid ( *type ) != typeid ( message ) )
        throw std::logic_error ( "Cannot add non-message reference \"" + type->idString ( ) +
                                 "\" to message \"" + message.idString ( ) + "\"" );

    // Only add messages that are at the top-level (i.e. not within another message)
    if ( ! isParentMessage ( *type ) )
    {
        m_references [ message.idString ( ) ].insert ( type->idString ( ) );
    }

    refcounted::dec ( type );
    refcounted::dec ( id );
}

bool astextresolver::isParentMessage ( const definition & type )
{
    identifier * parent ( type.id ( ).clone ( ) );
    parent->pop ( );
    const std::string parentid ( parent->asString ( "." ) );
    refcounted::dec ( parent );

    return  m_index.messageMap ( ).find ( parentid ) != m_index.messageMap ( ).end ( );
}

