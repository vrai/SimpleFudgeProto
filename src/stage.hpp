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

#ifndef INC_FUDGEPROTO_STAGE
#define INC_FUDGEPROTO_STAGE

#include "astdumper.hpp"
#include "astextrefs.hpp"
#include "astindexer.hpp"

namespace fudgeproto {

class Stage
{
    public:
        Stage ( fudgeproto::astwalker * walker,
                fudgeproto::astwalker & dumper,
                const std::string & name );
        virtual ~Stage ( );

        void run ( fudgeproto::refptr<fudgeproto::namespacedef> root,
                   bool verbose = false );

        inline static void destroy ( Stage * stage ) { delete stage; }

        inline static void defaultDump ( fudgeproto::astwalker & dumper,
                                         fudgeproto::refptr<fudgeproto::namespacedef> root )
        {
            dumper.walk ( root.get ( ) );
        }

    protected:
        virtual void dump ( fudgeproto::refptr<fudgeproto::namespacedef> root );

    private:
        fudgeproto::astwalker * m_walker;
        fudgeproto::astwalker & m_dumper;
        std::string m_name;
};

class IndexStage : public Stage
{
    public:
        IndexStage ( fudgeproto::astwalker * walker,
                     fudgeproto::astwalker & dumper,
                     const fudgeproto::astindex & index );

    private:
        const fudgeproto::astindex & m_index;

        void dump ( fudgeproto::refptr<fudgeproto::namespacedef> root );
};

class ResolverStage : public Stage
{
    public:
        ResolverStage ( fudgeproto::astwalker * walker,
                        fudgeproto::astwalker & dumper,
                        const fudgeproto::astextrefs & extrefs );

    private:
        const fudgeproto::astextrefs & m_extrefs;

        void dump ( fudgeproto::refptr<fudgeproto::namespacedef> root );
};

}

#endif

