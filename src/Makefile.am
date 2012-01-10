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

EXTRA_DIST = simplefudgeproto.xml

AM_YFLAGS = -d
AM_LFLAGS =

noinst_HEADERS = ast.hpp		\
		 astaliaser.hpp		\
                 astdumper.hpp		\
                 astextrefs.hpp 	\
		 astextresolver.hpp	\
                 astflattener.hpp 	\
                 astgenerator.hpp 	\
                 astindex.hpp 		\
                 astindexer.hpp 	\
                 astrenamer.hpp 	\
                 astresolver.hpp 	\
                 astwalker.hpp 		\
                 codewriter.hpp 	\
                 codewriterfactory.hpp 	\
                 config.h.in 		\
                 constants.hpp 		\
                 cppheaderwriter.hpp 	\
                 cppimplwriter.hpp 	\
                 cppwriter.hpp 		\
                 cppwriterfactory.hpp 	\
                 filenamegenerator.hpp 	\
		 identifiermutator.hpp	\
                 memoryutil.hpp 	\
                 parser.hpp		\
		 stage.hpp		\
		 template.hpp

libsimplefudgeproto_a_SOURCES = ast.cpp			\
				astaliaser.cpp		\
				astdumper.cpp		\
				astextrefs.cpp		\
				astextresolver.cpp	\
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
				identifiermutator.cpp	\
				memoryutil.cpp		\
				parser.cpp		\
				protolexer.ll		\
				protoparser.yy		\
				stage.cpp

BUILT_SOURCES = protolexer.cc 	\
		protoparser.h 	\
		protoparser.cc

simplefudgeproto_SOURCES = simplefudgeproto_main.cpp
simplefudgeproto_LDADD	 = libsimplefudgeproto.a

clean-local:
	$(RM) protolexer.cc protoparser.h protoparser.cc simplefudgeproto.1

dist-hook:
	for file in $(BUILT_SOURCES) $(noinst_LIBRARIES); do  \
	    $(RM) -f $(distdir)/$$file;	  \
	done

simplefudgeproto.1: simplefudgeproto.xml
	$(top_srcdir)/scripts/manbuilder.py -i simplefudgeproto.xml -o simplefudgeproto.1 -f nroff

