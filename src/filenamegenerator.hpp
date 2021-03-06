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

#ifndef INC_FUDGEPROTO_FILENAMEGENERATOR
#define INC_FUDGEPROTO_FILENAMEGENERATOR

#include "ast.hpp"

namespace fudgeproto {

class filenamegenerator
{
    public:
        filenamegenerator ( const std::string & path,
                            const std::string & headerext,
                            const std::string & implext,
                            bool lowercase );

        std::string generate ( const std::string & name,
                               bool header,
                               bool local ) const;
        std::string generate ( const messagedef & message,
                               bool header,
                               bool local ) const;

    private:
        std::string m_path,
                    m_headerext,
                    m_implext;
        bool m_lowercase;

        static const std::string s_pathsep,
                                 s_extsep;
};

}

#endif

