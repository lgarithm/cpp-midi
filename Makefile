build:
	cd src && make
test:
	./bin/midi
install: build
	mkdir -p ~/bin
	cp bin/midi ~/bin
