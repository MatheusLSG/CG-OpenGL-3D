CC = g++
FLAGS =
OPENGL = -lGL -lGLU -lglut

SOURCES = $(wildcard src/*.cpp)
HEADERS = $(wildcard lib/*.h)
OBJECTS = $(patsubst src/%.cpp, obj/%.o, $(SOURCES))

all: dir trabalhocg
	
dir:
	mkdir -p obj

trabalhocg: $(OBJECTS) 
	$(CC) $(FLAGS) $^ -o $@ $(OPENGL)

$(OBJECTS): obj/%.o: src/%.cpp $(HEADERS)
	$(CC) -c $(FLAGS) $< -o $@ 
		
clean:
	rm -rf obj
	rm trabalhocg