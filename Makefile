CFLAGS = -Wall -Wextra -O2
SRC = \
    src/rce.c \
    src/game/game.c \
    src/game/player.c \
    src/audio/audio.c \
	src/renderer/raycaster.c \
	src/renderer/textures.c \
	src/renderer/sprites.c \
	src/editor/editor.c \
	src/input/input.c \
	src/renderer/renderer.c \
	src/level/level.c \

MINGW = x86_64-w64-mingw32
MINGW_ROOT = /usr/$(MINGW)/sys-root/mingw

.PHONY: all linux windows clean

# Default: build Linux version
all: linux

linux:
	gcc $(CFLAGS) $(SRC) -o rce_game \
		-lGL -lGLU -lglut -lopenal -lm 

# Build Windows version + collect required DLLs
windows:
	$(MINGW)-gcc $(CFLAGS) $(SRC) -o rce_game.exe \
		-lopengl32 -lglu32 -lglut -lOpenAL32

# 	@mkdir -p dist
	@cp $(MINGW_ROOT)/bin/OpenAL32.dll .
	@cp $(MINGW_ROOT)/bin/libglut.dll .
	@cp $(MINGW_ROOT)/bin/libgcc_s_seh-1.dll .
	@cp $(MINGW_ROOT)/bin/libwinpthread-1.dll .
	@cp $(MINGW_ROOT)/bin/libstdc++-6.dll .

clean:
	rm -f rce_game rce_game.exe
	rm -rf dist


# CC_LINUX = gcc
# CC_WINDOWS = x86_64-w64-mingw32-gcc

# CFLAGS = -Wall -Wextra
# SRC = rce.c

# TARGET = unga_bunga.exe

# LIBS_WINDOWS = -lopengl32 -lglu32 -lfreeglut -lopenal32
# LIBS_LINUX = -lGL -lGLU -lglut -lopenal -lm

# all:
# 	$(CC_WINDOWS) $(CFLAGS) $(SRC) -o $(TARGET) $(LIBS_WINDOWS)

# clean:
# 	rm -f $(TARGET)



# CC = gcc
# CFLAGS = -Wall -Wextra
# LIBS = -lGL -lGLU -lglut -lopenal -lm 

# TARGET = unga_bunga.exe
# SRC = rce.c

# all: $(TARGET)

# $(TARGET): $(SRC)
# 	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LIBS)

# clean:
# 	rm -f $(TARGET)