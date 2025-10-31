# loiosh
HOMEDIR = /Users/jimbo
# work
#HOMEDIR = /Users/i859240
BASEDIR = $(HOMEDIR)/Documents/code
OUTPUTDIR = ../bin
CC = g++
CC_FLAGS = -w -std=c++11 -DNDEBUG -DBASEDIR=\"$(BASEDIR)/../data/RayTracer\" -I$(BASEDIR)/zmq_widgets/Widget/src -I$(BASEDIR)/RayTracer/libRayTracer2014/src -I/usr/local/include -I/usr/local/include/freetype2
LD_FLAGS = -L/usr/local/lib -lzmq -lPNGwriter -lpng -lfreetype $(BASEDIR)/zmq_widgets/Widget/src/Widget.o

EXEC = $(OUTPUTDIR)/$(notdir $(CURDIR:%/=%))
SOURCES = $(wildcard src/*.cpp)
OBJECTS = $(SOURCES:.cpp=.o)

$(EXEC): $(OBJECTS)
	$(CC) $(LD_FLAGS) $(OBJECTS) -o $(EXEC)

%.o: %.cpp
	$(CC) -c $(CC_FLAGS) $< -o $@

clean:
	rm -f $(EXEC) $(OBJECTS)
