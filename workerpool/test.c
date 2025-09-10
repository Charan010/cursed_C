#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "threadpool.h"

void print_task(void *arg){
    int num = *(int*)arg;
    printf("Thread running j*b with arg: %d\n", num);
    sleep(2);
}

int main(){
    ThreadPool pool;
    threadpool_init(&pool, 10);

    int args[10];
    for(int i = 0 ; i < 10 ; ++i){
        args[i] = i;
        threadpool_push_work(&pool, (void*)(print_task),&args[i]);
    }

    sleep(5);
    threadpool_destroy(&pool);

    return 0;

}