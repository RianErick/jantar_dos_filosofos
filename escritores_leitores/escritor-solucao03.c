#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#define READERS 100
#define WRITERS 10

#define READER_MAX_TIME 300     /* in ms*/
#define READER_WAIT_TIME 800    /* in ms*/

#define WRITER_MAX_TIME 400    /* in ms*/    
#define WRITER_WAIT_TIME 200    /* in ms*/       

#define READER_ITER 10
#define WRITER_ITER 10

#define WRITER_BATCH 10
#define READER_BATCH 100

sem_t mutex_writer_update;
sem_t mutex_writer;
sem_t mutex_reader;

sem_t sem_readers;
sem_t sem_writers;

sem_t mutex_controller;

int active_readers = 0;
int active_writers = 0;

void read_data() {

    sem_wait(&sem_readers);

    sem_wait(&mutex_reader);
    active_readers += 1;
    if (active_readers == 1) {
        sem_wait(&mutex_writer);
    }
    sem_post(&mutex_reader);
    
    printf("Reading... %d\n", active_readers);
    usleep((rand()%READER_MAX_TIME+50) * 1000L);

    sem_wait(&mutex_reader);
    active_readers -= 1;
    if (active_readers == 0) {
        sem_post(&mutex_writer);
    }
    sem_post(&mutex_reader);
    sem_post(&mutex_controller);
}

void write_data() {

    sem_wait(&sem_writers);

    sem_wait(&mutex_writer_update);
    active_writers += 1;
    sem_post(&mutex_writer_update);

    sem_wait(&mutex_writer);

    printf("Writting...%d\n", active_writers);
    usleep((rand()%WRITER_MAX_TIME) * 1000L);
    
    sem_wait(&mutex_writer_update);
    active_writers -= 1;
    sem_post(&mutex_writer_update);

    sem_post(&mutex_writer);
    sem_post(&mutex_controller);
}

void * reader(){
    int iter = READER_ITER;
    while(--iter){
        usleep((rand()%READER_WAIT_TIME+10) * 1000L);
        read_data();
    }
    return NULL;
}

void * writer(){
    int iter = WRITER_ITER;
    while(--iter){
        usleep((rand()%WRITER_WAIT_TIME) * 1000L);
        write_data();
    }
    return NULL;
}

volatile int running = 1;
void * controller(){

    int value = 0;
    while(running){
        while(active_writers)
            sem_wait(&mutex_controller);

        sem_getvalue(&sem_readers, &value);
        if(value <= 0)
            for(int i = 0; i < READER_BATCH; ++i){
                sem_post(&sem_readers);
            }

        while(active_readers)
            sem_wait(&mutex_controller);

        sem_getvalue(&sem_writers, &value);
        if(value <= 0 && active_writers == 0)
            for(int i = 0; i < WRITER_BATCH; ++i){
                sem_post(&sem_writers);
            }
    }
    return NULL;
}

int main() {

    srand(time(NULL));

    sem_init(&mutex_writer, 0, 1);
    sem_init(&mutex_writer_update, 0, 1);
    sem_init(&mutex_reader, 0, 1);
    sem_init(&mutex_controller, 0, 1);

    sem_init(&sem_readers, 0, 0);
    sem_init(&sem_writers, 0, 0);

    pthread_t reader_threads[READERS];
    pthread_t writer_threads[WRITERS];

    for(int i = 0; i < READERS; ++i){
        pthread_create(reader_threads+i, NULL, reader, NULL);
    }

    for(int i = 0; i < WRITERS; ++i){
        pthread_create(writer_threads+i, NULL, writer, NULL);
    }

    pthread_t tcontroller;
    pthread_create(&tcontroller, NULL, controller, NULL);

    for(int i = 0; i < READERS; ++i){
        pthread_join(reader_threads[i], NULL);
    }

    for(int i = 0; i < WRITERS; ++i){
        pthread_join(writer_threads[i], NULL);
    }

    running = 0;
    pthread_join(tcontroller, NULL);
    printf("Processes finished!\n");

    return 0;
}