#include "packer.h"
#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>

#define RED 1
#define BLUE 2
#define GREEN 3

// You can declare global variables here
int N;
int count[3];

typedef struct
{
  int *color;
  sem_t ballReady;
  sem_t ballInside;
  sem_t mutex;
} factory;

factory fact[3];

void packer_init(int balls_per_pack)
{
  for (int i = 0; i < 3; i++)
  {
    sem_init(&fact[i].ballReady, 0, 1);
    sem_init(&fact[i].ballInside, 0, 0);
    sem_init(&fact[i].mutex, 0, 1);
    fact[i].color = malloc(sizeof(int) * balls_per_pack);
  }

  N = balls_per_pack;
}

void packer_destroy(void)
{
  for (int i = 0; i < 3; i++)
  {
    sem_destroy(&fact[i].ballReady);
    sem_destroy(&fact[i].ballInside);
    sem_destroy(&fact[i].mutex);
    free(fact[i].color);

  }
}

void pack_ball(int colour, int id, int *other_ids)
{
  // Write your code here.
  colour -= 1;

  sem_wait(&fact[colour].ballReady);
  sem_wait(&fact[colour].mutex);
  fact[colour].color[count[colour]] = id;
  count[colour]++;

  if (count[colour] == 1)
  {
    sem_post(&fact[colour].ballReady);
  }

  if (count[colour] == N)
  {
    int match = 0;
    for (int i = 0; i < N; i++)
    {
      if (id != fact[colour].color[i])
      {
        other_ids[match++] = fact[colour].color[i];
      }
    }

    count[colour] -= 1;
    for (int i = 0; i < N - 1; i++)
    {
      sem_post(&fact[colour].ballInside);
    }
    sem_post(&fact[colour].mutex);
  }
  else if (count[colour] > 0)
  {
    int match = 0;
    sem_post(&fact[colour].mutex);
    sem_post(&fact[colour].ballReady);
    sem_wait(&fact[colour].ballInside);

    for (int i = 0; i < N; i++)
    {
      if (id != fact[colour].color[i])
      {
        other_ids[match++] = fact[colour].color[i];
      }
    }

    count[colour] -= 1;
  }
}
