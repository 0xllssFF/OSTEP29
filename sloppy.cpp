#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>
#include <pthread.h>
#include <cstdlib>
#include <sys/wait.h>

/*typedef unsigned long long cycles_t;

inline cycles_t currentcycles()
{
    cycles_t result;
    __asm__ __volatile__ ("rdtsc" : "=A" (result));

    return result;
}
 cycles_t t1,t2;
    t1=currentcycles();
    sleep(1);
    t2=currentcycles();
    printf("%lld\n",(t2-t1)/1000000);*/

#define NUMCPUS 4
#define N 4
#define stride 100000
typedef  struct __counter{
    int global;
    pthread_mutex_t glock;
    int local[NUMCPUS];
    pthread_mutex_t llock[NUMCPUS];
    int threshold;
}count_t;

struct pass{
    count_t* c;
    pthread_t id;
    int amt;
};
void init(count_t *c ,int threshold){
    c->threshold=threshold;
    c->global=0;
    pthread_mutex_init(&c->glock,NULL);
    for(int i=0;i<NUMCPUS;++i){
        c->local[i]=0;
        pthread_mutex_init(&c->llock[i],NULL);
    }
}
void update(count_t* c,int thread_id,int amt){
    int cpu=thread_id%NUMCPUS;
    pthread_mutex_lock(&c->llock[cpu]);
    c->local[cpu]+=amt;
    if(c->local[cpu]>=c->threshold){
        pthread_mutex_lock(&c->glock);
        c->global+=c->local[cpu];
        pthread_mutex_unlock(&c->glock);
        c->local[cpu]=0;
    }
    pthread_mutex_unlock(&c->llock[cpu]);
}
int get(count_t* c){
    pthread_mutex_lock(&c->glock);
    int val=c->global;
    pthread_mutex_unlock(&c->glock);
    return val;
}
void* thread(void* arg){
    pass tmp=*(pass*)arg;
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(tmp.id,&cpuset);
    pthread_t thread;
    thread=pthread_self();
    pthread_setaffinity_np(thread, sizeof(cpu_set_t),&cpuset);
    for(int i=0;i<100000;++i) {
        update(tmp.c, tmp.id, tmp.amt);
    }
    pthread_exit(0);

}
int main(){
    struct timeval tv_begin,tv_end;
    pthread_t* tid;
    pass* p;
    tid=(pthread_t*)malloc(N* sizeof(pthread_t));
    p=(pass* )malloc(N* sizeof(pass));
    count_t t;
    init(&t,stride);
    gettimeofday(&tv_begin,NULL);
    for(int i=0;i<N;++i){
        p[i].amt=1;
        p[i].c=&t;
        p[i].id=i;
        pthread_create(&tid[i],NULL,thread,&p[i]);
    }
    for(int i=0;i<N;++i){
        pthread_join(tid[i],NULL);
    }
    gettimeofday(&tv_end,NULL);
    printf("use=%ldm %ldus\n",tv_end.tv_sec-tv_begin.tv_sec,tv_end.tv_usec-tv_begin.tv_usec);
    printf("%d\n",t.global);
    int s;
    struct timeval sbegin,send;
    gettimeofday(&sbegin,NULL);
    for(int i=0;i<400000;++i){
        s++;
    }
    gettimeofday(&send,NULL);
    printf("single use=%ld\n",send.tv_usec-sbegin.tv_usec);

}