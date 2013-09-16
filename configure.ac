# Copyright (C) 2011 - 2011, Vrai Stacey.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

### Need modern autoconf, only 2.60+ has the AC_TYPE_INT*_T macros
AC_PREREQ([2.60])

AC_INIT([SimpleFudgeProto], [0.4.4], [vrai.stacey@gmail.com])
AC_CONFIG_AUX_DIR(config)
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADER(src/config.h)
AC_CONFIG_FILES([Makefile \
                 src/Makefile \
                 tests/Makefile])
AM_INIT_AUTOMAKE([1.12.1])

### Make sure we're in the right directory
AC_CONFIG_SRCDIR(./src/protoparser.yy)

### Make sure that the necessary programs are available
AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_LIBTOOL
AC_PROG_YACC
AM_PROG_LEX
AC_LANG_PUSH([C++])

### Check for functions that some compilers lack (or name differently)
AC_CHECK_FUNC(gmtime_r, AC_DEFINE(HAS_GMTIME_R, 1, [Define to 1 if gmtime_r is available.]))
AC_CHECK_FUNC(_gmtime64_s, AC_DEFINE(HAS__GMTIME_S, 1, [Define to 1 if _gmtime_s is available.]))
AC_CHECK_FUNC(getpid, AC_DEFINE(HAS_GETPID, 1, [Define to 1 if getpid is available.]))

### Check that FudgeC and FudgeCpp are present
AC_CHECK_HEADER(fudge/fudge.h,
                [AC_DEFINE(HAS_FUDGE_C, 1, [Define to 1 if <fudge/fudge.h> is present])],
                [AC_MSG_ERROR([Cannot find <fudge/fudge.h> - Is the FudgeC library installed?])])
AC_CHECK_HEADER(fudge-cpp/fudge.hpp,
                [AC_DEFINE(HAS_FUDGE_CPP, 1, [Define to 1 if <fudge-cpp/fudge.hpp> is present])],
                [AC_MSG_ERROR([Cannot find <fudge-cpp/fudge.hpp> - Is the FudgeCpp library installed?])])

### Warnings up to eleven
AX_CHECK_COMPILER_FLAGS(-Wall,CXXFLAGS+=" -Wall")

AC_LANG_POP
AC_OUTPUT

