#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

/**
 * Problema 01: Um escritor permanecendo um tempo considerável
 * escrevendo, e leitores esperando tempo aleatório bem mais curto,
 * aumenta o processamento da cpu, tornando inviável caso haja muitos leitores.
 * 
 * Problema 02: Os leitores sofrem de starvation, a medida que o número de 
 * escritores se aproxima do número de leitores.
 */

#define READERS 100
#define WRITERS 10

#define READER_MAX_TIME 300     /* in ms*/
#define READER_WAIT_TIME 800    /* in ms*/

#define WRITER_MAX_TIME 400     /* in ms*/    
#define WRITER_WAIT_TIME 200    /* in ms*/       

#define READER_ITER 10
#define WRITER_ITER 10

sem_t mutex_ready;
sem_t mutex_writer;
sem_t mutex_reader;
int reading = 0;

int writers_ready = 0;

void read_data() {

    while(writers_ready){
        sem_wait(&mutex_ready);
        usleep(rand()%50+10);
        sem_post(&mutex_ready);
    }

    sem_wait(&mutex_reader);
    reading += 1;
    sem_post(&mutex_reader);
    
    printf("Reading... %d\n", reading);
    usleep((rand()%READER_MAX_TIME+50) * 1000L);

    sem_wait(&mutex_reader);
    reading -= 1;
    sem_post(&mutex_reader);
}

void write_data() {
    sem_wait(&mutex_ready);
    writers_ready += 1;
    sem_post(&mutex_ready);

    while(reading) usleep(rand()%100+READER_MAX_TIME);

    sem_wait(&mutex_writer);
    printf("Writting...\n");
    usleep((rand()%WRITER_MAX_TIME) * 1000L);
    writers_ready -= 1;
    sem_post(&mutex_writer);
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

int main() {

    srand(time(NULL));

    sem_init(&mutex_writer, 0, 1);
    sem_init(&mutex_reader, 0, 1);
    sem_init(&mutex_ready, 0, 1);

    pthread_t reader_threads[READERS];
    pthread_t writer_threads[WRITERS];

    for(int i = 0; i < READERS; ++i){
        pthread_create(reader_threads+i, NULL, reader, NULL);
    }

    for(int i = 0; i < WRITERS; ++i){
        pthread_create(writer_threads+i, NULL, writer, NULL);
    }

    for(int i = 0; i < READERS; ++i){
        pthread_join(reader_threads[i], NULL);
    }

    for(int i = 0; i < WRITERS; ++i){
        pthread_join(writer_threads[i], NULL);
    }

    printf("Processes finished!\n");

    return 0;
}