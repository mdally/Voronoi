CC = g++
CFLAGS = -std=c++0x -c -Wall -O3
DEBUG = 
ARFLAGS = -cvq
LDFLAGS = 
EXECUTABLE = bin/VoronoiDiagramGenerator
SOURCES = main.cpp
LIBSOURCENAMES = BeachLine.cpp Cell.cpp CircleEventQueue.cpp Diagram.cpp Edge.cpp Point2.cpp Vector2.cpp VoronoiDiagramGenerator.cpp
LIBSOURCEDIR = src/
LIBSOURCES = $(addprefix $(LIBSOURCEDIR), $(LIBSOURCENAMES))
OBJS = $(SOURCES:.cpp=.o)
LIBOBJS = $(LIBSOURCES:.cpp=.o)
LIB = $(EXECUTABLE).a

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
	rm -rf $(LIBSOURCEDIR)*.o $(EXECUTABLE) $(LIB)
