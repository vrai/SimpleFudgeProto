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

TESTS = test_parser		\
	test_flatmessage	\
	test_nestedmessage

check_PROGRAMS = $(TESTS)

noinst_HEADERS = simpletest.hpp

INCLUDES = -I$(top_srcdir)/src

FRAMEWORK_SOURCE = simpletest.cpp
EXTRA_DIST = test_files

test_parser_SOURCES = test_parser.cpp		\
		      $(FRAMEWORK_SOURCE)
test_parser_LDADD   = $(top_builddir)/src/libsimplefudgeproto.a -lfudgecpp

test_flatmessage_SOURCES = built_combined_flatmessage.cpp	\
			   built_flatmessageone.cpp 		\
			   built_flatmessagetwo.cpp 		\
			   test_flatmessage.cpp			\
			   $(FRAMEWORK_SOURCE)
test_flatmessage_LDADD  = $(top_builddir)/src/libsimplefudgeproto.a -lfudgecpp

test_nestedmessage_SOURCES = built_combined_flatmessage.cpp     \
			     built_flatmessageone.cpp 		\
			     built_flatmessagetwo.cpp 		\
			     built_nestedmessageone.cpp		\
			     built_complex_nestedmessagetwo.cpp	\
			     test_nestedmessage.cpp	        \
			     $(FRAMEWORK_SOURCE)
test_nestedmessage_LDADD = $(top_builddir)/src/libsimplefudgeproto.a -lfudgecpp

# Build rules for generated code
PROTO_GENERATOR = $(top_srcdir)/src/simplefudgeproto -l cpp

built_flatmessageone.cpp:
	$(PROTO_GENERATOR) ./test_files/flat.proto
built_flatmessagetwo.cpp:
	$(PROTO_GENERATOR) ./test_files/flat.proto
built_combined_flatmessage.cpp:
	$(PROTO_GENERATOR) ./test_files/flat.proto
built_nestedmessageone.cpp:
	$(PROTO_GENERATOR) ./test_files/nested.proto
built_complex_nestedmessagetwo.cpp:
	$(PROTO_GENERATOR) ./test_files/nested.proto

clean-local:
	$(RM) -f *.log
	$(RM) -f *.dat
	$(RM) -f built_*.?pp

