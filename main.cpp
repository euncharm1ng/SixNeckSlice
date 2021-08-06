#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <time.h>
#include "mcts.h"

#define INFI 3e10
using namespace std;

int main() {
    Mcts m = Mcts(1);
    Node b = Node(1);

    Move start1 = {8,8};
    Move start2 = {10,8};
    Move start3 = {12,8};
    Move start4 = {9,9};
    Move start5 = {10,9};
    Move start6 = {11,9};
    Move start7 = {12,9};
    Move start8 = {8,10};
    Move start9 = {10,10};
    Move start10 = { 12,10 };
    Move start11 = { 8,12 };
    Move start12 = { 10,12 };
    Move start13 = { 12,12 };

    Move start14 = { 8,9 };
    Move start15 = { 9,10 };
    Move start16 = { 8,11 };
    Move start17 = { 9,11 };
    Move start18 = { 10,11 };
    Move start19 = { 11,11 };
    Move start20 = { 12,11 };
    Move start21 = { 11,12 };
    Move start22 = { 12,13 };





    m.placeStones(start1, 1, &b);
    m.placeStones(start2, 1, &b);
    m.placeStones(start3, 1, &b);
    m.placeStones(start4, 1, &b);
    m.placeStones(start5, 1, &b);
    m.placeStones(start6, 1, &b);
    m.placeStones(start7, 1, &b);
    m.placeStones(start8, 1, &b);
    m.placeStones(start9, 1, &b);
    m.placeStones(start10, 1, &b);
    m.placeStones(start11, 1, &b);
    m.placeStones(start12, 1, &b);
    m.placeStones(start13, 1, &b);

    m.placeStones(start14, 2, &b);
    m.placeStones(start15, 2, &b);
    m.placeStones(start16, 2, &b);
    m.placeStones(start17, 2, &b);
    m.placeStones(start18, 2, &b);
    m.placeStones(start19, 2, &b);
    m.placeStones(start20, 2, &b);
    m.placeStones(start21, 2, &b);
    m.placeStones(start22, 2, &b);
    b.printBoard();

    Move check1 = {9,0};
    Move check2 = {9,18};
    m.placeStones(check1, 2, &b);
    m.placeStones(check2, 2, &b);

    printf("%d\n",m.chkVic(b.board, check1, check2));


    b.printBoard();
    //printf("before select\n");
    //m.select(&b);
    //printf("after select\n");
}



