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

#include "astdumper.hpp"
#include "template.hpp"
#include <sstream>
#include <stdexcept>

using namespace fudgeproto;

astdumper::astdumper ( std::ostream & output )
    : m_output ( output )
    , m_depth ( 0 )
{
}

std::string astdumper::dumpIndex ( const astindex::definitionmap & index )
{
    std::ostringstream buffer;
    for ( fudgeproto::astindex::definitionmapcit it ( index.begin ( ) ); it != index.end ( ); ++it )
        buffer << "    " << it->first << " -> " << it->second << std::endl;
    return buffer.str ( );
}

void astdumper::walk ( enumdef & node )
{
    m_output << indent ( ) << "enum( " << node.idString ( ) << " )" << std::endl;
    ++m_depth;

    for ( size_t index ( 0 ); index < node.size ( ); ++index )
        m_output << indent ( ) << node [ index ].first << " = " << node [ index ].second << std::endl;

    --m_depth;
}

void astdumper::walk ( fielddef & node )
{
    m_output << indent ( ) << "field( ";
    if ( node.modifier ( ) != FUDGEPROTO_MODIFIER_NONE )
        m_output << modifierAsString ( node.modifier ( ) ) << " ";
    m_output << typeAsString ( node.type ( ) ) << constraintAsString ( node.constraints ( ) )
             << " " << node.idString ( );
    if ( node.hasOrdinal ( ) )
        m_output << " = " << node.ordinal ( );
    if ( node.hasDefValue ( ) )
        m_output << " [default=" << literalAsString ( node.defValue ( ) ) << "]";
    m_output << " )" << std::endl;
}

void astdumper::walk ( messagedef & node )
{
    if ( node.isExtern ( ) )
    {
        m_output << indent ( ) << "extern message( " << node.idString ( ) << " )" << std::endl;
    }
    else
    {
        m_output << indent ( ) << "message( " << node.idString ( );
        if ( ! node.parents ( ).empty ( ) )
        {
            m_output << " extends";
            for ( size_t index ( 0 ); index < node.parents ( ).size ( ); ++index )
                m_output << ( index ? ", " : " " ) << node.parents ( ) [ index ]->asString ( "." );
        }
        m_output << " )" << std::endl;
        ++m_depth;

        walkCollection ( node.enums ( ) );
        walkCollection ( node.messages ( ) );
        walkCollection ( node.fields ( ) );

        --m_depth;
        m_output << std::endl;
    }
}

void astdumper::walk ( namespacedef & node )
{
    if ( node.hasId ( ) )
    {
        m_output << indent ( ) << "namespace( " << node.idString ( ) << " )" << std::endl;
        ++m_depth;
    }

    walkCollection ( node.content ( ) );

    if ( node.hasId ( ) )
        --m_depth;
}

std::string astdumper::typeAsString ( const fieldtype & type )
{
    switch ( type.type ( ) )
    {
        case FUDGEPROTO_TYPE_INVALID:   return "INVALID_TYPE";
        case FUDGEPROTO_TYPE_INDICATOR: return "indicator";
        case FUDGEPROTO_TYPE_BOOLEAN:   return "bool";
        case FUDGEPROTO_TYPE_BYTE:      return "byte";
        case FUDGEPROTO_TYPE_SHORT:     return "short";
        case FUDGEPROTO_TYPE_INT:       return "int";
        case FUDGEPROTO_TYPE_LONG:      return "long";
        case FUDGEPROTO_TYPE_FLOAT:     return "float";
        case FUDGEPROTO_TYPE_DOUBLE:    return "double";
        case FUDGEPROTO_TYPE_STRING:    return "string";
        case FUDGEPROTO_TYPE_MESSAGE:   return "message";
        case FUDGEPROTO_TYPE_DATE:      return "date";
        case FUDGEPROTO_TYPE_TIME:      return "time";
        case FUDGEPROTO_TYPE_DATETIME:  return "datetime";
        case FUDGEPROTO_TYPE_USER:      return type.name ( ).asString ( "." );
    }

    std::ostringstream error;
    error << "UNKNOWN_TYPE(" << type.type ( ) << ")";
    return error.str ( );
}

std::string astdumper::modifierAsString ( fudgeproto_modifier modifier )
{
    std::list<std::string> strings;
    if ( modifier & FUDGEPROTO_MODIFIER_MUTABLE )  strings.push_back ( "mutable" );
    if ( modifier & FUDGEPROTO_MODIFIER_OPTIONAL ) strings.push_back ( "optional" );
    if ( modifier & FUDGEPROTO_MODIFIER_READONLY ) strings.push_back ( "readonly" );
    if ( modifier & FUDGEPROTO_MODIFIER_REPEATED ) strings.push_back ( "repeated" );
    if ( modifier & FUDGEPROTO_MODIFIER_REQUIRED ) strings.push_back ( "required" );
    return join_any ( strings.begin ( ), strings.end ( ), " " );
}

std::string astdumper::constraintAsString ( const std::vector<int> & constraint )
{
    std::vector<std::string> text ( constraint.size ( ) );
    for ( size_t index ( 0 ); index < text.size ( ); ++index )
    {
        std::ostringstream buffer;
        buffer << "[";
        if ( constraint [ index ] != FUDGEPROTO_CONSTRAINT_UNBOUNDED ) buffer << constraint [ index ];
        buffer << "]";
        text [ index ] = buffer.str ( );
    }
    return join_any ( text.begin ( ), text.end ( ), "" );
}

std::string astdumper::literalAsString ( const literalvalue & value )
{
    std::ostringstream buffer;
    switch ( value.type ( ) )
    {
        case FUDGEPROTO_TYPE_INT:     buffer << value.getInt ( ); break;
        case FUDGEPROTO_TYPE_DOUBLE:  buffer << value.getDouble ( ); break;
        case FUDGEPROTO_TYPE_STRING:  buffer << "\"" << value.getString ( ) << "\""; break;
        case FUDGEPROTO_TYPE_BOOLEAN: buffer << ( value.getBool ( ) ? "true" : "false" ); break;
        default:                      buffer << "UNKNOWN_TYPE(" << value.type ( ) << ")"; break;
    }
    return buffer.str ( );
}

std::string astdumper::indent ( ) const
{
    std::string string;
    for ( size_t index ( 0 ); index < m_depth; ++index )
        string += "    ";
    return string;
}

