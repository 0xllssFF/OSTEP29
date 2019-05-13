#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>
#include <pthread.h>
#include <cstdlib>
#include <sys/wait.h>

int NUMTHREAD;
struct Node{
    int value;
    Node* prev;
    Node* next;
    pthread_mutex_t lock;       //每个Node独有一把锁
    Node(){
        value=0;
        prev=NULL;
        next=NULL;
        pthread_mutex_init(&lock,NULL);
    }
};
struct List{
    Node* head;
    Node* tail;
    pthread_mutex_t lock;
}L;

int insert(int key) {
    Node *mynew = new Node();
    mynew->value = key;
    pthread_mutex_lock(&L.lock);   // get list lock
    mynew->next = L.head;
    L.head->prev=mynew;
    L.head = mynew;
    pthread_mutex_unlock(&L.lock); // release lock
    return 0;
}

int search(int key) {
    pthread_mutex_lock(&L.lock);       // get list lock
    if (L.head ->next==L.tail) {              // if the list is empty
        pthread_mutex_unlock(&L.lock); // release list lock and exit
        return -1;
    }
    Node *node = L.head;
    pthread_mutex_lock(&node->lock);   // get first node lock
    pthread_mutex_unlock(&L.lock);    // release list lock
    int i;
    for (i = 0; node->value != key; i++) {
        if (!node->next) {
            i = -1;
            break;
        }
        pthread_mutex_lock(&node->next->lock); // get next node's lock
        pthread_mutex_unlock(&node->lock);     // unlock this node's lock
        node = node->next;
    }
    pthread_mutex_unlock(&node->lock);  // release node lock
    return i;
}

void Listinit(){
    L.head=(Node*)new Node();
    L.tail=(Node*)new Node();
    L.head->next=L.tail;
    L.tail->prev=L.head;
}
void* threadbuild(void* arg){
    int value=*(int*)arg;
    insert(value);
}
void* threadsearch(void* arg){
    int value=*(int*)arg;
    search(value);
}
int main(int argc,char **argv){
        NUMTHREAD=atoi(argv[1]);
        struct timeval tv_begin, tv_end;
        pthread_t *tid;
        tid = (pthread_t *) new pthread_t[NUMTHREAD];
        Listinit();
        gettimeofday(&tv_begin, NULL);
        int* t;
        t=new int[NUMTHREAD];
        for(int i=0;i<NUMTHREAD;++i){
            t[i]=i;
        }
        for (int i = 0; i < NUMTHREAD; ++i) {
            pthread_create(&tid[i], NULL, threadbuild, &t[i]);
        }
        for (int i = 0; i < NUMTHREAD; ++i) {
            pthread_join(tid[i], NULL);
        }
        gettimeofday(&tv_end, NULL);
        if(tv_end.tv_usec - tv_begin.tv_usec>0)
        printf("%ld\n", tv_end.tv_usec - tv_begin.tv_usec);

    gettimeofday(&tv_begin, NULL);
    for (int i = 0; i < NUMTHREAD; ++i) {
        pthread_create(&tid[i], NULL, threadsearch,&t[i]);
    }
    for (int i = 0; i < NUMTHREAD; ++i) {
        pthread_join(tid[i], NULL);
    }
    gettimeofday(&tv_end, NULL);
    if(tv_end.tv_usec - tv_begin.tv_usec>0)
        printf("%ld\n", tv_end.tv_usec - tv_begin.tv_usec);

}
