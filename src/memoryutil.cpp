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

#include "memoryutil.hpp"
#include <cstring>
#include <iostream>

namespace fudgeproto {

char * strdup ( const char * string, size_t numchars )
{
    char * newstring ( new char [ numchars + 1 ] );
    memcpy ( newstring, string, numchars );
    newstring [ numchars ] = 0;
    return newstring;
}

char * strdup ( const char * string )
{
    return strdup ( string, strlen ( string ) );
}

refcounted::refcounted ( )
    : m_refcount ( new refcount_type )
{
    *m_refcount = 1;
}

refcounted::~refcounted ( )
{
    delete m_refcount;
}

void refcounted::increment ( ) const
{
    ( *m_refcount ) += 1;
}

bool refcounted::decrement ( ) const
{
    return ( ( *m_refcount ) -= 1 ) < 1;
}

}

