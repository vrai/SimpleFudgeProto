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

#include "astflattener.hpp"
#include "astgenerator.hpp"
#include "astrenamer.hpp"
#include "astresolver.hpp"
#include "cppwriterfactory.hpp"
#include "config.h"
#include "filenamegenerator.hpp"
#include "parser.hpp"
#include "stage.hpp"
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <getopt.h>

#define PROGRAM_NAME "simplefudgeproto"

namespace
{
    static const struct option longopts [] =
    {
        { "help",     no_argument,       NULL,   'h' },
        { "version",  no_argument,       NULL,   'v' },
        { "verbose",  no_argument,       NULL,   'V' },
        { "language", required_argument, NULL,   'l' },
        { "target",   optional_argument, NULL,   't' }
    };

    static void usage ( bool error, const char * errstr = 0 )
    {
        if ( errstr ) std::cerr << PROGRAM_NAME << ": " << errstr << std::endl;
        if ( error ) std::cout << std::endl;
        std::cout << "Usage: " << PROGRAM_NAME << " [-hvV] [-t dir] -l language file" << std::endl
                  << "  -h,--help          : print this help message" << std::endl
                  << "  -v,--version       : display version information then exit" << std::endl
                  << "  -V,--verbose       : generate debug output" << std::endl
                  << "  -l,--language=LANG : language to generate, one of: cpp" << std::endl
                  << "  -t,--target=DIR    : target directory for generated files (use" << std::endl
                  << "                       CWD if not specified" << std::endl;
        exit ( error ? 1 : 0 );
    }

    static void version ( )
    {
        std::cout << PACKAGE_STRING << std::endl
                  << "Licensed under the Apache License, version 2.0" << std::endl
                  << "Please report any bugs to " << PACKAGE_BUGREPORT << std::endl;
        exit ( 0 );
    }

    static fudgeproto::codewriterfactory * createCodeWriterFactory ( const std::string & language )
    {
        if ( language == "cpp" ) return new fudgeproto::cppwriterfactory;
        else return 0;
    }

    static std::pair<std::string, std::string> getFileExts ( const std::string & language )
    {
        if ( language == "cpp" ) return std::make_pair ( "hpp", "cpp" );
        else throw std::runtime_error ( "No file extensions found for language \"" + language + "\"" );
    }
}

int main ( int argc, char * argv [ ] )
{
    // Parse command-line options
    std::string language,
                target ( "." );
    bool verbose ( false );
    char option;
    while ( ( option = getopt_long ( argc, argv, "hvVl:t:", longopts, 0 ) ) >= 0 )
        switch ( option )
        {
            case 'v':   version ( );
            default:    usage ( true );
            case 'h':   usage ( false );

            case 'V':
                verbose = true;
                break;

            case 'l':
                if ( ! language.empty ( ) )
                    usage ( true, "must specify output language once and only once" );
                language = optarg;
                break;

            case 't':
                target = optarg;
                break;
        }
    argv += optind;
    if ( language.empty ( ) )
        usage ( true, "must specify the output language" );
    if ( ( argc -= optind ) != 1 )
        usage ( true, "must provide the filename of a single FudgeProto file" );

    try
    {
        // Create the correct code writer factory for the chosen language
        std::auto_ptr<fudgeproto::codewriterfactory> factory ( createCodeWriterFactory ( language ) );
        if ( ! factory.get ( ) )
            usage ( true, ( "Unrecogised language name \"" + language + "\"" ).c_str ( ) );

        // The language must exist, so it's safe to create the filename generator
        const std::pair<std::string, std::string> fileexts ( getFileExts ( language ) );
        std::auto_ptr<fudgeproto::filenamegenerator> filenamegen ( new fudgeproto::filenamegenerator ( target, fileexts.first, fileexts.second, true ) );

        // Construct the dumper and state objects that will be used during parsing/post-processing
        fudgeproto::astdumper dumper ( std::cout );
        fudgeproto::astindex index;
        fudgeproto::astextrefs extrefs;

        // Parse the proto file in to an AST
        fudgeproto::refptr<fudgeproto::namespacedef> root ( fudgeproto::parser::parse ( argv [ 0 ] ) );
        if ( verbose )
        {
            std::cout << "--- RAW AST ---" << std::endl;
            fudgeproto::Stage::defaultDump ( dumper, root );
        }

        // Construct the post-parser processing stages
        fudgeproto::Stage * stages [ 5 ];
        stages [ 0 ] = new fudgeproto::Stage ( new fudgeproto::astrenamer, dumper, "RENAME STAGE" );
        stages [ 1 ] = new fudgeproto::Stage ( new fudgeproto::astflattener, dumper, "TREE FLATTEN STAGE" );
        stages [ 2 ] = new fudgeproto::IndexStage ( new fudgeproto::astindexer ( index ), dumper, index );
        stages [ 3 ] = new fudgeproto::ResolverStage ( new fudgeproto::astresolver ( extrefs, index ), dumper, extrefs );
        stages [ 4 ] = new fudgeproto::Stage ( new fudgeproto::astgenerator ( extrefs, index, *factory, *filenamegen ), dumper, "CODE GENERATOR STAGE" );

        // Run the post-parser stages
        for ( size_t index ( 0 ); index < 5; ++index )
            stages [ index ]->run ( root, verbose );

        // Clean up
        std::for_each ( stages, stages + 5, fudgeproto::Stage::destroy );
    }
    catch ( const std::exception & exception )
    {
        std::cerr << "FATAL: " << exception.what ( ) << std::endl;
        return 1;
    }
}

