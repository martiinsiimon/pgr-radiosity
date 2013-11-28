FILES=main.cpp PGR_renderer.cpp PGR_model.cpp PGR_patch.cpp pgr.cpp
OBJ=${FILES:.cpp=.o}
CFLAGS_COMMON=-pipe -Wall -ggdb


EXE=radiosity

CC=g++
GCC=gcc

ifeq ($(OS), Windows_NT)
    LIBS=-lopengl32 -lglew32 -lSDL -lOpenCL
    FLAGS=$(CFLAGS_COMMON) -Wl,--enable-auto-import
    RM=del
    DEPS=pgr.o
else
    LIBS=-L/usr/lib `sdl-config --libs` -lGL -lGLEW -lOpenCL
    FLAGS=$(CFLAGS_COMMON) `sdl-config --cflags` -I. -IGL -DUSE_GLEE
    RM=rm -f
    USE_GLEE=1
    DEPS=pgr.o GLee.o
endif

.PHONY: all clean run

all: $(OBJ) exe
	$(CC) $(FLAGS) $(LIBS) $(OBJ) -o $(EXE)

exe: $(EXE)


run:
	./$(EXE)

clean:
	$(RM) *.o $(EXE)



%: %.cpp
	$(CC) -o $@ $^ $(FLAGS) $(LIBS)

%: %.c
	$(GCC) -o $@ $^ $(FLAGS) $(LIBS)

#zavislosti
pgr.o: pgr.cpp pgr.h

$(EXE): $(FILES) $(DEPS)