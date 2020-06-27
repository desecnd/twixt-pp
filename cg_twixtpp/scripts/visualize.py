# autor: pavveu

from tkinter import Tk, Canvas
import numpy as np
from subprocess import Popen, PIPE
import pathlib

dotsCnt = 12
boardSize = 1000
dotsDist = boardSize / dotsCnt
lineWidth = boardSize * .005
dotRadius = boardSize * .020
pegRadius = boardSize * .005
directions = [ [-1, 2], [ 1, 2], [-2,-1], [-2, 1], [ 1,-2], [-1,-2], [2, 1], [2,-1] ]

bgColor = '#303030'
pegColor= '#FFCC00'
p1Color = '#FF0066'
p2Color = '#0099FF'

class GraphicTwixTPP():
    def __init__(self):
        # self.gamePath = str(pathlib.Path(__file__).parent.absolute())
        self.gamePath = "bin/main"
        self.processHandle = Popen(self.gamePath, stdin=PIPE,  stdout=PIPE, stderr=PIPE, shell=True) 
        self.gameEnded = False

        self.window = Tk()
        self.window.title('TwixT-PP')
        self.canvas = Canvas(self.window, width=boardSize, height=boardSize)
        self.canvas.pack()

        self.canvas.configure(background=bgColor)
        self.window.bind('<Button-1>', self.click)

        self.getBoard()
        self.drawBoard()

    def posToMove(self, pos):
        return ''.join([chr(pos[1] + ord('A')), str(pos[0] + 1)])

    def moveToPos(self, move):
        return [ int(move[1:]) - 1, ord(move[0]) - ord('A')   ]

    def getBoard(self):
        self.pegs = np.zeros((dotsCnt, dotsCnt))
        self.links = []

        for i in range(dotsCnt):
            row = list(map(int, self.processHandle.stdout.readline().decode('UTF-8').split()))
            for j in range(dotsCnt):
                self.pegs[i][j] = row[j]

        linksCnt = int(self.processHandle.stdout.readline().decode('UTF-8'))
        for i in range(linksCnt):
            link = list(map(int, self.processHandle.stdout.readline().decode('UTF-8').split()))
            self.links.append(link)
        
        message = self.processHandle.stderr.readline().decode('UTF-8')
        print("MESSAGE: ", message)


    def sendMove(self, move):
        self.processHandle.stdin.write(bytes(move + "\n", 'UTF-8'))
        self.processHandle.stdin.flush()
        
    def __del__(self):
        self.processHandle.terminate()

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
        self.canvas.delete('all')

        for link in self.links:
            player = link[0]

            y1 = link[1]*dotsDist + dotsDist/2
            x1 = link[2]*dotsDist + dotsDist/2
            y2 = link[3]*dotsDist + dotsDist/2
            x2 = link[4]*dotsDist + dotsDist/2

            if player == 1:
                color = p1Color
            else:
                color = p2Color

            self.canvas.create_line(x1,y1, x2,y2, fill=color, width = lineWidth)

        for i in range(dotsCnt):
            for j in range(dotsCnt):
                y = i*dotsDist + dotsDist/2
                x = j*dotsDist + dotsDist/2

                radius = pegRadius
                color = pegColor

                if self.pegs[i][j] == 1: 
                    color = p1Color
                    radius = dotRadius
                elif self.pegs[i][j] == 2:
                    color = p2Color
                    radius = dotRadius

                self.canvas.create_oval(x-radius, y-radius, x + radius,y + radius,fill=color,outline=color)

        self.canvas.create_line(0, dotsDist, boardSize, dotsDist, fill=p1Color, width=lineWidth)
        self.canvas.create_line(0, boardSize - dotsDist, boardSize, boardSize - dotsDist, fill=p1Color, width=lineWidth)

        self.canvas.create_line(dotsDist, 0, dotsDist, boardSize, fill=p2Color, width = lineWidth)
        self.canvas.create_line(boardSize - dotsDist, 0, boardSize - dotsDist, boardSize, fill=p2Color, width = lineWidth)
    
    def playGame(self):
        self.window.mainloop()

    def click(self, event):
        gridPos = [event.x, event.y]
        boardPos = self.gridToBoardPosition(gridPos)
        if not self.gameEnded and boardPos:

            if self.processHandle.poll() != None:
                winner = self.processHandle.poll()
                self.gameEnded = True
                print("Player", winner, "wins!")
            else:
                self.sendMove(self.posToMove(boardPos))
                self.getBoard()
                self.drawBoard()

# gameInstance = ConsoleTwixTPP()
# print(gameInstance.play(BotHandler('bot'), BotHandler('bot'), 1))
gameInstance = GraphicTwixTPP()
gameInstance.playGame()