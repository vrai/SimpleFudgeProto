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

#include "astindexer.hpp"
#include <iostream>
#include <stdexcept>
#include <typeinfo>

using namespace fudgeproto;

astindexer::astindexer ( astindex & index )
    : m_index ( index )
{
}

void astindexer::walk ( definition * node )
{
    astwalker::walk ( node );

    if ( ! peekStack ( ) )
    {
        m_index.load ( m_types );
        checkFieldSet ( m_index );
    }
}

void astindexer::reset ( )
{
    astwalker::reset ( );
    astindex::decrementMap ( m_types );
    m_index.reset ( );
}

void astindexer::walk ( enumdef & node )
{
    std::pair<astindex::definitionmapit, bool> result ( m_types.insert ( std::make_pair ( node.idString ( ),
                                                                                          &node ) ) );
    if ( ! result.second )
        throw std::runtime_error ( "Cannot add enum \"" + node.idString ( ) + "\" - type name already used" );
    refcounted::inc ( &node );
}

void astindexer::walk ( fielddef & node )
{
    throw std::runtime_error ( "Field walker not implemented in indexer" );
}

void astindexer::walk ( messagedef & node )
{
    std::pair<astindex::definitionmapit, bool> result ( m_types.insert ( std::make_pair ( node.idString ( ),
                                                                                          &node ) ) );
    if ( result.second )
    {
        refcounted::inc ( &node );
    }
    else
    {
        if ( typeid ( *result.first->second ) != typeid ( messagedef ) )
            throw std::runtime_error ( "Cannot add message \"" + node.idString ( ) +
                                       "\" - type name used by a non-message" );

        // If the collision is a message and this is an extern, bail out now. The current node is of no better
        // than equal importance to that already in the map and cannot contain any child nodes.
        if ( node.isExtern ( ) )
            return;

        messagedef * collision ( dynamic_cast<messagedef *> ( result.first->second ) );
        if ( ! collision->isExtern ( ) )
            throw std::runtime_error ( "Cannot define message \"" + node.idString ( ) +
                                       "\" - message already defined" );

        // The current node is the definition while the collision is only an extern. Replace the node in the
        // map with the current one.
        refcounted::inc ( &node );
        refcounted::dec ( collision );
        result.first->second = &node;
    }

    walkCollection ( node.enums ( ) );
    walkCollection ( node.messages ( ) );
}

void astindexer::walk ( namespacedef & node )
{
    if ( peekStack ( 1 ) || node.hasId ( ) )
        throw std::runtime_error ( "AST indexer should only be passed an anonymous namespace at the top-level" );

    walkCollection ( node.content ( ) );
}

void astindexer::checkFieldSet ( const astindex & index )
{
    // Populate the message->field name/ordinal mappings and check for local collisions
    for ( astindex::definitionmapcit messageit ( index.messageMap ( ).begin ( ) ),
                                     messageend ( index.messageMap ( ).end ( ) );
          messageit != messageend;
          ++messageit )
    {
        checkFieldSet ( *dynamic_cast<messagedef *> ( messageit->second ) );
    }
}

void astindexer::checkFieldSet ( const messagedef & message )
{
    std::set<std::string> names;
    std::set<int> ordinals;

    for ( std::list<fielddef *>::const_iterator fieldit ( message.fields ( ).begin ( ) );
          fieldit != message.fields ( ).end ( );
          ++fieldit )
    {
        fielddef * field ( dynamic_cast<fielddef *> ( *fieldit ) );

        if ( ! names.insert ( field->idString ( ) ).second )
            throw std::runtime_error ( "Cannot add field \"" + field->idString ( ) + "\" to message \"" +
                                       message.idString ( ) + "\"; field name already in use" );

        if ( field->hasOrdinal ( ) &&
             ! ordinals.insert ( field->ordinal ( ) ).second )
            throw std::runtime_error ( "Cannot add field \"" + field->idString ( ) + "\" to message \"" +
                                       message.idString ( ) + "\"; field ordinal already in use" );
    }
}

