all:
	$(MAKE) -C lib/wool
	$(MAKE) -C src/libocean
	$(MAKE) -C src/osd
	$(MAKE) -C src/osput
	$(MAKE) -C src/osget

clean:
	$(MAKE) clean -C src/osd
	$(MAKE) clean -C src/osput
	$(MAKE) clean -C src/osget
	$(MAKE) clean -C src/libocean
	$(MAKE) clean -C lib/wool
