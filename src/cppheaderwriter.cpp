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

#include "cppheaderwriter.hpp"
#include <algorithm>
#include <ctype.h>
#include <iostream>

using namespace fudgeproto;

cppheaderwriter::cppheaderwriter ( std::ostream & output )
    : cppwriter ( output )
{
}

void cppheaderwriter::fileHeader ( const messagedef & ref,
                                   const filenamegenerator & )
{
    m_output << generateHeader ( ) << std::endl;

    const std::string guard ( generateGuardSymbol ( ref.id ( ) ) );
    m_output << "#ifndef " << guard << std::endl
             << "#define " << guard << std::endl
             << std::endl;
}

void cppheaderwriter::fileFooter ( const identifier & id )
{
    m_output << std::endl
             << "#endif    // " << generateGuardSymbol ( id ) << std::endl;
}

void cppheaderwriter::includeExternal ( const messagedef & ref,
                                        const filenamegenerator & filenamegen )
{
    m_output << "#include \"" << filenamegen.generate ( ref, true, true ) << "\"" << std::endl;
}

void cppheaderwriter::endOfExternals ( size_t count )
{
    if ( count ) m_output << std::endl;
}

void cppheaderwriter::includeStandard ( )
{
    m_output << "#include <fudge-cpp/codec.hpp>" << std::endl
             << "#include <fudge-cpp/fudge.hpp>" << std::endl
             << "#include <vector>"              << std::endl
             << std::endl;
}

void cppheaderwriter::startNamespace ( const identifier & ns )
{
    for ( size_t index ( 0 ); index < ns.size ( ); ++index )
        m_output << ( index ? " " : "" ) << "namespace " << ns [ index ] << " {";
    m_output << std::endl << std::endl;
}

void cppheaderwriter::endNamespace ( const identifier & ns )
{
    for ( size_t index ( 0 ); index < ns.size ( ); ++index )
        m_output << ( index ? " " : "" ) << "}";
    m_output << std::endl;
}

void cppheaderwriter::enumDefinition ( const enumdef & def )
{
    const std::string outerIndent ( generateIndent ( ) );
    m_output << outerIndent << "enum " << def.id ( ) [ def.id ( ).size ( ) - 1 ] << std::endl
             << outerIndent << "{" << std::endl;
    ++m_depth;

    const std::string innerIndent ( generateIndent ( ) );
    for ( size_t index ( 0 ); index < def.size ( ); ++index )
        m_output << innerIndent << def [ index ].first << " = "
                                << def [ index ].second << "," << std::endl;

    --m_depth;
    m_output << outerIndent << "};" << std::endl
             << std::endl;
}

void cppheaderwriter::startClass ( const messagedef & message )
{
    // Output class name and inherited types
    std::string indent ( generateIndent ( ) );
    m_output << indent << "class " << getIdLeaf ( message ) << std::endl;

    if ( ! message.parents ( ).empty ( ) )
        for ( size_t index ( 0 ); index < message.parents ( ).size ( ); ++index )
            m_output << indent << s_indent << ( index ? "," : ":" ) << " public "
                     << generateIdString ( *message.parents ( ) [ index ] )
                     << std::endl;

    m_output << indent << "{" << std::endl;
    ++m_depth;

    // First section of the class will be public visibility
    m_output << generateIndent ( ) << "public:" << std::endl;
    ++m_depth;

    // Start the public section with the constructor/destructor declarations
}

void cppheaderwriter::endClass ( const messagedef & message )
{
    m_depth -= 2;
    m_output << generateIndent ( ) << "};" << std::endl
             << std::endl;
}

void cppheaderwriter::classFields ( const messagedef & message )
{
    // Output constructors
    const std::string & name ( getIdLeaf ( message ) );
    std::string indent ( generateIndent ( ) );
    m_output << indent << name << " ( );" << std::endl
             << indent << name << " (const ::fudge::message & source);" << std::endl
             << indent << "virtual ~" << name << " ( );" << std::endl
             << std::endl;

    // Output encoder/decoder method
    m_output << indent << "::fudge::message asFudgeMessage ( ) const;" << std::endl
             << indent << "void fromFudgeMessage (const ::fudge::message & source);" << std::endl
             << std::endl;

    // Output field accessors
    std::for_each ( message.fields ( ).begin ( ),
                    message.fields ( ).end ( ),
                    std::bind1st ( std::mem_fun ( &cppheaderwriter::outputFieldGetter ), this ) );
    m_output << std::endl;
    std::for_each ( message.fields ( ).begin ( ),
                    message.fields ( ).end ( ),
                    std::bind1st ( std::mem_fun ( &cppheaderwriter::outputFieldSetter ), this ) );

    // Enter the protected section
    --m_depth;
    m_output << std::endl
             << generateIndent ( ) << "protected:" << std::endl;
    ++m_depth;

    // The real encoding/decoding is done in here
    m_output << indent << "void toFudgeMessage (::fudge::message & message) const;" << std::endl
             << indent << "void fromAnonFudgeMessage (const ::fudge::message & message);" << std::endl
             << std::endl;

    // Output field members
    std::for_each ( message.fields ( ).begin ( ),
                    message.fields ( ).end ( ),
                    std::bind1st ( std::mem_fun ( &cppheaderwriter::outputMemberDef ), this ) );

    // Enter private section
    --m_depth;
    m_output << std::endl
             << generateIndent ( ) << "private:" << std::endl;
    ++m_depth;

    // Output non-implemented copy blockers
    m_output << indent << "// Not implemented - object cannot be copied" << std::endl
             << indent << name << " (const " << name << "& );" << std::endl
             << indent << name << "& operator= (const " << name << "& );" << std::endl;
}

void cppheaderwriter::outputFieldGetter ( const fielddef * field )
{
    m_output << generateIndent ( ) << "inline " << generateArgType ( *field )
             << " " << getIdLeaf ( *field ) << " ( ) const { return "
             << generateMemberName ( *field ) << "; }" << std::endl;
}

void cppheaderwriter::outputFieldSetter ( const fielddef * field )
{
    m_output << generateIndent ( ) << "void set" << getIdLeaf ( *field ) << " ("
             << generateArgType ( *field ) << " value);" << std::endl;
}

void cppheaderwriter::outputMemberDef ( const fielddef * field )
{
    m_output << generateIndent ( ) << generateMemberType ( *field ) << " "
             << generateMemberName ( *field ) << ";" << std::endl;
}

std::string cppheaderwriter::generateGuardSymbol ( const identifier & id ) const
{
    std::string name ( id.asString ( "_" ) );
    std::transform ( name.begin ( ), name.end ( ), name.begin ( ), toupper );
    return "INC_" + name;
}

