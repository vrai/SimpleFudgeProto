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

#ifndef INC_FUDGEPROTO_ENCODERUTILS
#define INC_FUDGEPROTO_ENCODERUTILS

#include <cstdio>
#include <fudge/types.h>
#include <fudge-cpp/codec.hpp>
#include <memory>
#include <string>

namespace
{
    void dumpToFile ( const std::string & filename, const fudge_byte * bytes, fudge_i32 numbytes )
    {
        FILE * file ( fopen ( filename.c_str ( ), "wb" ) );
        if ( file )
        {
            fwrite ( bytes, 1, numbytes, file );
            fclose ( file );
        }
    }

    template<class T>
    std::pair<fudge_byte *, fudge_i32> encode ( const T & object, const std::string & name )
    {
        fudge_byte * bytes ( 0 );
        fudge_i32 numbytes ( 0 );
        fudge::codec ( ).encode ( fudge::envelope ( 0, 0, 0, object.asFudgeMessage ( ) ), bytes, numbytes );
        dumpToFile ( name, bytes, numbytes );
        return std::make_pair ( bytes, numbytes );
    }

    template<class T>
    T * decode ( const std::pair<fudge_byte *, fudge_i32> & source )
    {
        fudge::message result ( fudge::codec ( ).decode ( source.first, source.second ).payload ( ) );
        free ( source.first );

        std::auto_ptr<T> target ( new T );
        target->fromFudgeMessage ( result );
        return target.release ( );
    }
}

#endif

