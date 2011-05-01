#include "astwalker.hpp"
#include <sstream>
#include <stdexcept>
#include <typeinfo>

using namespace fudgeproto;

astwalker::astwalker ( )
{
}

astwalker::~astwalker ( )
{
    reset ( );
}

void astwalker::reset ( )
{
    while ( ! m_stack.empty ( ) ) popStack ( );
}

void astwalker::walk ( definition * node )
{
    if ( ! node )
        return;

    pushStack ( node );

    const std::type_info & type ( typeid ( *node ) );
    if (      type == typeid ( enumdef ) )      walk ( dynamic_cast<enumdef &> ( *node ) );
    else if ( type == typeid ( fielddef ) )     walk ( dynamic_cast<fielddef &> ( *node ) );
    else if ( type == typeid ( messagedef ) )   walk ( dynamic_cast<messagedef &> ( *node ) );
    else if ( type == typeid ( namespacedef ) ) walk ( dynamic_cast<namespacedef &> ( *node ) );
    else
    {
        std::ostringstream error;
        error << "Unknown type \"" << type.name ( ) << "\" for AST definition \""
              << node->idString ( ) << "\"";
        throw std::runtime_error ( error.str ( ) );
    }

    popStack ( );
}

void astwalker::pushStack ( definition * def )
{
    if ( ! def )
        throw std::runtime_error ( "Cannot push NULL on to AST stack" );

    refcounted::inc ( def );
    m_stack.push_front ( def );
}

void astwalker::popStack ( )
{
    if ( m_stack.empty ( ) )
        throw std::runtime_error ( "Cannot pop from an empty AST stack" );

    refcounted::dec ( m_stack.front ( ) );
    m_stack.pop_front ( );
}

definition * astwalker::peekStack ( size_t offset ) const
{
    return offset < m_stack.size ( ) ? m_stack [ offset ] : 0;
}

