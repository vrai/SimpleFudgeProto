" Fudge Proto syntax file
" Copyright Vrai Stacey, 2011.
"
" Licensed under the Apache License, Version 2.0 (the "License");
" you may not use this file except in compliance with the License.
" You may obtain a copy of the License at
"
"     http:"www.apache.org/licenses/LICENSE-2.0
"
" Unless required by applicable law or agreed to in writing, software
" distributed under the License is distributed on an "AS IS" BASIS,
" WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
" See the License for the specific language governing permissions and
" limitations under the License.

" Usage:
"
" - cp fudgeproto.vim ~/.vim/syntax/
"
" - Add following to ~/.vim/filetype.vim:
"
"        augroup filetypedetect
"           au! BufRead,BufNewFile *.proto setfiletype fudgeproto
"        augroup END

if version < 600
    syntax clear
elseif exists("b:current_syntax")
    finish
endif

syntax case match

" Keywords
syn keyword fpKeyword       enum extends extern message namespace

" Types
syn keyword fpType          indicator
syn keyword fpType          bool boolean
syn keyword fpType          int8 byte
syn keyword fpType          int16 short
syn keyword fpType          int int32
syn keyword fpType          long int64
syn keyword fpType          float double
syn keyword fpType          string
syn keyword fpType          date time datetime

" Constants
syn keyword fpConstant      true false

" Modifiers
syn keyword fpModifier      mutable optional readonly repeated required default

" Comments
syn match   fpComment       "\/\/.*$" contains=fpTodo
syn match   fpComment       "#.*$" contains=fpTodo
syn region  fpComment       start=/\/\*/ end=/\*\// contains=fpTodo
syn keyword fpTodo          TODO XXX FIXME contained

" Strings
syn region  fpString        start=/"/ skip=/\\"/ end=/"/ contains=fpEscaped
syn match   fpEscaped       /\\./ contained

" Numbers
syn match   fpNumber        "\<\d\+\>"
syn match   fpNumber        "\<\d\+\.\d+\>"
syn match   fpNumber        "\<\d\+[eE][-+]\?\d\+\>"

syn region fpBlock start='{' end='}' transparent fold


" Highlighting rules
if version >= 508 || !exists("did_bc_syntax_inits")
    if version < 508
        let did_proto_syn_inits = 1
        command -nargs=+ HiLink hi link <args>
    else
        command -nargs=+ HiLink hi def link <args>
    endif

    HiLink fpComment        Comment
    HiLink fpConstant       Constant
    HiLink fpEscaped        Special
    HiLink fpKeyword        Keyword
    HiLink fpModifier       Operator
    HiLink fpNumber         Number
    HiLink fpString         String
    HiLink fpTodo           Todo
    HiLink fpType           Type

    delcommand HiLink
endif

let b:current_syntax = "fudgeproto"
