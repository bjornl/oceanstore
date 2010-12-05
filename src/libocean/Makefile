CFLAGS = -ansi -Wall -g -pedantic -fPIC -I.
TARGET = libocean.so
OBJECTS = metadata.o sha1.o udp.o

$(TARGET): $(OBJECTS)
	cc -shared -o $(TARGET) $(OBJECTS) -lssl

clean:
	rm -f $(TARGET) $(OBJECTS)
