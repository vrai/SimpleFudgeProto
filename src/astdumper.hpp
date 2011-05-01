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

#ifndef INC_FUDGEPROTO_ASTDUMPER
#define INC_FUDGEPROTO_ASTDUMPER

#include "astwalker.hpp"
#include "astindex.hpp"
#include <ostream>

namespace fudgeproto {

class astdumper : public astwalker
{
    public:
        astdumper ( std::ostream & output );

        static std::string dumpIndex ( const astindex::definitionmap & index );

    private:
        std::ostream & m_output;
        size_t m_depth;

        void walk ( enumdef & node );
        void walk ( fielddef & node );
        void walk ( messagedef & node );
        void walk ( namespacedef & node );

        std::string typeAsString ( const fieldtype & type );
        std::string modifierAsString ( fudgeproto_modifier modifier );
        std::string constraintAsString ( const std::vector<int> & constraint );
        std::string literalAsString ( const literalvalue & value );

        std::string indent ( ) const;
};

}

#endif

