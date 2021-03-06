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

#ifndef INC_FUDGEPROTO_CODEWRITERFACTORY
#define INC_FUDGEPROTO_CODEWRITERFACTORY

#include "codewriter.hpp"
#include <ostream>

namespace fudgeproto {

class codewriterfactory
{
    public:
        virtual ~codewriterfactory ( );

        virtual bool hasHeaderFile ( ) const = 0;
        virtual codewriter * headerWriter ( std::ostream & output ) const = 0;
        virtual codewriter * implWriter ( std::ostream & output ) const = 0;

        void setUnsafe ( bool unsafe );

    protected:
        codewriterfactory ( );

        bool m_unsafe;
};

}

#endif

