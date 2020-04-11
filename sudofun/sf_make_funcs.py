# -*- coding: utf-8 -*-
import sudofun
import time
from sudofun.sf_solve_funcs import *

"""
Created on Sun Apr  5 17:18:51 2020

This module contains code which allows the user to build an unsolved sudoku puzzle.

This is currently a work in progress


@author: Ryan Goetz, ryan.m.goetz@gmail.com

last update: April 10, 2020 
"""

# define some useful index ranges as lists
rng9  = [x for x in range(9)]
rng81 = [x for x in range(81)]

'''
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
    else:
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
'''


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