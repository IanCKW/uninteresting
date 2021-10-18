#include "packer.h"
#include <pthread.h>
#include <semaphore.h>

int one_balls[2];
int two_balls[2];
int three_balls[2];

int num_ones = 0;
int num_twos = 0;
int num_threes = 0;

sem_t one_mutex, two_mutex, three_mutex;
sem_t one_wait, two_wait, three_wait;

void packer_init(void) {
    sem_init(&one_mutex, 0, 1);
    sem_init(&two_mutex, 0, 1);
    sem_init(&three_mutex, 0, 1);

    sem_init(&one_wait, 0, 1);
    sem_init(&two_wait, 0, 1);
    sem_init(&three_wait, 0, 1);
}

void packer_destroy(void) {
    sem_destroy(&one_mutex);
    sem_destroy(&two_mutex);
    sem_destroy(&three_mutex);

    sem_destroy(&one_wait);
    sem_destroy(&two_wait);
    sem_destroy(&three_wait);
}

int pack_ball(int colour, int id) {
    int *balls;
    int *mutex;
    int *wait;
    int *num_balls;
    if (colour == 1) {
        balls = one_balls;
        mutex = &one_mutex;
        wait = &one_wait;
        num_balls = &num_ones;
    }
    else if (colour == 2) {
        balls = two_balls;
        mutex = &two_mutex;
        wait = &two_wait;
        num_balls = &num_twos;
    }
    else {
        balls = three_balls;
        mutex = &three_mutex;
        wait = &three_wait;
        num_balls = &num_threes;
    }
    // critical section
    sem_wait(mutex);
    if (*num_balls == 0){
        sem_wait(wait);
    }

    balls[*num_balls] = id;
    *num_balls = *num_balls + 1;
    
    if (*num_balls == 2){
        sem_post(wait);
    }
    else{ 
        sem_post(mutex);
    }

    sem_wait(wait);
    int temp;
    for (int i =0; i<2; i++){
        if (id != balls[i]) temp = balls[i];
    }
    *num_balls -=1;
    sem_post(wait);
    if (*num_balls == 0){
        sem_post(mutex);
    }

    return temp;
}