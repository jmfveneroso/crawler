IDIR =include
CC=g++
CFLAGS=-I. -Wall

BUILD_DIR=build
LDIR =../lib

_DEPS = crawler.hpp
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = main.o crawler.o 
OBJ = $(patsubst %,$(BUILD_DIR)/%,$(_OBJ))

$(BUILD_DIR)/%.o: src/%.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) -I$(IDIR)

$(BUILD_DIR)/crawler: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f $(BUILD_DIR)/crawler $(BUILD_DIR)/*.o *~ core $(INCDIR)/*~ 
