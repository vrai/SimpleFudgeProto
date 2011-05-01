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

bin_PROGRAMS     = simplefudgeproto
noinst_LIBRARIES = libsimplefudgeproto.a

man1_MANS = simplefudgeproto.1

AM_YFLAGS = -d
AM_LFLAGS =

libsimplefudgeproto_a_SOURCES = ast.cpp			\
				astdumper.cpp		\
				astextrefs.cpp		\
				astflattener.cpp	\
				astgenerator.cpp	\
				astindex.cpp		\
				astindexer.cpp		\
				astrenamer.cpp		\
				astresolver.cpp		\
				astwalker.cpp		\
				codewriter.cpp		\
				codewriterfactory.cpp	\
				cppwriter.cpp		\
				cppheaderwriter.cpp	\
				cppimplwriter.cpp	\
				cppwriterfactory.cpp	\
				filenamegenerator.cpp	\
				memoryutil.cpp		\
				parser.cpp		\
				protolexer.ll		\
				protoparser.yy		\
				stage.cpp

simplefudgeproto_SOURCES = simplefudgeproto_main.cpp
simplefudgeproto_LDADD	 = libsimplefudgeproto.a

# Automake's dependency handling requires a little help
protolexer.o: protolexer.cc protoparser.h

clean-local:
	$(RM) protolexer.cc protoparser.h protoparser.cc


