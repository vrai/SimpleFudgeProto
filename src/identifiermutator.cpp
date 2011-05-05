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

#include "identifiermutator.hpp"
#include <sstream>

using namespace fudgeproto;

identifiermutator::identifiermutator ( )
{
}

identifiermutator::~identifiermutator ( )
{
    for ( std::map<std::string, node *>::iterator it ( m_nodes.begin ( ) ); it != m_nodes.end ( ); ++it )
        delete it->second;
}

size_t identifiermutator::add ( const identifier & id, const identifier & replacement )
{
    if ( id.size ( ) < 1 || id.size ( ) == 1 && id [ 0 ].empty ( ) )
        throw std::runtime_error ( "Identifier mutator cannot map a replacement to an empty Id" );

    node * current ( 0 );
    for ( size_t index ( 0 ); index < id.size ( ); ++index )
    {
        const std::string & element ( id [ index ] );
        std::map<std::string, node *> & nodes ( current ? current->nodes
                                                        : m_nodes );

        std::map<std::string, node *>::iterator it ( nodes.find ( element ) );
        if ( it == nodes.end ( ) )
        {
            nodes [ id [ index ] ] = current = new node ( element );
        }
        else
        {
            if ( ( current = it->second )->id )
                return index;
        }
    }

    if ( current )
    {
        current->id = replacement.clone ( );
        return std::string::npos;
    }
    else
        throw std::logic_error ( "Somehow ended up at the end of identifiermutator::add without a node" );
}

identifier * identifiermutator::mutatedClone ( const identifier & id ) const
{
    const std::pair<size_t, identifier *> result ( searchNodes ( id ) );
    if ( result.second )
    {
        identifier * clone ( 0 );
        if ( ! ( result.second->size ( ) == 1 && result.second->at ( 0 ).empty ( ) ) )
            clone = result.second->clone ( );

        for ( size_t index ( result.first ); index < id.size ( ); ++index )
        {
            if ( clone )
                clone->append ( id [ index ] );
            else
                clone = new identifier ( id [ index] );
        }

        return clone ? clone : new identifier ( "" );
    }
    else
        return id.clone ( );
}

std::pair<size_t, identifier *> identifiermutator::searchNodes ( const identifier & id ) const
{
    node * current ( 0 );
    size_t index ( 0 );
    for ( ; index < id.size ( ); ++index )
    {
        const std::string & element ( id [ index ] );
        const std::map<std::string, node *> & nodes ( current ? current->nodes
                                                              : m_nodes );
        std::map<std::string, node *>::const_iterator it ( nodes.find ( element ) );

        if ( it == nodes.end ( ) )
            break;

        current = it->second;
    }

    if ( current && current->id )
        return std::make_pair ( index, current->id );
    else
        return std::make_pair ( std::string::npos, static_cast<identifier *> ( 0 ) );
}

identifiermutator::node::node ( const std::string & _element )
    : element ( _element )
    , id ( 0 )
{
}

identifiermutator::node::~node ( )
{
    delete id;
    for ( std::map<std::string, node *>::iterator it ( nodes.begin ( ) ); it != nodes.end ( ); ++it )
        delete it->second;
}

