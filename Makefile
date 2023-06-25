# Tested on AlmaLinux 9.2, clang version 15.0.7
# Add these subdirectories and files:
# glad (https://glad.dav1d.de)
#   glad.c (renamed to glad.cpp)
#   glad.h
#   khrplatform.h
# glfw (https://www.glfw.org)
#   glfw3.h
#   libglfw3.a
# glm (https://glm.g-truc.net/0.9.9/index.html)

CXX := clang++

CXXFLAGS := -g -ggdb -O2 -Wall -fPIC

PROGRAM := meshview

INC := -I.

LIB := -L./glfw -lglfw3 \
       -lGL -lGLU -lX11 -lXrandr -lXinerama -lXi -lXcursor \
       -lrt -lm -ldl -lpthread

OBJ := main.o \
       glad/glad.o \
       mesh.o \
       shader.o

.SUFFIXES:
.SUFFIXES: .o .cpp .hpp .h

# Compile
.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(INC) -o $@ $<

all: $(PROGRAM)

# Link
$(PROGRAM): $(OBJ)
	$(CXX) $(OBJ) -o $@ $(LIB)

install: $(PROGRAM)
	echo "no install"

clean:
	rm -f $(OBJ)
	rm -f $(PROGRAM)
