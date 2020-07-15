from subprocess import Popen, PIPE
import pathlib

class BotHandler():
    def __init__(self, botFile):
        self.botPath = str(pathlib.Path(__file__).parent.absolute()) + '/' + botFile
        self.processHandle = Popen(self.botPath, stdin=PIPE,  stdout=PIPE, stderr=PIPE, shell=True) 

    def getMove(self, enemyMove):
        if self.processHandle.poll():
            return None 
        
        self.processHandle.stdin.write(bytes(enemyMove + "\n", 'UTF-8'))
        self.processHandle.stdin.flush()

        move = self.processHandle.stdout.readline().decode('UTF-8').split()
        return move[0]
        
    def __del__(self):
        self.processHandle.terminate()

if __name__ == '__main__':
    bhand = BotHandler('bot')
    print(bhand.getMove('FIRST'))