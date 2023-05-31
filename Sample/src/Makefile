INCL_PATH="../../TMEngine/src"
LINK_PATH="../../TMEngine/build"

build_win32:
	cl \
	/FC /Od /Zi /WX /EHsc /wd4668 /wd4100 /wd4062 /wd4820 /wd5045 /wd4324 /wd4711 /wd4710 /wd5220 /wd4191 /wd4255 /wd4201 /wd5039 /wd4514 /wd4587 /wd4365 /wd5219 \
	-DTM_WIN32 -DTM_DEBUG \
	-I$(INCL_PATH) \
	*.cpp systems/*.cpp \
	/Fo../build/ /Fe../build/game.exe /Fm../build/ /Fd../build/ \
	-link tmlib.lib /LIBPATH:$(LINK_PATH) \
	/SUBSYSTEM:CONSOLE

run_win32:
	../build/Game.exe
	
clean_win32:
	del ../build/Game.exe

build_macos:
#   compile with a .dylib (mac os dll) NOT WORKING YET ...
#	clang -std=c++17 -Wall -DTM_DEBUG -g -I$(INCL_PATH) *.cpp -o ../build/Game -L$(LINK_PATH) -lTMEngine

#	conpile with static library .a (mac os lib)
	clang -std=c++17 -DTM_DEBUG -DTM_MACOS -g -I$(INCL_PATH) *.cpp -o ../build/Game ../../TMEngine/build/tmlib.a -lglfw
run_macos:
	../build/Game
clean_macos:
	rm -R ../build/Game.dSYM
	rm ../build/Game
