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

#ifndef INC_FUDGEPROTO_IDENTIFIERMUTATOR
#define INC_FUDGEPROTO_IDENTIFIERMUTATOR

#include "ast.hpp"

namespace fudgeproto {

class identifiermutator
{
    public:
        identifiermutator ( );
        ~identifiermutator ( );

        size_t add ( const identifier & id, const identifier & replacement );
        identifier * mutatedClone ( const identifier & id ) const;
        identifier * mutatedCloneStem ( const identifier & id ) const;

    private:
        struct node
        {
            node ( const std::string & element );
            ~node ( );

            std::string element;
            std::map<std::string, node *> nodes;
            identifier * id;
        };

        std::map<std::string, node *> m_nodes;

        std::pair<size_t, identifier *> searchNodes ( const identifier & id ) const;
};

}

#endif

