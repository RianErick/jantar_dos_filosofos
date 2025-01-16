#ifndef __PLOT__
#define __PLOT__

#include <stdio.h>
#include <stdlib.h>

#define MAX_COLS 3

FILE * GNU_PLOT = NULL;
char GNU_PLOT_SORTING[10];
int SPLOT_SIZE = 0;

int splot_start(char * title){
    if(GNU_PLOT) return -1;

    SPLOT_SIZE = 0;

    GNU_PLOT = popen("gnuplot -persist", "w");
    if (!GNU_PLOT) {
        perror("Failed to open gnuplot");
        exit(EXIT_FAILURE);
    }

    fprintf(GNU_PLOT, "set datafile separator ','\n");
    fprintf(GNU_PLOT, "set title '%s'\n", title);
    fprintf(GNU_PLOT, "set key outside\n");
    fprintf(GNU_PLOT, "set view 90, 90, 1, 1\n");
    
    return 0;
}

int splot_setxyz(char * xlabel, char * ylabel, char * zlabel){
    if(!GNU_PLOT) return -1;
    fprintf(GNU_PLOT, "set xlabel '%s'\n", xlabel);
    fprintf(GNU_PLOT, "set ylabel '%s'\n", ylabel);
    fprintf(GNU_PLOT, "set zlabel '%s'\n", zlabel);
    fprintf(GNU_PLOT, "set xrange [0:*]\n");
    fprintf(GNU_PLOT, "set yrange [0:*]\n");
    fprintf(GNU_PLOT, "set zrange [0:*]\n");
    return 0;
}

int splot_sort(char * sorting){
    if(!GNU_PLOT) return -1;
    snprintf(GNU_PLOT_SORTING, 10, "%s", sorting);
    return 0;
}

int splot_add(char * file, char * line_title){
    if(!GNU_PLOT) return -1;

    if(SPLOT_SIZE == 0) fprintf(GNU_PLOT, "splot ");

    if(SPLOT_SIZE > 0) fprintf(GNU_PLOT, ", ");
    SPLOT_SIZE += 1;

    char * sorting = "1:2:3";
    if(GNU_PLOT_SORTING[0]) sorting = GNU_PLOT_SORTING;
    fprintf(GNU_PLOT, "'%s' using %s with lines title '%s'", file, sorting, line_title);

    return 0;
}

int splot_end(){
    if(!GNU_PLOT) return -1;
    fprintf(GNU_PLOT, "\n");
    fprintf(GNU_PLOT, "set contour both\n");
    pclose(GNU_PLOT);
    GNU_PLOT = NULL;
    return 0;
}

int plot2d(char * file, char * title, char * line_title, char * xlabel, char * ylabel, char * sorting){
    FILE * gnuplot_pipe = popen("gnuplot -persist", "w");

    if(!gnuplot_pipe){
        perror("Failed to open gnuplot");
        exit(EXIT_FAILURE);
    }

    fprintf(gnuplot_pipe, "set title '%s'\n", title);
    fprintf(gnuplot_pipe, "set datafile separator ','\n");
    // fprintf(gnuplot_pipe, "set key outside\n");
    fprintf(gnuplot_pipe, "set grid\n");
    fprintf(gnuplot_pipe, "set key autotitle columnhead\n");

    fprintf(gnuplot_pipe, "set xlabel '%s'\n", xlabel);
    fprintf(gnuplot_pipe, "set xrange [0:*]\n");

    fprintf(gnuplot_pipe, "set ylabel '%s'\n", ylabel);
    fprintf(gnuplot_pipe, "set yrange [0:*]\n");

    fprintf(gnuplot_pipe, "plot '%s' using %s with lines title '%s'\n", file, sorting, line_title);

    return pclose(gnuplot_pipe);
}

#endif /* __PLOT__ */