#!/bin/bash

jupyter nbconvert --to python experiments/all_socs_25_hyperparameters.ipynb

./experiments/all_socs_25_hyperparameters.py
