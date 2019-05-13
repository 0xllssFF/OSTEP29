#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>
#include <pthread.h>
#include <cstdlib>
#include <sys/wait.h>

#define N 2

struct conut{
    int value;
    pthread_mutex_t lock;
}counter;

void plus(){
    pthread_mutex_lock(&counter.lock);
    counter.value++;
    pthread_mutex_unlock(&counter.lock);
}
void* thread(void* arg){
    for(int i=0;i<100000;++i)
        plus();
}
int t=0;
int main(){

    struct timeval tv_begin,tv_end;
    pthread_t* tid;
    tid=(pthread_t*)malloc(N* sizeof(pthread_t));
    counter.value=0;
    pthread_mutex_init(&counter.lock,NULL);
    gettimeofday(&tv_begin,NULL);
    for(int i=0;i<N;++i){
        pthread_create(&tid[i],NULL,thread,NULL);
    }
    for(int i=0;i<N;++i){
        pthread_join(tid[i],NULL);
    }

    gettimeofday(&tv_end,NULL);
    printf("use=%ld\n",tv_end.tv_usec-tv_begin.tv_usec);

}
