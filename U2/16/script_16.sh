#!/bin/bash
#SBATCH --ntasks=4                  # 4 processos MPI
#SBATCH --time=00:30:00             # limite de 30 minutos
#SBATCH --partition=amd-512

for (( i=100; i<102400; i*=2 )); do
    mpirun ./MxV $i
done
