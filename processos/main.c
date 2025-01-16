#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include "config.h"

typedef struct process process_t;
struct process{
    int pid;
    int wait_time;
    int burst_time;
    int remaining_time;
    int executed_time;
    int return_at;
    int created_at;
};

typedef struct plist plist_t;
struct plist {
    process_t * process;
    plist_t * next;
};

typedef struct scheduler scheduler_t;
struct scheduler {
    int current_time;
    int quantum;
    void (*run)(scheduler_t *);
    void (*change_context)(scheduler_t *);
    char * throughput_filename;
    plist_t * ready;
    plist_t * finished;
};


void save_throughput_data(const char *filename, int current_time, int completed_processes, int quantum) {
    FILE *file = fopen(filename, "a");
    if (file == NULL) {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }
    fprintf(file, "%d,%d,%d\n", current_time, completed_processes, quantum);
    fclose(file);
}


void move_to_finished(scheduler_t * scheduler, plist_t * p){
    if(!scheduler->finished){
        scheduler->finished = p;
        scheduler->finished->next = NULL;
        return;
    }
    plist_t * current = scheduler->finished;
    while(current->next){
        current = current->next;
    }
    current->next = p;
    p->next = NULL;
}

void remove_process(scheduler_t * scheduler, int pid){
    if(pid == scheduler->ready->process->pid){
        plist_t * saved = scheduler->ready;
        scheduler->ready = scheduler->ready->next;
        move_to_finished(scheduler, saved);
        return;
    }
    plist_t * current = scheduler->ready;
    while(current->next){
        if(current->next->process->pid == pid) break;
        current = current->next;
    }
    if(current->next){
        plist_t * saved = current->next;
        current->next = current->next->next;
        move_to_finished(scheduler, saved);
    }
}

void change_context(scheduler_t * s){
    s->current_time += 1;
}

void round_robin(scheduler_t * scheduler){
    int executed = 0;
    int remaining = 0;
    int completed_processes = 0;

    plist_t * current = scheduler->ready;
    plist_t * next = NULL;

    while(current){

        current->process->wait_time += scheduler->current_time - current->process->executed_time;

        remaining = current->process->remaining_time - scheduler->quantum;

        if(remaining <= 0){
            executed = current->process->remaining_time;
            scheduler->current_time += executed;
            current->process->remaining_time = 0;
            current->process->executed_time += current->process->remaining_time;
            current->process->return_at = scheduler->current_time;

            next = current->next;
            remove_process(scheduler, current->process->pid);

            save_throughput_data(scheduler->throughput_filename, scheduler->current_time, ++completed_processes, scheduler->quantum);
        } else {
            executed = scheduler->quantum;
            scheduler->current_time += executed;
            current->process->executed_time += scheduler->quantum;
            current->process->remaining_time = remaining;

            next = current->next;
        }

        scheduler->change_context(scheduler);
        current = next;
        if(!current) current = scheduler->ready;
    }
}

void initialize_processes(process_t * processes, plist_t * ready_list){
    int max = MAX_PROCESS_TIME;
    int min = MIN_PROCESS_TIME;
    for(int i = 0; i < PROCESS_COUNT; ++i){
        processes[i].pid = i;
        processes[i].burst_time = ((rand()%((max)-min)) + min);
        processes[i].remaining_time = processes[i].burst_time;
        processes[i].executed_time = 0;
        processes[i].return_at = 0;
        processes[i].wait_time = 0;
        processes[i].created_at = 0;

        ready_list[i].process = &processes[i];
        if(i < PROCESS_COUNT - 1) ready_list[i].next = &ready_list[i+1];
        else ready_list[i].next = NULL;

        max = max+40;
        min = min+10;
    }

    // Shuffle
    for(int i = PROCESS_COUNT - 1; i > 0; --i){
        int j = rand() % (i + 1);

        process_t temp_process = processes[i];
        processes[i] = processes[j];
        processes[j] = temp_process;

        plist_t temp_ready = ready_list[i];
        ready_list[i] = ready_list[j];
        ready_list[j] = temp_ready;
    }
}

void reset_processes(process_t * processes, plist_t * ready_list){
    for(int i = 0; i < PROCESS_COUNT; ++i){
        processes[i].remaining_time = processes[i].burst_time;
        processes[i].executed_time = 0;
        processes[i].return_at = 0;
        processes[i].wait_time = 0;
        processes[i].created_at = 0;

        ready_list[i].process = &processes[i];
        if(i < PROCESS_COUNT - 1) ready_list[i].next = &ready_list[i+1];
        else ready_list[i].next = NULL;
    }
}

double average_wait_time(scheduler_t s){
    plist_t * current = s.finished;
    int m = 0;
    while(current){
        m += current->process->wait_time;
        current = current->next;
    }
    return m/PROCESS_COUNT;
}

double average_return_time(scheduler_t s){
    plist_t * current = s.finished;
    int m = 0;
    while(current){
        m += current->process->return_at;
        current = current->next;
    }
    return m/PROCESS_COUNT;
}

double calculate_variance(scheduler_t scheduler, double mean) {
    double variance = 0.0;
    plist_t * c = scheduler.finished;

    double diff = 0.0;
    while(c) {
        diff = c->process->wait_time - mean;
        variance += diff*diff;
        c = c->next;
    }
    variance /= PROCESS_COUNT;
    return variance;
}

void save_average_wait_time_data(const char *filename, scheduler_t scheduler) {
    FILE *file = fopen(filename, "a");
    if (file == NULL) {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }

    double mean = average_wait_time(scheduler);
    double var = calculate_variance(scheduler, mean);
    double desvio = sqrt(var);

    fprintf(file, "%d,%.0f,%.4f,%.4f\n", scheduler.quantum, mean, var, desvio);
    fclose(file);
}

void save_average_return_time_data(const char *filename, scheduler_t scheduler) {
    FILE *file = fopen(filename, "a");
    if (file == NULL) {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }

    double mean = average_return_time(scheduler);

    fprintf(file, "%d,%.0f\n", scheduler.quantum, mean);
    fclose(file);
}

void truncate_file(char * filename){
    FILE * fd = fopen(filename, "w");
    if(!fd){
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }
    fclose(fd);
}

int main(){

    srand(time(NULL));

    plist_t process_list[PROCESS_COUNT] = {0};
    process_t processes[PROCESS_COUNT] = {0};

    // Configurando Escalonador
    scheduler_t scheduler = {
        .current_time = 0,
        .quantum = 50,
        .change_context = change_context,
        .run = round_robin,
        .ready = process_list,
        .finished = NULL
    };

    initialize_processes(processes, process_list);

    char throughput_filename[128] = {0};
    scheduler.throughput_filename = throughput_filename;

    char waittime_filename[] = "./data/average_wait_time.csv";
    char return_filename[] = "./data/average_return_time.csv";
    truncate_file(waittime_filename);
    truncate_file(return_filename);

    int max = QUANTUM_STEPS*QUANTUM_COUNT;

    for(int quantum = QUANTUM_START; quantum < max; quantum += QUANTUM_STEPS){
        reset_processes(processes, process_list);
        scheduler.current_time = 0;
        scheduler.quantum = quantum;
        scheduler.ready = process_list;
        scheduler.finished = NULL;

        sprintf(throughput_filename, "./data/throughput_quantum_%d.csv", quantum);
        truncate_file(throughput_filename);

        scheduler.run(&scheduler);
        
        save_average_wait_time_data(waittime_filename, scheduler);
        save_average_return_time_data(return_filename, scheduler);
    }

    return 0;
}