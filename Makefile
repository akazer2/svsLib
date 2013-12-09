LIBOPENSLIDE = -L/usr/local/lib
#LIBTIFF = -L/usr/local/bigtiff/lib -L/usr/lib
LIBDIR = $(LIBOPENSLIDE) $(LIBTIFF) -ltiff -lopenslide -lglib-2.0 -lopenjpeg -lcurl

INCLUDEOPENSLIDE = -I/usr/local/include/openslide
INCLUDETIFF = -I/include
INCLUDECURRENT = -I./include
INCDIR = $(INCLUDECURRENT) $(INCLUDEOPENSLIDE) $(INCLUDETIFF)

CFLAGS = -c -O2

TARGET = tilerlib
CC = g++

all: lib driver
	$(CC) -g -O2 -o $(TARGET) $(LIBDIR) lib.o driver.o

driver: src/driver.cpp
	$(CC) $(LIBDIR) $(INCDIR) $(CFLAGS) src/driver.cpp -o driver.o

lib: src/tilerLibrary.cpp
	$(CC) $(LIBDIR) $(INCDIR) $(CFLAGS) src/tilerLibrary.cpp -o lib.o

clean:
	rm -f *.o $(TARGET)
