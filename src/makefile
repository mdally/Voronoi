CC = g++
CFLAGS = -std=c++0x -c -Wall -O3
DEBUG = -g
ARFLAGS = -cvq
LDFLAGS = 
EXECUTABLE = Voronoi
SOURCES = BeachLine.cpp Cell.cpp CircleEventQueue.cpp Diagram.cpp Edge.cpp Point2.cpp Vector2.cpp VoronoiDiagramGenerator.cpp main.cpp
LIBSOURCES = 
OBJS = $(SOURCES:.cpp=.o)
LIBOBJS = $(LIBSOURCES:.cpp=.o)
LIB = 

all : $(SOURCES) $(LIB) $(EXECUTABLE)

lib : $(LIB)

$(EXECUTABLE) : $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) $(LIB) -o $@
	rm -rf $^

$(LIB) : $(LIBOBJS)
	ar $(ARFLAGS) $@ $^
	rm -rf $^

.cpp.o :
	$(CC) $(CFLAGS) $(DEBUG) $< -o $@

clean :
	rm -rf *.o $(EXECUTABLE) $(LIB)
