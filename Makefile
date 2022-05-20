OBJECTS = wolf.o projectile.o pig.o element.o world.o logic.o main.o
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra
SLD = `sdl2-config --libs --cflags` -ggdb3 -O0 --std=c99 -Wall -lSDL2_image -lSDL2_ttf -lm
NAME = pooyan

all: pooyan

pooyan: $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(NAME) $(SLD)

main.o: main.cpp
	$(CXX) $(CXXFLAGS) -c main.cpp

world.o: world.cpp world.hpp
	$(CXX) $(CXXFLAGS) -c world.cpp

logic.o: logic.cpp logic.hpp
	$(CXX) $(CXXFLAGS) -c logic.cpp

element.o: element.cpp element.hpp
	$(CXX) $(CXXFLAGS) -c element.cpp

pig.o: pig.cpp pig.hpp
	$(CXX) $(CXXFLAGS) -c pig.cpp

projectile.o: projectile.cpp projectile.hpp
	$(CXX) $(CXXFLAGS) -c projectile.cpp

wolf.o: wolf.cpp wolf.hpp
	$(CXX) $(CXXFLAGS) -c wolf.cpp

clean:
	rm -f *.o
	rm -f $(NAME)
