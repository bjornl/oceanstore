CFLAGS = -I../libocean
LDFLAGS = -L../libocean -locean
TARGET = osput
OBJECTS = main.o

$(TARGET): $(OBJECTS)
	cc -o $(TARGET) $(OBJECTS) $(LDFLAGS)

clean:
	rm -f $(TARGET) $(OBJECTS)
