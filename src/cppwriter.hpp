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

#ifndef INC_FUDGEPROTO_CPPWRITER
#define INC_FUDGEPROTO_CPPWRITER

#include "codewriter.hpp"

namespace fudgeproto {

class cppwriter : public codewriter
{
    public:
        cppwriter ( std::ostream & output, bool unsafe );

        static const std::string s_indent;

    protected:
        size_t m_depth;

        const std::string & getIdLeaf ( const definition & def );

        std::string generateTypeName ( const fieldtype & type );
        std::string generateIndent ( ) const;
        std::string generateHeader ( ) const;
        std::string generateIdString ( const identifier & id );
        std::string generateMemberName ( const fielddef & field );
        std::string generateTrueType ( const fielddef & field );
        std::string generateMemberType ( const fielddef & field );
        std::string generateArgType ( const fielddef & field );

        std::string escapeString ( const std::string & string );

    private:
        std::string generateStorageType ( const fielddef & field );
};

}

#endif

