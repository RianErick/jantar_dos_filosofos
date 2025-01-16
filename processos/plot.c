#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "plot.h"
#include "config.h"

int main(){

    plot2d("./data/average_return_time.csv", "Gráfico do Tempo Médio de Retorno", "Tempo de Retorno", "Quantum", "Tempo", "1:2");
    plot2d("./data/average_wait_time.csv", "Gráfico do Tempo Médio de Espera", "Tempo de Espera", "Quantum", "Tempo", "1:2");

    splot_start("Vazão de Processos pelo Quantum sobre o Tempo");
    splot_setxyz("Quantum", "Tempo", "Vazão");
    splot_sort("3:1:2");
    int max = QUANTUM_STEPS*QUANTUM_COUNT;
    char title[20];
    char filename[256];
    for(int i = QUANTUM_START; i < max; i += QUANTUM_STEPS){
        sprintf(filename, "./data/throughput_quantum_%d.csv", i);
        sprintf(title, "Quantum %d", i);
        splot_add(filename, title);
    }
    splot_end();

    return 0;
}
