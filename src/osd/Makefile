CFLAGS = -Wall -g -I../libocean -I../wool
LDFLAGS = -L../libocean -locean
TARGET = osd
OBJECTS = main.o

$(TARGET): $(OBJECTS)
	cc -o $(TARGET) $(OBJECTS) $(LDFLAGS)

clean:
	rm -f $(TARGET) $(OBJECTS)
