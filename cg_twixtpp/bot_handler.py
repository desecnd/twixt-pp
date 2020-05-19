from subprocess import Popen, PIPE

class BotHandler():
    def activateBot(self):
        self.processHandle = Popen('cg_twixtpp/bot', stdin=PIPE,  stdout=PIPE, stderr=PIPE, shell=True) 

    def queryBot(self, enemyMove):
        if self.processHandle.poll():
            return None 
        
        self.processHandle.stdin.write(bytes(enemyMove + "\n", 'UTF-8'))
        self.processHandle.stdin.flush()

        move = self.processHandle.stdout.readline().decode('UTF-8').split()
        return move[0]
        
    def terminateBot(self):
        self.processHandle.terminate()

if __name__ == '__main__':
    bhand = BotHandler()
    bhand.activateBot()
    print(bhand.queryBot('B3'))
    bhand.terminateBot()