# -*- coding: utf-8 -*-
"""
Created on Thu Apr  2 00:03:13 2020

This code allows the user to input a series of clues to a sudoku puzzle, and
will output the (possibly partially) solved puzzle. Any unsolved squares will
be displayed as zeros in the output.


@author: Ryan Goetz
"""

import time
import copy



#
# SPECIAL FUNCTIONS
#-----------------------------------------------------------------------------


def countbits(n):
    '''
    The bit counting function:
        For any integer that can be expressed with 9 bits, count the number of
        nonzero bits in its binary representation.
    '''
    n = (n & 341) + ((n & 170) >> 1) # 341 = 0b101010101 and 170 = 0b010101010
    n = (n & 307) + ((n & 204) >> 2) # 307 = 0b100110011 and 204 = 0b011001100
    n = (n & 271) + ((n & 240) >> 4) # 271 = 0b100001111 and 240 = 0b011110000
    n = (n & 127) + ((n & 256) >> 8) # 127 = 0b001111111 and 256 = 0b100000000
    return n


def listdiff(lis1,lis2):
    '''
    List difference function:
        Finds the complement of the second list within the first list, ignoring
        repetition. The output will be unsorted.
    '''
    return list(set(lis1)-set(lis2))


def strike(PP,SS,QQ):
    '''
    The strike reduction function:
        For every solved element, remove the solved bit from each of it's row,
        column, and grid partners.
    Strike should always preceed an iteration of any other reduction function.
    '''
    is_done = 1
    while is_done > 0:
        before = len(QQ)    
        for s in SS:
            p = PP[s][0]
            for i_bud in PP[s][1]:
                PP[i_bud][0] = PP[i_bud][0] - (PP[i_bud][0] & p)
                if s in PP[i_bud][1]: PP[i_bud][1].remove(s)
            for j_bud in PP[s][2]:
                PP[j_bud][0] = PP[j_bud][0] - (PP[j_bud][0] & p)
                if s in PP[j_bud][2]: PP[j_bud][2].remove(s)
            for g_bud in P[s][3]:
                PP[g_bud][0] = PP[g_bud][0] - (PP[g_bud][0] & p)
                if s in PP[g_bud][3]: PP[g_bud][3].remove(s)
        # update the list of newly solved, and unsolved indices
        newS = []
        for q in QQ:
            if countbits(PP[q][0]) == 1:
                newS.append(q)
        SS = newS
        QQ = listdiff(QQ,SS)
        after = len(QQ)
        is_done = before-after
    return PP, SS, QQ


def unique(PP,SS,QQ):
    '''
    The uniqueness reduction function:
        If an unsolved element has a unique bit amongst its row, column, or grid
        partners, all but the unique bit are dropped for that element, and the
        element is now solved.
    '''
    for q in QQ:
        i_unq = 0
        j_unq = 0
        g_unq = 0
        for i_bud in PP[q][1]:
            i_unq |= PP[i_bud][0]
        i_unq ^= 511
        i_unq &= PP[q][0]
        if i_unq != 0: 
            PP[q][0] = i_unq
            for i_bud in PP[q][1]:
                if q in PP[i_bud][1]: PP[i_bud][1].remove(q)
        else:
            for j_bud in PP[q][2]:
                j_unq |= PP[j_bud][0]
            j_unq ^= 511
            j_unq &= PP[q][0]
            if j_unq != 0: 
                PP[q][0] = j_unq
                for j_bud in PP[q][2]:
                    if q in PP[j_bud][2]: PP[j_bud][2].remove(q)
            else:
                for g_bud in PP[q][3]:
                    g_unq |= PP[g_bud][0]
                g_unq ^= 511
                g_unq &= PP[q][0]
                if g_unq != 0: 
                    PP[q][0] = g_unq
                    for g_bud in PP[q][3]:
                        if q in PP[g_bud][3]: PP[g_bud][3].remove(q)
    # update the list of unsolved indices
    newS = []
    for q in QQ:
        if countbits(PP[q][0]) == 1:
            newS.append(q)
    SS = newS
    QQ = listdiff(QQ,SS)
    return PP, SS, QQ


def squeeze(PP,SS,QQ):
    '''
    The squeeze reduction function:
        In every row and column, divide the row/column into three parts according 
        to grid membership. If any of the three parts has a unique bit, remove
        that bit from the neighboring members of that part's grid.
    '''
    # first examine the rows
    for i in rng9:
        i_checkspots = list(range(9*i,9*(i+1)))
        i_checkvals  = [PP[q][0] for q in i_checkspots]
        subrows      = [0,0,0]
        for n in range(3):
            subrows[0] |= i_checkvals[n]
            subrows[1] |= i_checkvals[n+3]
            subrows[2] |= i_checkvals[n+6]
        # identify if there are unique bits in any of the partitions of the row
        first_unq  = ((subrows[1] | subrows[2])^511) & subrows[0]
        second_unq = ((subrows[0] | subrows[2])^511) & subrows[1]
        third_unq  = ((subrows[0] | subrows[1])^511) & subrows[2]
        # if the first partition has unique bits, remove them from the grid neighbors
        if first_unq != 0:
            g    = 3*(i//3)
            grid = [ 9*(3*(g // 3)+(n // 3))+3*(g % 3)+(n % 3) for n in range(9)]
            g_checkspots = list(set(grid).intersection(QQ))
            g_checkspots = listdiff(g_checkspots,i_checkspots[0:3])
            for q in g_checkspots:
                PP[q][0] = PP[q][0] - (PP[q][0] & first_unq)
        # if the second partition has unique bits, remove them from the grid neighbors
        if second_unq != 0:
            g    = 3*(i//3)+1
            grid = [ 9*(3*(g // 3)+(n // 3))+3*(g % 3)+(n % 3) for n in range(9)]
            g_checkspots = list(set(grid).intersection(QQ))
            g_checkspots = listdiff(g_checkspots,i_checkspots[3:6])
            for q in g_checkspots:
                PP[q][0] = PP[q][0] - (PP[q][0] & second_unq)
        # if the third partition has unique bits, remove them from the grid neighbors
        if third_unq != 0:
            g    = 3*(i//3)+2
            grid = [ 9*(3*(g // 3)+(n // 3))+3*(g % 3)+(n % 3) for n in range(9)]
            g_checkspots = list(set(grid).intersection(QQ))
            g_checkspots = listdiff(g_checkspots,i_checkspots[6:9])
            for q in g_checkspots:
                PP[q][0] = PP[q][0] - (PP[q][0] & third_unq)
    # now examine the columns
    for j in rng9:
        j_checkspots = [j+9*n for n in range(9)]
        j_checkvals  = [PP[q][0] for q in j_checkspots]
        subrows      = [0,0,0]
        for n in range(3):
            subrows[0] |= j_checkvals[n]
            subrows[1] |= j_checkvals[n+3]
            subrows[2] |= j_checkvals[n+6]
        # identify if there are unique bits in any of the partitions of the row
        first_unq  = ((subrows[1] | subrows[2])^511) & subrows[0]
        second_unq = ((subrows[0] | subrows[2])^511) & subrows[1]
        third_unq  = ((subrows[0] | subrows[1])^511) & subrows[2]
        # if the first partition has unique bits, remove them from the grid neighbors
        if first_unq != 0:
            g    = j//3
            grid = [ 9*(3*(g // 3)+(n // 3))+3*(g % 3)+(n % 3) for n in range(9)]
            g_checkspots = list(set(grid).intersection(QQ))
            g_checkspots = listdiff(g_checkspots,j_checkspots[0:3])
            for q in g_checkspots:
                PP[q][0] = PP[q][0] - (PP[q][0] & first_unq)
        # if the second partition has unique bits, remove them from the grid neighbors
        if second_unq != 0:
            g    = (j//3)+3
            grid = [ 9*(3*(g // 3)+(n // 3))+3*(g % 3)+(n % 3) for n in range(9)]
            g_checkspots = list(set(grid).intersection(QQ))
            g_checkspots = listdiff(g_checkspots,j_checkspots[3:6])
            for q in g_checkspots:
                PP[q][0] = PP[q][0] - (PP[q][0] & second_unq)
        # if the third partition has unique bits, remove them from the grid neighbors
        if third_unq != 0:
            g    = (j//3)+6
            grid = [ 9*(3*(g // 3)+(n // 3))+3*(g % 3)+(n % 3) for n in range(9)]
            g_checkspots = list(set(grid).intersection(QQ))
            g_checkspots = listdiff(g_checkspots,j_checkspots[6:9])
            for q in g_checkspots:
                PP[q][0] = PP[q][0] - (PP[q][0] & third_unq)    
    # update the list of unsolved indices
    newS = []
    for q in QQ:
        if countbits(PP[q][0]) == 1:
            newS.append(q)
    SS = newS
    QQ = listdiff(QQ,SS)
    return PP, SS, QQ
        


def pigeon(PP,SS,QQ):
    '''
    The pigeon-hole reduction function:
        In every row, column, and grid check for matching values. If the 
        multiplicity of the match is equal to the bit number of the value, then
        remove those bits from the non-matching row, column, or grid members.
        
    At the moment, it is unclear whether this function can be derived from the 
    other three rules.
    '''
    for i in rng9:
        i_checkspots = list(set(range(9*i,9*(i+1))).intersection(QQ))
        i_checkvals  = [PP[q][0] for q in i_checkspots]
        for v in i_checkvals:
            bits = countbits(v)
            cnt  = i_checkvals.count(v)
            if bits == cnt:
                for q in i_checkspots:
                    if PP[q][0] != v: PP[q][0] = PP[q][0] - (PP[q][0] & v)
    for j in rng9:
        j_checkspots = list(set([j+9*n for n in range(9)]).intersection(QQ))
        j_checkvals  = [PP[q][0] for q in j_checkspots]
        for v in j_checkvals:
            bits = countbits(v)
            cnt  = j_checkvals.count(v)
            if bits == cnt:
                for q in j_checkspots:
                    if PP[q][0] != v: PP[q][0] = PP[q][0] - (PP[q][0] & v)
    for g in rng9:
        grids = [ 9*(3*(g // 3)+(n // 3))+3*(g % 3)+(n % 3) for n in range(9)]
        g_checkspots = list(set(grids).intersection(QQ))
        g_checkvals  = [PP[q][0] for q in g_checkspots]
        for v in g_checkvals:
            bits = countbits(v)
            cnt  = g_checkvals.count(v)
            if bits == cnt:
                for q in g_checkspots:
                    if PP[q][0] != v: PP[q][0] = PP[q][0] - (PP[q][0] & v)
    # update the list of unsolved indices
    newS = []
    for q in QQ:
        if countbits(PP[q][0]) == 1:
            newS.append(q)
    SS = newS
    QQ = listdiff(QQ,SS)
    return PP, SS, QQ

    
def pipe(PP,SS,QQ):
    '''
    The pipe reduction function:
        In every grid select the unsolved elements and check if any bit is unique 
        to a row or column in the grid. For unique bits, eliminate them from 
        row/column members outside the grid
    '''
    for g in rng9:
        grids = [ 9*(3*(g // 3)+(n // 3))+3*(g % 3)+(n % 3) for n in range(9)]
        g_checkspots = list(set(grids).intersection(QQ))
        g_checkvals = [PP[q][0] for q in g_checkspots]
        bit_multiplicity = [0 for _ in rng9]
        W = [[] for _ in rng9]
        # find the multiplicity of each bit in the grid and record corresponding indices
        for idx,v in enumerate(g_checkvals):
            for bit in rng9:
                if (v>>bit)&1:
                    bit_multiplicity[bit] += 1
                    W[bit].append(g_checkspots[idx])
        cnt2 = bit_multiplicity.count(2)
        cnt3 = bit_multiplicity.count(3)
        # if there are doubles, determine for which bits they occur
        if cnt2 != 0:
            which_bits_double = []
            idx = 0
            for c in range(cnt2):
                dbl_bit = bit_multiplicity.index(2,idx)
                which_bits_double.append(dbl_bit)
                idx = dbl_bit+1
            # for all of the doubled bits, check if the cells share a row or column
            for bit in which_bits_double:
                if len(W[bit]) != 0:
                    romeo = W[bit][0] 
                    juliet = W[bit][1]
                    # if they share a row, find the row and remove the bit from all other members
                    if romeo//9 == juliet//9:
                        i = romeo//9
                        rj_row = list(set(range(9*i,9*(i+1))).intersection(QQ))
                        rj_row = listdiff(rj_row,W[bit])
                        bit_as_int = 1<<bit
                        for q in rj_row:
                            PP[q][0] = PP[q][0] - (PP[q][0] & bit_as_int)
                    # if they share a column, find the column and remove the bit from all other members
                    elif romeo % 9 == juliet % 9:
                        j = romeo % 9
                        rj_row = list(set([j+9*n for n in range(9)]).intersection(QQ))
                        rj_row = listdiff(rj_row,W[bit])
                        bit_as_int = 1<<bit
                        for q in rj_row:
                            PP[q][0] = PP[q][0] - (PP[q][0] & bit_as_int)
        # if there are triples, determine for which bits they occur
        if cnt3 != 0:
            which_bits_triple = []
            idx = 0
            for c in range(cnt3):
                trp_bit = bit_multiplicity.index(3,idx)
                which_bits_triple.append(trp_bit)
                idx = trp_bit+1
            # for all of the tripled bits, check if the cells share a row or column
            for bit in which_bits_triple:
                if len(W[bit]) != 0:
                    larry = W[bit][0] 
                    curly = W[bit][1]
                    mo    = W[bit][2]
                    # if they share a row, find the row and remove the bit from all other members
                    if larry//9 == curly//9 == mo//9:
                        i = larry//9
                        stooge_row = list(set(range(9*i,9*(i+1))).intersection(QQ))
                        stooge_row = listdiff(stooge_row,W[bit])
                        bit_as_int = 1<<bit
                        for q in stooge_row:
                            PP[q][0] = PP[q][0] - (PP[q][0] & bit_as_int)
                    # if they share a column, find the column and remove the bit from all other members
                    elif larry % 9 == curly % 9 == mo % 9:
                        j = larry % 9
                        stooge_row = list(set([j+9*n for n in range(9)]).intersection(QQ))
                        stooge_row = listdiff(stooge_row,W[bit])
                        bit_as_int = 1<<bit
                        for q in stooge_row:
                            PP[q][0] = PP[q][0] - (PP[q][0] & bit_as_int)
    # update the list of unsolved indices
    newS = []
    for q in QQ:
        if countbits(PP[q][0]) == 1:
            newS.append(q)
    SS = newS
    QQ = listdiff(QQ,SS)
    return PP, SS, QQ


def reduceloop(PP,SS,QQ):
    '''
    The reduction loop algorithm:
        Loop a series of the reduction functions over the puzzle until the puzzle
        is left unchanged by further iterations.
    '''
    trials = 0
    keep_going = True
    while keep_going:
        old_P = [q[0] for q in PP]
        PP,SS,QQ = strike(PP,SS,QQ)
        PP,SS,QQ = unique(PP,SS,QQ)
        PP,SS,QQ = strike(PP,SS,QQ)
        PP,SS,QQ = squeeze(PP,SS,QQ)
        #PP,SS,QQ = strike(PP,SS,QQ)
        #PP,SS,QQ = pigeon(PP,SS,QQ)
        PP,SS,QQ = strike(PP,SS,QQ)
        PP,SS,QQ = pipe(PP,SS,QQ)
        new_P = [q[0] for q in PP]
        keep_going = not (old_P == new_P)
        trials += 1
    return PP,SS,QQ,trials


def leastpopular(PP,QQ):
    '''
    The least popular function:
        Find the bit which shows up least often amongst the unsolved puzzle 
        entries. Return the multiplicity, bit, as well as the indices where it 
        occurs in the puzzle.
    '''
    tracker = [[0,n,[]] for n in rng9]
    for bit in rng9:
        mask = 1<<bit
        for q in QQ:
            if PP[q][0] & mask != 0:
                tracker[bit][0] += 1
                (tracker[bit][2]).append(q)
    tracked = sorted(tracker)
    while tracked[0][0] == 0:
        tracked = tracked[1:]
    return tracked
            

def checkstatus(PP):
    '''
    The checkstatus function:
        Returns False if any puzzle value is a zero, and True otherwise.
    '''
    check = True
    for p in PP:
        if p[0] == 0:
            check = False
    return check


#-----------------------------------------------------------------------------
#
#          
        
        
        

#
# INITIALIZE CERTAIN OBJECT TO PREPARE FOR INPUTS
#-----------------------------------------------------------------------------

# define some useful index ranges as lists
rng9 = [x for x in range(9)]
rng81 = [x for x in range(81)]


# initialize and fill the unsolved puzzle
P = []
for k in rng81:
    i = k // 9
    j = k % 9
    g = 3*(i // 3)+(j // 3)
    i_nbr = []
    j_nbr = []
    g_nbr = []
    for n in rng9:
        k_in = 9*i+n
        k_jn = 9*n+j
        k_gn = 9*(3*(g//3)+(n//3))+3*(g%3)+(n%3)
        i_nbr.append(k_in)
        j_nbr.append(k_jn)
        g_nbr.append(k_gn)
    i_nbr.remove(k)
    j_nbr.remove(k)
    g_nbr.remove(k)
    P.append([511,i_nbr,j_nbr,g_nbr]) # 511 = 0b111111111


#-----------------------------------------------------------------------------
#
#




#
# COLLECT AND ORGANIZE THE INPUT CLUES
#-----------------------------------------------------------------------------

print('''
      
---------------------------------------------------------------------------------
Clues should be given to solver in the following form:\n
      ijv\n
where i is the row of the clue (1-9), j is the column (1-9), and v is the value. 
Separate clues using colons, for example the following:\n
      235:941:628\n
signifies three clues: the 2nd row, 3rd column is 5, the 9th row, 4th column is 1
and the 6th row, 2nd column is 8.
---------------------------------------------------------------------------------
      ''') 

innie = input('Please enter clues in the format as prompted above: ')

cut_no = 0

# initialize the solved cells and define acceptable inputs
init_solved_i = []
init_solved_j = []
init_solved_p = []
accepted = ['1','2','3','4','5','6','7','8','9']

while len(innie)>0:
    if cut_no % 2 == 1:
        if innie[-1]==':': 
            innie = innie[:-1]
        else:
            raise Exception('Improperly formatted clue, please check your input and run again')
    else:
        newclue = innie[-3:]
        if newclue[0] and newclue[1] and newclue[2] in accepted:
            new_i = int(newclue[0])-1
            new_j = int(newclue[1])-1
            new_p = 2**(int(newclue[2])-1)
        else:
            raise Exception('')
        init_solved_i.append(new_i)
        init_solved_j.append(new_j)
        init_solved_p.append(new_p)
        innie = innie[:-3]
    cut_no += 1
            

init_clue_length = len(init_solved_p)

if init_clue_length == 0:
    raise Exception('Please enter clues for the solver')

if  not len(init_solved_i) == len(init_solved_j) == init_clue_length:
    print('Incomplete data for final entry; entry is ignored')
    init_solved_i = init_solved_i[:init_clue_length]
    init_solved_j = init_solved_j[:init_clue_length]


# add these solutions to P
for c in range(init_clue_length):
    i = init_solved_i[c]
    j = init_solved_j[c]
    p = init_solved_p[c]
    P[9*i+j][0] = p
    
# build the solved index list S
S = []
for k in rng81:
    if countbits(P[k][0]) == 1:
        S.append(k)
        
# define the unsolved index list Q
Q = listdiff(rng81,S)

#-----------------------------------------------------------------------------
#
#



#
# PERFORM REDUCTION ON PUZZLE
#-----------------------------------------------------------------------------

start_time = time.time()

# do reductions until no further solving is done
P,S,Q,trialsA = reduceloop(P,S,Q)

trialsB = 0
trialsC = 0

# if the puzzle isn't solved, test each choice and remove ones that lead to a contradiction
if len(Q) != 0:
    what_next = leastpopular(P,Q)
    for nxt in range(len(what_next)):
        test_trials = what_next[nxt][0]
        test_bit = 1<<what_next[nxt][1]
        test_spots = what_next[nxt][2]
        for q in test_spots:
            dum_P = copy.deepcopy(P) # necessary because P is not a list of primitives 
            dum_S = S[:]
            dum_Q = Q[:]
            dum_P[q][0] = test_bit
            dum_S.append(q)
            dum_Q.remove(q)
            dum_P,dum_S,dum_Q,dead = reduceloop(dum_P,dum_S,dum_Q)
            trialsB += dead
            if not checkstatus(dum_P):
                P[q][0] = P[q][0] - (P[q][0] & test_bit)
                
    # run reduction loop again
    P,S,Q, trialsC = reduceloop(P,S,Q)
    
    
end_time = time.time()
print('\nCalculation time: ',end_time-start_time,'s')
print('Reduction Loops: ',trialsA+trialsB+trialsC)
    
#-----------------------------------------------------------------------------
#
#




simpleP = [q[0] for q in P]

def fancyprint(X):
    print('\n')
    for row in rng9:
        Y = X[9*row:9*row+9]
        print(Y[:3],'|',Y[3:6],'|',Y[6:9])
        if 0<row and row<8 and (row % 3) == 2:
            print('---------------------------------------------------')
            
def fanciestprint(X):
    print('\n')
    for row in rng9:
        Y = X[9*row:9*row+9]
        for index,y in enumerate(Y):
            if countbits(y) == 1:
                cnt = 0
                while y > 0:
                    y>>=1
                    cnt+=1
                Y[index] = cnt
            else:
                Y[index] = 0
        print(Y[:3],'|',Y[3:6],'|',Y[6:9])
        if 0<row and row<8 and (row % 3) == 2:
            print('---------------------------------')
            
            
def ugliestprint(X):
    print('\n')
    for row in rng9:
        Y = X[9*row:9*row+9]
        for index,y in enumerate(Y):
            if countbits(y) == 1:
                Y[index] = '         '
            else:
                Y[index] = (bin(y)[2:]).zfill(9)
        print(Y[:3],'|',Y[3:6],'|',Y[6:9])
        if 0<row and row<8 and (row % 3) == 2:
            print('---------------------------------')
        
#
# PRINT THE SOLVED PUZZLE
#-----------------------------------------------------------------------------


#fancyprint(simpleP)
#ugliestprint(simpleP)  

fanciestprint(simpleP)   

   
