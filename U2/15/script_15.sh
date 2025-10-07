#!/bin/bash
#SBATCH --ntasks=4                  # 4 processos MPI
#SBATCH --time=00:30:00             # limite de 30 minutos
#SBATCH --partition=amd-512

mpirun ./HT
