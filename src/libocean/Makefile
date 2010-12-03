target = libocean.so
objects = metadata.o sha1.o

$(target): $(objects)
	cc -shared -o $(target) $(objects) -lssl

clean:
	rm -f $(target) $(objects)
