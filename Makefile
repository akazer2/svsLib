LIBOPENSLIDE = -L/opt/openslide/lib
LIBTIFF = -L/usr/local/bigtiff/lib -L/usr/lib
LIBDIR = $(LIBOPENSLIDE) $(LIBTIFF) -lopenslide -lglib-2.0 -lopenjpeg -lcurl

INCLUDEOPENSLIDE = -I/opt/openslide/include/openslide
INCLUDETIFF = -I/usr/local/bigtiff/include
INCLUDECURRENT = -I./include
INCDIR = $(INCLUDECURRENT) $(INCLUDEOPENSLIDE) $(INCLUDETIFF)

CFLAGS = -c -O2

TARGET = svslib
CC = g++

all: lib driver
	$(CC) -g -O2 -o $(TARGET) $(LIBDIR) lib.o driver.o

driver: src/driver.cpp
	$(CC) $(LIBDIR) $(INCDIR) $(CFLAGS) src/driver.cpp -o driver.o

lib: src/svsLibrary.cpp
	$(CC) $(LIBDIR) $(INCDIR) $(CFLAGS) src/svsLibrary.cpp -o lib.o

clean:
	rm -f *.o $(TARGET)
