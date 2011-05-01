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

#ifndef INC_FUDGEPROTO_ASTRENAMER
#define INC_FUDGEPROTO_ASTRENAMER

#include "astwalker.hpp"

namespace fudgeproto {

class astrenamer : public astwalker
{
    private:
        void walk ( enumdef & node );
        void walk ( fielddef & node );
        void walk ( messagedef & node );
        void walk ( namespacedef & node );

        void rename ( definition & node );

        identifier * currentNamespace ( ) const;
};

}

#endif

