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

#include "stage.hpp"
#include <iostream>

using namespace fudgeproto;

Stage::Stage ( fudgeproto::astwalker * walker,
               fudgeproto::astwalker & dumper,
               const std::string & name )
    : m_walker ( walker )
    , m_dumper ( dumper )
    , m_name ( name )
{
}

Stage::~Stage ( )
{
    delete m_walker;
}

void Stage::run ( fudgeproto::refptr<fudgeproto::namespacedef> root,
                  bool verbose )
{
    m_walker->walk ( root.get ( ) );

    if ( verbose )
    {
        std::cout << "--- POST " << m_name << " ---" << std::endl;
        dump ( root );
    }
}

void Stage::dump ( fudgeproto::refptr<fudgeproto::namespacedef> root )
{
    defaultDump ( m_dumper, root );
}

IndexStage::IndexStage ( fudgeproto::astwalker * walker,
                         fudgeproto::astwalker & dumper,
                         const fudgeproto::astindex & index )
    : Stage ( walker, dumper, "TYPE INDEXING STAGE" )
    , m_index ( index )
{
}

void IndexStage::dump ( fudgeproto::refptr<fudgeproto::namespacedef> root )
{
    std::cout << "Index enums:" << std::endl << fudgeproto::astdumper::dumpIndex ( m_index.enumMap ( ) ) << std::endl
              << "Index messages:" << std::endl << fudgeproto::astdumper::dumpIndex ( m_index.messageMap ( ) ) << std::endl;
}

ResolverStage::ResolverStage ( fudgeproto::astwalker * walker,
                               fudgeproto::astwalker & dumper,
                               const fudgeproto::astextrefs & extrefs )
    : Stage ( walker, dumper, "EXTERNAL TYPE RESOLVER STAGE" )
    , m_extrefs ( extrefs )
{
}

void ResolverStage::dump ( fudgeproto::refptr<fudgeproto::namespacedef> root )
{
    Stage::dump ( root );

    std::cout << "External references:" << std::endl;
    for ( fudgeproto::astextrefs::stringsetmapcit it ( m_extrefs.allrefs ( ).begin ( ) );
          it != m_extrefs.allrefs ( ).end ( );
          ++it )
        if ( ! it->second.empty ( ) )
        {
            std::cout << "    " << it->first << std::endl;
            for ( fudgeproto::astextrefs::stringsetcit dep ( it->second.begin ( ) );
                  dep != it->second.end ( );
                  ++dep )
                std::cout << "     <- " << *dep << std::endl;
        }
    std::cout << std::endl;
}

