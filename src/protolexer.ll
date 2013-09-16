%{
    #include "ast.hpp"
    #include "protoparser.hh"
    #include "memoryutil.hpp"
    #include <list>
    #include <cstdio>

    extern int protolexer_linecount;

    static const size_t protolexer_strmaxsize ( 1024 );
    static char protolexer_strbuffer [ protolexer_strmaxsize ];
    static size_t protolexer_stroffset;

    extern void protolexer_addToStringBuffer ( const char * string, size_t size );
%}

%option never-interactive nounput
%x COMMENT_M
%x STRING_M

identifier  [a-zA-Z_][a-zA-Z0-9_]*
natural     [0-9]+
float       [0-9]+(\.[0-9]*)?([eE][+-]?[0-9]+)?f?

tab         [\t]
cr          [\r]
whitespace  [ ]|{tab}|{cr}

newline     [\n]

%%

{whitespace}+           { }

{newline}               protolexer_linecount++;

"//".*$                 { }
"/*"                    BEGIN( COMMENT_M );

"\""                    {
                            protolexer_stroffset = 0;
                            BEGIN( STRING_M );
                        }

"{"                     return '{';
"}"                     return '}';
"["                     return '[';
"]"                     return ']';
";"                     return ';';
","                     return ',';
"="                     return '=';
"-"                     return '-';
"+"                     return '+';
"."                     return '.';

<COMMENT_M>"*"+[^*/\n]* { }
<COMMENT_M>[^*\n]*      { }
<COMMENT_M>{newline}    protolexer_linecount++;
<COMMENT_M>"*/"         BEGIN( INITIAL );

<STRING_M>"\""          {
                            const size_t size ( std::min ( protolexer_stroffset, protolexer_strmaxsize ) );
                            yylval.string = fudgeproto::strdup ( protolexer_strbuffer, size );
                            BEGIN( INITIAL );
                            return STRINGLITERAL;
                        }
<STRING_M>"\\b"         protolexer_addToStringBuffer ( "\b", 1 );
<STRING_M>"\\n"         protolexer_addToStringBuffer ( "\n", 1 );
<STRING_M>"\\r"         protolexer_addToStringBuffer ( "\r", 1 );
<STRING_M>"\\t"         protolexer_addToStringBuffer ( "\t", 1 );
<STRING_M>"\\".         protolexer_addToStringBuffer ( yytext + 1, 1 );
<STRING_M>[^\\\"]+      protolexer_addToStringBuffer ( yytext, strlen ( yytext ) );


default                 return DEFAULT;
enum                    return ENUM;
extends                 return EXTENDS;
extern                  return EXTERN;
message                 return MESSAGE;
namespace               return NAMESPACE;

mutable                 return MUTABLE;
optional                return OPTIONAL;
readonly                return READONLY;
repeated                return REPEATED;
required                return REQUIRED;

indicator               return INDICATOR;
bool                    return BOOLEAN;
boolean                 return BOOLEAN;
int8                    return BYTE;
byte                    return BYTE;
short                   return SHORT;
int16                   return SHORT;
int                     return INT;
int32                   return INT;
long                    return LONG;
int64                   return LONG;
float                   return FLOAT;
double                  return DOUBLE;
string                  return STRING;
date                    return DATE;
time                    return TIME;
datetime                return DATETIME;

true                    {
                            yylval.boolean = true;
                            return BOOLLITERAL;
                        }
false                   {
                            yylval.boolean = false;
                            return BOOLLITERAL;
                        }

{identifier}            {
                            yylval.string = fudgeproto::strdup ( yytext );
                            return IDENTIFIER;
                        }

{natural}               {
                            yylval.integer = atoi ( yytext );
                            return NATURALNUMBER;
                        }

{float}                 {
                            yylval.floating = strtod ( yytext, 0 );
                            return FLOATNUMBER;
                        }

<<EOF>>                 {
                            yyterminate ( );
                        }

%%

void protolexer_addToStringBuffer ( const char * string, size_t size )
{
    if ( protolexer_stroffset + size < protolexer_strmaxsize )
    {
        memcpy ( protolexer_strbuffer + protolexer_stroffset, string, size );
        protolexer_stroffset += size;
    }
    else
        fprintf ( stderr, "%s:%d - lexer string buffer size exceeded %lu (max is %lu)\n",
                          __FILE__, __LINE__,
                          static_cast<unsigned long> ( protolexer_stroffset + size ),
                          static_cast<unsigned long> ( protolexer_strmaxsize ) );
}

