CFLAGS = -Wall -g -I../libocean
LDFLAGS = -L../libocean -locean
TARGET = osd
OBJECTS = main.o

$(TARGET): $(OBJECTS)
	cc -o $(TARGET) $(OBJECTS) $(LDFLAGS)

clean:
	rm -f $(TARGET) $(OBJECTS)
