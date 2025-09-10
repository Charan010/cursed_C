#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct Job{
    void (*function)(void *arg);
    void *arg;
    struct Job* next;
} Job;

typedef struct{
    Job* front;
    Job* rear;

    pthread_mutex_t lock;
    pthread_cond_t has_jobs;
    int stop;

} JobQueue;

typedef struct{
    pthread_t *threads;
    int n_num;
    JobQueue jq;

} ThreadPool;

void jobqueue_init(JobQueue *jq){
    jq -> front = jq -> rear = NULL;
    jq -> stop = 0;

    pthread_mutex_init(&jq->lock, NULL);
    pthread_cond_init(&jq->has_jobs, NULL);
}

void jobqueue_deinit(JobQueue *jq){
    pthread_mutex_destroy(&jq->lock);
    pthread_cond_destroy(&jq->has_jobs);
}

void jobqueue_push(JobQueue *jq, void (*function)(void*), void *arg){
    
    Job *j = malloc(sizeof(Job));
    j -> function = function;
    j -> arg = arg;
    j -> next = NULL;

    pthread_mutex_lock(&jq->lock);

    if(jq -> rear == NULL && jq -> front == NULL){
        jq -> front = j;
        jq -> rear = j;
     
    }else{
        jq -> rear -> next = j;
        jq -> rear = j;
    }

    pthread_cond_signal(&jq->has_jobs);
    pthread_mutex_unlock(&jq->lock);
}

Job* jobqueue_pop(JobQueue *jq){
    if(jq -> rear == NULL)
        return NULL;

    Job* temp = jq -> front;
    jq -> front = temp -> next;

    //empty after popping ? make rear NULL as well
    if(jq -> front == NULL)
        jq -> rear = NULL;
    
    return temp;

}

void *worker_loop(void *arg){
    JobQueue* jq = (JobQueue*)arg;

    pthread_t tid = pthread_self();

    while(1){
        pthread_mutex_lock(&jq->lock);

        while(jq -> front == NULL && !jq->stop){
            // go back to waiting queue if no tasks instead of busy waiting
            pthread_cond_wait(&jq->has_jobs, &jq->lock);
        }

        if(jq -> stop){
            pthread_mutex_unlock(&jq->lock);
            break;
        }

        Job* j = jobqueue_pop(jq);
        pthread_mutex_unlock(&jq->lock);

        if(j){
            printf("[Slave %lu] picked up work\n", (unsigned long)tid);
            j -> function(j -> arg);
            free(j);
        }
    }

    return NULL;
}


void threadpool_init(ThreadPool *pool,int n_num){
    pool -> n_num = n_num;
    pool -> threads = malloc(sizeof(pool) * n_num);
    jobqueue_init(&pool ->jq);


    for(int i = 0 ; i < n_num ; ++i){
        
        
        /* pool -> threads is not really a array here but we are just using pointer 
        arithematic as arr[i] is decomposed to *(arr + i). 
        */
        pthread_create(&pool -> threads[i], NULL, worker_loop,&pool -> jq);
    }
}


void threadpool_push_work(ThreadPool *tp ,void (*function(void*)),void *arg){
    jobqueue_push(&tp -> jq, (void*)(function), arg);
}

void threadpool_destroy(ThreadPool *tp){
    pthread_mutex_lock(&tp -> jq.lock);
    tp -> jq.stop = 1;

    for(int i = 0 ; i < tp -> n_num ; ++i)
        pthread_join(tp -> threads[i], NULL);

    pthread_cond_destroy(&tp -> jq.has_jobs);
    pthread_mutex_destroy(&tp -> jq.lock);
    jobqueue_deinit(&tp -> jq);

    printf("sucessfully killed threadpool :)) \n");
}

/* sorry for not censoring j*b :( */