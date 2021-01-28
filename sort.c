#include "merge.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

/* LEFT index and RIGHT index of the sub-array of ARR[] to be sorted */
void singleThreadedMergeSort(int arr[], int left, int right) 
{
  if (left < right) {
    int middle = (left+right)/2;
    singleThreadedMergeSort(arr, left, middle); 
    singleThreadedMergeSort(arr, middle+1, right); 
    merge(arr, left, middle, right); 
  } 
}

// Struct to store array info for use throughout sort alg
static struct array
{
  int* arr;
  int left, right, thread2WaitForRight;
  pthread_t* thread2WaitFor;
}array;

// Sorts right-left, waits for right-right thread, then merges
void* threadSort(void* arg){
  singleThreadedMergeSort(((struct array*) arg)->arr, ((struct array*) arg)->left, ((struct array*) arg)->right);

  if (((struct array*)arg)->thread2WaitFor != NULL){
    pthread_join(*((struct array*)arg)->thread2WaitFor, NULL);
    merge(((struct array*) arg)->arr, ((struct array*) arg)->left, ((struct array*) arg)->right, ((struct array*) arg)->thread2WaitForRight);
  }

  pthread_exit(NULL);
}


/* 
 * P makes 2 threads and splits array, now stored in struct, into two quarters and a half
 * P and 1st thread together sort 1st half of array
 * 2nd thread makes it's own thread and together they sort 2nd half
 * P merges all once all threads terminate
 */
void multiThreadedMergeSort(int arr[], int left, int right) 
{
  pthread_t threads[3];
  struct array threadData[4];

  printf("Status of array on entry:\n");
  for (int i = left; i <= right; ++i){
    printf(" %d", arr[i]);
  }
  printf("\n");

  int rightBounds[4];
  rightBounds[1] = (right + left) / 2;
  rightBounds[0] = (rightBounds[1] + left) / 2;
  rightBounds[2] = (right + rightBounds[1]) / 2;
  rightBounds[3] = right;

  threadData[0].arr = arr;
  threadData[0].left = left;
  threadData[0].right = rightBounds[0];
  threadData[0].thread2WaitFor = NULL;
  for (int i = 1; i < 4; ++i){
    threadData[i].arr = arr;
    threadData[i].left = rightBounds[i - 1] + 1;
    threadData[i].right = rightBounds[i];
    if (i == 2){
      threadData[i].thread2WaitFor = &threads[2];
    }
    else{
      threadData[i].thread2WaitFor = NULL;
      threadData[i].thread2WaitForRight = right;
    }
  }

  for (int i = 0; i < 3; ++i){
    int checkSuccess = pthread_create(&threads[i], NULL, threadSort, &threadData[i + 1]);
    if (checkSuccess){
      fprintf(stderr, "ERROR: pthread_create() in multiThreadedMergeSort() returned %d\n", checkSuccess);
      exit(EXIT_FAILURE);
    }
  }

  singleThreadedMergeSort(arr, left, threadData[0].right);

  pthread_join(threads[0], NULL);

  printf("Status of array before merging LL & LR:\n");
  for (int i = left; i <= right; ++i){
    printf(" %d", arr[i]);
  }
  printf("\n");

  merge(arr, left, threadData[0].right, threadData[1].right);

  pthread_join(threads[1], NULL);
  merge(arr, left, threadData[1].right, right);
}
