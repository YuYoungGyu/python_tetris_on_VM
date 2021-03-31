from tetris import *

class CTetris(Tetris):
    setOfColorBlockObjects = 0


    def init(setOfBlockArrays):
        Tetris.nBlockTypes = len(setOfBlockArrays)
        Tetris.nBlockDegrees = len(setOfBlockArrays[0])
        Tetris.setOfBlockObjects = [[0] * Tetris.nBlockDegrees for _ in range(Tetris.nBlockTypes)]
        arrayBlk_maxSize = 0
        for i in range(Tetris.nBlockTypes):
            if arrayBlk_maxSize <= len(setOfBlockArrays[i][0]):
                arrayBlk_maxSize = len(setOfBlockArrays[i][0])
        Tetris.iScreenDw = arrayBlk_maxSize     # larget enough to cover the largest block

        for i in range(Tetris.nBlockTypes):
            for j in range(Tetris.nBlockDegrees):
                Tetris.setOfBlockObjects[i][j] = Matrix(setOfBlockArrays[i][j])

        CTetris.setOfColorBlockObjects = [[0] * Tetris.nBlockDegrees for _ in range(Tetris.nBlockTypes)]

        for i in range(Tetris.nBlockTypes):
            for j in range(Tetris.nBlockDegrees):
                CTetris.setOfColorBlockObjects[i][j] = Matrix(setOfBlockArrays[i][j])
                CTetris.setOfColorBlockObjects[i][j].mulc(i+1)
                print(CTetris.setOfColorBlockObjects[i][j])
        return 

    def __init__(self, iScreenDy, iScreenDx):
        super().__init__(iScreenDy, iScreenDx)
        arrayColorScreen = self.createArrayScreen()
        self.iCScreen = Matrix(arrayColorScreen)
        self.oCScreen = Matrix(self.iScreen)
        return
        
    def accept(self, key):
        self.state = TetrisState.Running

        if key >= '0' and key <= '6':
            if self.justStarted == False:
                self.deleteFullLines()
            self.iScreen = Matrix(self.oScreen)
            self.iCScreen = Matrix(self.oCScreen)
            self.idxBlockType = int(key)
            self.idxBlockDegree = 0
            self.currBlk = Tetris.setOfBlockObjects[self.idxBlockType][self.idxBlockDegree]
            self.currColorBlk = self.setOfColorBlockObjects[self.idxBlockType][self.idxBlockDegree]
            self.top = 0
            self.left = Tetris.iScreenDw + self.iScreenDx//2 - self.currBlk.get_dx()//2
            self.tempBlk = self.iScreen.clip(self.top, self.left, self.top+self.currBlk.get_dy(), self.left+self.currBlk.get_dx())
            self.tempColorBlk = self.iCScreen.clip(self.top, self.left, self.top+self.currColorBlk.get_dy(), self.left+self.currColorBlk.get_dx())
            self.tempBlk = self.tempBlk + self.currBlk
            self.tempColorBlk = self.tempColorBlk + self.currColorBlk
            self.justStarted = False
            print()

            if self.tempBlk.anyGreaterThan(1):
                self.state = TetrisState.Finished
            self.oScreen = Matrix(self.iScreen)
            self.oScreen.paste(self.tempBlk, self.top, self.left)
            self.oCScreen = Matrix(self.iCScreen)
            self.oCScreen.paste(self.tempColorBlk, self.top, self.left)
            return self.state
        elif key == 'q':
            pass
        elif key == 'a': # move left
            self.left -= 1
        elif key == 'd': # move right
            self.left += 1
        elif key == 's': # move down
            self.top += 1
        elif key == 'w': # rotate the block clockwise
            self.idxBlockDegree = (self.idxBlockDegree + 1) % Tetris.nBlockDegrees
            self.currBlk = Tetris.setOfBlockObjects[self.idxBlockType][self.idxBlockDegree]
            self.currColorBlk = self.setOfColorBlockObjects[self.idxBlockType][self.idxBlockDegree]
        elif key == ' ': # drop the block
            while not self.tempBlk.anyGreaterThan(1):
                    self.top += 1
                    self.tempBlk = self.iScreen.clip(self.top, self.left, self.top+self.currBlk.get_dy(), self.left+self.currBlk.get_dx())
                    self.tempBlk = self.tempBlk + self.currBlk
                    self.tempColorBlk = self.iCScreen.clip(self.top, self.left, self.top+self.currColorBlk.get_dy(), self.left+self.currColorBlk.get_dx())
                    self.tempColorBlk = self.tempColorBlk + self.currColorBlk
        else:
            print('Wrong key!!!')
            
        self.tempBlk = self.iScreen.clip(self.top, self.left, self.top+self.currBlk.get_dy(), self.left+self.currBlk.get_dx())
        self.tempBlk = self.tempBlk + self.currBlk
        self.tempColorBlk = self.iCScreen.clip(self.top, self.left, self.top+self.currColorBlk.get_dy(), self.left+self.currColorBlk.get_dx())
        self.tempColorBlk = self.tempColorBlk + self.currColorBlk

        if self.tempBlk.anyGreaterThan(1):   ## 벽 충돌시 undo 수행
            if key == 'a': # undo: move right
                self.left += 1
            elif key == 'd': # undo: move left
                self.left -= 1
            elif key == 's': # undo: move up
                self.top -= 1
                self.state = TetrisState.NewBlock
            elif key == 'w': # undo: rotate the block counter-clockwise
                self.idxBlockDegree = (self.idxBlockDegree - 1) % Tetris.nBlockDegrees
                self.currBlk = Tetris.setOfBlockObjects[self.idxBlockType][self.idxBlockDegree]
                self.currColorBlk = self.setOfColorBlockObjects[self.idxBlockType][self.idxBlockDegree]
            elif key == ' ': # undo: move up
                self.top -= 1
                self.state = TetrisState.NewBlock
            
            self.tempBlk = self.iScreen.clip(self.top, self.left, self.top+self.currBlk.get_dy(), self.left+self.currBlk.get_dx())
            self.tempBlk = self.tempBlk + self.currBlk
            self.tempColorBlk = self.iCScreen.clip(self.top, self.left, self.top+self.currColorBlk.get_dy(), self.left+self.currColorBlk.get_dx())
            self.tempColorBlk = self.tempColorBlk + self.currColorBlk

        self.oScreen = Matrix(self.iScreen)
        self.oScreen.paste(self.tempBlk, self.top, self.left)
        self.oCScreen = Matrix(self.iCScreen)
        self.oCScreen.paste(self.tempColorBlk, self.top, self.left)
        return self.state

    def deleteFullLines(self):
        array = self.oScreen.get_array()
        colorArray = self.oCScreen.get_array()
        for y in range(self.iScreenDy):
            fullLine = True
            for x in range (self.iScreenDx):
                if array[y][Tetris.iScreenDw + x] != 1: 
                    fullLine = False
                    break
            if fullLine == True: 
                for row in range(y, 0, -1): 
                    array[row] = array[row-1]
                    colorArray[row] = colorArray[row-1]
                for x in range(Tetris.iScreenDw):
                    array[0][x] = 1
                    colorArray[0][x] = 1
                for x in range(self.iScreenDx):
                    array[0][Tetris.iScreenDw + x] = 0
                    colorArray[0][Tetris.iScreenDw + x] = 0
                for x in range(Tetris.iScreenDw):
                    array[0][Tetris.iScreenDw + self.iScreenDx + x] = 1
                    colorArray[0][Tetris.iScreenDw + self.iScreenDx + x] = 1
        self.iScreen = Matrix(array)
        self.iScreen.paste(self.tempBlk, self.top, self.left)
        self.iCScreen = Matrix(colorArray)
        self.iCScreen.paste(self.tempColorBlk, self.top, self.left)
        return
