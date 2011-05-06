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

#include "astaliaser.hpp"

using namespace fudgeproto;

astaliaser::astaliaser ( astindex & index,
                         identifiermutator & mutator )
    : m_index ( index )
    , m_mutator ( mutator )
{
}

void astaliaser::walk ( definition * node )
{
    astwalker::walk ( node );
}


void astaliaser::walk ( enumdef & node )
{
    rename ( &node );
}

void astaliaser::walk ( fielddef & node )
{
    if ( node.type ( ).type ( ) == FUDGEPROTO_TYPE_USER )
        node.type ( ).resetIdentifier ( node.type ( ).def ( ).id ( ).clone ( ) );
}

void astaliaser::walk ( messagedef & node )
{
    node.saveOriginalId ( );
    rename ( &node );

    for ( size_t index ( 0 ), count ( node.parents ( ).size ( ) ); index < count; ++index )
    {
        const identifier * parent ( node.parents ( ) [ index ] );
        refptr<identifier> newid ( m_mutator.mutatedCloneStem ( *parent ) );
        if ( ! newid->equals ( *parent ) )
            node.replaceParent ( index, newid.get ( ) );
    }

    walkCollection ( node.enums ( ) );
    m_messages.push_back ( &node );
}

void astaliaser::walk ( namespacedef & node )
{
    if ( peekStack ( 1 ) || node.hasId ( ) )
        throw std::runtime_error ( "AST aliaser should only be passed an anonymous namespace at the top-level" );

    // Top-level - clear out state
    m_messages.clear ( );

    walkCollection ( node.content ( ) );

    // Tree/index is now internally consistent, update fields
    std::for_each ( m_messages.begin ( ),
                    m_messages.end ( ),
                    std::bind1st ( std::mem_fun( &astaliaser::renameFields ), this ) );

    m_messages.clear ( );
}

void astaliaser::rename ( definition * node )
{
    refptr<identifier> newid ( m_mutator.mutatedCloneStem ( node->id ( ) ) );
    if ( ! newid->equals ( node->id ( ) ) )
    {
        const std::string oldid ( node->idString ( ) );
        node->resetIdentifier ( newid.release ( ) );
        m_index.replace ( oldid, node );
    }
}

void astaliaser::renameFields ( messagedef * node )
{
    walkCollection ( node->fields ( ) );
}

