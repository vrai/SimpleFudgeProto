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

#ifndef INC_FUDGEPROTO_CPPIMPLWRITER
#define INC_FUDGEPROTO_CPPIMPLWRITER

#include "cppwriter.hpp"
#include <deque>

namespace fudgeproto {

class cppimplwriter : public cppwriter
{
    public:
        cppimplwriter ( std::ostream & output );
        ~cppimplwriter ( );

        void fileHeader ( const messagedef & ref,
                          const filenamegenerator & filenamegen );
        void fileFooter ( const identifier & id );
        void includeExternal ( const messagedef & ref,
                               const filenamegenerator & filenamegen );
        void endOfExternals ( size_t count );
        void includeStandard ( );
        void startNamespace ( const identifier & ns );
        void endNamespace ( const identifier & ns );
        void enumDefinition ( const enumdef & def );
        void startClass ( const messagedef & message );
        void endClass ( const messagedef & message );
        void classFields ( const messagedef & message );

    private:
        std::deque<std::string> m_stack;

        void outputMemberInitialiser ( const fielddef * field );
        void outputMemberCleanup ( const fielddef * field );
        void outputCollectionMemberCleanup ( const fielddef & field,
                                             const std::string & sourcevar,
                                             size_t index );
        void outputMemberSetterBody ( const fielddef * field );
        void outputEncoderWrapper ( const messagedef & message );
        void outputEncoderField ( const fielddef * field );
        void outputEncoderFieldAdd ( const std::string & targetvar,
                                     const std::string & sourcevar,
                                     const fielddef & field );
        void outputCollectionEncoder ( const std::string & targetvar,
                                       const std::string & sourcevar,
                                       const fielddef & field,
                                       size_t index );
        void outputCollectionRowValidation ( const fielddef & field,
                                             const std::string & sourcevar,
                                             const std::string & accessor,
                                             size_t index );
        void outputDecoderWrapper ( const messagedef & message );
        void outputParentDecoder ( const messagedef & message );
        void outputDecoderField ( const fielddef * field );
        void outputCollectionDecoder ( const std::string & sourcevar,
                                       const std::string & targetvar,
                                       const fielddef & field,
                                       size_t index );
        void outputCollectionFieldValidation ( const fielddef & field,
                                               const std::string & sourcevar,
                                               size_t index );

        std::string generateFieldAccessor ( const fielddef & field );
        std::string generateFieldAccessorCast ( const fielddef & field );
        std::string generateDefaultValue ( const fielddef & field );
        std::string generateLiteralValue ( const literalvalue & value );

        refptr<identifier> getStackAsId ( ) const;
};

}

#endif

