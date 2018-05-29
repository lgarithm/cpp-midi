binary:
	mkdir -p bin
	g++ -std=c++0x src/midi.cpp -o bin/midi

test:
	./bin/midi

install: binary
	install -v bin/midi ~/local/bin/
