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

#ifndef INC_FUDGEPROTO_AST
#define INC_FUDGEPROTO_AST

#include "constants.hpp"
#include "memoryutil.hpp"
#include <algorithm>
#include <deque>
#include <list>
#include <map>
#include <vector>
#include <stdexcept>
#include <string>

namespace fudgeproto {

class identifier : public refcounted
{
    public:
        identifier ( const std::string & first );
        ~identifier ( );

        identifier * clone ( ) const;

        inline size_t size ( ) const { return m_elements.size ( ); }
        inline const std::string & at ( size_t index ) const { return m_elements [ index ]; }
        inline const std::string & operator[] ( size_t index ) const { return at ( index ); }

        void append ( const std::string & element );
        void extend ( const identifier & source );
        void prepend ( const std::string & element );
        void prepend ( const identifier & source );
        void pop ( );
        void clear ( );

        std::string asString ( const std::string & separator ) const;

        bool equals ( const identifier & id ) const;

        static identifier * createAndConsume ( char * source );
        static identifier * prependAndConsume ( identifier * id, char * source );
        static identifier * createFromString ( const std::string & source,
                                               const std::string & separator,
                                               bool ignoreEmpty = true );

    private:
        identifier ( );

        std::deque<std::string> m_elements;
};

class definition : public refcounted
{
    protected:
        definition ( identifier * id );

    public:
        virtual ~definition ( );

        virtual void addContent ( definition * ) = 0;

        inline bool hasId ( ) const { return m_id; }
        inline const identifier & id ( ) const { return *m_id; }
        inline std::string idString ( ) const { return m_id ? m_id->asString ( "." ) : "NULL"; }

        void setIdentifier ( identifier * id );
        void resetIdentifier ( identifier * id );

        static definition * setIdentifierAndConsume ( definition * target, char * source );
        static definition * setIdentifierAndConsume ( definition * target, identifier * source );

    private:
        definition ( );         // Not implemented

        identifier * m_id;
};

class fieldtype
{
    public:
        fieldtype ( int type );
        fieldtype ( identifier * id );
        fieldtype ( const fieldtype & source );
        fieldtype & operator= ( const fieldtype & source );
        ~fieldtype ( );

        inline fudgeproto_type type ( ) const { return m_type; }
        inline const identifier & name ( ) const { return *m_id; }

        bool isComplex ( ) const;
        bool isInteger ( ) const;
        bool isFloating ( ) const;

        void resetIdentifier ( const identifier * id );

        const definition & def ( ) const;
        void setDefinition ( const definition * def );

    private:
        fudgeproto_type m_type;
        const identifier * m_id;
        const definition * m_definition;
};

class fieldconstraint
{
    public:
        fieldconstraint ( int bound );

        void append ( int bound );

        inline const std::vector<int> & bounds ( ) const { return m_bounds; }

    private:
        std::vector<int> m_bounds;
};

class literalvalue : public refcounted
{
    public:
        literalvalue ( int value );
        literalvalue ( double value );
        literalvalue ( const std::string & value );
        literalvalue ( bool value );
        ~literalvalue ( );

        inline fudgeproto_type type ( ) const { return m_type; }

        bool getBool ( ) const;
        double getDouble ( ) const;
        int getInt ( ) const;
        const std::string & getString ( ) const;

        bool isCompatibleType ( fudgeproto_type type ) const;

        static literalvalue * createAndConsume ( char * value );

    private:
        fudgeproto_type m_type;
        union
        {
            int integer;
            double dprecision;
            bool boolean;
            std::string * string;
        } m_value;
};

template<class T>
class generic_list : public refcounted
{
    public:
        typedef std::list<T *> valuetype;

        ~generic_list ( )
        {
            std::for_each ( m_content.begin ( ), m_content.end ( ), refcounted::dec );
        }

        void prepend ( T * value )
        {
            if ( ! value ) throw std::runtime_error ( "Cannot prepend NULL value to list" );

            refcounted::inc ( value );
            m_content.push_front ( value );
        }

        static generic_list * createAndConsume ( T * value )
        {
            if ( ! value ) throw std::runtime_error ( "Cannot create a list with a NULL value" );

            generic_list<T> * list ( new generic_list<T> );
            list->prepend ( value );
            refcounted::dec ( value );
            return list;
        }

        static generic_list * prependAndConsume ( generic_list * list, T * value )
        {
            if ( ! list ) throw std::runtime_error ( "Cannot prepend to a NULL list" );
            if ( ! value ) throw std::runtime_error ( "Cannot prepend a list with a NULL value" );

            list->prepend ( value );
            refcounted::dec ( value );
            return list;
        }

        inline const valuetype & content ( ) const { return m_content; }
        inline size_t size ( ) const { return m_content.size ( ); }

    private:
        valuetype m_content;
};

typedef generic_list<identifier> identifier_list;

class enumdef : public definition
{
    public:
        enumdef ( );
        enumdef ( identifier * id );

        inline void addContent ( definition * ) { throw std::runtime_error ( "Cannot add content to an enum" ); }

        inline size_t size ( ) const { return m_pairs.size ( ); }
        inline const std::pair<std::string, int32_t> & operator[] ( size_t index ) const { return m_pairs [ index ]; }

        void append ( const std::string & key );
        void append ( const std::string & key, int32_t value );

        static enumdef * appendAndConsume ( enumdef * enumeration, char * key, int32_t value, bool hasValue );

    private:
        typedef std::pair<std::string, int32_t> pair;
        std::vector<pair> m_pairs;
};

class fielddef : public definition
{
    public:
        fielddef ( identifier * id,
                   const fieldtype & type,
                   fudgeproto_modifier modifier,
                   const std::vector<int> & constraints,
                   literalvalue * defvalue );
        ~fielddef ( );

        inline void addContent ( definition * ) { throw std::runtime_error ( "Cannot add content to a field" ); }

        inline fieldtype & type ( ) { return m_type; }
        inline const fieldtype & type ( ) const { return m_type; }
        inline fudgeproto_modifier modifier ( ) const { return m_modifier; }
        inline const std::vector<int> & constraints ( ) const { return m_constraints; }
        inline bool hasOrdinal ( ) const { return m_ordinal; }
        inline int ordinal ( ) const { return *m_ordinal; }
        inline bool hasDefValue ( ) const { return m_defvalue; }
        inline const literalvalue & defValue ( ) const { return *m_defvalue; }
        inline bool isOptional ( ) const { return m_modifier & FUDGEPROTO_MODIFIER_OPTIONAL; }

        inline bool isCollection ( ) const { return ! m_constraints.empty ( ); }

        void setOrdinal ( int ordinal );

        static fielddef * createAndConsume ( char * name,
                                             fieldtype * type,
                                             int modifier,
                                             fieldconstraint * constraints,
                                             int ordinal,
                                             literalvalue * defvalue );

        static fudgeproto_modifier cleanModifier ( int modifier );

    private:
        fieldtype m_type;
        fudgeproto_modifier m_modifier;
        std::vector<int> m_constraints;
        int * m_ordinal;
        literalvalue * m_defvalue;
};

class messagedef : public definition
{
    public:
        messagedef ( );
        messagedef ( identifier * id, bool isExtern = false );
        ~messagedef ( );

        inline bool isExtern ( ) const { return m_extern; }

        inline const std::vector<const identifier *> & parents ( ) const { return m_parents; }
        inline const std::list<enumdef *> & enums ( ) const { return m_enums; }
        inline const std::list<fielddef *> & fields ( ) const { return m_fields; }
        inline const std::list<messagedef *> messages ( ) const { return m_messages; }

        void addContent ( definition * content );
        void addParents ( const identifier_list & parents );

        void replaceParent ( size_t index, const identifier * parent );

        inline std::string originalIdString ( ) const { return m_originalId ? m_originalId->asString ( "." ) : idString ( ); }

        void saveOriginalId ( );

        static messagedef * addContentAndConsume ( messagedef * message, definition * definition );
        static messagedef * addParentsAndConsume ( messagedef * message, identifier_list * parents );

    private:
        bool m_extern;
        identifier * m_originalId;
        std::vector<const identifier *> m_parents;
        std::list<enumdef *> m_enums;
        std::list<fielddef *> m_fields;
        std::list<messagedef *> m_messages;
};

class namespacedef: public definition
{
    public:
        namespacedef ( );
        namespacedef ( identifier * id );
        namespacedef & operator= ( const namespacedef & source );
        ~namespacedef ( );

        void addContent ( definition * content );
        void clear ( );
        void removeContentIf ( bool ( *predicate ) ( const definition & ) );

        inline const std::list<definition *> & content ( ) const { return m_content; }

        static namespacedef * createAndConsume ( identifier * id );
        static namespacedef * addContentAndConsume ( namespacedef * ns, definition * content );
        static void assignAndConsume ( namespacedef & target, namespacedef * source );

    private:
        std::list<definition *> m_content;
};

}

#endif

