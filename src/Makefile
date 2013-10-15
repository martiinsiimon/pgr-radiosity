FILES=main.cpp PGR_renderer.cpp PGR_model.cpp
OBJ=${FILES:.cpp=.o}
FLAGS=-Wall -pedantic -g

EXE=radiosity

CC=g++
RM=rm -rf

ifeq ($(OS), Windows_NT)
        LIBS=-lopengl32 -lglu32 -lglut32
else
        LIBS=-lGL -lGLU -lglut -lXmu -lX11 -lXi -L/usr/X11R6/lib
endif


all: $(OBJ)
	$(CC) $(FLAGS) $(LIBS) $(OBJ) -o $(EXE) 

#$(EXE): $(OBJ)
#	$(CC) $(FLAGS) $(LIBS) -o $(EXE) $(OBJ)

#all: main.o
#main.o: main.cpp PGR_renderer.h
#	$(CC) $(FLAGS) $(LIBS) -c -o $@ main.cpp

#PGR_renderer.o: PGR_renderer.cpp PGR_renderer.h
#	$(CC) $(FLAGS) $(LIBS) -c -o $@ PGR_renderer.cpp

run:
	./$(EXE)

clean:
	$(RM) *.o $(EXE)
