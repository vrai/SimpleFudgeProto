%{
    #include <stdio.h>
    #include <string>
    #include "ast.hpp"

    // The file-level namespace that parsed file contents are loaded in to
    fudgeproto::namespacedef protoparser_root;

    // Will be populated with the error message in the event of an error
    std::string protoparser_error;

    extern int yylex ( );
    extern void yyerror ( const char * );
    extern "C" int yywrap ( );
%}

// The "message" token is used both as a type and to mark the start of a message block.
%expect 1

%error-verbose

%token IDENTIFIER
%token FLOATNUMBER NATURALNUMBER
%token BOOLLITERAL STRINGLITERAL
%token DEFAULT ENUM EXTENDS EXTERN MESSAGE NAMESPACE
%token MUTABLE OPTIONAL READONLY REPEATED REQUIRED
%token INDICATOR BOOLEAN BYTE SHORT INT LONG FLOAT DOUBLE STRING
%token DATE TIME DATETIME

%union
{
    char * string;
    int integer;
    double floating;
    bool boolean;

    struct
    {
        char * key;
        int32_t value;
        bool has_value;
    } enumrow;

    fudgeproto::definition * definition;
    fudgeproto::enumdef * enumdef;
    fudgeproto::fieldconstraint * constraint;
    fudgeproto::fielddef * fielddef;
    fudgeproto::fieldtype * fieldtype;
    fudgeproto::identifier * identifier;
    fudgeproto::identifier_list * identifier_list;
    fudgeproto::messagedef * messagedef;
    fudgeproto::namespacedef * namespacedef;
    fudgeproto::literalvalue * literal;
}

%type <integer>         field_constraint field_modifier field_modifiers
%type <integer>         field_ordinal
%type <integer>         integer_constant
%type <floating>        float_constant

%type <constraint>      field_constraints
%type <definition>      message_content namespace_content
%type <enumdef>         enum_def enum_rows
%type <enumrow>         enum_row
%type <fielddef>        field_def
%type <fieldtype>       field_type
%type <identifier>      fqname
%type <identifier_list> fqname_list
%type <literal>         field_default literal
%type <messagedef>      message_def extern_message_def message_contents
%type <namespacedef>    namespace_contents namespace_def

%type <boolean>         BOOLLITERAL
%type <integer>         NATURALNUMBER
%type <floating>        FLOATNUMBER
%type <string>          STRINGLITERAL IDENTIFIER

%start root

%%

root: namespace_contents   { fudgeproto::namespacedef::assignAndConsume ( protoparser_root, $1 ); }
    |                      { }
    ;

namespace_def:  NAMESPACE fqname '{' '}'                    { $$ = fudgeproto::namespacedef::createAndConsume ( $2 ); }
             |  NAMESPACE fqname '{' namespace_contents '}' { $$ = ( fudgeproto::namespacedef * ) fudgeproto::definition::setIdentifierAndConsume ( $4, $2 ); }
             ;

namespace_contents: namespace_content                           { $$ = fudgeproto::namespacedef::addContentAndConsume ( new fudgeproto::namespacedef ( ), $1 ); }
                  | namespace_contents namespace_content        { $$ = fudgeproto::namespacedef::addContentAndConsume ( $1, $2 ); }
                  ;

namespace_content:  namespace_def                               { $$ = $1; }
                 |  message_def                                 { $$ = $1; }
                 |  extern_message_def                          { $$ = $1; }
                 ;

extern_message_def: EXTERN MESSAGE IDENTIFIER ';'               { $$ = new fudgeproto::messagedef ( fudgeproto::identifier::createAndConsume ( $3 ), true ); }
                  ;

message_def:    MESSAGE IDENTIFIER '{' '}'                                      { $$ = new fudgeproto::messagedef ( fudgeproto::identifier::createAndConsume ( $2 ) ); }
           |    MESSAGE IDENTIFIER EXTENDS fqname_list '{' '}'                  {
                                                                                    $$ = new fudgeproto::messagedef ( fudgeproto::identifier::createAndConsume ( $2 ) );
                                                                                    fudgeproto::messagedef::addParentsAndConsume ( $$, $4 );
                                                                                }
           |    MESSAGE IDENTIFIER '{' message_contents '}'                     { $$ = ( fudgeproto::messagedef * ) fudgeproto::definition::setIdentifierAndConsume ( $4, $2 ); }
           |    MESSAGE IDENTIFIER EXTENDS fqname_list '{' message_contents '}' {
                                                                                    $$ = ( fudgeproto::messagedef * ) fudgeproto::definition::setIdentifierAndConsume ( $6, $2 );
                                                                                    fudgeproto::messagedef::addParentsAndConsume ( $$, $4 );
                                                                                }
           ;

message_contents:   message_content                         { $$ = fudgeproto::messagedef::addContentAndConsume ( new fudgeproto::messagedef ( ), $1 ); }
                |   message_contents message_content        { $$ = fudgeproto::messagedef::addContentAndConsume ( $1, $2 ); }
                ;

message_content:    field_def ';'                           { $$ = $1; }
               |    message_def                             { $$ = $1; }
               |    enum_def                                { $$ = $1; }
               ;

field_def:  field_modifiers field_type field_constraints IDENTIFIER field_ordinal field_default { $$ = fudgeproto::fielddef::createAndConsume ( $4, $2, $1, $3, $5, $6 ); }
         |  field_modifiers field_type IDENTIFIER field_ordinal field_default                   { $$ = fudgeproto::fielddef::createAndConsume ( $3, $2, $1, 0, $4, $5 ); }
         |  field_type field_constraints IDENTIFIER field_ordinal field_default                 { $$ = fudgeproto::fielddef::createAndConsume ( $3, $1, FUDGEPROTO_MODIFIER_NONE, $2, $4, $5 ); }
         |  field_type IDENTIFIER field_ordinal field_default                                   { $$ = fudgeproto::fielddef::createAndConsume ( $2, $1, FUDGEPROTO_MODIFIER_NONE, 0, $3, $4 ); }
         ;

field_modifiers:    field_modifier                      { $$ = $1; }
               |    field_modifier field_modifiers      { $$ = $1 | $2; }
               ;

field_modifier:     MUTABLE     { $$ = FUDGEPROTO_MODIFIER_MUTABLE; }
              |     OPTIONAL    { $$ = FUDGEPROTO_MODIFIER_OPTIONAL; }
              |     READONLY    { $$ = FUDGEPROTO_MODIFIER_READONLY; }
              |     REPEATED    { $$ = FUDGEPROTO_MODIFIER_REPEATED; }
              |     REQUIRED    { $$ = FUDGEPROTO_MODIFIER_REQUIRED; }
              ;

field_type: INDICATOR       { $$ = new fudgeproto::fieldtype ( FUDGEPROTO_TYPE_INDICATOR ); }
          | BOOLEAN         { $$ = new fudgeproto::fieldtype ( FUDGEPROTO_TYPE_BOOLEAN ); }
          | BYTE            { $$ = new fudgeproto::fieldtype ( FUDGEPROTO_TYPE_BYTE ); }
          | SHORT           { $$ = new fudgeproto::fieldtype ( FUDGEPROTO_TYPE_SHORT ); }
          | INT             { $$ = new fudgeproto::fieldtype ( FUDGEPROTO_TYPE_INT ); }
          | LONG            { $$ = new fudgeproto::fieldtype ( FUDGEPROTO_TYPE_LONG ); }
          | FLOAT           { $$ = new fudgeproto::fieldtype ( FUDGEPROTO_TYPE_FLOAT ); }
          | DOUBLE          { $$ = new fudgeproto::fieldtype ( FUDGEPROTO_TYPE_DOUBLE ); }
          | STRING          { $$ = new fudgeproto::fieldtype ( FUDGEPROTO_TYPE_STRING ); }
          | MESSAGE         { $$ = new fudgeproto::fieldtype ( FUDGEPROTO_TYPE_MESSAGE ); }
          | DATE            { $$ = new fudgeproto::fieldtype ( FUDGEPROTO_TYPE_DATE ); }
          | TIME            { $$ = new fudgeproto::fieldtype ( FUDGEPROTO_TYPE_TIME ); }
          | DATETIME        { $$ = new fudgeproto::fieldtype ( FUDGEPROTO_TYPE_DATETIME ); }
          | fqname          { $$ = new fudgeproto::fieldtype ( $1 ); }
          ;

field_constraints: field_constraint                     { $$ = new fudgeproto::fieldconstraint ( $1 ); }
                 | field_constraints field_constraint   { $$->append ( $2 ); }
                 ;

field_constraint: '[' ']'               { $$ = FUDGEPROTO_CONSTRAINT_UNBOUNDED; }
                | '[' NATURALNUMBER ']' { $$ = $2; }
                ;

field_ordinal:                      { $$ = FUDGEPROTO_ORDINAL_NONE; }
             | '=' NATURALNUMBER    { $$ = $2; }
             ;

field_default:                                  { $$ = 0; }
             |  '[' DEFAULT '=' literal ']'     { $$ = $4; }
             ;

enum_def:   ENUM IDENTIFIER '{' '}'             { $$ = new fudgeproto::enumdef ( fudgeproto::identifier::createAndConsume ( $2 ) ); }
        |   ENUM IDENTIFIER '{' enum_rows '}'   { $$ = ( fudgeproto::enumdef * ) fudgeproto::definition::setIdentifierAndConsume ( $4, $2 ); }
        ;

enum_rows:  enum_row                            { $$ = fudgeproto::enumdef::appendAndConsume ( new fudgeproto::enumdef, $1.key, $1.value, $1.has_value ); }
         |  enum_rows enum_row                  { $$ = fudgeproto::enumdef::appendAndConsume ( $1, $2.key, $2.value, $2.has_value ); }
         ;

enum_row:   IDENTIFIER ';'                      {
                                                    $$.key = $1;
                                                    $$.has_value = false;
                                                }
        |   IDENTIFIER '=' integer_constant ';' {
                                                    $$.key = $1;
                                                    $$.value = $3;
                                                    $$.has_value = true;
                                                }
        ;

integer_constant:   NATURALNUMBER       { $$ = $1; }
                |   '-' NATURALNUMBER   { $$ = - $2; }
                ;

float_constant:     FLOATNUMBER         { $$ = $1; }
              |     '+' FLOATNUMBER     { $$ = $2; }
              |     '-' FLOATNUMBER     { $$ = - $2; }
              ;

literal:    integer_constant    { $$ = new fudgeproto::literalvalue ( $1 ); }
       |    float_constant      { $$ = new fudgeproto::literalvalue ( $1 ); }
       |    STRINGLITERAL       { $$ = fudgeproto::literalvalue::createAndConsume ( $1 ); }
       |    BOOLLITERAL         { $$ = new fudgeproto::literalvalue ( $1 ); }
       ;

fqname_list:    fqname                  { $$ = fudgeproto::identifier_list::createAndConsume ( $1 ); }
           |    fqname ',' fqname_list  { $$ = fudgeproto::identifier_list::prependAndConsume ( $3, $1 ); }
           ;

fqname: IDENTIFIER              { $$ = fudgeproto::identifier::createAndConsume ( $1 ); }
      | IDENTIFIER '.' fqname   { $$ = fudgeproto::identifier::prependAndConsume ( $3, $1 ); }
      ;

%%

// Global variables
int protolexer_linecount;

// Global function definitions
void yyerror ( const char * error )
{
    protoparser_error = error;
}

int yywrap ( )
{
    return 1;
}

