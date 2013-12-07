FILES=main.cpp PGR_renderer.cpp PGR_model.cpp PGR_patch.cpp PGR_radiosity.cpp pgr.cpp
OBJ=${FILES:.cpp=.o}
CFLAGS=-pipe -Wall -ggdb -g -O0


EXE=radiosity

CC=g++
GCC=gcc

ifeq ($(OS), Windows_NT)
    LIBS=-lopengl32 -lglew32 -lSDL -lOpenCL
    FLAGS=$(CFLAGS) -Wl,--enable-auto-import
    RM=del
    DEPS=pgr.o
else
    LIBS=-L/usr/lib `sdl-config --libs` -lGL -lGLU -lGLEW -lOpenCL
    FLAGS=$(CFLAGS) `sdl-config --cflags` -I. -IGL -DUSE_GLEE
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

PGR_model.o: PGR_model.cpp PGR_model.h PGR_patch.h model.h
PGR_renderer.o: PGR_renderer.cpp PGR_renderer.h PGR_radiosity.h PGR_model.h pgr.h model.h 
PGR_radiosity.o: PGR_radiosity.cpp PGR_radiosity.h PGR_model.h
PGR_patch.o: PGR_patch.cpp PGR_patch.h model.h

$(EXE): $(FILES) $(DEPS)
