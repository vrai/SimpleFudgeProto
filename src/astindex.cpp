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

#include "astindex.hpp"
#include <typeinfo>

using namespace fudgeproto;

astindex::astindex ( )
{
}

astindex::~astindex ( )
{
    reset ( );
}

void astindex::load ( const definitionmap & source )
{
    for ( definitionmapcit it ( source.begin ( ) ); it != source.end ( ); ++it )
    {
        if (      typeid ( *it->second ) == typeid ( enumdef ) )    m_enums.insert ( *it );
        else if ( typeid ( *it->second ) == typeid ( messagedef ) ) m_messages.insert ( *it );
        else
            throw std::runtime_error ( "Index only accepts enum and messages" );

        refcounted::inc ( it->second );
    }
}

void astindex::reset ( )
{
    decrementMap ( m_enums );
    decrementMap ( m_messages );
}

const definition * astindex::find ( const std::string & id ) const
{
    definition * result ( 0 );

    definitionmapcit it ( m_enums.find ( id ) );
    if ( it != m_enums.end ( ) )
        result = it->second;
    else if ( ( it = m_messages.find ( id ) ) != m_messages.end ( ) )
        result = it->second;

    refcounted::inc ( result );
    return result;
}

void astindex::decrementMap ( definitionmap & map )
{
    for ( definitionmapit it ( map.begin ( ) ); it != map.end ( ); ++it )
        refcounted::dec ( it->second );
    map.clear ( );
}

