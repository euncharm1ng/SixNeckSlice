import gui, go, goban, mctsEva
import time

<<<<<<< HEAD
print("start")

a = mctsEva.node("root")
b = mctsEva.node(a)
# a.board[1][1] = 1
# a.board[1][2] = 1
# a.board[1][3] = 2
# a.board[1][4] = 1
# a.board[1][5] = 1
# a.board[1][6] = 1
# a.board[0][15] = 2
# a.board[1][7] = 2
# a.board[17][13] = 1
# a.board[17][14] = 2
# a.board[17][15] = 2
# a.board[17][16] = 2
# a.board[17][17] = 2
# a.board[17][18] = 2

# a.board[0][1] = 4
# a.board[2][1] = 1
# a.board[3][1] = 1
# a.board[4][1] = 2
# a.board[5][1] = 1
# a.board[6][1] = 1
# a.board[13][18] = 1
# a.board[14][18] = 1
# a.board[15][18] = 1
# a.board[16][18] = 1
# a.board[17][18] = 1
# a.board[18][18] = 1

# a.board[1][1] = 1
# a.board[2][2] = 1
# a.board[3][3] = 1
# a.board[4][4] = 1
# a.board[5][5] = 1
# a.board[6][6] = 2
# a.board[18][18] = 2

# a.board[6][10] = 2
# a.board[7][11] = 2
# a.board[8][12] = 2
# a.board[9][13] = 1
# a.board[10][14] = 2
# a.board[11][15] = 2

# a.board[12][5] = 2
# a.board[13][6] = 2
# a.board[14][7] = 2
# a.board[15][8] = 1
# a.board[16][9] = 2
# a.board[17][10] = 2

a.board[0][13] = 2
a.board[1][14] = 2
a.board[2][15] = 2
a.board[3][16] = 2
a.board[4][17] = 2
a.board[5][18] = 2

a.board[13][0] = 1
a.board[14][1] = 1
a.board[15][2] = 1
a.board[16][3] = 2
a.board[17][4] = 1
a.board[18][5] = 1

a.board[0][5] = 1
a.board[1][4] = 1
a.board[2][3] = 1
a.board[3][2] = 2
a.board[4][1] = 1
a.board[5][0] = 1

a.board[9][9] = 1
a.board[10][8] = 1
a.board[11][7] = 2
a.board[12][6] = 1
a.board[13][5] = 1
a.board[14][4] = 1

a.board[0][18] = 1
a.board[1][17] = 1
a.board[2][16] = 1
a.board[3][15] = 1
a.board[4][14] = 1
a.board[5][13] = 1

# a.board[10][18] = 1
# a.board[11][17] = 1
# a.board[12][16] = 2
# a.board[13][15] = 2
# a.board[14][14] = 2
# a.board[15][13] = 1

# a.board[10][10] = 1
# a.board[11][9] = 1
# a.board[12][8] = 1
# a.board[13][7] = 1
# a.board[14][6] = 2
# a.board[15][5] = 1

# a.board[18][10] = 1
# a.board[17][11] = 1
# a.board[16][12] = 2
# a.board[15][13] = 1
# a.board[14][14] = 1
# a.board[13][15] = 1

# a.board[18][17] = 1

mcts = mctsEva.mcts()

mcts.printB(a)
print("middle")
# mcts.findMoves(a)
# mcts.printAvailM(a)
print(mcts.chkVic(a))



print("end")




=======
BLACK = 1
WHITE = 2
if __name__ == '__main__':
    aiColor = BLACK
    userColor = WHITE
    ai = mctsEva.mcts()

    a = mctsEva.node("root",BLACK)
    b = mctsEva.node(a,BLACK)
    b.board[10][10] = BLACK
    ai.rollout(b, True)
    # a.board[10][10] = BLACK
    # ai.expansion(a)
    # for i in range(5) :
    #     print(a.children[i].board)

    


    # (0,1) x
    # o x
    # o o
>>>>>>> 5f9da478c43e8725c08913530de27d8df69d9c25
