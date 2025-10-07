#!/bin/bash
#SBATCH --ntasks=2                  # 2 processos MPI
#SBATCH --time=00:10:00             # limite de 10 minutos
#SBATCH --partition=amd-512

mpirun ./MPI
