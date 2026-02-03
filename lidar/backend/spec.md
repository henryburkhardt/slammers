# slam backend
input: pose (factor) graph in g2o format 
output: gauss-newton optimized version of the input pose graph

## Library options
1. g2o - this is a commonly used framework, looks easy enought to impliment. Should be easy to make work with g2o format (although it should be noted that g2o format is agnostic to the solver used).

2. GSAM - Georgia Tech library for solving these factor graphs. Has a python version, but I'm thinking this might be a little slow? 

3. CERES solver - works in C, there's some good examples about how it works

## General architecture (initial thoughs)
- It would be tight to have this work as a service (i.e. running async on a port or something, so that it can just be called via api from the frontend, and await an async response of the optimized pose graph- simmilar to a web frontend/backend)
- there are some considerations around how often to optimize, and what the feedback should look like between FE and BE. I think to start we should just optimize every time a loop closure is detected, and then feed the graph back to the front end
- this (the above step) implies that we need read g2o graph functionality on the front end, that updates the graph
- i think maybe the back end will iterativley generate an occupancy grid, maybe?? this doesn't seem precisley like a front end OR backend thing, but maybe for now it can be implimented in the backend
- there is the potential here to have some sort of ui monitoring the state of thigs (future consideration)
- maybe data should be stored in a tertiary location? idk what the best way is. i guess for now we will just pass it back and forth 
- if the iterative pass back and forth works i think we will have online slam, if the optimization occurs once at the very end then we will have offline slam. 
- i think after this, the emphasis will really shift to loop closures, because the whole system is gonna be very sensitive to initial guesses (emperical fact)
- if knowing when to search for loop closures becomes an issue we always add a manual trigger 