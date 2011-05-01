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

#include "cppimplwriter.hpp"
#include <sstream>

using namespace fudgeproto;

cppimplwriter::cppimplwriter ( std::ostream & output )
    : cppwriter ( output )
{
}

cppimplwriter::~cppimplwriter ( )
{
}

void cppimplwriter::fileHeader ( const messagedef & ref,
                                 const filenamegenerator & filenamegen )
{
    m_output << generateHeader ( ) << std::endl;

    m_output << "#include \"" << filenamegen.generate ( ref, true, true ) << "\"" << std::endl
             << std::endl;
}

void cppimplwriter::fileFooter ( const identifier & id )
{
    if ( ! m_stack.empty ( ) )
        throw std::logic_error ( "C++ Impl writer reached end of file whilst still in scope" );
}

void cppimplwriter::includeExternal ( const messagedef &, const filenamegenerator & )
{
    // Does nothing
}

void cppimplwriter::endOfExternals ( size_t )
{
    // Does nothing
}

void cppimplwriter::includeStandard ( )
{
    // Does nothing
}

void cppimplwriter::startNamespace ( const identifier & ns )
{
    m_output << "using namespace " << generateIdString ( ns ) << ";" << std::endl
             << std::endl;
}

void cppimplwriter::endNamespace ( const identifier & )
{
    // Does nothing
}

void cppimplwriter::enumDefinition ( const enumdef & )
{
    // Does nothing
}

void cppimplwriter::startClass ( const messagedef & message )
{
    m_stack.push_back ( getIdLeaf ( message ) );
}

void cppimplwriter::endClass ( const messagedef & message )
{
    m_output << std::endl;
    m_stack.pop_back ( );
}

void cppimplwriter::classFields ( const messagedef & message )
{
    refptr<identifier> id ( getStackAsId ( ) );
    const std::string path ( generateIdString ( *id ) );
    const std::string & name ( getIdLeaf ( message ) );

    // Generate constructors
    m_output << path << "::" << name << " ( )" << std::endl
             << "{" << std::endl;
    ++m_depth;
    std::for_each ( message.fields ( ).begin ( ),
                    message.fields ( ).end ( ),
                    std::bind1st ( std::mem_fun ( &cppimplwriter::outputMemberInitialiser ), this ) );
    --m_depth;
    m_output << "}" << std::endl
             << std::endl
             << path << "::" << name << " (const fudge::message & source)" << std::endl
             << "{" << std::endl
             << s_indent << "fromFudgeMessage (source);" << std::endl
             << "}" << std::endl
             << std::endl;

    // Generate destructor
    m_output << path << "::~" << name << " ( )" << std::endl
             << "{" << std::endl;
    ++m_depth;
    std::for_each ( message.fields ( ).begin ( ),
                    message.fields ( ).end ( ),
                    std::bind1st ( std::mem_fun ( &cppimplwriter::outputMemberCleanup ), this ) );
    --m_depth;
    m_output << "}" << std::endl
             << std::endl;

    // Generate encoder method
    m_output << "fudge::message " << path << "::asFudgeMessage ( ) const" << std::endl
             << "{" << std::endl;
    ++m_depth;
    outputEncoderWrapper ( message );
    --m_depth;
    m_output << "}" << std::endl
             << std::endl;

    // Generate the real encoder method
    m_output << "void " << path << "::toFudgeMessage (fudge::message & target) const" << std::endl
            << "{" << std::endl;
    ++m_depth;
    std::for_each ( message.fields ( ).begin ( ),
                    message.fields ( ).end ( ),
                    std::bind1st ( std::mem_fun ( &cppimplwriter::outputEncoderField ), this ) );
    --m_depth;
    m_output << "}" << std::endl
             << std::endl;

    // Generate the decoder method
    m_output << "void " << path << "::fromFudgeMessage (const fudge::message & source)" << std::endl
             << "{" << std::endl;
    ++m_depth;
    outputDecoderWrapper ( message );
    --m_depth;
    m_output << "}" << std::endl
             << std::endl;

    // Generate the anonymous decoder method
    m_output << "void " << path << "::fromAnonFudgeMessage (const fudge::message & source)" << std::endl
             << "{" << std::endl;
    ++m_depth;
    outputParentDecoder ( message );
    std::for_each ( message.fields ( ).begin ( ),
                    message.fields ( ).end ( ),
                    std::bind1st ( std::mem_fun ( &cppimplwriter::outputDecoderField ), this ) );
    --m_depth;
    m_output << "}" << std::endl
             << std::endl;

    // Generate setters
    for ( std::list<fielddef *>::const_iterator it ( message.fields ( ).begin ( ) );
          it != message.fields ( ).end ( );
          ++it )
    {
        m_output << "void " << path << "::" << "set" << getIdLeaf ( **it )
                 << " (" << generateArgType ( **it ) << " value)" << std::endl
                 << "{" << std::endl;
        ++m_depth;
        outputMemberSetterBody ( *it );
        --m_depth;
        m_output << "}" << std::endl << std::endl;
    }
}

void cppimplwriter::outputMemberInitialiser ( const fielddef * field )
{
    // Collections don't have constructors - as they're real C++ objects they start empty
    if ( ! field->constraints ( ).empty ( ) )
        return;

    // Not-collections are initialised with either the literal value specified, or a type
    // specific default.
    m_output << generateIndent ( ) << generateMemberName ( *field )
             << " = " << generateDefaultValue ( *field ) << ";"
             << std::endl;
}

void cppimplwriter::outputMemberCleanup ( const fielddef * field )
{
    // Complex types (that aren't enums) and collections of complex types need destroying
    if ( ! field->type ( ).isComplex ( ) )
        return;

    if ( field->isCollection ( ) )
    {
        std::string sourcevar ( generateMemberName ( *field  ) );
        if ( field->isOptional ( ) )
        {
            m_output << generateIndent ( ) << "if (" << sourcevar << ")" << std::endl
                     << generateIndent ( ) << "{" << std::endl;
            ++m_depth;
            sourcevar = "(*" + sourcevar + ")";
        }

        outputCollectionMemberCleanup ( *field,
                                        sourcevar,
                                        field->constraints ( ).size ( ) - 1 );
        m_output << std::endl;

        if ( field->isOptional ( ) )
        {
            --m_depth;
            m_output << "}" << std::endl;
        }
    }
    else
        m_output << generateIndent ( ) << "delete " << generateMemberName ( *field )
                                       << ";" << std::endl;
}

void cppimplwriter::outputCollectionMemberCleanup ( const fielddef & field,
                                                    const std::string & sourcevar,
                                                    size_t index )
{
    const std::string indent ( generateIndent ( ) );
    m_output << indent << "for (size_t index" << index << " (0); index" << index << " < "
                       << sourcevar << ".size ( ); ++index" << index << ")" << std::endl
             << indent << "{" << std::endl;

    // Build the variable name of the current element
    std::stringstream namebuf;
    namebuf << sourcevar << "[index" << index << "]";

    if ( index )
    {
        ++m_depth;
        outputCollectionMemberCleanup ( field, namebuf.str ( ), index - 1 );
        --m_depth;
    }
    else
        m_output << indent << s_indent << "delete " << namebuf.str ( ) << ";" << std::endl;

    m_output << indent << "}" << std::endl;
}

void cppimplwriter::outputMemberSetterBody ( const fielddef * field )
{
    // TODO Validate dimensions of collection values - tricky to do without walking potentially
    //      massive nested vectors.

    outputMemberCleanup ( field );
    m_output << generateIndent ( ) << generateMemberName ( *field ) << " = value;" << std::endl;
}

void cppimplwriter::outputEncoderWrapper ( const messagedef & message )
{
    const std::string indent ( generateIndent ( ) );
    m_output << indent << "fudge::message target;" << std::endl
             << indent << "target.addField (fudge::string (\"" << message.idString ( )
                       << "\"), fudge::message::noname, 0);" << std::endl
             << std::endl;

    for ( size_t index ( 0 ); index < message.parents ( ).size ( ); ++index )
        m_output << indent << generateIdString ( *message.parents ( ) [ index ] )
                           << "::toFudgeMessage (target);" << std::endl;

    m_output << indent << "toFudgeMessage (target);" << std::endl
             << std::endl
             << generateIndent ( ) << "return target;" << std::endl;
}

void cppimplwriter::outputEncoderField ( const fielddef * field )
{
    const std::string membername ( generateMemberName ( *field ) );
    std::string memberargname;

    if ( field->isOptional ( ) )
    {
        m_output << generateIndent ( ) << "if (" << membername << ")" << std::endl
                 << generateIndent ( ) << "{" << std::endl;
        ++m_depth;
        memberargname = "(*" + membername + ")";
    }
    else
    {
        if ( field->type ( ).isComplex ( ) && ! field->isCollection ( ) )
            m_output << generateIndent ( ) << "if (!" << membername << ")" << std::endl
                     << generateIndent ( ) << s_indent << "throw std::runtime_error (\"Missing value for field \\\"" << field->idString ( )
                                           << "\\\"\");" << std::endl;

        memberargname = membername;
    }

    const std::string indent ( generateIndent ( ) );
    if ( field->isCollection ( ) )
    {
        outputCollectionEncoder ( "target", memberargname, *field, field->constraints ( ).size ( ) - 1 );
        m_output << std::endl;
    }
    else if ( field->type ( ).isComplex ( ) )
    {
        const std::string messagevar ( "submsg_" + getIdLeaf ( *field ) );

        m_output << indent << "fudge::message " << messagevar << " (" << membername
                           << "->asFudgeMessage ( ));" << std::endl;
        outputEncoderFieldAdd ( "target", messagevar, *field );
        m_output << std::endl;
    }
    else
        outputEncoderFieldAdd ( "target", memberargname, *field );

    if ( field->isOptional ( ) )
    {
        --m_depth;
        m_output << generateIndent ( ) << "}" << std::endl;
    }
}

void cppimplwriter::outputEncoderFieldAdd ( const std::string & targetvar,
                                            const std::string & sourcevar,
                                            const fielddef & field )
{
    m_output << generateIndent ( ) << targetvar << ".addField (" << sourcevar << ", ";

    m_output << "fudge::string (\"" << generateIdString ( field.id ( ) ) << "\")";
    m_output << ", ";
    if ( field.hasOrdinal ( ) )
        m_output << field.ordinal ( );
    else
        m_output << "fudge::message::noordinal";
    m_output << ");" << std::endl;
}

void cppimplwriter::outputCollectionEncoder ( const std::string & targetvar,
                                              const std::string & sourcevar,
                                              const fielddef & field,
                                              size_t index )
{
    // The collection encoding code can be a bit verbose, use a comment to denote the start
    if ( index + 1 == field.constraints ( ).size ( ) )
        m_output << generateIndent ( ) << "// Encode collection " << sourcevar << " (" << field.idString ( ) << ")" << std::endl;

    // Make sure the vector is of the correct size
    outputCollectionRowValidation ( field, sourcevar, "size", index );

    if ( index == 0 && ( field.type ( ).isInteger ( ) || field.type ( ).isFloating ( ) ) )
    {
        // Arrays of integers or floats can be added as Fudge native arrays
        if ( index + 1 == field.constraints ( ).size ( ) )
            outputEncoderFieldAdd ( targetvar, sourcevar, field );
        else
            m_output << generateIndent ( ) << targetvar << ".addField (" << sourcevar << ");" << std::endl;
    }
    else
    {
        // Create a target message for this submessage
        std::stringstream targetbuf;
        targetbuf << getIdLeaf ( field ) << index;
        m_output << generateIndent ( ) << "fudge::message " << targetbuf.str ( ) << ";" << std::endl;

        // Loop over elements in list, simple elements go in as fields, complex ones as messages
        m_output << generateIndent ( ) << "for (size_t index" << index << " (0); index" << index
                 << " < " << sourcevar << ".size ( ); ++index" << index << ")" << std::endl
                 << generateIndent ( ) << "{" << std::endl;
        ++m_depth;

        // Reference to source element
        std::stringstream namebuf;
        namebuf << sourcevar << "[index" << index << "]";

        if ( index )
        {
            // Recurse in to the next level
            outputCollectionEncoder ( targetbuf.str ( ), namebuf.str ( ), field, index - 1 );
        }
        else if ( field.type ( ).isComplex ( ) )
        {
            // Complex element needs to be encoded in to a submessage, which is added to the target
            const std::string subvarname ( "submsg_" + getIdLeaf ( field ) ),
                              indent ( generateIndent ( ) );

            m_output << indent << "fudge::message " << subvarname << " (" << namebuf.str ( )
                               << "->asFudgeMessage ( ));" << std::endl
                     << indent << targetbuf.str ( ) << ".addField (" << subvarname << ");" << std::endl;
        }
        else
        {
            // Simple types can be added directly in to the target
            m_output << generateIndent ( ) << targetbuf.str ( ) << ".addField (" << namebuf.str ( ) << ");"
                     << std::endl;
        }

        // Finish element loop and add variable to target
        --m_depth;
        m_output << generateIndent ( ) << "}" << std::endl;

        // Add the submessage to the target
        if ( index + 1 == field.constraints ( ).size ( ) )
            outputEncoderFieldAdd ( targetvar, targetbuf.str ( ), field );
        else
            m_output << generateIndent ( ) << targetvar << ".addField (" << targetbuf.str ( )
                                           << ");" << std::endl;
    }

}

void cppimplwriter::outputCollectionRowValidation ( const fielddef & field,
                                                    const std::string & sourcevar,
                                                    const std::string & accessor,
                                                    size_t index )
{
        const int constraint ( field.constraints ( ) [ index ] );
        if ( constraint >= 0 )
        {
            m_output << generateIndent ( ) << "if (" << sourcevar << "." << accessor << " () != " << constraint << ")" << std::endl
                     << generateIndent ( ) << s_indent << "throw std::runtime_error ( \"Collection field \\\"" << field.idString ( )
                                                       << "\\\" has incorrect dimensions\");" << std::endl;
        }
}

void cppimplwriter::outputDecoderWrapper ( const messagedef & message )
{
    const std::string indent ( generateIndent ( ) );
    m_output << indent << "static const fudge_i16 ordinal (0);" << std::endl
             << indent << "static const fudge::string expected (\"" << message.idString ( ) << "\");" << std::endl
             << std::endl
             << indent << "const fudge::field type (source.getField (ordinal));" << std::endl
             << indent << "if (type.type () != FUDGE_TYPE_STRING)" << std::endl
             << indent << s_indent << "throw std::runtime_error (\"Zero ordinal field not of type string\");" << std::endl
             << indent << "const fudge::string typestr (type.getString ());" << std::endl
             << indent << "if (expected != typestr)" << std::endl
             << indent << s_indent << "throw std::runtime_error (\"Expected type string \\\"\" + expected.convertToStdString () +" << std::endl
             << indent << s_indent << "                          \"\\\", got \\\"\" + typestr.convertToStdString () + \"\\\"\");" << std::endl
             << std::endl
             << indent << "fromAnonFudgeMessage (source);" << std::endl;
}

void cppimplwriter::outputParentDecoder ( const messagedef & message )
{
    if ( ! message.parents ( ).empty (  ) )
    {
        const std::string indent ( generateIndent ( ) );
        for ( size_t index ( 0 ); index < message.parents ( ).size ( ); ++index )
            m_output << indent << generateIdString ( *message.parents ( ) [ index ] )
                     << "::fromAnonFudgeMessage (source);" << std::endl;
        m_output << std::endl;
    }
}

void cppimplwriter::outputDecoderField ( const fielddef * field )
{
    // Attempt to retrieve field from message by name
    const std::string fieldname ( generateIdString ( field->id ( ) ) + "_field" );
    m_output << generateIndent ( ) << "fudge::field " << fieldname << ";" << std::endl
             << generateIndent ( ) << "if (source.getField(" << fieldname << ", fudge::string (\""
                                   << generateIdString ( field->id ( ) ) << "\")))" << std::endl
             << generateIndent ( ) << "{" << std::endl;
    ++m_depth;
    const std::string indent ( generateIndent ( ) );

    const std::string membername ( generateMemberName ( *field ) );
    std::string memberargname;
    if ( field->isOptional ( ) && ( field->isCollection ( ) || ! field->type ( ).isComplex ( ) ) )
        memberargname = "(*" + membername + ")";
    else
        memberargname = membername;

    // Type dependent decoding code
    if ( field->isCollection ( ) )
    {
        if ( field->isOptional ( ) )
            m_output << indent << membername << " = " << generateTrueType ( *field ) <<  " ();" << std::endl
                     << std::endl;

        outputCollectionDecoder ( fieldname, memberargname, *field, field->constraints ( ).size ( ) - 1 );
        m_output << std::endl;
    }
    else if ( field->type ( ).isComplex ( ) )
    {
        const std::string member ( generateMemberName ( *field ) );
        m_output << indent << "delete " << member << ";" << std::endl
                 << indent << member << " = new " << generateTypeName ( field->type ( ) ) << ";" << std::endl
                 << indent << membername << "->fromFudgeMessage (" << fieldname
                           << ".getMessage ());" << std::endl;
    }
    else
    {
        m_output << indent << membername << " = " << generateFieldAccessorCast ( *field )
                           << fieldname << "." << generateFieldAccessor ( *field ) << ";" << std::endl;
    }

    --m_depth;
    m_output << generateIndent ( ) << "}" << std::endl;

    // For required fields, handle the missing field case by throwing an exception
    if ( ! field->isOptional ( ) )
    {
        m_output << generateIndent ( ) << "else" << std::endl
                 << generateIndent ( ) << "{" << std::endl;
        ++m_depth;
        m_output << generateIndent ( ) << "throw std::runtime_error (\"Required field \\\"" << generateIdString ( field->id ( ) )
                                       << "\\\" missing from Fudge message\");" << std::endl;
        --m_depth;
        m_output << generateIndent ( ) << "}" << std::endl;
    }

    m_output << std::endl;
}

void cppimplwriter::outputCollectionDecoder ( const std::string & sourcevar,
                                              const std::string & targetvar,
                                              const fielddef & field,
                                              size_t index )
{
    // The collection decodeder code can be a bit verbose, use a comment to denote the start
    if ( index + 1 == field.constraints ( ).size ( ) )
        m_output << generateIndent ( ) << "// Decode collection " << targetvar << " (" << field.idString ( ) << ")" << std::endl;

    if ( index == 0 && ( field.type ( ).isInteger ( ) || field.type ( ).isFloating ( ) ) )
    {
        // Arrays of integers or floats are stored as Fudge native arrays
        outputCollectionRowValidation ( field, sourcevar, "numelements", index );
        m_output << generateIndent ( ) << sourcevar << ".getArray (" << targetvar << ");" << std::endl;
    }
    else
    {
        // Retrieve the submessage for this array
        std::stringstream messagebuf;
        messagebuf << getIdLeaf ( field ) << index;
        m_output << generateIndent ( ) << "const fudge::message " << messagebuf.str ( ) << " ("
                                       << sourcevar << ".getMessage ());" << std::endl;

        // Ensure the array has the correct number of elements
        outputCollectionRowValidation ( field, messagebuf.str ( ), "size", index );

        // Make sure the target has sufficient space
        m_output << generateIndent ( ) << targetvar << ".resize (" << messagebuf.str ( ) << ".size ());" << std::endl;

        // Loop over the fields in the submessage
        m_output << generateIndent ( ) << "for (size_t index" << index << " (0); index" << index
                                       << " < " << messagebuf.str ( ) << ".size (); ++index" << index << ")" << std::endl
                 << generateIndent ( ) << "{" << std::endl;
        ++m_depth;

        const std::string fieldname ( messagebuf.str ( ) + "_field" );
        m_output << generateIndent ( ) << "const fudge::field " << fieldname << " (" << messagebuf.str ( )
                                       << ".getFieldAt (index" << index << "));" << std::endl;

        // Reference to target element
        std::stringstream targetbuf;
        targetbuf << targetvar << "[index" << index << "]";

        if ( index )
        {
            outputCollectionDecoder ( fieldname, targetbuf.str ( ), field, index - 1 );
        }
        else if ( field.type ( ).isComplex ( ) )
        {
            m_output << generateIndent ( ) << targetbuf.str ( ) << " = new " << generateTypeName ( field.type ( ) )
                                           << ";" << std::endl
                     << generateIndent ( ) << targetbuf.str ( ) << "->fromFudgeMessage (" << fieldname
                                           << ".getMessage ());" << std::endl;
        }
        else
        {
            m_output << generateIndent ( ) << targetbuf.str ( ) << " = " << generateFieldAccessorCast ( field )
                                           << fieldname << "." << generateFieldAccessor ( field ) << ";" << std::endl;
        }

        // Finish element loop
        --m_depth;
        m_output << generateIndent ( ) << "}" << std::endl;
    }
}

std::string cppimplwriter::generateFieldAccessor ( const fielddef & field )
{
    const fieldtype & type ( field.type ( ) );
    switch ( type.type ( ) )
    {
        case FUDGEPROTO_TYPE_BOOLEAN:   return "getAsBoolean ()";
        case FUDGEPROTO_TYPE_BYTE:      return "getAsByte ()";
        case FUDGEPROTO_TYPE_SHORT:     return "getAsInt16 ()";
        case FUDGEPROTO_TYPE_INT:       return "getAsInt32 ()";
        case FUDGEPROTO_TYPE_LONG:      return "getAsInt64 ()";
        case FUDGEPROTO_TYPE_FLOAT:     return "getAsFloat32 ()";
        case FUDGEPROTO_TYPE_DOUBLE:    return "getAsFloat64 ()";
        case FUDGEPROTO_TYPE_STRING:    return "getString ()";
        case FUDGEPROTO_TYPE_MESSAGE:   return "getMessage ()";
        case FUDGEPROTO_TYPE_DATE:      return "getDate ()";
        case FUDGEPROTO_TYPE_TIME:      return "getTime ()";
        case FUDGEPROTO_TYPE_DATETIME:  return "getDateTime ()";
        case FUDGEPROTO_TYPE_USER:
            if ( typeid ( type.def ( ) ) == typeid ( enumdef ) )
                return "getAsInt32 ()";
            // Fall through
        default:
            throw std::invalid_argument ( "No accessor for field type in C++ Impl writer" );
    }
}

std::string cppimplwriter::generateFieldAccessorCast ( const fielddef & field )
{
    const fieldtype & type ( field.type ( ) );
    if ( type.type ( ) == FUDGEPROTO_TYPE_USER && typeid ( type.def ( ) ) == typeid ( enumdef ) )
        return "(" + generateTypeName ( type ) + ") ";
    else if ( type.type ( ) == FUDGEPROTO_TYPE_BOOLEAN )
        return "(fudge_bool) ";
    else
        return "";
}

std::string cppimplwriter::generateDefaultValue ( const fielddef & field )
{
    if ( field.hasDefValue ( ) )
        return generateLiteralValue ( field.defValue ( ) );

    const fieldtype & type ( field.type ( ) );

    switch ( type.type ( ) )
    {
        case FUDGEPROTO_TYPE_BOOLEAN:   return "FUDGE_FALSE";
        case FUDGEPROTO_TYPE_BYTE:
        case FUDGEPROTO_TYPE_SHORT:
        case FUDGEPROTO_TYPE_INT:
        case FUDGEPROTO_TYPE_LONG:      return "0";
        case FUDGEPROTO_TYPE_FLOAT:
        case FUDGEPROTO_TYPE_DOUBLE:    return "0.0";
        case FUDGEPROTO_TYPE_STRING:    return "fudge::string ( )";
        case FUDGEPROTO_TYPE_MESSAGE:   return "fudge::message ( )";
        case FUDGEPROTO_TYPE_DATE:      return "fudge::date ( )";
        case FUDGEPROTO_TYPE_TIME:      return "fudge::time ( )";
        case FUDGEPROTO_TYPE_DATETIME:  return "fudge::datetime ( )";
        case FUDGEPROTO_TYPE_USER:
            if ( typeid ( type.def ( ) ) == typeid ( enumdef ) )
                return "(" + generateTypeName ( type ) + ") 0";
            else
                return "0";
        default:
            throw std::invalid_argument ( "Unsupported type for C++ Impl writer" );
    }
}

std::string cppimplwriter::generateLiteralValue ( const literalvalue & value )
{
    std::ostringstream buffer;

    switch ( value.type ( ) )
    {
        case FUDGEPROTO_TYPE_BOOLEAN:
            buffer << ( value.getBool ( ) ? "FUDGE_TRUE" : "FUDGE_FALSE" );
            break;

        case FUDGEPROTO_TYPE_BYTE:
        case FUDGEPROTO_TYPE_SHORT:
        case FUDGEPROTO_TYPE_INT:
        case FUDGEPROTO_TYPE_LONG:
            buffer << value.getInt ( );
            break;

        case FUDGEPROTO_TYPE_FLOAT:
        case FUDGEPROTO_TYPE_DOUBLE:
            buffer << value.getDouble ( );
            break;

        case FUDGEPROTO_TYPE_STRING:
            buffer << "fudge::string (\"" << escapeString ( value.getString ( ) ) << "\")";
            break;

        default:
            throw std::invalid_argument ( "Invalid literal value type for C++ Impl writer" );
    }

    return buffer.str ( );
}

refptr<identifier> cppimplwriter::getStackAsId ( ) const
{
    if ( m_stack.empty ( ) )
        throw std::logic_error ( "C++ Impl writer cannot get empty stack as Id" );

    refptr<identifier> id ( new identifier ( m_stack.front ( ) ) );
    for ( size_t index ( 1 ); index < m_stack.size ( ); ++index )
        id->append ( m_stack [ index ] );
    return id;
}

