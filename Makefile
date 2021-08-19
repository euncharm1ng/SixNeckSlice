all: 
    g++ -c sfml.cpp -Isrc\include   
    g++ sfml.o -o sfml -Lsrc\lib -lsfml-graphics -lsfml-window -lsfml-system