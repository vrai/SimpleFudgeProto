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

#ifndef INC_FUDGEPROTO_ASTINDEXER
#define INC_FUDGEPROTO_ASTINDEXER

#include "astindex.hpp"
#include "astwalker.hpp"
#include <set>

namespace fudgeproto {

class astindexer : public astwalker
{
    public:
        astindexer ( astindex & index );

        void walk ( definition * node );
        void reset ( );

    private:
        astindex & m_index;
        astindex::definitionmap m_types;

        void walk ( enumdef & node );
        void walk ( fielddef & node );
        void walk ( messagedef & node );
        void walk ( namespacedef & node );

        void checkFieldSet ( const astindex & index );
        void checkFieldSet ( const messagedef & message );
};

}

#endif

