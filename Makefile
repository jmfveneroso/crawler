TOP := $(shell pwd)
ARCH := $(shell getconf LONG_BIT)
UNAME_S := $(shell uname -s)
MKDIR_P := $(shell mkdir -p build)

IDIR=include
CC=g++
CPP_FLAGS=-I. -W -Wall -Werror -pedantic -std=c++11 -O3 -D_FILE_OFFSET_BITS=64

CPP_FLAGS_32 := -Dx86 -m32
CPP_FLAGS_64 := -Dx64 -m64
CPP_FLAGS += $(CPP_FLAGS_$(ARCH))

BUILD_DIR=build
LDIR=lib

_DEPS = injector.hpp crawler.hpp fetcher.hpp scheduler.hpp storage.hpp logger.hpp
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = main.o injector.o crawler.o fetcher.o scheduler.o storage.o logger.o
OBJ = $(patsubst %,$(BUILD_DIR)/%,$(_OBJ))

# $(info $$TOP is [${TOP}])

# Chilkat.
ifeq ($(UNAME_S),Linux)
   INC += -Ilib/chilkat-9.5.0-x86_64-linux-gcc/include
   LDLIBS += $(LDIR)/chilkat-9.5.0-x86_64-linux-gcc/lib/libchilkat-9.5.0.a -pthread
else ifeq ($(UNAME_S),Darwin)
   INC += -Ilib/chilkat-9.5.0-macosx-cpp11/include
   LDLIBS += $(LDIR)/chilkat-9.5.0-macosx-cpp11/libStatic/libchilkat_x86_64.a
endif

$(BUILD_DIR)/%.o: src/%.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CPP_FLAGS) -I$(IDIR) $(INC)

$(BUILD_DIR)/crawler: $(OBJ)
	$(CC) -o $@ $^ $(LDLIBS) $(CPP_FLAGS)

.PHONY: clean

clean:
	rm -f $(BUILD_DIR)/crawler $(BUILD_DIR)/chilkat_sample $(BUILD_DIR)/*.o *~ core $(INCDIR)/*~ 
