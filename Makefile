CXX = clang++

DEBUG = -g

CXXFLAGS += $(DEBUG)
CXXFLAGS += -I/usr/local/include
CXXFLAGS += -DIB_USE_STD_STRING -DKXVER=3
CXXFLAGS += -m32
CXXFLAGS += -stdlib=libc++ -std=c++11
#CXXFLAGS += -fPIC -shared
CXXFLAGS += -bundle -undefined dynamic_lookup
CXXFLAGS += -Wall

OUT_DIR = ../kdb_tick
BIN = feed_handler.so

LDFLAGS = $(DEBUG) 
LDLIBS += -L/usr/local/lib -ltwsapi -lpthread

OBJ = feed_handler.cpp
DEPS = $(OBJ) feed_handler.h ib_wrapper.h k.h

$(OUT_DIR)/$(BIN): $(DEPS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(LDLIBS) $(OBJ) -o $@

clean:
	rm -f $(OUT_DIR) *.o *.gch *.dSYM !(c32.o)