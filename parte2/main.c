#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <pthread.h>
#include "GetTimeMs64.h"

#define DATA_COUNT 1000000000

long glob_acc = 0;
volatile char glob_acc_lock = 0;
char glob_data[DATA_COUNT];
int	 glob_batch_size;



void acquire(){
  while(__sync_lock_test_and_set(&glob_acc_lock, 1));
}

void release(){
  glob_acc_lock = 0;
}

void *threadCode(void *arg){
  char *batch = (char *) arg;


  long local_acc = 0;
  for(int i = 0; i < glob_batch_size; i++) {
    local_acc += batch[i];
  }

  acquire();

  glob_acc += local_acc;
  release();

  return NULL;
}

int main(int argc, char **argv){
  // Get thread count
  if(argc != 2){
    printf("run: spinlock $thread_count\n");
    return 1;
  }

  int thread_count = atoi(argv[1]);

  if(!thread_count){
    printf("thread_count should be a positive integer\n");
    return 1;
  }

  if(thread_count > DATA_COUNT){
    printf("thread_count bigger than dataset (%d), will not spawn unnecessary threads\n", DATA_COUNT);
    thread_count = DATA_COUNT;
  }

  // Fill array
  srand(time(NULL));
  for(int i = 0; i < DATA_COUNT; i++){
    glob_data[i] = (rand()%200) - 100;
  }

  // Start Threads
  printf("Starting %d threads\n", thread_count);
  uint64 start_time = GetTimeMs64();
  pthread_t *thread_ids;
  thread_ids = (pthread_t *) malloc(sizeof(pthread_t) * thread_count);

  glob_batch_size = DATA_COUNT / thread_count;
  int thread_index;
  for(thread_index = 0; thread_index < thread_count; thread_index++){
    pthread_create(&(thread_ids[thread_index]), NULL, threadCode,
                   (void *) &(glob_data[thread_index * glob_batch_size]));
  }

  // Add remainder
  long local_acc = 0;
  for(int i = thread_index * glob_batch_size; i < DATA_COUNT; i++) {
    local_acc += glob_data[i];
  }
//  printf("Remainder: %li\n", local_acc);
  // Join Threads
  for(int i = 0; i < thread_count; i++) pthread_join(thread_ids[i], NULL);

//  printf("%li + %li = %li\n", glob_acc, local_acc, glob_acc + local_acc);
  glob_acc += local_acc;
  // Print Result
  printf("Result: %li (%llums)\n", glob_acc, GetTimeMs64() - start_time);

  free(thread_ids);
  return 0;
}

