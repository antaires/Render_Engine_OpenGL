#
# Cross Platform Makefile
# Compatible with MSYS2/MINGW, Ubuntu 14.04.1 and Mac OS X
#

CXX		  	:= g++
CXX_FLAGS 	+= -Wall -Wextra -std=c++17 -g

BIN			:= bin
SRC			:= src
SOURCES 	 = main.cpp
INCLUDE1	:= include
INCLUDE2	:= src/vender/
INCLUDE3	:= src
LIB			:= lib
LIBS		 =

LIBRARIES	:= -lglew -lglfw3
EXECUTABLE	:= app
UNAME_S := $(shell uname -s)

##---------------------------------------------------------------------
## BUILD FLAGS PER PLATFORM
##---------------------------------------------------------------------

ifeq ($(UNAME_S), Linux) #LINUX
	ECHO_MESSAGE = "Linux"
	LIBS += $(LINUX_GL_LIBS) `pkg-config --static --libs glfw3 glew`

	CXXFLAGS += `pkg-config --cflags glfw3`
	CFLAGS = $(CXXFLAGS)
endif

ifeq ($(UNAME_S), Darwin) #APPLE
	ECHO_MESSAGE = "Mac OS X"
	LIBS += -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo
	LIBRARIES += -lglfw3 -lglew

	CXX = clang++
	CXXFLAGS += -Iinclude
	CFLAGS = $(CXXFLAGS)
endif

ifeq ($(OS), Windows_NT) #WINDOWS
	ECHO_MESSAGE = "MinGW"
	LIBS += -lglfw3 -lgdi32 -lopengl32 -limm32 -lglew

	CFLAGS = $(CXXFLAGS)
endif

##---------------------------------------------------------------------
## BUILD RULES
##---------------------------------------------------------------------

all: $(BIN)/$(EXECUTABLE)

run: clean all
	@./$(BIN)/$(EXECUTABLE)

$(BIN)/$(EXECUTABLE): $(SRC)/*.cpp
	@mkdir -p $(BIN)
	@$(CXX) $(CXX_FLAGS) main.cpp -I$(INCLUDE1) -I$(INCLUDE2) -I$(INCLUDE3) -L$(LIB)  $^ -o $@ $(LIBRARIES) $(LIBS)
	@echo Build complete for $(ECHO_MESSAGE)

clean:
	@rm -rf $(BIN)/* $(OBJS)