CFLAGS = -ansi -Wall -g -pedantic -fPIC
TARGET = libocean.so
OBJECTS = metadata.o sha1.o

$(TARGET): $(OBJECTS)
	cc -shared -o $(TARGET) $(OBJECTS) -lssl

clean:
	rm -f $(TARGET) $(OBJECTS)
