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

#include "astflattener.hpp"
#include <stdexcept>
#include <typeinfo>

using namespace fudgeproto;

namespace
{
    bool isDefinitionNamespace ( const definition & def )
    {
        return typeid ( def ) == typeid ( namespacedef );
    }
}

void astflattener::walk ( definition * node )
{
    if ( node && ( node->hasId ( ) ^ peekStack ( ) != 0 ) )
        throw std::runtime_error ( "Cannot flatten an id-less node below the top-level" );

    astwalker::walk ( node );
}

void astflattener::walk ( enumdef & node )
{
    throw std::runtime_error ( "Enum walker not implemented in flattener" );
}

void astflattener::walk ( fielddef & node )
{
    throw std::runtime_error ( "Field walker not implemented in flattener" );
}

void astflattener::walk ( messagedef & node )
{
    // Do nothing - only namespace important here
}

void astflattener::walk ( namespacedef & node )
{
    walkCollection ( node.content ( ) );

    node.removeContentIf ( isDefinitionNamespace );
    
    definition * parent ( peekStack ( 1 ) );
    if ( parent )
        for ( std::list<definition *>::const_iterator it ( node.content ( ).begin ( ) );
              it != node.content ( ).end ( );
              ++it )
            parent->addContent ( *it );
}

bool astflattener::isParent ( definition & node )
{
    return ( ( typeid ( node ) == typeid ( messagedef ) ) ||
             ( typeid ( node ) == typeid ( namespacedef ) && ! node.hasId ( ) ) );
}

