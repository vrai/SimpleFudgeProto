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

#ifndef INC_FUDGEPROTO_ASTINDEX
#define INC_FUDGEPROTO_ASTINDEX

#include "ast.hpp"
#include <set>

namespace fudgeproto {

class astindex
{
    public:
        typedef std::map<std::string, definition *> definitionmap;
        typedef definitionmap::iterator definitionmapit;
        typedef definitionmap::const_iterator definitionmapcit;

        astindex ( );
        ~astindex ( );

        void load ( const definitionmap & source );
        void reset ( );

        const definition * find ( const std::string & id ) const;

        inline size_t numEnums ( ) const { return m_enums.size ( ); }
        inline size_t numMessages ( ) const { return m_messages.size ( ); }

        inline const definitionmap & enumMap ( ) const { return m_enums; }
        inline const definitionmap & messageMap ( ) const { return m_messages; }

        static void decrementMap ( definitionmap & map );

    private:
        definitionmap m_enums,
                      m_messages;
};

}

#endif

