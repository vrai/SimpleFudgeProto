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

#ifndef INC_FUDGEPROTO_CPPHEADERWRITER
#define INC_FUDGEPROTO_CPPHEADERWRITER

#include "cppwriter.hpp"

namespace fudgeproto {

class cppheaderwriter : public cppwriter
{
    public:
        cppheaderwriter ( std::ostream & output, bool unsafe );

        void fileHeader ( const messagedef & ref,
                          const filenamegenerator & filenamegen );
        void fileFooter ( const identifier & id );
        void includeExternal ( const messagedef & ref,
                               const filenamegenerator & filenamegen );
        void endOfExternals ( size_t count );
        void includeStandard ( );
        void startNamespace ( const identifier & ns );
        void endNamespace ( const identifier & ns );
        void enumDefinition ( const enumdef & def );
        void startClass ( const messagedef & message );
        void endClass ( const messagedef & message );
        void classFields ( const messagedef & message );

    private:
        void outputFieldGetter ( const fielddef * field );
        void outputFieldSetter ( const fielddef * field );
        void outputMemberDef ( const fielddef * field );

        std::string generateGuardSymbol ( const identifier & id ) const;
        std::string generateInArgType ( const fielddef & field );
};

}

#endif

