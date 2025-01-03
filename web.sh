mkdir -p web
mkdir -p web/premade

emcc src/*.cpp -o web/index.html \
    -s USE_GLFW=3 \
    -s WASM=1 \
 	-s ASYNCIFY=1 \
    -I/h/git/raylib/src \
    /h/git/raylib/src/libraylib.web.a \
	-Os -g\
    --preload-file assets\
    --preload-file assets/fonts\
    --preload-file assets/maps\
    --preload-file assets/music\
	-D PLATFORM_WEB\
	-D IsWindowResized=IsWindowResizedWasm

cp web/premade/index.html web/index.html

zip -r web/archive.zip web
