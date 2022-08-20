# the compiler: gcc for C program, define as g++ for C++
# CXX = g++
CXX = clang++

# Sourc tree dirs
SRC_DIR := src
LIB_DIR := lib
INC_DIR := include
BIN_DIR := bin
OBJ_DIR := obj

# compiler flags:
#  -g     - this flag adds debugging information to the executable file
#  -Wall  - this flag is used to turn on most compiler warnings
CXX_FLAGS  =-Wall -Wextra -pedantic -Weffc++ -Wsign-conversion -Wshadow
CXX_COMMON =-std=c++17 -fPIC -pthread
EXE_FLAGS =-O2 -s -DNDEBUG

# include directories
INC_FLAGS = -I$(INC_DIR)
LIB_FLAGS = -L$(LIB_DIR)
LIBS = -lm

# The build target
TARGET = symlink++
EXE = $(BIN_DIR)/$(TARGET)

# Internal variables.
# -----------------------------

# The build helpers
# helpers = MyCustomUtilities.h
# utilities=myUtilities.cpp myDebugHelper.cpp myFileTools.cpp myParticles.cpp

# Source files and objects
main=main.cpp
MAIN_OBJ=$(OBJ_DIR)/main.o
objects = $(subst .cpp,.o,$(sources))
SRC := $(wildcard $(SRC_DIR)/*.cxx )
OBJ := $(SRC:$(SRC_DIR)/%.cxx=$(OBJ_DIR)/%.o)
DEPS := $(wildcard $(INC_DIR)/*.hpp)

.PHONY: all install clean cleanall trash trashall

all: $(EXE)

install: all
	cp $(EXE) ~/.local/bin/$(TARGET)

static: archive | $(BIN_DIR)
	$(CXX) -o $(EXE) $(EXE_FLAGS) $(CXX_COMMON) $(CXX_FLAGS) $(MAIN_OBJ) $(LIB_FLAGS) -lsymlinkpp

archive: $(OBJ) $(MAIN_OBJ) | $(LIB_DIR)
	ar rcv $(LIB_DIR)/libsymlinkpp.a $(OBJ)

$(EXE): $(OBJ) $(MAIN_OBJ) | $(BIN_DIR)
	$(CXX) -o $(EXE) $(EXE_FLAGS) $(CXX_COMMON) $(CXX_FLAGS) $(LIB_FLAGS) $(OBJ) $(MAIN_OBJ) $(LIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cxx $(DEPS) | $(OBJ_DIR)
	$(CXX) -o $@ $(CXX_COMMON) $(CXX_FLAGS)\
	 $(INC_FLAGS) -c $<

$(MAIN_OBJ): $(SRC_DIR)/$(main) $(DEPS) | $(OBJ_DIR)
	$(CXX) -o $@ $(CXX_COMMON) $(CXX_FLAGS)\
	 $(INC_FLAGS) -c $<

$(BIN_DIR):
	mkdir -p $@

$(OBJ_DIR):
	mkdir -p $@

$(LIB_DIR):
	mkdir -p $@

trash:
	trash $(OBJ_DIR)

trashall:
	trash $(OBJ_DIR) $(BIN_DIR)

clean:
	rm -rvf $(OBJ_DIR)

cleanall:
	rm -rvf $(OBJ_DIR) $(BIN_DIR)
