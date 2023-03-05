INCL_PATH="../../TMEngine/src"
LINK_PATH="../build/"

build:
#   compile with a .dylib (mac os dll) NOT WORKING YET ...
#	clang -std=c++17 -Wall -DTM_DEBUG -g -I$(INCL_PATH) *.cpp -o ../build/Game -L$(LINK_PATH) -lTMEngine

#	conpile with static library .a (mac os lib)
	clang -std=c++17 -DTM_DEBUG -g -I$(INCL_PATH) *.cpp -o ../build/Game ../../TMEngine/build/tmlib.a -lglfw
run:
	../build/Game
clean:
	rm -R ../build/Game.dSYM
	rm ../build/Game
