EXE := du-ftp

BIN := bin
OBJ := obj
SRC := src


INCLUDES = -I$(CURDIR)/$(SRC)
CFLAGS = -std=c++17 -pthread -Wall -Wno-unused-function
CC = g++

MAIN := $(SRC)/main.cpp
SOURCES := $(wildcard $(SRC)/**/*.cpp) $(filter-out $(MAIN), $(wildcard $(SRC)/*.cpp))
OBJECTS := $(patsubst $(SRC)/%.cpp, $(OBJ)/%.o, $(SOURCES))

.DEFAULT_GOAL = all

.PHONY: all
all: $(BIN)/$(EXE)

$(BIN)/$(EXE): $(SRC) $(OBJ) $(BIN) $(OBJECTS)
	$(CC) $(CFLAGS) $(INCLUDES) -o$@ $(MAIN) $(OBJECTS)

$(SRC):
	mkdir -p $(SRC)

$(OBJ):
	mkdir -p $(OBJ)

$(BIN):
	mkdir -p $(BIN)

$(OBJ)/%.o:	$(SRC)/%.cpp
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(INCLUDES) -c -o$@ $<

# force rebuild
.PHONY: remake
remake:	clean $(BIN)/$(EXE)


.PHONY: clean
clean:
	$(RM) $(OBJECTS)
	$(RM) $(BIN)/$(EXE)

run:
	./$(BIN)/$(EXE)	
