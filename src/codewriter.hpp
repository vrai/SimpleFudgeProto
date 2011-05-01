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

#ifndef INC_FUDGEPROTO_CODEWRITER
#define INC_FUDGEPROTO_CODEWRITER

#include "filenamegenerator.hpp"
#include <ostream>

namespace fudgeproto {

class codewriter
{
    public:
        codewriter ( std::ostream & output );
        virtual ~codewriter ( );

        virtual void fileHeader ( const messagedef & ref,
                                  const filenamegenerator & filenamegen ) = 0;
        virtual void fileFooter ( const identifier & id ) = 0;
        virtual void includeExternal ( const messagedef & ref,
                                       const filenamegenerator & filenamegen ) = 0;
        virtual void endOfExternals ( size_t count ) = 0;
        virtual void includeStandard ( ) = 0;
        virtual void startNamespace ( const identifier & ns ) = 0;
        virtual void endNamespace ( const identifier & ns ) = 0;
        virtual void enumDefinition ( const enumdef & def ) = 0;
        virtual void startClass ( const messagedef & message ) = 0;
        virtual void endClass ( const messagedef & message ) = 0;
        virtual void classFields ( const messagedef & message ) = 0;

    protected:
        std::ostream & m_output;
};

}

#endif

