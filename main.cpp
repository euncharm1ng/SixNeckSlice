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
    
    Move start1 = {10,10};
    // Move start2 = {9,10};
    // Move start3 = {8,10};
    // Move start4 = {6,10};
    // Move start5 = {9,11};

    // Move start6 = {9,7};
    // Move start7 = {8,8};
    // Move start8 = {8,9};
    // Move start9 = {7,9};


    m.placeStones(start1, 1, &b);
    // m.placeStones(start2, 1, &b);
    // m.placeStones(start3, 1, &b);
    // m.placeStones(start4, 1, &b);
    // m.placeStones(start5, 1, &b);

    // m.placeStones(start6, 2, &b);
    // m.placeStones(start7, 2, &b);
    // m.placeStones(start8, 2, &b);
    // m.placeStones(start9, 2, &b);

    printf("before select\n");
    m.select(&b);
    printf("after select\n");
}



