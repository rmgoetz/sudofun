# -*- coding: utf-8 -*-
import sudofun
import time
from sudofun.solve import *

"""
This module contains code which allows the user to build an unsolved sudoku puzzle. This is currently a work in progress

@author: Ryan Goetz
"""

# define some useful index ranges as lists
rng9  = [x for x in range(9)]
rng81 = [x for x in range(81)]


#
# 
#-----------------------------------------------------------------------------
def build(**kwargs):
    
    clue = kwargs.pop('clue',None)
    difficulty = kwargs.pop('difficulty',None)
    output= kwargs.pop('output',None)
    
    # initiate the puzzle
    P = init_puzzle() 
    
    # handle case when a seed clue is provided
    if isinstance(clue,str):
        clue_Q = True
        clue_len,init_i,init_j,init_p = translate_clues(clue)
        
        # populate the puzzle
        for c in range(clue_len):
            i = init_i[c]
            j = init_j[c]
            p = init_p[c]
            P[9*i+j][0] = p
        
        # make another puzzle that is as solved as possible
        PSOL = solve(clue,verbose=False,output='puzzle')
    
    # handle the case with no clue seed
    else:
        # build the solved index list S
        S = []
        for k in rng81:
            if countbits(P[k][0]) == 1:
                S.append(k)
        
        # define the unsolved index list Q
        Q = listdiff(rng81,S)
        
        PSOL = puzzlecopy(P)
        PSOL = [p[0] for p in P]
        
    # One idea for difficulty options: a measure of the redundancy of a clue
    # should be established. Easier difficulty puzzles could be built with 
    # clues that are, on average, less redundant. This is a far-future project.
    
    
    
    
    
    
#-----------------------------------------------------------------------------
#
# 



#
# 
#-----------------------------------------------------------------------------
def rando(mod=9,sleep=False):
    '''
    A quick function for generating psuedo-random numbers. A work in progress.
    Outputs from rando calls made within ~ 1e-5 seconds of one another may be
    correlated.
    '''
    t = time.time_ns()
    if sleep:
        time.sleep(1e-7) # calling this function kills more time than the value
    a = (t//100) % 10000
    b = a ^ 10922
    c = a ^ 5461
    d = a ^ 3276
    e = a ^ 13107
    f = (b<<(e % 3))*(c<<(b % 5))*(d<<(c % 7))*(e<<(d % 11))
    g = int(str(f)[:4])
    return (g % mod)+1
#-----------------------------------------------------------------------------
#
# 



#
# 
#-----------------------------------------------------------------------------
def translate_puzzle(PP):
    '''
    The translate puzzle function:
        Takes a full-form puzzle and converts it into a clue string.
    '''
    clue = ''
    for k in rng81:
        p = P[k][0]
        if countbits(p) == 1:
            i = k // 9
            j = k % 9
            newclue = str(i+1)+str(j+1)+str(p)+':'
            clue += newclue
    clue = clue[:-1]
    return clue    
#-----------------------------------------------------------------------------
#
# 
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    