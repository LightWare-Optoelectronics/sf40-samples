BIN=bin
CPPFLAGS=g++ -O3 -I.
LDFLAGS=g++
LDLIBS=-lrt
build_folder := $(shell mkdir -p $(BIN))

output:	$(BIN)/main.o $(BIN)/lwSerialPortLinux.o $(BIN)/platformLinux.o $(BIN)/lwNx.o
	$(LDFLAGS) $(BIN)/main.o $(BIN)/lwSerialPortLinux.o $(BIN)/platformLinux.o $(BIN)/lwNx.o -o $(BIN)/sf40_example $(LDLIBS)

$(BIN)/main.o: main.cpp
	$(CPPFLAGS) -c main.cpp -o $(BIN)/main.o

$(BIN)/lwNx.o: lwNx.cpp
	$(CPPFLAGS) -c lwNx.cpp -o $(BIN)/lwNx.o

$(BIN)/lwSerialPortLinux.o: linux/lwSerialPortLinux.cpp
	$(CPPFLAGS) -c linux/lwSerialPortLinux.cpp -o $(BIN)/lwSerialPortLinux.o

$(BIN)/platformLinux.o: linux/platformLinux.cpp
	$(CPPFLAGS) -c linux/platformLinux.cpp -o $(BIN)/platformLinux.o

clean:
	-rm -r $(BIN)