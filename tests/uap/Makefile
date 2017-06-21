LDFLAGS=-lboost_regex -lyaml-cpp
CXXFLAGS=-std=c++0x -Wall -Werror -fPIC -g -O3 -fno-strict-aliasing -fno-omit-frame-pointer -v

all: clean test

# wildcard object build target
%.o: %.cpp
	$(CXX)  -c $(CXXFLAGS) $*.cpp -o $*.o
	@$(CXX) -MM $(CXXFLAGS) $*.cpp > $*.d

uaparser_cpp: libuaparser_cpp.a

libuaparser_cpp.a: UaParser.o
	ar rcs $@ $^

libuaparser_cpp.so: UaParser.o
	$(LD) $< -shared $(LDFLAGS) -o $@

UaParserTest: libuaparser_cpp.a UaParserTest.o
	$(CXX) $^ -o $@ libuaparser_cpp.a $(LDFLAGS) -lgtest -lpthread

test: UaParserTest
	./UaParserTest

# clean everything generated
clean:
	find . -name "*.o" -exec rm -rf {} \;
	find . -name "*.d" -exec rm -rf {} \;
	rm -f UaParserTest *.a *.so

# automatically include the generated *.d dependency make targets
# that are created from the wildcard %.o build target above
-include $(OBJS:.o=.d)
