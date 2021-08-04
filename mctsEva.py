'''
where monte carlo tree search is taking place. 
selection
expansion
sumulation (rollout)
backpropagation 
refereced: 
    https://www.analyticsvidhya.com/blog/2019/01/monte-carlo-tree-search-introduction-algorithm-deepmind-alphago/
    https://towardsdatascience.com/monte-carlo-tree-search-an-introduction-503d8c04e168
'''
'''
todo: 
    decide t according to consequences

'''
from numpy import log as ln

import time 
import numpy
import math, copy
import random


movRange = 2
BLACK = 1
WHITE = 2

class move:
    def __init__(self, x, y):
        self.x = x
        self.y = y

    def __str__(self):
        return "x: " + str(self.x) + " y: " + str(self.y)

class node:
    def __init__(self, parent,present_color):#, mov1, mov2):
        self.t = 0 # value 
        self.n = 0 # number of times visited
        self.ucb = float('inf')
        self.name = ""
        self.children = []
        self.parent = parent
        self.color = present_color
        #self.mov1 = mov1
        #self.mov2 = mov2
        if(parent == "root"):
            self.board = numpy.zeros((19,19), dtype = int)
        else: 
            parent.addC(self)
            self.board = copy.deepcopy(parent.board)
        self.availMov = []

    def addC(self, child):
        self.children.append(child)

    def printC(self):
        for child in self.children:
            print(child)

    def setName(self, name):
        self.name = name

    def __str__(self):
        return "ucb value is : " + str(self.ucb) #self.name #

    def printBoard(self):
        for i in range(19):
            for j in range(19):
                print(str(self.board[i][j]) + " ", end = "")
            print("")

    def printAvailMov(self):
        for mov in self.availMov:
            print(mov)



class mcts: 
    def __init__(self):
        #self.board = numpy.zeros((19,19)) #int assuming 0 is empty and 1,2,3 for b, w, r
        pass
        #todo

    # the biggest method for run game
    # input: board status, output: coor
    # gui.py calls this method to calculate the ai's move
    def runGame(self):
        pass

    #1 no need return, update node.ucb
    def calcUCB(self, node):
        node.ucb = node.t + 2*(math.sqrt(ln(node.parent.n)/node.n))
        #todo

    #1
    def expansion(self,currnode):
        self.findMoves(currnode)
        color = WHITE if currnode.color == BLACK else BLACK
        for mov in currnode.availMov :
            print(mov)
            # tempnode = copy.deepcopy(currnode)
            # tempnode.color = color
            # tempnode.parent = currnode
            # self.placeStones(mov, tempnode.color,tempnode)
            # currnode.addC(tempnode)
        #todo

    #3
    def backprop(self):
        pass
        #todo

    #5
    def select(self,currnode):
        # if currnode.child is empty :
        #     expansion()
        #     each child rollout
        #     backprop()
        # select node which has biggest value of ucb
        pass
        #todo

    #2 
    # input : current node, Next turn / output : win? lose? tie?
    def rollout(self, currNode, isAiTurn):
        aicolor = currNode.color
        usercolor = WHITE if aicolor == BLACK else BLACK
        turn = aicolor if isAiTurn == True else usercolor
        tempnode = copy.deepcopy(currNode)
        
        while self.isFull(tempnode) :
            tempnode.availMov = []
            self.findMoves(tempnode)
            mov1, mov2 = numpy.random.choice(tempnode.availMov, 2, replace = False)
            turn = usercolor if turn == aicolor else aicolor
            self.placeStones(mov1,turn, tempnode)
            self.placeStones(mov2,turn, tempnode)
            victory, winnerColor = self.chkVic(tempnode)
            if victory :
                return
            #     currNode.t = 20 if winnerColor == aicolor else 0
            #     return

        currNode.t = 10

    
        #todo find available moves -> make random move for ai -> check victory -> findMoves -> make random move for user -> check victory -> find -> .... -> somebody won -> update current node's t value 

        #findMoves(tempnode)
        #random 2 moves = (tempnode.availMov)
        #tempnode.board[random 2 moves] = 1 if it's ai's turn, 2 if it's user's turn
        #check victory
        #findMoves


    #2 input: node, output: none
    #calculate available moves according to node.board, and appedn the moves to node.availMov
    def findMoves(self, node):
        moveBoard = numpy.ones((19,19), dtype = int)
        for i in range(19):
            for j in range(19):
                if node.board[i][j] :
                    moveBoard[i][j] = 0
                    for k in range(self.ifSmaller(i-movRange), self.ifBigger(i+movRange+1)):
                        for l in range(self.ifSmaller(j-movRange), self.ifBigger(j+movRange+1)):
                            moveBoard[k][l] *= 2

        moveBoard[moveBoard < 2] = 0
        # print("in findMoves: ")
        for i in range(19):
            for j in range(19):
                # print(str(moveBoard[i][j]) + " ", end = "")
                if moveBoard[i][j]:
                    node.availMov.append(move(i,j))
            # print("")
        
    
    #return num if it is smaller than 19, or 19 if num is bigger than 19
    def ifBigger(self, num):
        if num > 19 :
            return 19
        else :
            return num

    #return num if it is bigger than 0, or 0 if num is smaller than 0
    def ifSmaller(self, num):
        if num < 0 :
            return 0
        else :
            return num
        

    def chkTime(self):
        pass
        #todo

    #input: node, output: bool, true for a winner
    def chkVic(self, node):
        tag = 0
        cnt = 0
        #dir1 horizontal
        i = 0
        while i < 19:
            j = 0
            while j < 19:
                if node.board[i][j]:
                    tag = node.board[i][j]
                    while j < 19 and tag == node.board[i][j]:
                        cnt += 1
                        j = j + 1
                    #print("outside while, cnt = " + str(cnt) + " " + str(j))
                    if cnt > 5:
                        return True, tag
                    else:
                        cnt = 0
                        j -= 1
                j+=1
            i+=1
        #dir2 vertical
        j = 0
        while j < 19:
            i = 0
            while i < 19:
                if node.board[i][j]:
                    tag = node.board[i][j]
                    while i < 19 and tag == node.board[i][j]:
                        cnt += 1
                        i = i + 1
                    #print("outside while, cnt = " + str(cnt) + " " + str(i))
                    if cnt > 5:
                        return True, tag
                    else:
                        cnt = 0
                        i -= 1
                i+=1
            j+=1

        #dir3 diag top left to bot right
        for i in range(14):
            j = 0 
            while i < 19:
                #print("loop var i : "+ str(i))
                if node.board[i][j]:
                    tag = node.board[i][j]
                    while i < 19 and tag == node.board[i][j]:
                        cnt += 1
                        i += 1
                        j += 1
                    #print("outside while, cnt = " + str(cnt) + " " + str(i) + " " + str(j))
                    if cnt > 5:
                        return True, tag
                    else:
                        cnt = 0
                        continue
                j += 1
                i += 1

        for j in range (1, 14):
            i = 0
            while j < 19:
                if node.board[i][j]:
                    tag = node.board[i][j]
                    while j < 19 and tag == node.board[i][j]:
                        cnt += 1
                        i += 1
                        j += 1
                    #print("outside while, cnt = " + str(cnt) + " " + str(i) + " " + str(j))
                    if cnt > 5:
                        return True, tag
                    else: 
                        cnt = 0
                        continue
                j += 1
                i += 1

        #dir4 diag top right to bot left
        for j in range (5, 19):
            i = 0 
            while j > -1:
                if node.board[i][j]:
                    tag = node.board[i][j]
                    while j > -1 and tag == node.board[i][j]: 
                        cnt += 1
                        i += 1
                        j -= 1

                    #print("outside while, cnt = " + str(cnt) + " " + str(i) + " " + str(j))

                    if cnt > 5:
                        return True, tag
                    else:
                        cnt = 0
                        continue
                i += 1
                j -= 1

        for i in range (1, 14):
            j = 0
            while i < 19:
                if node.board[i][j]:
                    tag = node.board[i][j]
                    while i < 19 and tag == node.board[i][j]:
                        cnt += 1
                        i += 1
                        j -= 1

                    #print("outside while, cnt = " + str(cnt) + " " + str(i) + " " + str(j))

                    if cnt > 5:
                        return True, tag
                    else: 
                        cnt = 0
                        continue
                i += 1
                j -= 1

        return False, 0


    def undo(self):
        pass
        #todo later

    #input stone's coor, color, current node / output place stones on currnode.board
    def placeStones(self, stone, color, currnode) :
        currnode.board[stone.x][stone.y] = color

    #return true is board is not full, false if there is no space for moves
    def isFull(self, checknode):
        return 0 in checknode.board

