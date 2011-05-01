#ifndef INC_FUDGEPROTO_ASTWALKER
#define INC_FUDGEPROTO_ASTWALKER

#include "ast.hpp"

namespace fudgeproto {

class astwalker
{
    public:
        astwalker ( );
        virtual ~astwalker ( );

        virtual void walk ( definition * node );
        virtual void reset ( );

    protected:
        template<class C>
        void walkCollection ( const C & collection )
        {
            for ( typename C::const_iterator it ( collection.begin ( ) ); it != collection.end ( ); ++it )
                walk ( *it );
        }

        inline size_t stackSize ( ) const { return m_stack.size ( ); }

        void pushStack ( definition * def );
        void popStack ( );
        definition * peekStack ( size_t offset = 0 ) const;

        virtual void walk ( enumdef & node ) = 0;
        virtual void walk ( fielddef & node ) = 0;
        virtual void walk ( messagedef & node ) = 0;
        virtual void walk ( namespacedef & node ) = 0;

    private:
        std::deque<definition *> m_stack;
};

}

#endif

