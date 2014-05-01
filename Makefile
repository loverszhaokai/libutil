#
# Makefile of unit test for libutil-3.0
# author: zhaokai
# date: 2013-11-28
#

GTEST_INCLUDE_DIR = $(THIRD_PARTY_DIR)/gtest/include
GMOCK_INCLUDE_DIR = $(THIRD_PARTY_DIR)/gmock/include

# Where to find user code.
THIRD_PARTY_DIR=./third_party
CURRENT_DIR=.
SRC_DIR = .

# Flags passed to the preprocessor.
# Set Google Test's header directory as a system directory, such that
# the compiler doesn't generate warnings in Google Test headers.
CPPFLAGS += -isystem $(GTEST_INCLUDE_DIR)  -isystem $(GMOCK_INCLUDE_DIR)

# Flags passed to the C++ compiler.
CXXFLAGS += -g -std=c++11 -Wall -Wextra -pthread -I./ -I./include/util/include -I/usr/local/include/libxml2 -DUNIT_TEST

CPPLIB = -lpthread -lxml2

TESTS = ./libutil_test

GTEST_HEADERS = $(GTEST_INCLUDE_DIR)/gtest/*.h \
                $(GTEST_INCLUDE_DIR)/gtest/internal/*.h

GMOCK_HEADERS = $(GMOCK_INCLUDE_DIR)/gmock/*.h \
                $(GMOCK_INCLUDE_DIR)/gmock/internal/*.h

#test source dir
SRC_DIR = ./src
LIB_DIR = ./lib


SOURCE_FILES=\
	$(wildcard $(SRC_DIR)/*.cc)

OBJ_FILES=\
	$(patsubst %.cc, %.o, $(SOURCE_FILES))

all : $(TESTS)

clean :
	rm -f libutil_test.o
	rm -f $(TESTS)
	rm -f $(OBJ_FILES)
	rm -f $(LIB_DIR)/libutil.a


$(SRC_DIR)/%.o : $(SRC_DIR)/%.cc
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(CPPLIB)  -c $< -o $@

$(LIB_DIR)/libutil.a : $(OBJ_FILES)
	$(AR) $(ARFLAGS) $@ $^

libutil_test.o : $(CURRENT_DIR)/libutil_test.cc $(GTEST_HEADERS) $(GMOCK_HEADERS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(CPPLIB) -c $(CURRENT_DIR)/libutil_test.cc


$(TESTS) : libutil_test.o $(LIB_DIR)/libutil.a $(OBJ_FILES) \
	$(THIRD_PARTY_DIR)/gtest/lib/gtest.a $(THIRD_PARTY_DIR)/gmock/lib/gmock.a 
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(CPPLIB) $^ -o $@
