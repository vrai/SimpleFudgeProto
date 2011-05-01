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

#ifndef INC_FUDGEPROTO_MEMORYUTIL
#define INC_FUDGEPROTO_MEMORYUTIL

#include <cstddef>
#include <typeinfo>

namespace fudgeproto {

char * strdup ( const char * string, size_t numchars );
char * strdup ( const char * string );

class refcounted
{
    public:
        refcounted ( );

        static inline void inc ( const refcounted * object )
        {
            if ( object )
                object->increment ( );
        }

        static inline void dec ( const refcounted * object )
        {
            if ( object && object->decrement ( ) )
                delete object;
        }

        inline int refcount ( ) const { return *m_refcount; }

    protected:
        virtual ~refcounted ( );

        void increment ( ) const;
        bool decrement ( ) const;

    private:
        typedef int refcount_type;

        mutable refcount_type * m_refcount;

        // Don't allow copying - use the reference counting!
        refcounted ( const refcounted & source );
        refcounted & operator= ( const refcounted & source );
};

template<class T>
class refptr
{
    public:
        refptr ( )
            : m_ptr ( 0 )
        {
        }

        refptr ( T * ptr )      // Steals reference
            : m_ptr ( ptr )
        {
        }

        refptr ( const refptr & source )
            : m_ptr ( source.m_ptr )
        {
            refcounted::inc ( m_ptr );
        }

        refptr & operator= ( const refptr & source )
        {
            if ( this != &source && m_ptr != source.m_ptr )
            {
                T * old ( m_ptr );
                refcounted::inc ( ( m_ptr = source.m_ptr ) );
                refcounted::dec ( old );
            }
            return *this;
        }

        ~refptr ( )
        {
            refcounted::dec ( m_ptr );
        }

        inline operator bool ( ) { return m_ptr != 0; }
        inline T * operator-> ( ) { return m_ptr; }
        inline const T * operator-> ( ) const { return m_ptr; }
        inline T & operator* ( ) { return *m_ptr; }
        inline const T & operator* ( ) const { return *m_ptr; }

        inline T * get ( ) { return m_ptr; }
        inline const T * get ( ) const { return m_ptr; }

        T * release ( )
        {
            refcounted::inc ( m_ptr );
            return m_ptr;
        }

        inline const std::type_info & type ( ) const { return typeid ( *m_ptr ); }

        template<class C>
        inline bool istype ( ) const { return type ( ) == typeid ( C ); }

    private:
        T * m_ptr;
};

}

#endif
