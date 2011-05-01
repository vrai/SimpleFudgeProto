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

#include "filenamegenerator.hpp"
#include <algorithm>
#include <ctype.h>

using namespace fudgeproto;

const std::string filenamegenerator::s_pathsep ( "/" ),
                  filenamegenerator::s_extsep ( "." );

filenamegenerator::filenamegenerator ( const std::string & path,
                                       const std::string & headerext,
                                       const std::string & implext,
                                       bool lowercase )
    : m_path ( path )
    , m_headerext ( headerext )
    , m_implext ( implext )
    , m_lowercase ( lowercase )
{
}

std::string filenamegenerator::generate ( const std::string & name,
                                          bool header,
                                          bool local ) const
{
    std::string converted ( name );
    if ( m_lowercase )
        std::transform ( converted.begin ( ), converted.end ( ), converted.begin ( ), tolower );

    std::string path ( converted + s_extsep + ( header ? m_headerext : m_implext ) );
    if ( ! local )
        path = m_path + s_pathsep + path;
    return path;
}

std::string filenamegenerator::generate ( const messagedef & message,
                                          bool header,
                                          bool local ) const
{
    return generate ( message.id ( ).asString ( "_" ), header, local );
}

