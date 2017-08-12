# OBJS specifies which files to compile as part of the project
OBJS = nosupergoldrule.cpp boardgraph.cpp boardnode.cpp islandcountrule.cpp nochitstouchingrule.cpp nopondsrule.cpp abstractrule.cpp config.cpp shader.cpp draw.cpp main.cpp

# CC specifies which compiler we're using
CC = g++

# INCLUDE_PATHS specifies the additional include paths we'll need
INCLUDE_PATHS = -I/usr/local/include -I/opt/X11/include

# LIBRARY_PATHS specifies the additional library paths we'll need
LIBRARY_PATHS = -L/usr/local/lib -I/opt/X11/lib

# COMPILER_FLAGS specifies the additional compilation options we're using
# -w suppresses all warnings
COMPILER_FLAGS = -w -std=c++14 -DRAPIDJSON_HAS_CXX11_RANGE_FOR=1

# LINKER_FLAGS specifies the libraries we're linking against
# Cocoa, IOKit, and CoreVideo are needed for static GLFW3.
LINKER_FLAGS = -framework OpenGL -lglfw -lglew -lpng

# OBJ_NAME specifies the name of our exectuable
OBJ_NAME = catangen

#This is the target that compiles our executable
all : $(OBJS)
	$(CC) $(OBJS) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)
