INCL_PATH_WIN32=libs/stb_image
INCL_PATH_MACOS=libs/glad/include -Ilibs/stb_image -Ilibs/glext

build_win32:
	cl \
	/FC /Od /Zi /WX /EHsc /wd4668 /wd4100 /wd4062 /wd4820 /wd5045 /wd4324 /wd4711 /wd4710 /wd5220 /wd4191 /wd4255 /wd4201 /wd5039 /wd4514 /wd4587 /wd4365 /wd5219 \
	-DTM_WIN32 -DTM_DEBUG \
	-I$(INCL_PATH_WIN32) \
	*.cpp utils/*.cpp platform_win32/*.cpp \
	/Fo../build/ /Fe../build/tmlib.dll /Fm../build/ /Fd../build/ \
	-link User32.lib Gdi32.lib Winmm.lib Ole32.lib Xinput.lib Xaudio2.lib d3d11.lib d3dcompiler.lib \
	-DLL
	copy ..\build\tmlib.dll ..\..\Game\build
	copy ..\build\tmlib.pdb ..\..\Game\build
clean_win32:
	del ..\build\tmlib.dll
	del ..\build\tmlib.exp
	del ..\build\tmlib.lib

build_macos:
#   compile to a .dylib (mac os dll) NOT WORKING YET ...
#	clang -std=c++17 -Wall -DTM_DEBUG -g -dynamiclib -I$(INCL_PATH) *.cpp libs/glad/src/glad.cpp -o /Users/manuelcabrerizo/Dev/tmLibs/Game/build/libTMEngine.dylib -lglfw

#	conpile to static library .a (mac os lib)
	clang \
	-std=c++17 \
	-c -DTM_DEBUG -DTM_MACOS -g \
	-I$(INCL_PATH_MACOS) \
	*.cpp utils/*.cpp platform_macos/*.cpp libs/glad/src/glad.cpp \
	&& ar r ../build/tmlib.a *.o
	rm *.o
clean_mac:
# 	this if for dylib build
#	rm -R ../../Game/build/libTMEngine.dylib.dSYM
#	rm ../../Game/build/libTMEngine.dylib

#	this is for a build
	rm ../build/tmlib.a
