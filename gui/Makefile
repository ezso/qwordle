CC=gcc
CFLAGS=-Wall -g -I include
CARGS=`pkg-config --cflags --libs gtk4` -L. -lwordle
BIN_FILES=src/game.c src/gui.c src/settings.c src/common.c
BINS=gui

all: gui

gui: $(BIN_FILES)
# link does not always work in the VM
	cp -rf ../include ./
	make bin/libwordle.so -C ..
	cp -f ../bin/libwordle.so libwordle.so
	$(CC) $(CARGS) $(CFLAGS) -o $@ $(BIN_FILES)

clean:
	$(RM) *.o $(BINS)