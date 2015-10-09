import sys 
import random
import signal

#Timer handler, helper function

class TimedOutExc(Exception):
        pass

def handler(signum, frame):
    #print 'Signal handler called with signal', signum
    raise TimedOutExc()

class Player1:
	
	def __init__(self):
		pass

	def get_cells_allowed(self, old_move, temp_board, temp_block):
		for_corner = [0,2,3,5,6,8]

		#List of permitted blocks, based on old move.
		blocks_allowed  = []

		if old_move[0] in for_corner and old_move[1] in for_corner:
			## we will have 3 representative blocks, to choose from

			if old_move[0] % 3 == 0 and old_move[1] % 3 == 0:
				## top left 3 blocks are allowed
				blocks_allowed = [0, 1, 3]
			elif old_move[0] % 3 == 0 and old_move[1] in [2, 5, 8]:
				## top right 3 blocks are allowed
				blocks_allowed = [1,2,5]
			elif old_move[0] in [2,5, 8] and old_move[1] % 3 == 0:
				## bottom left 3 blocks are allowed
				blocks_allowed  = [3,6,7]
			elif old_move[0] in [2,5,8] and old_move[1] in [2,5,8]:
				### bottom right 3 blocks are allowed
				blocks_allowed = [5,7,8]
			else:
				print "SOMETHING REALLY WEIRD HAPPENED!"
				sys.exit(1)
		else:
		#### we will have only 1 block to choose from (or maybe NONE of them, which calls for a free move)
			if old_move[0] % 3 == 0 and old_move[1] in [1,4,7]:
				## upper-center block
				blocks_allowed = [1]
	
			elif old_move[0] in [1,4,7] and old_move[1] % 3 == 0:
				## middle-left block
				blocks_allowed = [3]
		
			elif old_move[0] in [2,5,8] and old_move[1] in [1,4,7]:
				## lower-center block
				blocks_allowed = [7]

			elif old_move[0] in [1,4,7] and old_move[1] in [2,5,8]:
				## middle-right block
				blocks_allowed = [5]
			elif old_move[0] in [1,4,7] and old_move[1] in [1,4,7]:
				blocks_allowed = [4]

                for i in reversed(blocks_allowed):
                    if temp_block[i] != '-':
                        blocks_allowed.remove(i)
	# We get all the empty cells in allowed blocks. If they're all full, we get all the empty cells in the entire board.
		cells = get_empty_out_of(temp_board, blocks_allowed,temp_block)
		return (cells,blocks_allowed)
		
	def getCurrentBlockStructure(self, currentBlock):
		if currentBlock == 0:
			return [(0,2), (0,2)]
		elif currentBlock == 1:
			return [(0,2), (3,5)]
		elif currentBlock == 2:
			return [(0,2), (6,8)]
		elif currentBlock == 3:
			return [(3,5), (0,2)]
		elif currentBlock == 4:
			return [(3,5), (3,5)]
		elif currentBlock == 5:
			return [(3,5), (6,8)]
		elif currentBlock == 6:
			return [(6,8), (0,2)]
		elif currentBlock == 7:
			return [(6,8), (3,5)]
		elif currentBlock == 8:
			return [(6,8), (6,8)]

	def calculateHeuristic(self, currentBlockStructure, temp_board, temp_block, playerNum):
		playerSymbol = ''
		if(playerNum == 1):
			playerSymbol = 'x'
		else:
		 	playerSymbol = 'o'
		
		countRow = [[0,0,0], [0, 0, 0]]
		countColumn = [[0,0,0], [0, 0, 0]]
		countDiagonal = [[0, 0], [0, 0]]

		for i in range(currentBlockStructure[0][0], currentBlockStructure[0][1]+1):
			for j in range(currentBlockStructure[1][0], currentBlockStructure[1][1]+1):
				if(temp_board[i][j] == playerSymbol):
					countRow[0][i%3] += 1;
				elif temp_board[i][j] != '-':
					countRow[1][i%3] += 1;
		
		for i in range(currentBlockStructure[1][0], currentBlockStructure[1][1]+1):	
			for j in range(currentBlockStructure[0][0], currentBlockStructure[0][1]+1):
				if(temp_board[j][i] == playerSymbol):
					countColumn[0][i%3] += 1;
				elif temp_board[j][i] != '-':
					countColumn[1][i%3] += 1;

		
		for i in range(0, 3):
			if(temp_board[currentBlockStructure[0][0]+i][currentBlockStructure[1][0]+i] == playerSymbol):
				countDiagonal[0][0] += 1;
			elif(temp_board[currentBlockStructure[0][0]+i][currentBlockStructure[1][0]+i] != '-'):
				countDiagonal[1][0] += 1;
		
		for i in range(0, 3):
			if(temp_board[currentBlockStructure[0][1]-i][currentBlockStructure[1][1]-i] == playerSymbol):
				countDiagonal[0][1] += 1;
			elif(temp_board[currentBlockStructure[0][1]-i][currentBlockStructure[1][1]-i] != '-'):
				countDiagonal[1][1] += 1;

		heuristicSum = 0
		for i in range(0, 2):
			for j in range(0, 3):
				if i == 0 and countRow[i][j] and countColumn[i][j]:
					heuristicSum += 10 ** (countRow[i][j]-1)
					heuristicSum += 10 ** (countColumn[i][j]-1)
				elif i == 1 and countRow[i][j] and countColumn[i][j]:
					heuristicSum -= 10 ** (countRow[i][j]-1)
					heuristicSum -= 10 ** (countColumn[i][j]-1)
		
		for i in range(0, 2):
			for j in range(0, 2):
				if i == 0 and countDiagonal[i][j]:
					heuristicSum += 10 ** (countDiagonal[i][j]-1)
					heuristicSum += 10 ** (countDiagonal[i][j]-1)
				elif i == 1 and countDiagonal[i][j]:
					heuristicSum -= 10 ** (countDiagonal[i][j]-1)
					heuristicSum -= 10 ** (countDiagonal[i][j]-1)
		
		countRow = [[0,0,0],[0,0,0]]
		countColumn = [[0,0,0],[0,0,0]]

		for i in range(0, 3):
			for j in range(0,3):
				if temp_block[i*3+j] == playerSymbol:
					countRow[0][i] += 1
				elif temp_block[i*3+j] != '-':
					countRow[1][i] += 1

		for i in range(0, 3):
			for j in range(0,3):
				if temp_block[i+j*3] == playerSymbol:
					countColumn[0][i] += 1
				elif temp_block[i+j*3] != '-':
					countColumn[1][i] += 1
		
		countDiagonal = [[0, 0], [0,0]]

		for i in range(0, 3):
			if temp_block[i*4] == playerSymbol:
				countDiagonal[0][0] += 1
			elif temp_block[i*4] != '-':
				countDiagonal[1][0] += 1

		for i in range(0, 3):
			if temp_block[(i+1)*2] == playerSymbol:
				countDiagonal[0][1] += 1
			elif temp_block[(i+1)*2] == playerSymbol:
				countDiagonal[1][1] += 1

		boardHeuristicSum = 0	
		for i in range(0, 2):
			for j in range(0, 3):
				if i == 0 and countRow[i][j] and countColumn[i][j]:
					boardHeuristicSum += 10 ** (countRow[i][j]-1)
					boardHeuristicSum += 10 ** (countColumn[i][j]-1)
				elif i == 1 and countRow[i][j] and countColumn[i][j]:
					boardHeuristicSum -= 10 ** (countRow[i][j]-1)
					boardHeuristicSum -= 10 ** (countColumn[i][j]-1)
		
		for i in range(0, 2):
			for j in range(0, 2):
				if i == 0 and countDiagonal[i][j]:
					boardHeuristicSum += 10 ** (countDiagonal[i][j]-1)
					boardHeuristicSum += 10 ** (countDiagonal[i][j]-1)
				elif i == 1 and countDiagonal[i][j]:
					boardHeuristicSum -= 10 ** (countDiagonal[i][j]-1)
					boardHeuristicSum -= 10 ** (countDiagonal[i][j]-1)
		return 0.25*boardHeuristicSum + 0.75*heuristicSum

	def minimax(self, temp_board, temp_block, playerNum, cell, max_depth):
		if max_depth == 0:
			currentBlock = (cell[0]/3)*3 + (cell[1]/3)
			currentBlockStructure = self.getCurrentBlockStructure(currentBlock)
			return self.calculateHeuristic(currentBlockStructure, temp_board, temp_block, playerNum)
	
		best_value = sys.maxsize * -playerNum

		cells, blocksAllowed = self.get_cells_allowed(cell, temp_board, temp_block)
		
		for i in cells:
			cell_val = self.minimax(temp_board, temp_block, -playerNum, i, max_depth-1)
			if(abs(sys.maxsize * playerNum - cell_val) < abs(sys.maxsize * playerNum - best_value)):
				best_value = cell_val

		return best_value

	def checkOpponent(self, temp_board, playerNum, cell):	
		playerSymbol = ''
		if(playerNum == 1):
			playerSymbol = 'x'
		else:
		 	playerSymbol = 'o'

		currentBlock = (cell[0]/3)*3 + cell[1]/3
		currentBlockStructure = self.getCurrentBlockStructure(currentBlock)
		if (cell[0] == currentBlockStructure[0][0] and cell[1] == currentBlockStructure[1][0]):
			if (temp_board[cell[0]][cell[1]+1] != playerSymbol and temp_board[cell[0]][cell[1]+1] != '-') and (temp_board[cell[0]][cell[1]+2] != playerSymbol and temp_board[cell[0]][cell[1]+2] != '-'):
				return 1
			if (temp_board[cell[0]+1][cell[1]] != playerSymbol and temp_board[cell[0]+1][cell[1]] != '-') and (temp_board[cell[0]+2][cell[1]] != playerSymbol and temp_board[cell[0]+2][cell[1]] != '-'):
				return 1
			if (temp_board[cell[0]+1][cell[1]+1] != playerSymbol and temp_board[cell[0]+1][cell[1]+1] != '-') and (temp_board[cell[0]+2][cell[1]+2] != playerSymbol and temp_board[cell[0]+2][cell[1]+2] != '-'):
				return 1


		if (cell[0] == currentBlockStructure[0][0] and cell[1] == currentBlockStructure[1][1]):
			if (temp_board[cell[0]][cell[1]-1] != playerSymbol and temp_board[cell[0]][cell[1]-1] != '-') and (temp_board[cell[0]][cell[1]-2] != playerSymbol and temp_board[cell[0]][cell[1]-2] != '-'):
				#print "I m here!!"
				return 1

			if (temp_board[cell[0]+1][cell[1]] != playerSymbol and temp_board[cell[0]+1][cell[1]] != '-') and (temp_board[cell[0]+2][cell[1]] != playerSymbol and temp_board[cell[0]+2][cell[1]] != '-'):
				return 1
			if (temp_board[cell[0]+1][cell[1]-1] != playerSymbol and temp_board[cell[0]+1][cell[1]-1] != '-') and (temp_board[cell[0]+2][cell[1]-2] != playerSymbol and temp_board[cell[0]+2][cell[1]-2] != '-'):
				return 1
		
		if (cell[0] == currentBlockStructure[0][1] and cell[1] == currentBlockStructure[1][0]):
			if (temp_board[cell[0]][cell[1]+1] != playerSymbol and temp_board[cell[0]][cell[1]+1] != '-') and (temp_board[cell[0]][cell[1]+2] != playerSymbol and temp_board[cell[0]][cell[1]+2] != '-'):
				return 1
			if (temp_board[cell[0]-1][cell[1]] != playerSymbol and temp_board[cell[0]-1][cell[1]] != '-') and (temp_board[cell[0]-2][cell[1]] != playerSymbol and temp_board[cell[0]-2][cell[1]] != '-'):
				return 1
			if (temp_board[cell[0]-1][cell[1]+1] != playerSymbol and temp_board[cell[0]-1][cell[1]+1] != '-') and (temp_board[cell[0]-2][cell[1]+2] != playerSymbol and temp_board[cell[0]-2][cell[1]+2] != '-'):
				return 1
		
		if (cell[0] == currentBlockStructure[0][1] and cell[1] == currentBlockStructure[1][1]):
			if (temp_board[cell[0]][cell[1]-1] != playerSymbol and temp_board[cell[0]][cell[1]-1] != '-') and (temp_board[cell[0]][cell[1]-2] != playerSymbol and temp_board[cell[0]][cell[1]-2] != '-'):
				return 1
			if (temp_board[cell[0]-1][cell[1]] != playerSymbol and temp_board[cell[0]-1][cell[1]] != '-') and (temp_board[cell[0]-2][cell[1]] != playerSymbol and temp_board[cell[0]-2][cell[1]] != '-'):
				return 1
			if (temp_board[cell[0]-1][cell[1]-1] != playerSymbol and temp_board[cell[0]-1][cell[1]-1] != '-') and (temp_board[cell[0]-2][cell[1]-2] != playerSymbol and temp_board[cell[0]-2][cell[1]-2] != '-'):
				return 1

		if (cell[0] == currentBlockStructure[0][0] and cell[1] == currentBlockStructure[1][0]+1):
			if (temp_board[cell[0]][cell[1]-1] != playerSymbol and temp_board[cell[0]][cell[1]-1] != '-') and (temp_board[cell[0]][cell[1]+1] != playerSymbol and temp_board[cell[0]][cell[1]+1] != '-'):
				return 1
			if (temp_board[cell[0]+1][cell[1]] != playerSymbol and temp_board[cell[0]+1][cell[1]] != '-') and (temp_board[cell[0]+2][cell[1]] != playerSymbol and temp_board[cell[0]+2][cell[1]] != '-'):
				return 1
		
		if (cell[0] == currentBlockStructure[0][0]+1 and cell[1] == currentBlockStructure[1][0]):
			if (temp_board[cell[0]][cell[1]+1] != playerSymbol and temp_board[cell[0]][cell[1]+1] != '-') and (temp_board[cell[0]][cell[1]+2] != playerSymbol and temp_board[cell[0]][cell[1]+2] != '-'):
				return 1
			if (temp_board[cell[0]+1][cell[1]] != playerSymbol and temp_board[cell[0]+1][cell[1]] != '-') and (temp_board[cell[0]-1][cell[1]] != playerSymbol and temp_board[cell[0]-1][cell[1]] != '-'):
				return 1
		
		if (cell[0] == currentBlockStructure[0][0]+2 and cell[1] == currentBlockStructure[1][0]+1):
			if (temp_board[cell[0]][cell[1]-1] != playerSymbol and temp_board[cell[0]][cell[1]-1] != '-') and (temp_board[cell[0]][cell[1]+1] != playerSymbol and temp_board[cell[0]][cell[1]+1] != '-'):
				return 1
			if (temp_board[cell[0]-1][cell[1]] != playerSymbol and temp_board[cell[0]-1][cell[1]] != '-') and (temp_board[cell[0]-2][cell[1]] != playerSymbol and temp_board[cell[0]-2][cell[1]] != '-'):
				return 1

		if (cell[0] == currentBlockStructure[0][0]+1 and cell[1] == currentBlockStructure[1][0]+2):
			if (temp_board[cell[0]][cell[1]-1] != playerSymbol and temp_board[cell[0]][cell[1]-1] != '-') and (temp_board[cell[0]][cell[1]-2] != playerSymbol and temp_board[cell[0]][cell[1]-2] != '-'):
				return 1
			if (temp_board[cell[0]+1][cell[1]] != playerSymbol and temp_board[cell[0]+1][cell[1]] != '-') and (temp_board[cell[0]-1][cell[1]] != playerSymbol and temp_board[cell[0]-1][cell[1]] != '-'):
				return 1

		if (cell[0] == currentBlockStructure[0][0]+1 and cell[1] == currentBlockStructure[1][0]+1):
			if (temp_board[cell[0]][cell[1]-1] != playerSymbol and temp_board[cell[0]][cell[1]-1] != '-') and (temp_board[cell[0]][cell[1]+1] != playerSymbol and temp_board[cell[0]][cell[1]+1] != '-'):
				return 1
			if (temp_board[cell[0]+1][cell[1]] != playerSymbol and temp_board[cell[0]+1][cell[1]] != '-') and (temp_board[cell[0]-1][cell[1]] != playerSymbol and temp_board[cell[0]-1][cell[1]] != '-'):
				return 1
			if (temp_board[cell[0]+1][cell[1]+1] != playerSymbol and temp_board[cell[0]+1][cell[1]+1] != '-') and (temp_board[cell[0]-1][cell[1]-1] != playerSymbol and temp_board[cell[0]-1][cell[1]-1] != '-'):
				return 1
			if (temp_board[cell[0]+1][cell[1]-1] != playerSymbol and temp_board[cell[0]+1][cell[1]-1] != '-') and (temp_board[cell[0]-1][cell[1]+1] != playerSymbol and temp_board[cell[0]-1][cell[1]+1] != '-'):
				return 1
				
	def move(self,temp_board,temp_block,old_move,flag):
		cells, blocks_allowed = self.get_cells_allowed(old_move, temp_board, temp_block)
		if old_move == (-1, -1):
			return cells[random.randrange(len(cells))]
		else:
		 	best_cell = ()

			if flag == 'x':
				playerNum = 1
			else:
				playerNum = -1

			best_value = sys.maxsize * -playerNum

			for i in cells:
				if(not self.checkOpponent(temp_board, playerNum, i)):
					cell_val = self.minimax(temp_board, temp_block, playerNum, i, 3)
					if(abs(sys.maxsize * playerNum - cell_val) < abs(sys.maxsize * playerNum - best_value)):
						best_value = cell_val
						best_cell = i
				else:
				  	return i
			if best_value == 0:
			   	return cells[random.randrange(len(cells))]
			else:
				return best_cell
				
#Gets empty cells from the list of possible blocks. Hence gets valid moves. 
def get_empty_out_of(gameb, blal,block_stat):
	cells = []  # it will be list of tuples
	#Iterate over possible blocks and get empty cells
	for idb in blal:
		id1 = idb/3
		id2 = idb%3
		for i in range(id1*3,id1*3+3):
			for j in range(id2*3,id2*3+3):
				if gameb[i][j] == '-':
					cells.append((i,j))

	# If all the possible blocks are full, you can move anywhere
	if cells == []:
		for i in range(9):
			for j in range(9):
                                no = (i/3)*3
                                no += (j/3)
				if gameb[i][j] == '-' and block_stat[no] == '-':
					cells.append((i,j))	
	return cells
		



