#!/bin/bash

# Programas já compilados
COMPUTE=./lim_cpu
MEMORY=./lim_mem

# Números de threads a testar
THREADS=(1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16)

# Arquivo de saída
OUTPUT="resultados.csv"

# Cabeçalho do CSV
echo "Threads,CPU_Bound(s),Memory_Bound(s)" > $OUTPUT

# Executa os testes
for t in "${THREADS[@]}"; do
    echo "Rodando compute-bound com $t threads..."
    TEMPO_CPU=$($COMPUTE $t | awk '{print $7}')
    echo "Rodando memory-bound com $t threads..."
    TEMPO_MEM=$($MEMORY $t | awk '{print $7}')
    echo "$t,$TEMPO_CPU,$TEMPO_MEM" >> $OUTPUT
done

echo "Resultados salvos em $OUTPUT"