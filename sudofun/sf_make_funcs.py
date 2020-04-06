# -*- coding: utf-8 -*-
import sudofun
import time

"""
Created on Sun Apr  5 17:18:51 2020

This module contains code which allows the user to build an unsolved sudoku puzzle.

This is currently a work in progress


@author: Ryan Goetz, ryan.m.goetz@gmail.com

last update: April 5, 2020 
"""

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