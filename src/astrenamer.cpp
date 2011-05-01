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

#include "astrenamer.hpp"
#include <iostream>
#include <stdexcept>
#include <typeinfo>

using namespace fudgeproto;

void astrenamer::walk ( enumdef & node )
{
    rename ( node );
}

void astrenamer::walk ( fielddef & node )
{
    throw std::runtime_error ( "Field walker not implemented in renamer" );
}

void astrenamer::walk ( messagedef & node )
{
    walkCollection ( node.enums ( ) );
    walkCollection ( node.messages ( ) );
    
    rename ( node );
}

void astrenamer::walk ( namespacedef & node )
{
    walkCollection ( node.content ( ) );
}

void astrenamer::rename ( definition & node )
{
    identifier * ns ( currentNamespace ( ) );
    if ( ! ns )
        throw std::runtime_error ( "Attempt to rename \"" + node.idString ( ) + "\" failed" );
    node.resetIdentifier ( ns );
    refcounted::dec ( ns );
}

identifier * astrenamer::currentNamespace ( ) const
{
    identifier * ns ( 0 );
    for ( size_t index ( 0 ); index < stackSize ( ); ++index )
    {
        definition * stack (  peekStack ( stackSize ( ) - ( index + 1 ) ) );
        if ( ! stack->hasId ( ) )
            continue;

        if ( ns )
            ns->extend ( stack->id ( ) );
        else
            ns = stack->id ( ).clone ( );
    }

    return ns;
}

