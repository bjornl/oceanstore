CFLAGS = -Wall -g -fPIC -I. -I../wool
LDFLAGS = -lssl -lpthread ../wool/wool.o
TARGET = libocean.so
OBJECTS = metadata.o sha1.o udp.o storage.o pipeline.o engine.o protocol.o

$(TARGET): $(OBJECTS)
	cc -shared -o $(TARGET) $(OBJECTS) $(LDFLAGS)

clean:
	rm -f $(TARGET) $(OBJECTS)
