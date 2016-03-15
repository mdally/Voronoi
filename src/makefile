CC = g++
CFLAGS = -std=c++0x -c -Wall
ARFLAGS = -cvq
DEBUG = -g
LDFLAGS = 
EXECUTABLE = Voronoi
SOURCES = test.cpp
LIBSOURCES = Vector2.cpp Point2.cpp DCEL.cpp Geometry.cpp Voronoi.cpp BeachLine.cpp
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
	rm -rf *o $(EXECUTABLE) $(LIB)
