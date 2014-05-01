	#
# Makefile of util sample
# author: zhaokai
# date: 2014-04-27
#

# Flags passed to the C++ compiler.
CXXFLAGS += -g -std=c++11 -Wall -Wextra -I./ -I../include

CPPLIB = -L../lib -lutil

TESTS = string_number_conversions_sample

#test source dir

all : $(TESTS)

clean :
	rm -f $(TESTS)
	rm -f ./*.o

string_number_conversions_sample.o : string_number_conversions_sample.cc 
	$(CXX) $(CXXFLAGS) $(CPPLIB)  -c $< -o $@

$(TESTS) : string_number_conversions_sample.o ../lib/libutil.a
	$(CXX) $(CXXFLAGS) $(CPPLIB) $^ -o $@
