# SixNeckSlice
dahun

cd to obj directory and compile object files out of src/, and link the object files to build 
executable under directory bin. the executable has to be located under bin with .dll files to
run properly. the project is compiled with mingw verion 7.3.0

    `g++ -c ..\src\main.cpp ..\src\gui.cpp ..\src\mcts.cpp -I..\include`
    `g++ main.o gui.o mcts.o -o ..\bin\SNS -L..\lib -lsfml-graphics -lsfml-window -lsfml-system`
    