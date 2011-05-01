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

#include "astgenerator.hpp"
#include <fstream>
#include <iostream>
#include <stdexcept>

using namespace fudgeproto;

astgenerator::astgenerator ( const astextrefs & extrefs,
                             const astindex & index,
                             const codewriterfactory & factory,
                             const filenamegenerator & filenamegen )
    : m_extrefs ( extrefs )
    , m_index ( index )
    , m_factory ( factory )
    , m_filenamegen ( filenamegen )
{
}

void astgenerator::walkTopLevelMessage ( messagedef & node )
{
    std::ofstream output;
    output.exceptions ( std::ios::failbit | std::ios::badbit );

    if ( m_factory.hasHeaderFile ( ) )
    {
        output.open ( m_filenamegen.generate ( node, true, false ).c_str ( ) );
        m_writer.reset ( m_factory.headerWriter ( output ) );
        startFile ( node );
        walk ( node );
        endFile ( node );
        output.close ( );
    }

    output.open ( m_filenamegen.generate ( node, false, false ).c_str ( ) );
    m_writer.reset ( m_factory.implWriter ( output ) );
    startFile ( node );
    walk ( node );
    endFile ( node );
    output.close ( );
}

void astgenerator::walk ( enumdef & node )
{
    m_writer->enumDefinition ( node );
}

void astgenerator::walk ( fielddef & node )
{
    throw std::logic_error ( "Field walker not implemented in generator" );
}

void astgenerator::walk ( messagedef & node )
{
    m_writer->startClass ( node );
    walkCollection ( node.messages ( ) );
    walkCollection ( node.enums ( ) );
    m_writer->classFields ( node );
    m_writer->endClass ( node );
}

void astgenerator::walk ( namespacedef & node )
{
    if ( peekStack ( 1 ) )
        throw std::invalid_argument ( "AST generator only accepts a namespace at the top-level" );

    namespacedef & ns ( dynamic_cast<namespacedef &> ( node ) );
    for ( std::list<definition *>::const_iterator it ( ns.content ( ).begin ( ) );
          it != ns.content ( ).end ( );
          ++it )
    {
        if ( typeid ( **it ) != typeid ( messagedef ) )
            continue;
        messagedef & message ( dynamic_cast<messagedef &> ( **it ) );

        if ( ! message.isExtern ( ) )
            walkTopLevelMessage ( message );
    }
}

void astgenerator::startFile ( messagedef & node )
{
    m_writer->fileHeader ( node, m_filenamegen );
    includeExternal ( node );
    m_writer->includeStandard ( );

    refptr<identifier> ns ( node.id ( ).clone ( ) );
    ns->pop ( );
    m_writer->startNamespace ( *ns );
}

void astgenerator::endFile ( messagedef & node )
{
    refptr<identifier> ns ( node.id ( ).clone ( ) );
    ns->pop ( );
    m_writer->endNamespace ( *ns );

    m_writer->fileFooter ( node.id ( ) );
}

void astgenerator::includeExternal ( messagedef & node )
{
    astextrefs::stringset refids;
    m_extrefs.findAllrefs ( refids, node.idString ( ) );
    for ( astextrefs::stringsetcit it ( refids.begin ( ) ); it != refids.end ( ); ++it )
    {
        refptr<const definition> ref ( m_index.find ( *it ) );
        if ( ! ref )
            throw std::logic_error ( "Missing external reference \"" + *it + "\" in index" );
        if ( ! ref.istype<messagedef> ( ) )
            throw std::logic_error ( "Non-message external reference \"" + ref->idString ( ) + "\" in index" );

        m_writer->includeExternal ( dynamic_cast<const messagedef &> ( *ref ), m_filenamegen );
    }

    m_writer->endOfExternals ( refids.size ( ) );
}

