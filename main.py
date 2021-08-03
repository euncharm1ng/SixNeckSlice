import gui, go, goban, mctsEva

BLACK = 1
WHITE = 2
if __name__ == '__main__':
    aiColor = BLACK
    userColor = WHITE
    ai = mctsEva.mcts()

    a = mctsEva.node("root",BLACK)
    b = mctsEva.node(a,BLACK)
    c = mctsEva.node(a,BLACK)
    b.board[10][10] = BLACK
    ai.rollout(b, False)
    print(b.t)
    

    # (0,1) x
    # o x
    # o o
