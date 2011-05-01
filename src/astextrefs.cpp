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

#include "astextrefs.hpp"
#include <deque>
#include <iostream>
#include <stdexcept>

using namespace fudgeproto;

void astextrefs::load ( const stringsetmap & source )
{
    m_refs = source;

    m_allrefs.clear ( );
    for ( stringsetmapcit it ( m_refs.begin ( ) ); it != m_refs.end ( ); ++it )
        walkMessage ( it->first );
}

void astextrefs::findAllrefs ( stringset & refs, const std::string & id ) const
{
    stringsetmapcit it ( m_allrefs.find ( id ) );
    if ( it == m_allrefs.end ( ) )
        refs.clear ( );
    else
        refs = it->second;
}

void astextrefs::walkMessage ( const std::string & toplevel )
{
    m_allrefs.insert ( std::make_pair ( toplevel, stringset ( ) ) );

    std::deque<std::string> queue;
    std::set<std::string> visited;
    queue.push_back ( toplevel );
    while ( ! queue.empty ( ) )
    {
        const std::string id ( queue.front ( ) );
        queue.pop_front ( );
        if ( ! visited.insert ( id ).second )
            continue;

        if ( id != toplevel )
            m_allrefs [ toplevel ].insert ( id );

        const std::set<std::string> & deps ( m_refs [ id ] );
        queue.insert ( queue.end ( ), deps.begin ( ), deps.end ( ) );
    }
}


