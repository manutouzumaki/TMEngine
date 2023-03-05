INCL_PATH=libs/glad/include -Ilibs/stb_image

build:
#   compile to a .dylib (mac os dll) NOT WORKING YET ...
#	clang -std=c++17 -Wall -DTM_DEBUG -g -dynamiclib -I$(INCL_PATH) *.cpp libs/glad/src/glad.cpp -o /Users/manuelcabrerizo/Dev/tmLibs/Game/build/libTMEngine.dylib -lglfw

#	conpile to static library .a (mac os lib)
	clang \
	-std=c++17 \
	-c -DTM_DEBUG -g \
	-I$(INCL_PATH) \
	*.cpp utils/*cpp libs/glad/src/glad.cpp \
	&& ar r ../build/tmlib.a *.o
	rm *o
clean:
# 	this if for dylib build
#	rm -R ../../Game/build/libTMEngine.dylib.dSYM
#	rm ../../Game/build/libTMEngine.dylib

#	this is for a build
	rm ../build/tmlib.a
