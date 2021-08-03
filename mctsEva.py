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

'''

class node:
    def __init__(self, parent):
        self.t = 0
        self.n = 0
        self.ucb = 0
        self.name = ""
        self.children = []
        self.parent = parent

    def addC(self, child):
        self.children.append(child)

    def printC(self):
        for child in self.children:
            print(child)

    def setName(self, name):
        self.name = name

    def __str__(self):
        return self.name #"ucb value is : " + str(self.ucb)



class mcts: 
    def __init__(self):
        board = [19][19] #int
        pass
        #todo

    #1 
    def calcUCB(self, node, userinput):
        pass
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
    def rollout(self):
        pass
        #todo

    #5
    def select(self):
        pass
        #todo

    #2 input: board, output: available moves
    def findMoves(self):
        pass
        #todo



class checker:
    log=[] #for undo 
    def chkTime(self):
        pass
        #todo


    #input: board, output: bool, true for win
    def chkVic(self):
        pass
        #todo


    def undo(self):
        pass
        #todo later`