#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <time.h>
#include "mcts.h"

#define INFI 3e10
using namespace std;

int main(){
    Mcts m = Mcts();
    Node b = Node(1);
    Move start = {8,9};

    m.placeStones(start, 1, &b);

    printf("before select\n");
    m.select(&b);
    printf("after select\n");
}



