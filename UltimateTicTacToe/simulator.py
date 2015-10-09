import sys
import random
import signal

#Timer handler, helper function

class TimedOutExc(Exception):
        pass

def handler(signum, frame):
    #print 'Signal handler called with signal', signum
    raise TimedOutExc()


class Manual_player:
	def __init__(self):
		pass
	def move(self, temp_board, temp_block, old_move, flag):
		print 'Enter your move: <format:row column> (you\'re playing with', flag + ")"	
		mvp = raw_input()
		mvp = mvp.split()
		return (int(mvp[0]), int(mvp[1]))
		

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
				if i == 0:
					if countRow[i][j]:
						heuristicSum += 15 ** (countRow[i][j]-1)
					elif countColumn[i][j]:
						heuristicSum += 15 ** (countColumn[i][j]-1)

				elif i == 1: 
					if countRow[i][j]:
						heuristicSum -= 15 ** (countRow[i][j]-1)
					elif countColumn[i][j]:
						heuristicSum -= 15 ** (countColumn[i][j]-1)
		
		for i in range(0, 2):
			for j in range(0, 2):
				if i == 0 and countDiagonal[i][j]:
					heuristicSum += 15 ** (countDiagonal[i][j]-1)
					heuristicSum += 15 ** (countDiagonal[i][j]-1)
				elif i == 1 and countDiagonal[i][j]:
					heuristicSum -= 15 ** (countDiagonal[i][j]-1)
					heuristicSum -= 15 ** (countDiagonal[i][j]-1)
		
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
		return 0.2*boardHeuristicSum + 0.8*heuristicSum

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
				print "I m here!!"
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
#	return (4, 4)
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
					print i
					print ": %d" % cell_val
					print "best value: %d" % best_value		
					if(cell_val > best_value):
						print i
						print ": %d" % cell_val	
						best_value = cell_val
						best_cell = i
				else:
				  	return i
			if best_value == 0:
				return cells[random.randrange(len(cells))]
			else:
				return best_cell
				

class Player2:
	
        def __init__(self):
                pass
        def move(self,temp_board,temp_block,old_move,flag):
                print "------------------------------   "+ flag +"   ------------------------------"
                a = -999999
                b = 999999
                return self.alphabeta(temp_board,temp_block,5 , a , b, True,old_move,flag)

        def alphabeta(self, board, block, depth, a, b, maximizingPlayer, old_move, flag):
                if(depth == 0):
                        return self.heuristic(board, block, flag)

                if maximizingPlayer:
                        v = -999999
                        cells = get_empty_out_of(board, self.blocks_allowed(old_move, block), block);
                        if len(cells) == 0:
                                cells = get_empty_out_of(board, range(0,8), block);

                        temp = cells[0];

                        for i in cells:

                                temp_board = []
                                for j in xrange(9):
                                        row = ['-']*9
                                        temp_board.append(row)

                                new_block = ['-']*9
                                for j in xrange(0,len(block)):
                                        new_block[j] = block[j]

                                for j in xrange(0,9):
                                        for k in xrange(0,9):
                                                temp_board[j][k] = board[j][k]

                                update_lists(temp_board,new_block,i,flag)
                                vtemp = self.alphabeta(temp_board, new_block,depth - 1, a, b, False, i, flag)
                                if(vtemp > v):
                                        v = vtemp
                                        temp = i

                                a = max(a,v)
                                if(b <= a):
                                        break

                        if(depth == 5):
                                return temp
                        else:
                                return v
                else:
                        v = 999999
                        cells = get_empty_out_of(board, self.blocks_allowed(old_move, block), block);
                        if len(cells) == 0:
                                cells = get_empty_out_of(board, range(0,8), block);
                        temp = cells[0];

                        for i in cells:

                                new_block = ['-']*9
                                for j in xrange(0,9):
                                        new_block[j] = block[j]

                                temp_board = []
                                for j in xrange(9):
                                        row = ['-']*9
                                        temp_board.append(row)

                                for j in xrange(0,9):
                                        for k in xrange(0,9):
                                                temp_board[j][k] = board[j][k]

                                if flag == 'x':
                                        flag1 = 'o'
                                elif flag == 'o':
                                        flag1 = 'x'

                                update_lists(temp_board, new_block, i, flag1)
                                vtemp = self.alphabeta(temp_board,new_block,depth - 1, a , b , True,i,flag)

                                if(vtemp < v):
                                        v = vtemp
                                        temp = i

                                b = min(b,v)
                                if(b <= a):
                                        break

                        if(depth == 5):
                                return temp
                        else:
                                return v


        def heuristic(self, board, block, flag):
                count1 = 0
                count2 = 0
                value = 0
                for k in xrange(0,3):
                        count1 = 0
                        count2 = 0
                        space = []
                        l = 0
                        for i in xrange(k*3,k*3+3):
                                if block[i] == flag:
                                        count1 += 1
                                elif block[i] != '-':
                                        count2 += 1
                                elif block[i] == '-':
                                        space.append(i)
                                        l += 1

                        if count1 == 3:
                                return 100
                        elif count2 == 3:
                                return -100
                        elif count1 == 0 and count2 == 1:
                                value -= 10
                        elif count1 == 0 and count2 == 2:
                                value -= 40
                        elif count2 == 0 and count1 == 1:
                                for j in xrange(0,l):
                                        inc = 0
                                        new_block = ['-']*9
                                        r = space[j]
                                        row = r/3
                                        col = r%3
                                        for m in xrange(row*3,row*3+3):
                                                for n in xrange(col*3,col*3+3):
                                                        new_block[inc] = board[m][n]
                                                        inc += 1
                                        value += self.cell_hue(new_block,flag)
                                value += 10
                        elif count2 == 0 and count1 == 2:
                                for j in xrange(0,l):
                                        inc = 0
                                        new_block = ['-']*9
                                        r = space[j]
                                        row = r/3
                                        col = r%3
                                        for m in xrange(row*3,row*3+3):
                                                for n in xrange(col*3,col*3+3):
                                                        new_block[inc] = board[m][n]
                                                        inc += 1
                                        value += self.cell_hue(new_block,flag)
                                value += 40
                        elif count2 < count1:
                                value += 20
                        elif count2 > count1:
                                value -= 20
                        elif count2 == count1:
                                value += 10

                for k in xrange(0,3):
                        count1 = 0
                        count2 = 0
                        space1 = []
                        l = 0
                        for i in xrange(0,3):
                                if block[i*3+k] == flag:
                                        count1 += 1
                                elif block[i*3+k] != '-':
                                        count2 += 1
                                elif block[i] == '-':
                                        space1.append(i)
                                        l += 1

                        if count1 == 3:
                                return 100
                        elif count2 == 3:
                                return -100
                        elif count1 == 0 and count2 == 1:
                                value -= 10
                        elif count1 == 0 and count2 == 2:
                                value -= 40
                        elif count2 == 0 and count1 == 1:
                                for j in xrange(0,l):
                                        new_block = ['-']*9
                                        inc = 0
                                        r = space1[j]
                                        row = r/3
                                        col = r%3
                                        for m in xrange(row*3,row*3+3):
                                                for n in xrange(col*3,col*3+3):
                                                        new_block[inc] = board[m][n]
                                                        inc += 1
                                        value += self.cell_hue(new_block,flag)
                                value += 10
                        elif count2 == 0 and count1 == 2:
                                for j in xrange(0,l):
                                        new_block = ['-']*9
                                        inc = 0
                                        r = space1[j]
                                        row = r/3
                                        col = r%3
                                        for m in xrange(row*3,row*3+3):
                                                for n in xrange(col*3,col*3+3):
                                                        new_block[inc] = board[m][n]
                                                        inc += 1
                                        value += self.cell_hue(new_block,flag)
                                value += 40
                        elif count2 < count1:
                                value += 20
                        elif count2 > count1:
                                value -= 20
                        elif count2 == count1:
                                value += 10

                count1 = 0
                count2 = 0
                space2 = []
                l = 0
                for i in xrange(0,3):
                        if block[i*4] == flag:
                                count1 += 1
                        elif block[i*4] != '-':
                                count2 += 1
                        elif block[i*4] == '-':
                                space2.append(i)
                                l += 1

                if count1 == 3:
                        return 100
                elif count2 == 3:
                        return -100
                elif count1 == 0 and count2 == 1:
                        value -= 10
                elif count1 == 0 and count2 == 2:
                        value -= 40
                elif count2 == 0 and count1 == 1:
                        for j in xrange(0,l):
                                inc = 0
                                new_block = ['-']*9
                                r = space2[j]
                                row = r/3
                                col = r%3
                                for m in xrange(row*3,row*3+3):
                                        for n in xrange(col*3,col*3+3):
                                                new_block[inc] = board[m][n]
                                                inc += 1
                                value += self.cell_hue(new_block,flag)
                        value += 10
                elif count2 == 0 and count1 == 2:
                        for j in xrange(0,l):
                                inc = 0
                                new_block = ['-']*9
                                r = space2[j]
                                row = r/3
                                col = r%3
                                for m in xrange(row*3,row*3+3):
                                        for n in xrange(col*3,col*3+3):
                                                new_block[inc] = board[m][n]
                                                inc += 1
                                value += self.cell_hue(new_block,flag)
                        value += 40
                elif count2 < count1:
                        value += 20
                elif count2 > count1:
                        value -= 20
                elif count2 == count1:
                        value += 10

                count1 = 0
                count2 = 0
                space3 = []
                l = 0
                for i in xrange(1,4):
                        if block[i*2] == flag:
                                count1 += 1
                        elif block[i*2] != '-':
                                count2 += 1
                        elif block[i*2] == '-':
                                space3.append(i)
                                l += 1

                if count1 == 3:
                        return 100
                elif count2 == 3:
                        return -100
                elif count1 == 0 and count2 == 1:
                        value -= 10
                elif count1 == 0 and count2 == 2:
                        value -= 40
                elif count2 == 0 and count1 == 1:
                        for j in xrange(0,l):
                                inc = 0
                                new_block = ['-']*9
                                r = space3[j]
                                row = r/3
                                col = r%3
                                for m in xrange(row*3,row*3+3):
                                        for n in xrange(col*3,col*3+3):
                                                new_block[inc] = board[m][n]
                                                inc += 1
                                value += self.cell_hue(new_block,flag)
                        value += 10
                elif count2 == 0 and count1 == 2:
                        for j in xrange(0,l):
                                inc = 0
                                new_block = ['-']*9
                                r = space3[j]
                                row = r/3
                                col = r%3
                                for m in xrange(row*3,row*3+3):
                                        for n in xrange(col*3,col*3+3):
                                                new_block[inc] = board[m][n]
                                                inc += 1
                                value += self.cell_hue(new_block,flag)
                        value += 40
                elif count2 < count1:
                        value += 20
                elif count2 > count1:
                        value -= 20
                elif count2 == count1:
                        value += 10

                return value


        def cell_hue(self,block,flag):
                countX = 0
                countO = 0
                value = 0
                for k in xrange(0,3):
                        countX = 0
                        countO = 0
                        for i in xrange(k*3,k*3+3):
                                if block[i] == flag:
                                        countX += 1
                                elif block[i] != '-':
                                        countO += 1

                        if countX == 3:
                                return 10
                        elif countO == 3:
                                return -10
                        elif countX == 0 and countO == 1:
                                value -= 1
                        elif countX == 0 and countO == 2:
                                value -= 4
                        elif countO == 0 and countX == 1:
                                value += 1
                        elif countO == 0 and countX == 2:
                                value += 4
                        elif countO < countX:
                                value += 2
                        elif countO > countX:
                                value -= 2
                        elif countO == countX:
                                value += 1

                for k in xrange(0,3):
                        countX = 0
                        countO = 0
                        for i in xrange(0,3):
                                if block[i*3+k] == flag:
                                        countX += 1
                                elif block[i*3+k] != '-':
                                        countO += 1

                        if countX == 3:
                                return 10
                        elif countO == 3:
                                return -10
                        elif countX == 0 and countO == 1:
                                value -= 1
                        elif countX == 0 and countO == 2:
                                value -= 4
                        elif countO == 0 and countX == 1:
                                value += 1
                        elif countO == 0 and countX == 2:
                                value += 4
                        elif countO < countX:
                                value += 2
                        elif countO > countX:
                                value -= 2
                        elif countO == countX:
                                value += 1

                countX = 0
                countO = 0

                for i in xrange(0,3):
                        if block[i*4] == flag:
                                countX += 1
                        elif block[i*4] != '-':
                                countO += 1

                if countX == 3:
                        return 10
                elif countO == 3:
                        return -10
                elif countX == 0 and countO == 1:
                        value -= 1
                elif countX == 0 and countO == 2:
                        value -= 4
                elif countO == 0 and countX == 1:
                        value += 1
                elif countO == 0 and countX == 2:
                        value += 4
                elif countO < countX:
                        value += 2
                elif countO > countX:
                        value -= 2
                elif countO == countX:
                        value += 1

                countX = 0
                countO = 0

                for i in xrange(1,4):
                        if block[i*2] == flag:
                                countX += 1
                        elif block[i*2] != '-':
                                countO += 1

                if countX == 3:
                        return 10
                elif countO == 3:
                        return -10
                elif countX == 0 and countO == 1:
                        value -= 1
                elif countX == 0 and countO == 2:
                        value -= 4
                elif countO == 0 and countX == 1:
                        value += 1
                elif countO == 0 and countX == 2:
                        value += 4
                elif countO < countX:
                        value += 2
                elif countO > countX:
                        value -= 2
                elif countO == countX:
                        value += 1

                return value

        def blocks_allowed(self,old_move, temp_block):
                blocks_allowed  = []
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

                return blocks_allowed





#Initializes the game
def get_init_board_and_blockstatus():
	board = []
	for i in range(9):
		row = ['-']*9
		board.append(row)
	
	block_stat = ['-']*9
	return board, block_stat

# Checks if player has messed with the board. Don't mess with the board that is passed to your move function. 
def verification_fails_board(board_game, temp_board_state):
	return board_game == temp_board_state	

# Checks if player has messed with the block. Don't mess with the block array that is passed to your move function. 
def verification_fails_block(block_stat, temp_block_stat):
	return block_stat == temp_block_stat	

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
		
# Note that even if someone has won a block, it is not abandoned. But then, there's no point winning it again!
# Returns True if move is valid
def check_valid_move(game_board,block_stat, current_move, old_move):

	# first we need to check whether current_move is tuple of not
	# old_move is guaranteed to be correct
	if type(current_move) is not tuple:
		return False
	
	if len(current_move) != 2:
		return False

	a = current_move[0]
	b = current_move[1]	

	if type(a) is not int or type(b) is not int:
		return False
	if a < 0 or a > 8 or b < 0 or b > 8:
		return False

	#Special case at start of game, any move is okay!
	if old_move[0] == -1 and old_move[1] == -1:
		return True


	for_corner = [0,2,3,5,6,8]

	#List of permitted blocks, based on old move.
	blocks_allowed  = []

	if old_move[0] in for_corner and old_move[1] in for_corner:
		## we will have 3 representative blocks, to choose from

		if old_move[0] % 3 == 0 and old_move[1] % 3 == 0:
			## top left 3 blocks are allowed
			blocks_allowed = [0,1,3]
		elif old_move[0] % 3 == 0 and old_move[1] in [2,5,8]:
			## top right 3 blocks are allowed
			blocks_allowed = [1,2,5]
		elif old_move[0] in [2,5,8] and old_move[1] % 3 == 0:
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

        #Check if the block is won, or completed. If so you cannot move there. 

        for i in reversed(blocks_allowed):
            if block_stat[i] != '-':
                blocks_allowed.remove(i)
        
        # We get all the empty cells in allowed blocks. If they're all full, we get all the empty cells in the entire board.
        cells = get_empty_out_of(game_board, blocks_allowed,block_stat)

	#Checks if you made a valid move. 
        if current_move in cells:
     	    return True
        else:
    	    return False

def update_lists(game_board, block_stat, move_ret, fl):
	#move_ret has the move to be made, so we modify the game_board, and then check if we need to modify block_stat
	game_board[move_ret[0]][move_ret[1]] = fl

	block_no = (move_ret[0]/3)*3 + move_ret[1]/3
	id1 = block_no/3
	id2 = block_no%3
	mg = 0
	mflg = 0
	if block_stat[block_no] == '-':
		if game_board[id1*3][id2*3] == game_board[id1*3+1][id2*3+1] and game_board[id1*3+1][id2*3+1] == game_board[id1*3+2][id2*3+2] and game_board[id1*3+1][id2*3+1] != '-':
			mflg=1
		if game_board[id1*3+2][id2*3] == game_board[id1*3+1][id2*3+1] and game_board[id1*3+1][id2*3+1] == game_board[id1*3][id2*3 + 2] and game_board[id1*3+1][id2*3+1] != '-':
			mflg=1
		
                if mflg != 1:
                    for i in range(id2*3,id2*3+3):
                        if game_board[id1*3][i]==game_board[id1*3+1][i] and game_board[id1*3+1][i] == game_board[id1*3+2][i] and game_board[id1*3][i] != '-':
                                mflg = 1
                                break

                ### row-wise
		if mflg != 1:
                    for i in range(id1*3,id1*3+3):
                        if game_board[i][id2*3]==game_board[i][id2*3+1] and game_board[i][id2*3+1] == game_board[i][id2*3+2] and game_board[i][id2*3] != '-':
                                mflg = 1
                                break

	
	if mflg == 1:
		block_stat[block_no] = fl
	
        #check for draw on the block.

        id1 = block_no/3
	id2 = block_no%3
        cells = []
	for i in range(id1*3,id1*3+3):
	    for j in range(id2*3,id2*3+3):
		if game_board[i][j] == '-':
		    cells.append((i,j))

        if cells == [] and mflg!=1:
            block_stat[block_no] = 'd' #Draw
        
        return

def terminal_state_reached(game_board, block_stat):
	
        #Check if game is won!
        bs = block_stat
	## Row win
	if (bs[0] == bs[1] and bs[1] == bs[2] and bs[1]!='-' and bs[1]!='d') or (bs[3]!='d' and bs[3]!='-' and bs[3] == bs[4] and bs[4] == bs[5]) or (bs[6]!='d' and bs[6]!='-' and bs[6] == bs[7] and bs[7] == bs[8]):
		print block_stat
		return True, 'W'
	## Col win
	elif (bs[0]!='d' and bs[0] == bs[3] and bs[3] == bs[6] and bs[0]!='-') or (bs[1]!='d'and bs[1] == bs[4] and bs[4] == bs[7] and bs[4]!='-') or (bs[2]!='d' and bs[2] == bs[5] and bs[5] == bs[8] and bs[5]!='-'):
		print block_stat
		return True, 'W'
	## Diag win
	elif (bs[0] == bs[4] and bs[4] == bs[8] and bs[0]!='-' and bs[0]!='d') or (bs[2] == bs[4] and bs[4] == bs[6] and bs[2]!='-' and bs[2]!='d'):
		print block_stat
		return True, 'W'
	else:
		smfl = 0
		for i in range(9):
			for j in range(9):
				if game_board[i][j] == '-' and block_stat[(i/3)*3+(j/3)] == '-':
					smfl = 1
					break
		if smfl == 1:
                        #Game is still on!
			return False, 'Continue'
		
		else:
                        #Changed scoring mechanism
                        # 1. If there is a tie, player with more boxes won, wins.
                        # 2. If no of boxes won is the same, player with more corner move, wins. 
                        point1 = 0
                        point2 = 0
                        for i in block_stat:
                            if i == 'x':
                                point1+=1
                            elif i=='o':
                                point2+=1
			if point1>point2:
				return True, 'P1'
			elif point2>point1:
				return True, 'P2'
			else:
                                point1 = 0
                                point2 = 0
                                for i in range(len(game_board)):
                                    for j in range(len(game_board[i])):
                                        if i%3!=1 and j%3!=1:
                                            if game_board[i][j] == 'x':
                                                point1+=1
                                            elif game_board[i][j]=='o':
                                                point2+=1
			        if point1>point2:
				    return True, 'P1'
			        elif point2>point1:
				    return True, 'P2'
                                else:
				    return True, 'D'	


def decide_winner_and_get_message(player,status, message):
	if player == 'P1' and status == 'L':
		return ('P2',message)
	elif player == 'P1' and status == 'W':
		return ('P1',message)
	elif player == 'P2' and status == 'L':
		return ('P1',message)
	elif player == 'P2' and status == 'W':
		return ('P2',message)
	else:
		return ('NONE','DRAW')
	return


def print_lists(gb, bs):
	print '=========== Game Board ==========='
	for i in range(9):
		if i > 0 and i % 3 == 0:
			print
		for j in range(9):
			if j > 0 and j % 3 == 0:
				print " " + gb[i][j],
			else:
				print gb[i][j],

		print
	print "=================================="

	print "=========== Block Status ========="
	for i in range(0, 9, 3):
		print bs[i] + " " + bs[i+1] + " " + bs[i+2] 
	print "=================================="
	print
	

def simulate(obj1,obj2):
	
	# Game board is a 9x9 list, block_stat is a 1D list of 9 elements
	game_board, block_stat = get_init_board_and_blockstatus()

	pl1 = obj1 
	pl2 = obj2

	### basically, player with flag 'x' will start the game
	pl1_fl = 'x'
	pl2_fl = 'o'

	old_move = (-1, -1) # For the first move

	WINNER = ''
	MESSAGE = ''

        #Make your move in 6 seconds!
	TIMEALLOWED = 6

	print_lists(game_board, block_stat)

	while(1):

		# Player1 will move
		
		temp_board_state = game_board[:]
		temp_block_stat = block_stat[:]
	
		signal.signal(signal.SIGALRM, handler)
		signal.alarm(TIMEALLOWED)
		# Player1 to complete in TIMEALLOWED secs. 
		try:
			ret_move_pl1 = pl1.move(temp_board_state, temp_block_stat, old_move, pl1_fl)
		except TimedOutExc as e:
			WINNER, MESSAGE = decide_winner_and_get_message('P1', 'L',   'TIMED OUT')
			break
		signal.alarm(0)
	
                #Checking if list hasn't been modified! Note: Do not make changes in the lists passed in move function!
		if not (verification_fails_board(game_board, temp_board_state) and verification_fails_block(block_stat, temp_block_stat)):
			#Player1 loses - he modified something
			WINNER, MESSAGE = decide_winner_and_get_message('P1', 'L',   'MODIFIED CONTENTS OF LISTS')
			break
		
		# Check if the move made is valid
		if not check_valid_move(game_board, block_stat,ret_move_pl1, old_move):
			## player1 loses - he made the wrong move.
			WINNER, MESSAGE = decide_winner_and_get_message('P1', 'L',   'MADE AN INVALID MOVE')
			break


		print "Player 1 made the move:", ret_move_pl1, 'with', pl1_fl

                #So if the move is valid, we update the 'game_board' and 'block_stat' lists with move of pl1
                update_lists(game_board, block_stat, ret_move_pl1, pl1_fl)

		# Checking if the last move resulted in a terminal state
		gamestatus, mesg =  terminal_state_reached(game_board, block_stat)
		if gamestatus == True:
			print_lists(game_board, block_stat)
			WINNER, MESSAGE = decide_winner_and_get_message('P1', mesg,  'COMPLETE')	
			break

		
		old_move = ret_move_pl1
		print_lists(game_board, block_stat)

                # Now player2 plays

                temp_board_state = game_board[:]
                temp_block_stat = block_stat[:]


		signal.signal(signal.SIGALRM, handler)
		signal.alarm(TIMEALLOWED)
		try:
                	ret_move_pl2 = pl2.move(temp_board_state, temp_block_stat, old_move, pl2_fl)
		except TimedOutExc as e:
			WINNER, MESSAGE = decide_winner_and_get_message('P2', 'L',   'TIMED OUT')
			break
		signal.alarm(0)

                if not (verification_fails_board(game_board, temp_board_state) and verification_fails_block(block_stat, temp_block_stat)):
			WINNER, MESSAGE = decide_winner_and_get_message('P2', 'L',   'MODIFIED CONTENTS OF LISTS')
			break
			
                if not check_valid_move(game_board, block_stat,ret_move_pl2, old_move):
			WINNER, MESSAGE = decide_winner_and_get_message('P2', 'L',   'MADE AN INVALID MOVE')
			break


		print "Player 2 made the move:", ret_move_pl2, 'with', pl2_fl
                
                update_lists(game_board, block_stat, ret_move_pl2, pl2_fl)

		gamestatus, mesg =  terminal_state_reached(game_board, block_stat)
                if gamestatus == True:
			print_lists(game_board, block_stat)
                        WINNER, MESSAGE = decide_winner_and_get_message('P2', mesg,  'COMPLETE' )
                        break
		old_move = ret_move_pl2
		print_lists(game_board, block_stat)
	
	print WINNER
	print MESSAGE

if __name__ == '__main__':
	## get game playing objects

	if len(sys.argv) != 2:
		print 'Usage: python simulator.py <option>'
		print '<option> can be 1 => Random player vs. Random player'
		print '                2 => Human vs. Random Player'
		print '                3 => Human vs. Human'
		sys.exit(1)
 
	obj1 = ''
	obj2 = ''
	option = sys.argv[1]	
	if option == '1':
		obj1 = Player1()
		obj2 = Player2()

	elif option == '2':
		obj1 = Player1()
		obj2 = Manual_player()
	elif option == '3':
		obj1 = Manual_player()
		obj2 = Manual_player()
        
        # Deciding player1 / player2 after a coin toss
        # However, in the tournament, each player will get a chance to go 1st. 
        num = random.uniform(0,1)
        if num > 0.5:
		simulate(obj2, obj1)
	else:
		simulate(obj1, obj2)
