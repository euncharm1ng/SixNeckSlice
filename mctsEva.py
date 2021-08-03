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
    URGENT: ds
    decide t according to consequences

'''
from numpy import log as ln
import numpy
import math, copy

movRange = 2

class move:
    def __init__(self, x, y):
        self.x = x
        self.y = y

    def __str__(self):
        return "x: " + str(self.x) + " y: " + str(self.y)

class node:
    def __init__(self, parent):#, mov1, mov2):
        self.t = 0 # value 
        self.n = 0 # number of times visited
        self.ucb = float('inf')
        self.name = ""
        self.children = []
        self.parent = parent
        #self.mov1 = mov1
        #self.mov2 = mov2
        if(parent == "root"):
            self.board = numpy.zeros((19,19), dtype = int)
        else: 
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



class mcts: 
    def __init__(self):
        #self.board = numpy.zeros((19,19)) #int assuming 0 is empty and 1,2,3 for b, w, r
        pass
        #todo

    #1 no need return, update node.ucb
    def calcUCB(self, node):
        node.ucb = node.t + 2*(math.sqrt(ln(node.parent.n)/node.n))
        #todo

    #1
    def expansion(self):
        pass
        #todo

    #3
    def backprop(self):
        pass
        #todo

    #2 
    def rollout(self, currNode):

       ''' tempnode = copy.deepcopy(self)

		while True :
			self.findMoves(tempnode)
			#choose randome next move and move 2 stones
			self.availMov
			if self.chkVic(tempnode):
				break

		return self.t = 20

        pass'''
        #todo find available moves -> make random move for ai -> check victory -> findMoves -> make random move for user -> check victory -> find -> .... -> somebody won -> update current node's t value 

        #findMoves(tempnode)
        #random 2 moves = (tempnode.availMov)
        #tempnode.board[random 2 moves] = 1 if it's ai's turn, 2 if it's user's turn
        #check victory
        #findMoves


    #5
    def select(self):
        pass
        #todo

    #2 input: board, output: available moves
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

        for i in range(19):
            for j in range(19):
                print(str(moveBoard[i][j]) + " ", end = "")
                if moveBoard[i][j]:
                    node.availMov.append(move(i,j))
            print("")
        
    def printAvailM(self, node):
        for mov in node.availMov:
            print(mov)


    def ifBigger(self, num):
        if num > 19 :
            return 19
        else :
            return num


    def ifSmaller(self, num):
        if num < 0 :
            return 0
        else :
            return num
        

    def printB(self, node):
        for i in range(19):
            for j in range(19):
                print(str(node.board[i][j]) + " ", end = "")
            print("")


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
        #return False, 0
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
                    print("outside while, cnt = " + str(cnt) + " " + str(i) + " " + str(j))
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
                    print("outside while, cnt = " + str(cnt) + " " + str(i) + " " + str(j))
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
                    print("outside while, cnt = " + str(cnt) + " " + str(i) + " " + str(j))
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
                    print("outside while, cnt = " + str(cnt) + " " + str(i) + " " + str(j))
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
        #todo later`

