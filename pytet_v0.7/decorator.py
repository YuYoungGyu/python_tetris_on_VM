from tetris import *
from game import *
from matrix import *

##################################################
### ColorDecorator for Tetris Class
##################################################

class ColorDecorator(Game):
	def initCBlocks(self, setOfBlockObjects):
		### initialize self.setOfCBlockObjects
		self.blockType = len(setOfBlockObjects)
		self.blockDgree = len(setOfBlockObjects[0])
		self.setOfCBlockObjects = []
		for j in range(self.blockType):
			blockTypeArray = []
			for i in range(self.blockDgree):
				blockTypeArray.append(Matrix(setOfBlockObjects[j][i].mulc(j+1)))
			self.setOfCBlockObjects.append(blockTypeArray)
		return
		
	def __init__(self, game):
		self.game = game
		self.initCBlocks(game.setOfBlockObjects)
		arrayScreen = game.createArrayScreen()
		self.iCScreen = Matrix(arrayScreen)
		self.oCScreen = Matrix(self.iCScreen)
		return
	
	def accept(self, key):
		self.state = TetrisState.Running

		if key >= '0' and key <= '6':
			if self.game.justStarted == False:
				self.deleteFullLines()
			self.game.iScreen = Matrix(self.game.oScreen)
			self.iCScreen = Matrix(self.oCScreen)
			self.idxBlockType = int(key)
			self.idxBlockDegree = 0
			self.game.currBlk = Tetris.setOfBlockObjects[self.idxBlockType][self.idxBlockDegree]
			self.currColorBlk = self.setOfCBlockObjects[self.idxBlockType][self.idxBlockDegree]
			self.top = 0
			self.left = Tetris.iScreenDw + self.game.iScreenDx//2 - self.game.currBlk.get_dx()//2
			self.game.tempBlk = self.game.iScreen.clip(self.top, self.left, self.top+self.game.currBlk.get_dy(), self.left+self.game.currBlk.get_dx())
			self.tempColorBlk = self.iCScreen.clip(self.top, self.left, self.top+self.currColorBlk.get_dy(), self.left+self.currColorBlk.get_dx())
			self.game.tempBlk = self.game.tempBlk + self.game.currBlk
			self.tempColorBlk = self.tempColorBlk + self.currColorBlk
			self.game.justStarted = False
			print()

			if self.game.tempBlk.anyGreaterThan(1):
				self.state = TetrisState.Finished
			self.game.oScreen = Matrix(self.game.iScreen)
			self.game.oScreen.paste(self.game.tempBlk, self.top, self.left)
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
			self.game.currBlk = Tetris.setOfBlockObjects[self.idxBlockType][self.idxBlockDegree]
			self.currColorBlk = self.setOfCBlockObjects[self.idxBlockType][self.idxBlockDegree]
		elif key == ' ': # drop the block
			while not self.game.tempBlk.anyGreaterThan(1):
					self.top += 1
					self.game.tempBlk = self.game.iScreen.clip(self.top, self.left, self.top+self.game.currBlk.get_dy(), self.left+self.game.currBlk.get_dx())
					self.game.tempBlk = self.game.tempBlk + self.game.currBlk
					self.tempColorBlk = self.iCScreen.clip(self.top, self.left, self.top+self.currColorBlk.get_dy(), self.left+self.currColorBlk.get_dx())
					self.tempColorBlk = self.tempColorBlk + self.currColorBlk
		else:
			print('Wrong key!!!')
			
		self.game.tempBlk = self.game.iScreen.clip(self.top, self.left, self.top+self.game.currBlk.get_dy(), self.left+self.game.currBlk.get_dx())
		self.game.tempBlk = self.game.tempBlk + self.game.currBlk
		self.tempColorBlk = self.iCScreen.clip(self.top, self.left, self.top+self.currColorBlk.get_dy(), self.left+self.currColorBlk.get_dx())
		self.tempColorBlk = self.tempColorBlk + self.currColorBlk

		if self.game.tempBlk.anyGreaterThan(1):   ## 벽 충돌시 undo 수행
			if key == 'a': # undo: move right
				self.left += 1
			elif key == 'd': # undo: move left
				self.left -= 1
			elif key == 's': # undo: move up
				self.top -= 1
				self.state = TetrisState.NewBlock
			elif key == 'w': # undo: rotate the block counter-clockwise
				self.idxBlockDegree = (self.idxBlockDegree - 1) % Tetris.nBlockDegrees
				self.game.currBlk = Tetris.setOfBlockObjects[self.idxBlockType][self.idxBlockDegree]
				self.currColorBlk = self.setOfCBlockObjects[self.idxBlockType][self.idxBlockDegree]
			elif key == ' ': # undo: move up
				self.top -= 1
				self.state = TetrisState.NewBlock
			
			self.game.tempBlk = self.game.iScreen.clip(self.top, self.left, self.top+self.game.currBlk.get_dy(), self.left+self.game.currBlk.get_dx())
			self.game.tempBlk = self.game.tempBlk + self.game.currBlk
			self.tempColorBlk = self.iCScreen.clip(self.top, self.left, self.top+self.currColorBlk.get_dy(), self.left+self.currColorBlk.get_dx())
			self.tempColorBlk = self.tempColorBlk + self.currColorBlk

		self.game.oScreen = Matrix(self.game.iScreen)
		self.game.oScreen.paste(self.game.tempBlk, self.top, self.left)
		self.oCScreen = Matrix(self.iCScreen)
		self.oCScreen.paste(self.tempColorBlk, self.top, self.left)
		return self.state	

	def getScreen(self):
		return self.oCScreen

	def deleteFullLines(self):
		array = self.game.oScreen.get_array()
		colorArray = self.oCScreen.get_array()
		for y in range(self.game.iScreenDy):
			fullLine = True
			for x in range(self.game.iScreenDx):
				if array[y][Tetris.iScreenDw+x] != 1:
					fullLine = False
					break
			if fullLine == True: 
				for row in range(y, 0, -1): 
					array[row] = array[row-1]
					colorArray[row] = colorArray[row-1]
				for x in range(Tetris.iScreenDw):
					array[0][x] = 1
					colorArray[0][x] = 1
				for x in range(self.game.iScreenDx):
					array[0][Tetris.iScreenDw + x] = 0
					colorArray[0][Tetris.iScreenDw + x] = 0
				for x in range(Tetris.iScreenDw):
					array[0][Tetris.iScreenDw + self.game.iScreenDx + x] = 1
					colorArray[0][Tetris.iScreenDw + self.game.iScreenDx + x] = 1
		self.game.iScreen = Matrix(array)
		self.game.iScreen.paste(self.tempBlk, self.top, self.left)
		self.iCScreen = Matrix(colorArray)
		self.iCScreen.paste(self.tempColorBlk, self.top, self.left)
		return