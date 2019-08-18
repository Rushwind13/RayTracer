# loiosh
HOMEDIR = /Users/jimbo
# work
#HOMEDIR = /Users/i859240
BASEDIR = $(HOMEDIR)/Documents/code
OUTPUTDIR = ../bin
CC = g++
CC_FLAGS = -w -I$(BASEDIR)/zmq_widgets/Widget/src -I$(BASEDIR)/RayTracer2014/libRayTracer2014/src -I/usr/local/include/freetype2
LD_FLAGS = -lzmq -lPNGwriter -lpng -lfreetype $(BASEDIR)/zmq_widgets/Widget/src/Widget.o
#LD_FLAGS = -lzmq -lPNGwriter -lpng -lfreetype -L$(BASEDIR)/zmq_widgets/bin -lWidget
#LD_FLAGS = -lSDL2 -lSDL2_image -framework OpenGL

EXEC = $(OUTPUTDIR)/$(notdir $(CURDIR:%/=%))
SOURCES = $(wildcard src/*.cpp)
OBJECTS = $(SOURCES:.cpp=.o)

$(EXEC): $(OBJECTS)
	$(CC) $(LD_FLAGS) $(OBJECTS) -o $(EXEC)

%.o: %.cpp
	$(CC) -c $(CC_FLAGS) $< -o $@

clean:
	rm -f $(EXEC) $(OBJECTS)
