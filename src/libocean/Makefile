target = libocean.so
objects = metadata.o

$(target): $(objects)
	cc -shared -o $(target) $(objects)

clean:
	rm -f $(target) $(objects)
