all:
	$(MAKE) -C src/osd
	$(MAKE) -C src/osput
	$(MAKE) -C src/osget

clean:
	$(MAKE) clean -C src/osd
	$(MAKE) clean -C src/osput
	$(MAKE) clean -C src/osget
