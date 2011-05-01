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

#ifndef INC_FUDGEPROTO_CONSTANTS
#define INC_FUDGEPROTO_CONSTANTS

#include <limits.h>

namespace
{
    enum fudgeproto_type
    {
        FUDGEPROTO_TYPE_INVALID     = 0x00,
        FUDGEPROTO_TYPE_INDICATOR   = 0x01,
        FUDGEPROTO_TYPE_BOOLEAN     = 0x02,
        FUDGEPROTO_TYPE_BYTE        = 0x03,
        FUDGEPROTO_TYPE_SHORT       = 0x04,
        FUDGEPROTO_TYPE_INT         = 0x05,
        FUDGEPROTO_TYPE_LONG        = 0x06,
        FUDGEPROTO_TYPE_FLOAT       = 0x07,
        FUDGEPROTO_TYPE_DOUBLE      = 0x08,
        FUDGEPROTO_TYPE_STRING      = 0x09,
        FUDGEPROTO_TYPE_MESSAGE     = 0x0a,
        FUDGEPROTO_TYPE_DATE        = 0x0b,
        FUDGEPROTO_TYPE_TIME        = 0x0c,
        FUDGEPROTO_TYPE_DATETIME    = 0x0d,
        FUDGEPROTO_TYPE_USER        = 0x0e
    };

    enum fudgeproto_modifier
    {
        FUDGEPROTO_MODIFIER_NONE        = 0x00,
        FUDGEPROTO_MODIFIER_MUTABLE     = 0x01,
        FUDGEPROTO_MODIFIER_OPTIONAL    = 0x02,
        FUDGEPROTO_MODIFIER_READONLY    = 0x04,
        FUDGEPROTO_MODIFIER_REPEATED    = 0x08,
        FUDGEPROTO_MODIFIER_REQUIRED    = 0x10
    };

    enum fudgeproto_constraint_constants
    {
        FUDGEPROTO_CONSTRAINT_UNBOUNDED  = -1
    };

    enum fudgeproto_ordinal_constants
    {
        FUDGEPROTO_ORDINAL_NONE = INT_MIN
    };
}

#endif

