#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#include "names.c"

#define TTHINK_L 1
#define TTHINK_H 1

#define TEAT_L 1
#define TEAT_H 1

#define MAX_PHILOSOPHERS 100
#define LEN_NAME 20

#define FOOD_COUNT 100

typedef pthread_mutex_t hashi_t;
typedef pthread_mutex_t mutex_t;

typedef struct philosopher {
    int id;
    char name[LEN_NAME];
    hashi_t * hashi_l;
    hashi_t * hashi_r;
    int execs;
} philosopher_t;

mutex_t food_mutex;
int food = FOOD_COUNT;

int pick_food(){
    pthread_mutex_lock(&food_mutex);
    if(food >= 0) food -= 1;
    pthread_mutex_unlock(&food_mutex);
    return food;
}

void think(philosopher_t * p){
    printf("Philosopher %d is thinking...\n", p->id);
    usleep((rand()%TTHINK_H + TTHINK_L) * 1000L);
}

void eat(philosopher_t * p){

    if(p->id % 2){
        pthread_mutex_lock(p->hashi_l);
        pthread_mutex_lock(p->hashi_r);
    } else {
        pthread_mutex_lock(p->hashi_r);
        pthread_mutex_lock(p->hashi_l);
    }

    p->execs += 1;
    printf("Philosopher %d is eating...\n", p->id);
    usleep((rand()%TEAT_H + TEAT_L) * 1000L);

    pthread_mutex_unlock(p->hashi_r);
    pthread_mutex_unlock(p->hashi_l);
} 

void * handle_philosopher(void * p){

    philosopher_t * philosopher = p;
    while(pick_food() >= 0){
        think(philosopher);
        eat(philosopher);
    }

    printf("Philosopher %d has finished!\n", philosopher->id);

    return NULL;
}

int main() {

    philosopher_t philosophers[MAX_PHILOSOPHERS];
    hashi_t hashis[MAX_PHILOSOPHERS];

    srand(time(NULL));

    /**
     * Inicializando mutexes
     */
    pthread_mutex_init(&food_mutex, NULL);
    for(int i = 0; i < MAX_PHILOSOPHERS; ++i) {
        pthread_mutex_init(&hashis[i], NULL);
    }

    /**
     * Inicializando filósofos
     */
    for(int i = 0; i < MAX_PHILOSOPHERS; ++i) {
        philosophers[i].id = i;
        philosophers[i].hashi_l = &hashis[i];
        philosophers[i].hashi_r = &hashis[(i+1)%MAX_PHILOSOPHERS];
        philosophers[i].execs = 0;
        generate_philosopher_name(philosophers[i].name);
    }

    /*
     * Criando threads para cada filósofo
     */
    pthread_t threads[MAX_PHILOSOPHERS];
    for(int i = 0; i < MAX_PHILOSOPHERS; ++i) {
        pthread_create(&threads[i], NULL, handle_philosopher, &philosophers[i]);
    }
    for(int i = 0; i < MAX_PHILOSOPHERS; ++i) pthread_join(threads[i], NULL);

    /*
     * Removendo mutexes inicializados.
     */
    pthread_mutex_destroy(&food_mutex);
    for(int i = 0; i < MAX_PHILOSOPHERS; ++i) pthread_mutex_destroy(&hashis[i]);

    /*
     * Imprimindo execuções de cada filósofo.
     */
    for(int i = 0; i < MAX_PHILOSOPHERS; ++i) {
        printf("Philosopher %s execs %d times in the shared memory area!\n", philosophers[i].name, philosophers[i].execs);
    }
}