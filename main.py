import gui, go, goban, mctsEva
import time


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

