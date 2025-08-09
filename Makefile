OBJECTS = main.o chip8.o graphics.o
TARGET = c8emu
FLAGS = -Wall -lSDL3

$(TARGET): $(OBJECTS)
	g++ -o $(TARGET) $(OBJECTS) ${FLAGS}
	

main.o: main.cc chip8.hh graphics.hh
	g++ -c $<
chip8.o: chip8.cc chip8.hh
	g++ -c $<
graphics.o: graphics.cc graphics.hh
	g++ -c $<

.PHONY: clean
clean:
	-rm *.o $(TARGET)
