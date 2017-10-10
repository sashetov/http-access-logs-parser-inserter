LDFLAGS=-lboost_regex -lyaml-cpp -lgtest -lpthread
CXXFLAGS=-std=c++11 -Wall -Werror -fPIC -g -O3 -fno-strict-aliasing -fno-omit-frame-pointer -v
all: clean test uap

UaParserTest.o: UaParserTest.cpp
	$(CXX) -c $(CXXFLAGS) UaParserTest.cpp -o UaParserTest.o
	$(CXX) -MM $(CXXFLAGS) UaParserTest.cpp > UaParserTest.d

UaParser.o: UaParser.cpp
	$(CXX) -c $(CXXFLAGS) UaParser.cpp -o UaParser.o
	$(CXX) -MM $(CXXFLAGS) UaParser.cpp > UaParser.d

uaparser_cpp: libuaparser_cpp.a

libuaparser_cpp.a: UaParser.o
	ar rcs libuaparser_cpp.a UaParser.o

libuaparser_cpp.so: UaParser.o
	$(LD) UaParser.o  -shared -lboost_regex -lyaml-cpp -lgtest -lpthread -o libuaparser_cpp.so #fails with my version of g++ (4.9.2 compiled from source )

UaParserTest: libuaparser_cpp.a UaParserTest.o
	$(CXX) $(CXXFLAGS) libuaparser_cpp.a UaParserTest.o -o UaParserTest libuaparser_cpp.a $(LDFLAGS)

uap:
	$(CXX) $(CXXFLAGS) uap.cpp -o uap $(LDFLAGS)

test: UaParserTest
	./UaParserTest

clean:
	find . -name "*.o" -exec rm -rf {} \;
	find . -name "*.d" -exec rm -rf {} \;
	rm -f UaParserTest *.a *.so uap

-include $(OBJS:.o=.d)
