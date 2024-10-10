CXX = g++
CXXFLAGS = -std=c++17 -pthread -Wall # -fprofile-arcs -ftest-coverage
LDFLAGS = -lgcov

SOURCES = ActiveObject.cpp Graph.cpp KruskalMST.cpp MSTFactory.cpp PrimMST.cpp Server.cpp ThreadPool.cpp
OBJECTS = $(SOURCES:.cpp=.o)

all: server

server: $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o server 

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f *.o *.gcno *.gcda server
