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

#include "parser.hpp"
#include <cerrno>
#include <cstdio>
#include <sstream>
#include <cstring>
#include <stdexcept>

using namespace fudgeproto;

// Include the various global symbols defined by the lexer/parser
extern int protolexer_linecount;
extern void yyrestart ( FILE * input );
extern int yyparse ( );
extern std::string protoparser_error;
extern fudgeproto::namespacedef protoparser_root;

namespacedef * parser::parse ( const std::string & filename )
{
    // Open the file
    FILE * handle ( fopen ( filename.c_str ( ), "rb" ) );
    if ( ! handle )
    {
        char buffer [ 256 ];
        strerror_r ( errno, buffer, sizeof ( buffer ) );
        throw std::runtime_error ( "Failed to open \"" + filename + "\": " + buffer );
    }

    // Reset the parser
    protolexer_linecount = 1;
    protoparser_root.clear ( );
    yyrestart ( handle );
    
    // Do the parse
    if ( yyparse ( ) )
    {
        std::ostringstream error;
        error << "Error at line " << protolexer_linecount << ": " << protoparser_error;
        throw std::runtime_error ( error.str ( ) );
    }

    // Copy the root node (not a deep copy) and clear the parser
    refptr<namespacedef> root ( new namespacedef ( ) );
    *root = protoparser_root;
    protoparser_root.clear ( );
    return root.release ( );
}

