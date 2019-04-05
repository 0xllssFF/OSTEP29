
#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>
#include <pthread.h>
#include <cstdlib>
#include <sys/wait.h>

#define NUMTHREAD 1000
struct Node{
    int value;
    Node* prev;
    Node* next;
    pthread_mutex_t lock;
    Node(){
        value=0;
        prev=NULL;
        next=NULL;
        pthread_mutex_init(&lock,NULL);
    }
};
Node* head;
Node* tail;
void Listinit(){
    head=(Node*)malloc(sizeof(Node));
    tail=(Node*)malloc(sizeof(Node));
    head->next=tail;
    tail->prev=head;
}
void* thread(void* arg){
    int value=*(int*)arg;
    Node* tmpnode=(Node*)malloc(sizeof(Node));
    tmpnode->value=value;
    Node* current=head;
    pthread_mutex_lock(&current->lock);
    Node* next=current->next;
    while(true){
        pthread_mutex_lock(&next->lock);
        if(next==tail||next->value<value){
            next->prev=tmpnode;
            current->next=tmpnode;
            tmpnode->prev=current;
            tmpnode->next=next;
            pthread_mutex_unlock(&next->lock);
            pthread_mutex_unlock(&current->lock);
            return 0;
        }
        pthread_mutex_unlock(&current->lock);
        current=next;
        next=current->next;
    }
}
int main(){
    struct timeval tv_begin,tv_end;
    pthread_t* tid;
    tid=(pthread_t*)malloc(sizeof(pthread_t)*NUMTHREAD);
    Listinit();
    gettimeofday(&tv_begin,NULL);
    for(int i=0;i<NUMTHREAD;++i) {
        int t=random();
        pthread_create(&tid[i], NULL, thread, &t);
    }
    for(int i=0;i<NUMTHREAD;++i){
        pthread_join(tid[i],NULL);
    }
    gettimeofday(&tv_end,NULL);
    printf("use=%ldus\n",tv_end.tv_usec-tv_begin.tv_usec);
}
