#!/bin/bash
#SBATCH --partition=intel-128  # partição para a qual o job é enviado
#SBATCH --time=0-0:40    # Especifica o tempo máximo de execução do job, dado no padrão dias-horas:minutos

pascalanalyzer ./NS -t aut -c 1,2,4,8,16 -i 128,256,512,1024,2048 -o output.json