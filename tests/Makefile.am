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

TESTS = test_identifiermutator	\
	test_parser		\
	test_flatmessage	\
	test_nestedmessage	\
	test_safemessage	\
	test_arraymessage	\
	test_optobjects		\
	test_deepinheritance	\
	test_opaquemessage

check_PROGRAMS = $(TESTS)

noinst_HEADERS = encoderutils.hpp	\
		 simpletest.hpp

INCLUDES = -I$(top_srcdir)/src

FRAMEWORK_SOURCE = simpletest.cpp
EXTRA_DIST = test_files

test_identifiermutator_SOURCES = test_identifiermutator.cpp	\
				 $(FRAMEWORK_SOURCE)
test_identifiermutator_LDADD   = $(top_builddir)/src/libsimplefudgeproto.a -lfudgecpp

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

test_safemessage_SOURCES = built_safe_safemessageone.cpp	\
			   built_safe_safemessagetwo.cpp	\
			   built_unsafe_safemessageone.cpp	\
			   built_unsafe_safemessagetwo.cpp	\
			   test_safemessage.cpp			\
			   $(FRAMEWORK_SOURCE)
test_safemessage_LDADD = $(top_builddir)/src/libsimplefudgeproto.a -lfudgecpp

test_arraymessage_SOURCES = built_array_arraymessage.cpp	\
			    built_array_elementmessage.cpp	\
			    test_arraymessage.cpp		\
			    $(FRAMEWORK_SOURCE)
test_arraymessage_LDADD = $(top_builddir)/src/libsimplefudgeproto.a -lfudgecpp

test_optobjects_SOURCES = built_optobjects_inner.cpp		\
			  built_optobjects_outer.cpp		\
			  test_optobjectsmessage.cpp		\
			  $(FRAMEWORK_SOURCE)
test_optobjects_LDADD   = $(top_builddir)/src/libsimplefudgeproto.a -lfudgecpp

test_deepinheritance_SOURCES = built_basemessage.cpp		\
			       built_intermediatemessage.cpp	\
			       built_topmessage.cpp		\
			       test_deepinheritance.cpp		\
			       $(FRAMEWORK_SOURCE)
test_deepinheritance_LDADD   = $(top_builddir)/src/libsimplefudgeproto.a -lfudgecpp

test_opaquemessage_SOURCES = built_opaqueholdermessage.cpp	\
			     test_opaquemessage.cpp		\
			     $(FRAMEWORK_SOURCE)
test_opaquemessage_LDADD   = $(top_builddir)/src/libsimplefudgeproto.a -lfudgecpp

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
built_safe_safemessageone.cpp:
	$(PROTO_GENERATOR) ./test_files/safe.proto
built_safe_safemessagetwo.cpp:
	$(PROTO_GENERATOR) ./test_files/safe.proto
built_unsafe_safemessageone.cpp:
	$(PROTO_GENERATOR) -u -a built.safe:built.unsafe ./test_files/safe.proto
built_unsafe_safemessagetwo.cpp:
	$(PROTO_GENERATOR) -u -a built.safe:built.unsafe ./test_files/safe.proto
built_array_elementmessage.cpp:
	$(PROTO_GENERATOR) ./test_files/array.proto
built_array_arraymessage.cpp:
	$(PROTO_GENERATOR) ./test_files/array.proto
built_optobjects_inner.cpp:
	$(PROTO_GENERATOR) ./test_files/optional_objects.proto
built_optobjects_outer.cpp:
	$(PROTO_GENERATOR) ./test_files/optional_objects.proto
built_basemessage.cpp:
	$(PROTO_GENERATOR) ./test_files/deep_inheritance.proto
built_intermediatemessage.cpp:
	$(PROTO_GENERATOR) ./test_files/deep_inheritance.proto
built_topmessage.cpp:
	$(PROTO_GENERATOR) ./test_files/deep_inheritance.proto
built_opaqueholdermessage.cpp:
	$(PROTO_GENERATOR) ./test_files/opaque.proto

clean-local:
	$(RM) -f *.log
	$(RM) -f *.dat
	$(RM) -f built_*.?pp

dist-hook:
	$(RM) -f $(distdir)/built_*.?pp

