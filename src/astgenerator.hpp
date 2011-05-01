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

#ifndef INC_FUDGEPROTO_ASTGENERATOR
#define INC_FUDGEPROTO_ASTGENERATOR

#include "astextrefs.hpp"
#include "astindex.hpp"
#include "astwalker.hpp"
#include "filenamegenerator.hpp"
#include "codewriterfactory.hpp"
#include <memory>

namespace fudgeproto {

class astgenerator : public astwalker
{
    public:
        astgenerator ( const astextrefs & extrefs,
                       const astindex & index,
                       const codewriterfactory & factory,
                       const filenamegenerator & filenamegen );

    private:
        const astextrefs & m_extrefs;
        const astindex & m_index;
        const codewriterfactory & m_factory;
        const filenamegenerator & m_filenamegen;

        std::auto_ptr<codewriter> m_writer;

        void walkTopLevelMessage ( messagedef & node );

        void walk ( enumdef & node );
        void walk ( fielddef & node );
        void walk ( messagedef & node );
        void walk ( namespacedef & node );

        void startFile ( messagedef & node );
        void endFile ( messagedef & node );

        void includeExternal ( messagedef & node );
};

}

#endif

