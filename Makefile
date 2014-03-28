OBJS=jpge.o jpgd.o encoder.o
BIN=encoder_mw
CXXFLAGS ?= -O3 -ffast-math -fno-signed-zeros

$(BIN): $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^

clean:
	rm $(OBJS) $(BIN)
