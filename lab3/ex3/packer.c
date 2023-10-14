#include "packer.h"
#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>


#define RED 0
#define BLUE 1
#define GREEN 2


// You can declare global variables here
int N = 2;
int count[3];

typedef struct {
    int *color;
    sem_t ballReady;
    sem_t ballInside;
    sem_t mutex;
} factory;

factory fact[3];


void packer_init(void) {
   for (int i = 0; i < 3; i++) {
    sem_init(&fact[i].ballReady, 0, 1);
    sem_init(&fact[i].ballInside, 0, 0);
    sem_init(&fact[i].mutex, 0, 1);
    fact[i].color = malloc(sizeof(int) * N);
   }


}

void packer_destroy(void) {
    for (int i = 0; i < 3; i++) {
        sem_destroy(&fact[i].ballReady);
        sem_destroy(&fact[i].ballInside);
        sem_destroy(&fact[i].mutex);
        free(fact[i].color);
    }
}

int pack_ball(int colour, int id) {
    // Write your code here.
    colour -= 1;
    int res = -1;

    sem_wait(&fact[colour].ballReady);
    sem_wait(&fact[colour].mutex);
    fact[colour].color[count[colour]] = id;
    count[colour]++;

   if (count[colour] == N) {
    res = fact[colour].color[0];
    count[colour] -= 1;
    sem_post(&fact[colour].ballInside);
    sem_post(&fact[colour].mutex);
   } else if (count[colour] == N - 1) {
    sem_post(&fact[colour].mutex);
    sem_post(&fact[colour].ballReady);
    sem_wait(&fact[colour].ballInside);
    res = fact[colour].color[1];
    count[colour] -= 1;
    sem_post(&fact[colour].ballReady);
   }

    return res;
}