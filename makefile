default: 
	gcc -o main.exe main.c -I include -L lib -lraylib -lgdi32 -lwinmm

web:
	emcc -o index.html main.c -Os -Wall ./lib/libraylib.web.a  -I./include/ -L./lib/ -s USE_GLFW=3 --preload-file assets@/assets --shell-file E:/_dev/raylib/src/minshell.html -DPLATFORM_WEB

