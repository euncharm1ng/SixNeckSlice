#include <stdlib.h>
#include <stdio.h>
#include <time.h>
// #include <windows.h>
#include "mcts.h"

using namespace std;

int main(){
    // printf("hello\n");
    Mcts m = Mcts();
    // a.rollout();
    Node b = Node(1);

    Move mov1 = {1,1};
    Move mov2 = {2,2};
    Move start = {8,9};
    m.placeStones(start, 1, b);
    // // int vic = m.rollout(b);
    // // printf("%d", vic);
    //time_t start = clock_t();
    time_t startTime = clock();
    // Sleep(1000);
    m.expansion(b);
    for(auto& childNode : b.children){
        m.rollout(*childNode);
    }
    time_t endTime = clock();
    //double end = (clock_t() - startTime) / double(CLOCKS_PER_SEC);
    printf("main time: %f\n", (endTime - startTime) / double(CLOCKS_PER_SEC));
}


