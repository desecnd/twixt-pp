# autor: pavveu
# inspired by: aqeelanwar

from tkinter import Tk, Canvas
import numpy as np
from bot_handler import BotHandler

dotsCnt = 12
boardSize = 1024
dotsDist = boardSize / dotsCnt
lineWidth = 5
dotRadius = 20
pegRadius = 5
directions = [ [-1, 2], [ 1, 2], [-2,-1], [-2, 1], [ 1,-2], [-1,-2], [2, 1], [2,-1] ]

bgColor = '#303030'
pegColor= '#FFCC00'
p1Color = '#FF0066'
p2Color = '#0099FF'

class GameLogic():
    def __init__(self):
        self.startingPlayer = 2
        self.reset()

    def opponent(self, player):
        return 3 - player

    def reset(self):
        self.gameEnded = False
        self.dots = np.zeros(shape=(dotsCnt, dotsCnt))
        self.p1Links = []
        self.p2Links = []
        self.startingPlayer = self.opponent(self.startingPlayer)
        self.currentPlayer = self.startingPlayer

    def ccw(self,A,B,C):
        return (A[0]-C[0]) * (B[1]-A[1]) > (A[0]-B[0]) * (C[1]-A[1])

    def intersect(self,A,B,C,D):
        return self.ccw(A,C,D) != self.ccw(B,C,D) and self.ccw(A,B,C) != self.ccw(A,B,D)

    def collision(self, pos1, pos2):
        if self.currentPlayer == 1:
            for link in self.p2Links: 
                if self.intersect(pos1, pos2, link[0], link[1]):
                    return True
            return False
        else:
            for link in self.p1Links: 
                if self.intersect(pos1, pos2, link[0], link[1]):
                    return True
            return False

    def checkLinks(self, boardPos):
        for dir in directions: 
            pos = [ boardPos[0] + dir[0], boardPos[1] + dir[1] ]

            if pos[0] >= 0 and pos[0] < dotsCnt and pos[1] >= 0 and pos[1] < dotsCnt:
                if self.dots[pos[0]][pos[1]] == self.currentPlayer and not self.collision(boardPos, pos):
                    if self.currentPlayer == 1:
                        self.p1Links.append([boardPos, pos])
                    else:
                        self.p2Links.append([boardPos, pos])

    def legal(self, boardPos):
        if self.currentPlayer == 1:
            if boardPos[1] == 0 or boardPos[1] == dotsCnt - 1:
                return False
            else: 
                return self.empty(boardPos)
        else:
            if boardPos[0] == 0 or boardPos[0] == dotsCnt - 1:
                return False
            else:
                return self.empty(boardPos)

    def makeMove(self, boardPos):
        if self.legal(boardPos):
            self.takeDot(boardPos)
            self.checkLinks(boardPos)

            if self.gameOver():
                self.gameEnded = True
            else:
                self.currentPlayer = self.opponent(self.currentPlayer) 

    def takeDot(self, boardPos):
        self.dots[boardPos[0]][boardPos[1]] = self.currentPlayer

    def gameOver(self):
        dp = np.zeros(shape=(dotsCnt, dotsCnt))

        if self.currentPlayer == 1:
            for j in range(dotsCnt):
                dp[0][j] = 1

            for i in range(dotsCnt):
                for j in range(dotsCnt):
                    for link in self.p1Links:
                        if link[0] == [i, j]:
                            dp[link[1][0]][link[1][1]] += dp[i][j]
                        elif link[1] == [i, j]:
                            dp[link[0][0]][link[0][1]] += dp[i][j]
                        else:
                            continue

            for j in range(dotsCnt):
                if dp[dotsCnt - 1][j] != 0:
                    return True
            return False
        else:
            for i in range(dotsCnt):
                dp[i][0] = 1

            for j in range(dotsCnt):
                for i in range(dotsCnt):
                    for link in self.p2Links:
                        if link[0] == [i, j]:
                            dp[link[1][0]][link[1][1]] += dp[i][j]
                        elif link[1] == [i, j]:
                            dp[link[0][0]][link[0][1]] += dp[i][j]
                        else:
                            continue

            for i in range(dotsCnt):
                if dp[i][dotsCnt - 1] != 0:
                    return True
            return False

    def empty(self, boardPos):
        return self.dots[boardPos[0]][boardPos[1]] == 0

class TwixTPP():
    def posToPeg(self, pos):
        return ''.join([chr(pos[1] + ord('A')), str(pos[0] + 1)])

    def pegToPos(self, peg):
        return [ int(peg[1:]) - 1, ord(peg[0]) - ord('A')   ]

class GraphicTwixTPP(TwixTPP):
    def __init__(self):
        self.logic = GameLogic()
        self.window = Tk()
        self.window.title('TwixT-PP')
        self.canvas = Canvas(self.window, width=boardSize, height=boardSize)
        self.canvas.pack()
        self.botHandler = BotHandler('bot')
        self.canvas.configure(background=bgColor)
        self.window.bind('<Button-1>', self.click)
        self.drawBoard()

    def gridToBoardPosition(self, gridPos):
        gridPos = np.array(gridPos)
        position = (gridPos-dotsDist/4)//(dotsDist/2)

        boardPos = None
        if position[0] % 2 == 0 and position[1] % 2 == 0:
            c = int( position[0] / 2 )
            r = int( position[1] / 2 )
            boardPos = [r, c]
        
        return boardPos
    
    def play(self):
        self.window.mainloop()

    def drawBoard(self):
        for AB in self.logic.p1Links:
            A = AB[0]
            B = AB[1]

            y1 = A[0]*dotsDist + dotsDist/2
            x1 = A[1]*dotsDist + dotsDist/2
            y2 = B[0]*dotsDist + dotsDist/2
            x2 = B[1]*dotsDist + dotsDist/2

            self.canvas.create_line(x1,y1, x2,y2, fill=p1Color, width = lineWidth)

        for AB in self.logic.p2Links:
            A = AB[0]
            B = AB[1]

            y1 = A[0]*dotsDist + dotsDist/2
            x1 = A[1]*dotsDist + dotsDist/2
            y2 = B[0]*dotsDist + dotsDist/2
            x2 = B[1]*dotsDist + dotsDist/2

            self.canvas.create_line(x1,y1, x2,y2, fill=p2Color, width = lineWidth)

        for i in range(dotsCnt):
            for j in range(dotsCnt):
                y = i*dotsDist + dotsDist/2
                x = j*dotsDist + dotsDist/2

                radius = pegRadius
                color = pegColor

                if self.logic.dots[i][j] == 1: 
                    color = p1Color
                    radius = dotRadius
                elif self.logic.dots[i][j] == 2:
                    color = p2Color
                    radius = dotRadius

                self.canvas.create_oval(x-radius, y-radius, x + radius,y + radius,fill=color,outline=color)

        self.canvas.create_line(0, dotsDist, boardSize, dotsDist, fill=p1Color, width=lineWidth)
        self.canvas.create_line(0, boardSize - dotsDist, boardSize, boardSize - dotsDist, fill=p1Color, width=lineWidth)

        self.canvas.create_line(dotsDist, 0, dotsDist, boardSize, fill=p2Color, width = lineWidth)
        self.canvas.create_line(boardSize - dotsDist, 0, boardSize - dotsDist, boardSize, fill=p2Color, width = lineWidth)

    def click(self, event):
        gridPos = [event.x, event.y]
        boardPos = self.gridToBoardPosition(gridPos)
        if boardPos:
            self.logic.makeMove(boardPos)
            self.drawBoard()
            print(boardPos)
            botMove = self.botHandler.getMove(self.posToPeg(boardPos))
            print(botMove)
            if botMove:
                self.logic.makeMove(self.pegToPos(botMove))
                self.drawBoard()

class ConsoleTwixTPP(TwixTPP):

    def __init__(self):
        self.logic = GameLogic()
    
    def play(self, p1, p2, rounds):
        ratio = 0
        for i in range(rounds):
            ratio += self.brawl(p1, p2)
        return [ (rounds + ratio)//2, (rounds - ratio)//2 ]

    def brawl(self, p1, p2):
        p1Move = 'FIRST'
        p2Move = 'FIRST'

        for turn in range(1, 250):
            print(turn)
            p1Move = p1.getMove(p2Move)

            print(p1Move)
            self.logic.makeMove(self.pegToPos(p1Move))

            if self.logic.gameEnded:
                return 1
            
            p2Move = p2.getMove(p1Move)

            print(p2Move)
            self.logic.makeMove(self.pegToPos(p2Move))

            if self.logic.gameEnded:
                return -1

        return 0

"""
class TwixTPP():
    def __init__(self):
        self.botHandler = BotHandler()
        self.botHandler.activateBot()
        self.window = Tk()
        self.window.title('TwixT-PP')
        self.canvas = Canvas(self.window, width=boardSize, height=boardSize)
        self.canvas.pack()
        self.canvas.configure(background=bgColor)
        self.window.bind('<Button-1>', self.click)
        self.startingPlayer = 2
        self.reset()

    def opponent(self, player):
        return 3 - player

    def positionToName(self, boardPos):
        return ''.join([chr(boardPos[1] + ord('A')), str(boardPos[0] + 1)])

    def nameToPosition(self, name):
        return [ int(name[1:]) - 1, ord(name[0]) - ord('A')   ]
    
    def reset(self):
        self.dots = np.zeros(shape=(dotsCnt, dotsCnt))
        self.p1Links = []
        self.p2Links = []
        self.update()
        self.startingPlayer = self.opponent(self.startingPlayer)
        self.currentPlayer = self.startingPlayer


    def ccw(self,A,B,C):
        return (A[0]-C[0]) * (B[1]-A[1]) > (A[0]-B[0]) * (C[1]-A[1])

    def intersect(self,A,B,C,D):
        return self.ccw(A,C,D) != self.ccw(B,C,D) and self.ccw(A,B,C) != self.ccw(A,B,D)

    def collision(self, pos1, pos2):
        if self.currentPlayer == 1:
            for link in self.p2Links: 
                if self.intersect(pos1, pos2, link[0], link[1]):
                    return True
            return False
        else:
            for link in self.p1Links: 
                if self.intersect(pos1, pos2, link[0], link[1]):
                    return True
            return False

    def checkLinks(self, boardPos):
        for dir in directions: 
            pos = [ boardPos[0] + dir[0], boardPos[1] + dir[1] ]

            if pos[0] >= 0 and pos[0] < dotsCnt and pos[1] >= 0 and pos[1] < dotsCnt:
                if self.dots[pos[0]][pos[1]] == self.currentPlayer and not self.collision(boardPos, pos):
                    if self.currentPlayer == 1:
                        self.p1Links.append([boardPos, pos])
                    else:
                        self.p2Links.append([boardPos, pos])

    def legal(self, boardPos):
        if self.currentPlayer == 1:
            if boardPos[1] == 0 or boardPos[1] == dotsCnt - 1:
                return False
            else: 
                return self.empty(boardPos)
        else:
            if boardPos[0] == 0 or boardPos[0] == dotsCnt - 1:
                return False
            else:
                return self.empty(boardPos)


    def makeMove(self, boardPos):
        if self.legal(boardPos):
            self.takeDot(boardPos)
            self.checkLinks(boardPos)
            self.update()

            if self.gameOver():
                self.canvas.delete("all")
                print( "Player", self.currentPlayer, ' wins!')
                self.reset()

            self.currentPlayer = self.opponent(self.currentPlayer) 

    def takeDot(self, boardPos):
        self.dots[boardPos[0]][boardPos[1]] = self.currentPlayer

    def gameOver(self):
        dp = np.zeros(shape=(dotsCnt, dotsCnt))

        if self.currentPlayer == 1:
            for j in range(dotsCnt):
                dp[0][j] = 1

            for i in range(dotsCnt):
                for j in range(dotsCnt):
                    for link in self.p1Links:
                        if link[0] == [i, j]:
                            dp[link[1][0]][link[1][1]] += dp[i][j]
                        elif link[1] == [i, j]:
                            dp[link[0][0]][link[0][1]] += dp[i][j]
                        else:
                            continue

            for j in range(dotsCnt):
                if dp[dotsCnt - 1][j] != 0:
                    return True
            return False
        else:
            for i in range(dotsCnt):
                dp[i][0] = 1

            for j in range(dotsCnt):
                for i in range(dotsCnt):
                    for link in self.p2Links:
                        if link[0] == [i, j]:
                            dp[link[1][0]][link[1][1]] += dp[i][j]
                        elif link[1] == [i, j]:
                            dp[link[0][0]][link[0][1]] += dp[i][j]
                        else:
                            continue

            for i in range(dotsCnt):
                if dp[i][dotsCnt - 1] != 0:
                    return True
            return False

    def gridToBoardPosition(self, gridPos):
        gridPos = np.array(gridPos)
        position = (gridPos-dotsDist/4)//(dotsDist/2)

        boardPos = []
        found = False
        if position[0] % 2 == 0 and position[1] % 2 == 0:
            c = int( position[0] / 2 )
            r = int( position[1] / 2 )
            boardPos = [r, c]
            found = True
        
        return boardPos, found

    def empty(self, boardPos):
        return self.dots[boardPos[0]][boardPos[1]] == 0

    def update(self):
        for AB in self.p1Links:
            A = AB[0]
            B = AB[1]

            y1 = A[0]*dotsDist + dotsDist/2
            x1 = A[1]*dotsDist + dotsDist/2
            y2 = B[0]*dotsDist + dotsDist/2
            x2 = B[1]*dotsDist + dotsDist/2

            self.canvas.create_line(x1,y1, x2,y2, fill=p1Color, width = lineWidth)

        for AB in self.p2Links:
            A = AB[0]
            B = AB[1]

            y1 = A[0]*dotsDist + dotsDist/2
            x1 = A[1]*dotsDist + dotsDist/2
            y2 = B[0]*dotsDist + dotsDist/2
            x2 = B[1]*dotsDist + dotsDist/2

            self.canvas.create_line(x1,y1, x2,y2, fill=p2Color, width = lineWidth)

        for i in range(dotsCnt):
            for j in range(dotsCnt):
                y = i*dotsDist + dotsDist/2
                x = j*dotsDist + dotsDist/2

                radius = pegRadius
                color = pegColor

                if self.dots[i][j] == 1: 
                    color = p1Color
                    radius = dotRadius
                elif self.dots[i][j] == 2:
                    color = p2Color
                    radius = dotRadius

                self.canvas.create_oval(x-radius, y-radius, x + radius,y + radius,fill=color,outline=color)

        self.canvas.create_line(0, dotsDist, boardSize, dotsDist, fill=p1Color, width=lineWidth)
        self.canvas.create_line(0, boardSize - dotsDist, boardSize, boardSize - dotsDist, fill=p1Color, width=lineWidth)

        self.canvas.create_line(dotsDist, 0, dotsDist, boardSize, fill=p2Color, width = lineWidth)
        self.canvas.create_line(boardSize - dotsDist, 0, boardSize - dotsDist, boardSize, fill=p2Color, width = lineWidth)

    def playerError(self):
        print("Player", self.currentPlayer, " WRONG PLAY")

    def click(self, event):
        gridPos = [event.x, event.y]
        boardPos, valid = self.gridToBoardPosition(gridPos)
        if valid:
            self.makeMove(boardPos)
            botMove = self.botHandler.queryBot(self.positionToName(boardPos))
            print(botMove)
            if botMove:
                self.makeMove(self.nameToPosition(botMove))
            else:
                self.playerError()

"""

# gameInstance = ConsoleTwixTPP()
# print(gameInstance.play(BotHandler('bot'), BotHandler('bot'), 1))
gameInstance = GraphicTwixTPP()
gameInstance.play()