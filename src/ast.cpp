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

#include "ast.hpp"
#include <cstring>
#include <sstream>
#include <typeinfo>

using namespace fudgeproto;

identifier::identifier ( )
{
}

identifier::identifier ( const std::string & first )
{
    m_elements.push_back ( first );
}

identifier::~identifier ( )
{
}

identifier * identifier::clone ( ) const
{
    identifier * result ( new identifier );
    result->m_elements = m_elements;
    return result;
}

void identifier::append ( const std::string & element )
{
    m_elements.push_back ( element );
}

void identifier::extend ( const identifier & source )
{
    std::copy ( source.m_elements.begin ( ),
                source.m_elements.end ( ),
                std::back_inserter ( m_elements ) );
}

void identifier::prepend ( const std::string & element )
{
    m_elements.push_front ( element );
}

void identifier::pop ( )
{
    if ( m_elements.empty ( ) )
        throw std::runtime_error ( "Cannot pop from empty identifier" );
    m_elements.pop_back ( );
}

void identifier::clear ( )
{
    m_elements.clear ( );
}

std::string identifier::asString ( const std::string & separator ) const
{
    std::ostringstream text;
    for ( size_t index ( 0 ); index < m_elements.size ( ); ++index )
    {
        if ( index ) text << separator;
        text << m_elements [ index ];

    }
    return text.str ( );
}

identifier * identifier::createAndConsume ( char * source )
{
    if ( ! source ) throw std::runtime_error ( "Cannot create identifier with NULL character array" );
    identifier * newid ( new identifier ( source ) );
    delete [] source;
    return newid;
}

identifier * identifier::prependAndConsume ( identifier * id, char * source )
{
    if ( ! id )
        throw std::runtime_error ( "Cannot prepend a string to a NULL identifier" );
    if ( ! source )
        throw std::runtime_error ( "Cannot prepend an identifier with a NULL string" );

    id->prepend ( source );
    delete [] source;
    return id;
}

identifier * identifier::createFromString ( const std::string & source,
                                            const std::string & separator,
                                            bool ignoreEmpty )
{
    size_t position ( 0 ), next;
    std::string element;
    identifier * id ( 0 );

    while ( ( next = source.find ( separator, position ) ) != std::string::npos )
    {
        element = source.substr ( position, next - position );
        if ( element.size ( ) > 0 || ! ignoreEmpty )
        {
            if ( id )
                id->append ( element );
            else
                id = new identifier ( element );
        }
        position = next + separator.size ( );
    }

    if ( position < source.size ( ) )
        element = source.substr ( position, next - position );
    else
        element = "";

    if ( id )
    {
        if ( element.size ( ) > 0 || ! ignoreEmpty )
            id->append ( element );
    }
    else
        id = new identifier ( element );

    return id;
}

definition::definition ( identifier * id )
    : m_id ( id )
{
    refcounted::inc ( m_id );
}

definition::~definition ( )
{
    refcounted::dec ( m_id );
}

void definition::setIdentifier ( identifier * id )
{
    if ( m_id )
        throw std::runtime_error ( "Cannot set definition identifier to \"" + id->asString ( "." ) +
                                  "\", already set to \"" + idString ( ) + "\"" );

    refcounted::inc ( ( m_id = id ) );
}

void definition::resetIdentifier ( identifier * id )
{
    if ( ! m_id )
        throw std::runtime_error ( "Cannot reset identifier on unnamed defintion \"" +
                                   id->asString ( "." ) + "\"" );

    identifier * old ( m_id );
    refcounted::inc ( ( m_id = id ) );
    refcounted::dec ( old );
}

definition * definition::setIdentifierAndConsume ( definition * target, char * source )
{
    if ( ! target ) throw std::runtime_error ( "Cannot set the identifier (char) of a NULL definition" );
    if ( ! source ) throw std::runtime_error ( "Cannot set a definition's identifier with a NULL character array" );
    target->setIdentifier ( identifier::createAndConsume ( source ) );
    return target;
}

definition * definition::setIdentifierAndConsume ( definition * target, identifier * source )
{
    if ( ! target ) throw std::runtime_error ( "Cannot set the identifier of a NULL definition" );
    if ( ! source ) throw std::runtime_error ( "Cannot set a definition's identifier with a NULL identifier" );
    target->setIdentifier ( source );
    refcounted::dec ( source );
    return target;
}

fieldtype::fieldtype ( int type )
    : m_type ( static_cast<fudgeproto_type> ( type ) )
    , m_id ( 0 )
    , m_definition ( 0 )
{
    if ( type < FUDGEPROTO_TYPE_INVALID ) throw std::runtime_error ( "Invalid type for fieldtype" );
    if ( type == FUDGEPROTO_TYPE_USER )   throw std::runtime_error ( "Simple constructor used for user fieldtype" );
}

fieldtype::fieldtype ( identifier * id )
    : m_type ( FUDGEPROTO_TYPE_USER )
    , m_id ( id )
    , m_definition ( 0 )
{
    if ( ! id ) throw std::runtime_error ( "Cannot create user field with NULL identifier" );
    refcounted::inc ( m_id );
}

fieldtype::fieldtype ( const fieldtype & source )
    : m_type ( source.m_type )
    , m_id ( source.m_id )
    , m_definition ( source.m_definition )
{
    refcounted::inc ( m_id );
    refcounted::inc ( m_definition );
}

fieldtype & fieldtype::operator= ( const fieldtype & source )
{
    if ( this != &source )
    {
        const identifier * oldId ( m_id );
        refcounted::inc ( ( m_id = source.m_id ) );
        refcounted::dec ( oldId );
        const definition * oldDef ( m_definition );
        refcounted::inc ( ( m_definition = source.m_definition ) );
        refcounted::dec ( oldDef );
        m_type = source.m_type;
    }
    return *this;
}

fieldtype::~fieldtype ( )
{
    refcounted::dec ( m_id );
    refcounted::dec ( m_definition );
}

bool fieldtype::isComplex ( ) const
{
    return m_type == FUDGEPROTO_TYPE_USER &&
           typeid ( def ( ) ) != typeid ( enumdef );
}

bool fieldtype::isInteger ( ) const
{
    switch ( m_type )
    {
        case FUDGEPROTO_TYPE_BYTE:
        case FUDGEPROTO_TYPE_SHORT:
        case FUDGEPROTO_TYPE_INT:
        case FUDGEPROTO_TYPE_LONG:  return true;
        default:                    return false;
    }
}

bool fieldtype::isFloating ( ) const
{
    switch ( m_type )
    {
        case FUDGEPROTO_TYPE_FLOAT:
        case FUDGEPROTO_TYPE_DOUBLE:    return true;
        default:                        return false;
    }
}

void fieldtype::resetIdentifier ( const identifier * id )
{
    if ( ! id )                           throw std::invalid_argument ( "Cannot reset fieldtype identifier with NULL" );
    if ( m_type != FUDGEPROTO_TYPE_USER ) throw std::invalid_argument ( "Cannot reset fieldtype identifier on non-user type" );
    const identifier * old ( m_id );
    refcounted::inc ( ( m_id = id ) );
    refcounted::dec ( old );
}

const definition & fieldtype::def ( ) const
{
    if ( m_type != FUDGEPROTO_TYPE_USER ) throw std::runtime_error ( "Cannot get definition for non-user type fieldtype" );
    if ( ! m_definition )                 throw std::runtime_error ( "Cannot get definition for fieldtype \"" +
                                                                     m_id->asString ( "." ) + "\", none has been set" );
    return *m_definition;
}

void fieldtype::setDefinition ( const definition * def )
{
    if ( m_type != FUDGEPROTO_TYPE_USER ) throw std::invalid_argument ( "Cannot set definition for non-user type fieldtype" );
    if ( ! def )                          throw std::invalid_argument ( "Cannot set fieldtype definition with NULL" );
    if ( m_definition )                   throw std::invalid_argument ( "Cannot set fieldtype definition, already set" );
    refcounted::inc ( ( m_definition = def ) );
}

fieldconstraint::fieldconstraint ( int bound )
{
    append ( bound );
}

void fieldconstraint::append ( int bound )
{
    m_bounds.push_back ( bound );
}

literalvalue::literalvalue ( int value )
    : m_type ( FUDGEPROTO_TYPE_INT )
{
    m_value.integer = value;
}

literalvalue::literalvalue ( double value )
    : m_type ( FUDGEPROTO_TYPE_DOUBLE )
{
    m_value.dprecision = value;
}

literalvalue::literalvalue ( const std::string & value )
    : m_type ( FUDGEPROTO_TYPE_STRING )
{
    m_value.string = new std::string ( value );
}

literalvalue::literalvalue ( bool value )
    : m_type ( FUDGEPROTO_TYPE_BOOLEAN )
{
    m_value.boolean = value;
}

literalvalue::~literalvalue ( )
{
    if ( m_type == FUDGEPROTO_TYPE_STRING )
        delete m_value.string;
}

bool literalvalue::getBool ( ) const
{
    if ( m_type == FUDGEPROTO_TYPE_BOOLEAN )
        return m_value.boolean;
    throw std::runtime_error ( "Cannot getBool from non-boolean literal" );
}

double literalvalue::getDouble ( ) const
{
    if ( m_type == FUDGEPROTO_TYPE_DOUBLE )
        return m_value.dprecision;
    throw std::runtime_error ( "Cannot getDouble from non-double literal" );
}

int literalvalue::getInt ( ) const
{
    if ( m_type == FUDGEPROTO_TYPE_INT )
        return m_value.integer;
    throw std::runtime_error ( "Cannot getInt from non-integer literal" );
}

const std::string & literalvalue::getString ( ) const
{
    if ( m_type == FUDGEPROTO_TYPE_STRING )
        return *m_value.string;
    throw std::runtime_error ( "Cannot getString from non-string literal" );
}

bool literalvalue::isCompatibleType ( fudgeproto_type type ) const
{
    switch ( m_type )
    {
        case FUDGEPROTO_TYPE_INVALID:
        case FUDGEPROTO_TYPE_BOOLEAN:
        case FUDGEPROTO_TYPE_STRING:
        case FUDGEPROTO_TYPE_DATE:
        case FUDGEPROTO_TYPE_TIME:
        case FUDGEPROTO_TYPE_DATETIME:
            return m_type == type;

        case FUDGEPROTO_TYPE_BYTE:
        case FUDGEPROTO_TYPE_SHORT:
        case FUDGEPROTO_TYPE_INT:
        case FUDGEPROTO_TYPE_LONG:
            switch ( type )
            {
                case FUDGEPROTO_TYPE_BYTE:
                case FUDGEPROTO_TYPE_SHORT:
                case FUDGEPROTO_TYPE_INT:
                case FUDGEPROTO_TYPE_LONG:
                    return true;
                default:
                    return false;
            }

        case FUDGEPROTO_TYPE_FLOAT:
        case FUDGEPROTO_TYPE_DOUBLE:
            switch ( type )
            {
                case FUDGEPROTO_TYPE_FLOAT:
                case FUDGEPROTO_TYPE_DOUBLE:
                    return true;
                default:
                    return false;
            }

        default:
            throw std::runtime_error ( "Cannot check compatibility with unknown type" );
    }
}

literalvalue * literalvalue::createAndConsume ( char * value )
{
    if ( ! value ) throw std::runtime_error ( "Cannot create literal value with NULL string" );
    literalvalue * literal ( new literalvalue ( std::string ( value ) ) );
    delete [] value;
    return literal;
}

enumdef::enumdef ( )
    : definition ( 0 )
{
}

enumdef::enumdef ( identifier * id )
    : definition ( id )
{
}

void enumdef::append ( const std::string & key )
{
    append ( key, m_pairs.empty ( ) ? 0 : m_pairs.back ( ).second + 1 );
}

void enumdef::append ( const std::string & key, int32_t value )
{
    m_pairs.push_back ( std::make_pair ( key, value ) );
}

enumdef * enumdef::appendAndConsume ( enumdef * enumeration, char * key, int32_t value, bool hasValue )
{
    if ( ! enumeration ) throw std::runtime_error ( "Cannot append to a NULL enumdef" );
    if ( ! key )         throw std::runtime_error ( "Cannot append a NULL key to an enumdef" );

    if ( hasValue )
        enumeration->append ( key, value );
    else
        enumeration->append ( key );
    delete [] key;
    return enumeration;
}

fielddef::fielddef ( identifier * id,
                     const fieldtype & type,
                     fudgeproto_modifier modifier,
                     const std::vector<int> & constraints,
                     literalvalue * defvalue )
    : definition ( id )
    , m_type ( type )
    , m_modifier ( modifier )
    , m_constraints ( constraints )
    , m_ordinal ( 0 )
    , m_defvalue ( defvalue )
{
    refcounted::inc ( m_defvalue );
}

fielddef::~fielddef ( )
{
    delete m_ordinal;
    refcounted::dec ( m_defvalue );
}

void fielddef::setOrdinal ( int ordinal )
{
    if ( m_ordinal ) throw std::runtime_error ( "Attempted to set ordinal for \"" + idString ( ) + "\" twice" );
    m_ordinal = new int ( ordinal );
}

fielddef * fielddef::createAndConsume ( char * name,
                                        fieldtype * type,
                                        int modifier,
                                        fieldconstraint * constraints,
                                        int ordinal,
                                        literalvalue * defvalue )
{
    static const std::vector<int> emptyConstraints;

    if ( ! name ) throw std::runtime_error ( "Cannot create field with NULL name" );
    if ( ! type ) throw std::runtime_error ( "Cannot create field with NULL type" );

    if ( defvalue && ! defvalue->isCompatibleType ( type->type ( ) ) )
    {
        std::ostringstream buffer;
        buffer << "Default value for field \"" << name << "\" not compatible with field type";
        throw std::runtime_error ( buffer.str ( ) );
    }

    fielddef * field ( new fielddef ( identifier::createAndConsume ( name ),
                                      *type,
                                      cleanModifier ( modifier ),
                                      constraints ? constraints->bounds ( ) : emptyConstraints,
                                      defvalue ) );
    if ( ordinal != FUDGEPROTO_ORDINAL_NONE )
        field->setOrdinal ( ordinal );
    delete type;
    delete constraints;
    refcounted::dec ( defvalue );
    return field;
}

fudgeproto_modifier fielddef::cleanModifier ( int modifier )
{
    if ( ! ( modifier & FUDGEPROTO_MODIFIER_REQUIRED ) )
        modifier |= FUDGEPROTO_MODIFIER_OPTIONAL;

    if ( ( modifier & FUDGEPROTO_MODIFIER_REQUIRED ) && ( modifier & FUDGEPROTO_MODIFIER_OPTIONAL ) )
        throw std::runtime_error ( "Field cannot be both required AND optional" );
    if ( ( modifier & FUDGEPROTO_MODIFIER_MUTABLE ) && ( modifier & FUDGEPROTO_MODIFIER_READONLY ) )
        throw std::runtime_error ( "Field cannot be both mutable and readonly" );
    if ( modifier & FUDGEPROTO_MODIFIER_REPEATED )
        throw std::runtime_error ( "Field modifier \"repeated\" not supported - use arrays" );

    return static_cast<fudgeproto_modifier> ( modifier );
}

messagedef::messagedef ( )
    : definition ( 0 )
    , m_extern ( false )
{
}

messagedef::messagedef ( identifier * id, bool isExtern )
    : definition ( id )
    , m_extern ( isExtern )
{
}

messagedef::~messagedef ( )
{
    std::for_each ( m_messages.begin ( ), m_messages.end ( ), refcounted::dec );
    std::for_each ( m_enums.begin ( ),    m_enums.end ( ),    refcounted::dec );
    std::for_each ( m_fields.begin ( ),   m_fields.end ( ),   refcounted::dec );
    std::for_each ( m_parents.begin ( ),  m_parents.end ( ),  refcounted::dec );
}

void messagedef::addContent ( definition * content )
{
    if ( ! content ) throw std::runtime_error ( "Cannot add NULL content to \"" + idString ( ) + "\"" );

    const std::type_info & contentType ( typeid ( *content ) );
    if (      contentType == typeid ( fielddef ) )   m_fields.push_back   ( dynamic_cast<fielddef *> ( content ) );
    else if ( contentType == typeid ( messagedef ) ) m_messages.push_back ( dynamic_cast<messagedef *> ( content ) );
    else if ( contentType == typeid ( enumdef ) )    m_enums.push_back    ( dynamic_cast<enumdef *> ( content ) );
    else
    {
        std::ostringstream error;
        error << "Cannot add content of unknown type \"" << contentType.name ( )
              << "\" to \"" << idString ( ) << "\"";
        throw std::runtime_error ( error.str ( ) );
    }

    refcounted::inc ( content );
}

void messagedef::addParents ( const identifier_list & parents )
{
    if ( m_extern ) throw std::runtime_error ( "Cannot add parents to an extern message definition" );

    m_parents.reserve ( m_parents.size ( ) + parents.size ( ) );
    std::for_each ( parents.content ( ).begin ( ), parents.content ( ).end ( ), refcounted::inc );
    std::copy ( parents.content ( ).begin ( ), parents.content ( ).end ( ), std::back_inserter ( m_parents ) );
}

void messagedef::replaceParent ( size_t index, const identifier * parent )
{
    if ( index >= m_parents.size ( ) ) throw std::out_of_range ( "Invalid index for message parent replace" );
    const identifier * old ( m_parents [ index ] );
    refcounted::inc ( ( m_parents [ index ] = parent ) );
    refcounted::dec ( old );
}

messagedef * messagedef::addContentAndConsume ( messagedef * message, definition * definition )
{
    if ( ! message ) throw std::runtime_error ( "Cannot add content to a NULL message" );
    message->addContent ( definition );
    refcounted::dec ( definition );
    return message;
}

messagedef * messagedef::addParentsAndConsume ( messagedef * message, identifier_list * parents )
{
    if ( ! message ) throw std::runtime_error ( "Cannot add parents to a NULL message" );
    if ( ! parents ) throw std::runtime_error ( "Cannot add a NULL parents list to message" );
    message->addParents ( *parents );
    refcounted::dec ( parents );
    return message;
}

namespacedef::namespacedef ( )
    : definition ( 0 )
{
}

namespacedef::namespacedef ( identifier * id )
    : definition ( id )
{
}

namespacedef & namespacedef::operator= ( const namespacedef & source )
{
    if ( this != &source )
    {
        clear ( );
        m_content = source.m_content;
        std::for_each ( m_content.begin ( ),m_content.end ( ), refcounted::inc );
    }
    return *this;
}

namespacedef::~namespacedef ( )
{
    clear ( );
}

void namespacedef::addContent ( definition * content )
{
    if ( ! content )       throw std::runtime_error ( "Cannot add NULL content to \"" + idString ( ) + "\"" );
    if ( content == this ) throw std::runtime_error ( "Cannot add namespace \"" + idString ( ) + "\" to self" );
    m_content.push_back ( content );
    refcounted::inc ( content );
}

void namespacedef::clear ( )
{
    std::for_each ( m_content.begin ( ), m_content.end ( ), refcounted::dec );
    m_content.clear ( );
}

void namespacedef::removeContentIf ( bool ( *predicate ) ( const definition & ) )
{
    std::list<definition *>::iterator it ( m_content.begin ( ) ), prev;
    while ( it != m_content.end ( ) )
    {
        prev = it++;
        if ( predicate ( **prev ) )
            m_content.erase ( prev );
    }
}

namespacedef * namespacedef::createAndConsume ( identifier * id )
{
    if ( ! id ) throw std::runtime_error ( "Cannot create namespace with NULL identifier" );
    namespacedef * ns ( new namespacedef ( id ) );
    refcounted::dec ( id );
    return ns;
}

namespacedef * namespacedef::addContentAndConsume ( namespacedef * ns, definition * content )
{
    if ( ! ns ) throw std::runtime_error ( "Cannot add content to a NULL namespace" );
    ns->addContent ( content );
    refcounted::dec ( content );
    return ns;
}

void namespacedef::assignAndConsume ( namespacedef & target, namespacedef * source )
{
    if ( ! source ) throw std::runtime_error ( "Cannot assign contents of a NULL namesapce" );
    target = *source;
    refcounted::dec ( source );
}

